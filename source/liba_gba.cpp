#include <string>
#include <map>
#include "liba_gba.h"

#include "tonc.h"
#include "gba_sram.h"
#include "gba_flash.h"
#include "agbabi.h"

// #include <string>
// #include "scene.hpp"
// #include "main.hpp"
bool unlock_gbp();
void loadAudio();
void onKeyInterrupt();

int spriteVOffset = 0;

u8 * soundbank = nullptr;

bool sortSprites = false;
int musicVolume = 0;

bool disableHBlank = true;
bool disableDMA = true;

bool inaccurateEmulator = false;
bool emulatorPrompted = false;

OBJ_ATTR obj_buffer[128];

OBJ_AFFINE* obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;

void toggleBG(int layer, bool state){
    if(state){
        REG_DISPCNT |= ((1 << layer) << 8);
    }else{
        REG_DISPCNT &= ~((1 << layer) << 8);
    }
}

void buildBG(int layer, int cbb, int sbb, int size, int prio, int mos){
    *(vu16*) (REG_BASE + 0x0008 + 0x0002 * layer) = BG_CBB(cbb) | BG_SBB(sbb) | BG_SIZE(size) | BG_PRIO(prio) | BG_MOSAIC * mos;
}

void toggleSprites(bool state){
    if(state){
        REG_DISPCNT |= DCNT_OBJ;
    }else{
        REG_DISPCNT &= ~DCNT_OBJ;
    }
}

void vsync(){
    VBlankIntrWait();
}

void showSprites(int count){
    if(sortSprites){
        int counters[4] = {0,0,0,0};

        int positions[count];

        for(int i = 0; i < count; i++){
            const int priority = (obj_buffer[i].attr2 >> 0xa) & 0b11;
            positions[i] = counters[priority]++ + (priority << 8);
        }

        int offsets[4] = {0,0,0,0};

        for(int i = 0; i < 4; i++){
            for(int j = 0; j < i; j++){
                offsets[i] += counters[j];
            }
        }

        for(int i = 0; i < count; i++){
            const int priority = positions[i] >> 8;
            const int position = positions[i] & 0xff;
            const int offset = offsets[priority];

            oam_mem[offset + position] = obj_buffer[i];
        }
    }else{
        oam_copy(oam_mem,obj_buffer,count);
    }

    obj_aff_copy(obj_aff_mem, obj_aff_buffer, 32);
}

unsigned int calculateRemoteTimeout(u32 interval, u32 framesToWait) {
    // Calculate the duration of one transfer in milliseconds
    float transferDurationMs = (float)interval * 61.04f / 1000.0f;
    // Calculate the total duration for the specified number of frames in milliseconds
    float totalDurationMs = (float)framesToWait * 16.67f;
    // Calculate how many transfers can occur in the total duration
    u32 transfers = (u32)((float)totalDurationMs / transferDurationMs);
    return transfers;
}

extern struct FlashInfo gFlashInfo;
struct FlashInfo flashInfo;
struct SramInfo sramInfo;

void testSaveMedia() {
    const int testRegion = 0x7FFF;
    const int testMirror = 0xFFFF;
    flashInfo.size = 0;
    int err;
    // Test SRAM
    err = sram_write(testRegion, (u8 *) "S", 1);
    if (!err) {
        // Is SRAM
        char a = '\0';
        // Write to mirror region and read lower mirror half
        __agbabi_memcpy1(sram_mem + testMirror, (u8 *) "R", 1);
        __agbabi_memcpy1((u8 *)(&a), sram_mem + testRegion, 1);
        // See if it was overwritten (mirrored) or persisted
        switch (a) {
            case 'R':
                sramInfo.size = testRegion;
                break;
            case 'S':
                sramInfo.size = testMirror;
                break;
            default:
                sramInfo.size = testRegion;
                break;
        }
        // Detect bootlegs
        detect_rom_backed_flash();
        return;
    }

    // Test Flash
    err = flash_init(FLASH_SIZE_AUTO);
    if (err) return; // EEPROM or no save media, no luck
    flashInfo = gFlashInfo;
}

void setUpLinkUniversal(bool detectEmus) {
    const bool inaccurateEmulator = detectEmus ? detect_inaccurate_emulator() : true;
    const u32 wirelessTimeout = inaccurateEmulator ? 45 : 15;
    const u32 wirelessRemoteTimeoutFrames = inaccurateEmulator ? 45 : 15;
    const u32 interval = 50;
    const u32 wirelessRemoteTimeout = calculateRemoteTimeout(interval, wirelessRemoteTimeoutFrames);
    delete linkUniversal;
    linkUniversal = new LinkUniversal(LinkUniversal::WIRELESS_AUTO,
                                      "Apotris",
                                      (LinkUniversal::CableOptions){
                                              .baudRate = LinkCable::BAUD_RATE_3,
                                              .timeout = LINK_CABLE_DEFAULT_TIMEOUT,
                                              .remoteTimeout = LINK_CABLE_DEFAULT_REMOTE_TIMEOUT,
                                              .interval = interval * 3,
                                              .sendTimerId = LINK_CABLE_DEFAULT_SEND_TIMER_ID},
                                      (LinkUniversal::WirelessOptions){
                                              .retransmission = true,
                                              .maxPlayers = LINK_WIRELESS_MAX_PLAYERS,
                                              .timeout = wirelessTimeout,
                                              .remoteTimeout = wirelessRemoteTimeout,
                                              .interval = interval,
                                              .sendTimerId = LINK_WIRELESS_DEFAULT_SEND_TIMER_ID,
                                              .asyncACKTimerId = 2});
    linkConnection = linkUniversal;
}

void deinitialize(){
}

bool closed(){
    return true;
}

IWRAM_CODE void memcpy16_fast(void* dest, const void* src, int hwCount) {
    __agbabi_memcpy2(dest, src, hwCount * 2);
}

IWRAM_CODE void memcpy32_fast(void* dest, const void* src, int wCount) {
    __aeabi_memcpy4(dest, src, wCount * 4);
}

IWRAM_CODE void memset32_fast(void* dest, int word, int wCount) {
    __aeabi_memset4(dest, wCount * 4, word);
}

void loadPalette(int palette, int index, const void *src, int count){
    memcpy16_fast(&pal_bg_mem[16*palette + index], src, count);
}

void loadTiles(int tileset, int index, const void *src, int count){
    memcpy32_fast(&tile_mem[tileset][index], src,count*8);
}

void loadTilemap(int tilemap, int index, const void *src, int count){
    u16 * dst = (u16 * ) &se_mem[tilemap];
    memcpy16_fast(&dst[index], src, count);
}

void clearTilemap(int tilemap){
    memset16(&se_mem[tilemap], 0,32*32);
}

void clearSpriteTiles(int index, int lengthX, int lengthY){
    memset32_fast(&tile_mem[4][index],0,lengthX*lengthY*8);
}

void clearSpriteTile(int index, int tx, int ty, int width){
    memset32_fast(&tile_mem[4][index + ty * width + tx],0,8);
}

void loadSpriteTiles(int index, const void *src, int lengthX, int lengthY){
    memcpy32_fast(&tile_mem[4][index], src,lengthX * lengthY * 8);
}

void loadSpriteTilesPartial(int index, const void *src, int tx, int ty, int lengthX, int lengthY, int rowLength){
    memcpy32_fast(&tile_mem[4][index + ty * rowLength + tx], src,lengthX * lengthY * 8);
}

void setPaletteColor(int palette, int index, u16 color, int count){
    memset16(&pal_bg_mem[palette * 16 + index],color,count);
}

void sprite_hide(OBJ_ATTR * sprite){
    obj_hide(sprite);
}

void sprite_unhide(OBJ_ATTR * sprite, int mode){
    obj_unhide(sprite,mode);
}

void sprite_set_pos(OBJ_ATTR * sprite, int x, int y){
    obj_set_pos(sprite,x,y);
}

void sprite_set_attr(OBJ_ATTR* sprite, int shape, int size, int tile_start, int palette, int priority){
    obj_set_attr(sprite, ATTR0_SHAPE(shape), ATTR1_SIZE(size), ATTR2_BUILD(tile_start, palette, priority));
}

void setLayerScroll(int layer, int x, int y){
    switch(layer){
        case 0:
            REG_BG0HOFS = x;
            REG_BG0VOFS = y;
            break;
        case 1:
            REG_BG1HOFS = x;
            REG_BG1VOFS = y;
            break;
        case 2:
            REG_BG2HOFS = x;
            REG_BG2VOFS = y;
            break;
        case 3:
            REG_BG3HOFS = x;
            REG_BG3VOFS = y;
            break;

    }
}

void enableLayerBlend(int layer){
    REG_BLDCNT = (1 << 6) + (1 << 0xb) + (1<<layer);
    REG_BLDALPHA = BLD_EVA(31) | BLD_EVB(15);

    // TLN_SetLayerBlendMode(2,BLEND_ADD,0);
}

void color_fade_palette(int palette, int index, const COLOR *src, COLOR color, int count, u32 alpha){
    clr_fade(src,color,&pal_bg_mem[palette*16+index],count,alpha);
}

void color_fade(COLOR * dst, const COLOR * src, COLOR color, int count, u32 alpha){
    clr_fade(src,color,dst,count,alpha);
}

void color_adj_brightness(int palette, int index, const COLOR * src, u32 count, FIXED alpha){
    clr_adj_brightness(&pal_bg_mem[palette*16+index], src, count, alpha);
}

void color_adj_MEM(COLOR * dst, const COLOR * src, u32 count, u32 alpha){
    clr_adj_brightness(dst, src, count, alpha);
}

void color_blend(COLOR *dst, const COLOR *srca, const COLOR *srcb, int nclrs, u32 alpha) {
    clr_blend(srca,srcb,dst,nclrs,alpha);
}

void addColorToPalette(int palette, int index, COLOR color, int count){
    for(int i = 0; i < count; i++){
        pal_bg_mem[palette * 16 + index + i] += color;
    }
}

void stopDMA(){
    disableDMA = true;
}

void setMosaic(int sx, int sy){
    REG_MOSAIC = MOS_BUILD(sx,sy,sx,sy);
}

void clearSprites(int count){
    oam_init(obj_buffer, count);
}

void clearTiles(int tileset, int index, int count){
    memset32_fast(&tile_mem[tileset][index],0,8 * count);
}

void sprite_enable_affine(OBJ_ATTR * sprite,int affineId, bool doubleSize){
    sprite->attr0 |= ATTR0_AFF + doubleSize * ATTR0_AFF_DBL_BIT;
    sprite->attr1 |= ATTR1_AFF_ID(affineId);
}

void sprite_enable_mosaic(OBJ_ATTR * sprite){
    sprite->attr0 |= ATTR0_MOSAIC;
}

void sprite_set_id(OBJ_ATTR * sprite, int id){
    sprite = &obj_buffer[id];
}

void sprite_set_size(OBJ_ATTR * sprite, FIXED size, int aff_id){
    obj_aff_identity(&obj_aff_buffer[aff_id]);
    obj_aff_scale(&obj_aff_buffer[aff_id], size, size);
}

void sprite_rotscale(OBJ_ATTR * sprite, FIXED sizex, FIXED sizey, int angle , int aff_id){
    obj_aff_identity(&obj_aff_buffer[aff_id]);
    obj_aff_rotscale(&obj_aff_buffer[aff_id], sizex, sizey, angle);
}

void setTiles(int tilemap, int index, int count, u32 tile){
    u16 * dst = (u16 * ) &se_mem[tilemap];
    memset16(&dst[index], tile, count);
}

void clearTilemapEntries(int tilemap, int index, int count){
    u16 * dst = (u16*) &se_mem[tilemap];

    memset16(&dst[index],0,count);
}

void mirrorPalettes(int index, int count){
    memcpy16_fast(&pal_bg_mem[index],&pal_obj_mem[index],(8 * 16));
}

void savePalette(COLOR * dst){
    memcpy32_fast(dst, pal_bg_mem, 256);
}

void enableBlend(int info){
    REG_BLDCNT = info;
}

void sprite_enable_blend(OBJ_ATTR * sprite){
    sprite->attr0 |= ATTR0_BLEND;
}

int prevDispCnt = 0;

void toggleRendering(bool state){
    if(state){
        REG_DISPCNT = prevDispCnt;
    } else {
        prevDispCnt = REG_DISPCNT;
        REG_DISPCNT = 0;
    }
}

void sprite_enable_flip(OBJ_ATTR *sprite, bool flipX, bool flipY){
    sprite->attr1 |= flipX << 0xc;
    sprite->attr1 |= flipY << 0xd;
}

void quit(){}

void findSoundbank(){
    const GBFS_FILE * gbfsFile = find_first_gbfs_file(reinterpret_cast<const void *>(AGB_ROM));
    const void * myFile = gbfs_get_obj(gbfsFile, "soundbank.bin", NULL);

    if(myFile == NULL){
        log("couldn't find soundbank.bin");
        return;
    }

    log("found soundbank.bin");

    soundbank = (u8*) myFile;
}

void buildEffectLocations(){

    const GBFS_FILE * gbfsFile = find_first_gbfs_file(reinterpret_cast<const void *>(AGB_ROM));
    const void * myFile = gbfs_get_obj(gbfsFile, "effect_locations.bin", NULL);

    u32 * file = (u32*) myFile;

    if(file[0] != 0x51){
        log("couldn't find effect location file");
        return;
    }

    log("found location file");

    file++;

    int count = *file++;
    for(int i = 0; i < count; i++){
        SoundEffectIds[i] = *file++;
    }

    count = *file++;
    log("found " + std::to_string(count) + " menu songs");
    for(int i = 0; i < count; i++){
        songs.menu.push_back(*file++);
    }

    count = *file++;
    log("found " + std::to_string(count) + " game songs");
    for(int i = 0; i < count; i++){
        songs.game.push_back(*file++);
    }
}

void loadAudio(){

    findSoundbank();
    buildEffectLocations();
}

void enableLayerWindow(int layer, int x1, int y1, int x2, int y2, bool invert){}

void disableLayerWindow(int layer){}

std::map<int,std::string> keyToString = {
        {KEY_LEFT,"Left"},
        {KEY_RIGHT,"Right"},
        {KEY_UP,"Up"},
        {KEY_DOWN,"Down"},
        {KEY_A,"A"},
        {KEY_B,"B"},
        {KEY_L,"L"},
        {KEY_R,"R"},
        {KEY_START,"Start"},
        {KEY_SELECT,"Select"},
};

void refreshWindowSize(){}

void toggleSpriteSorting(bool state){
    sortSprites = state;
}

u32 tileBuild(u16 value) {
    return value;
}

void setSpritePixel(int index, u8 tx, u8 ty, u8 width, u8 x, u8 y, u8 color) {
    TILE * t = &tile_mem[4][index + ty * width + tx];

    if(color)
        t->data[y] |= color << (x * 4);
    else
        t->data[y] &= ~(0xf << (x * 4));
}

void setTile(int sbb, int x, int y, uint32_t value) {
    se_mem[sbb][y * 32 + x] = value & 0xffff;
}

void setSpriteTile(int index, u8 tx, u8 ty, u8 width, const void *src) {
    memcpy32(&tile_mem[4][index + ty * width + tx], src, 8);
}

void clearSpriteTile(int index, u8 tx, u8 ty, u8 width) {
    memset32(&tile_mem[4][index+ ty * width + tx],0,8);
}

void setTileRow(int tileset, int index, int row, u32 data) {
    TILE* tile = &tile_mem[tileset][index];
    tile->data[row] = data;
}

void setTilesetPixel(int cbb, int index, int x, int y, int color) {
    TILE* tile = &tile_mem[cbb][index];

    tile->data[y] |= (color) << (x*4);
}

void color_fade_fast(int palette, int index, COLOR *src, COLOR color, int count, u32 alpha) {
    clr_fade_fast(src, color, &pal_bg_mem[palette * 16 + index], count, alpha);
}

s16 sinLut(int angle) {
    return sin_lut[angle];
}

void setSpriteTileRow(int index, int y, int n) {
    TILE* tile = &tile_mem[4][index];

    tile->data[y] |= n;
}

IWRAM_CODE void onKeyInterrupt(){
    // Soft reset on bootleg carts will destroy the save
    if (bootleg_type)
        return;
    REG_IME = 0;
    REG_DMA0CNT_H &= ~DMA_ENABLE;
    REG_DMA1CNT_H &= ~DMA_ENABLE;
    REG_DMA2CNT_H &= ~DMA_ENABLE;
    REG_DMA3CNT_H &= ~DMA_ENABLE;
    REG_TM0D = 0;
    REG_TM1D = 0;
    REG_TM2D = 0;
    REG_TM3D = 0;
    SoftReset();
}

