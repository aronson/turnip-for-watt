#pragma once
#include <string>
#include <cstdint>
#include "tonc_memmap.h"
#include "seven/attributes.h"
#define LIBSEVEN_COMPATIBLE
#include "seven/types.h"
#include <posprintf.h>
#include <map>
#include <memory>
#include "flashSaves.h"

#define INLINE inline

static inline void setTile(int sbb, int x, int y, uint32_t value);

static inline uint32_t tileBuild(int index, bool flipx, bool flipy, int palette){
    return 0x1000 * palette + 0x400 * flipx + 0x800 * flipy + index;
}

INLINE u32 tileBuild(u16 value);

INLINE void setSpritePixel(int index, u8 tx, u8 ty, u8 width, u8 x, u8 y, u8 color);

INLINE void setSpriteTile(int index, u8 tx, u8 ty, u8 width, const void *src);

INLINE void clearSpriteTile(int index, u8 tx, u8 ty, u8 width);

INLINE void setTileRow(int tileset, int index, int row, u32 data);

INLINE void setTilesetPixel(int cbb, int index, int x, int y, int color);

INLINE void color_fade_fast(int palette, int index, COLOR *src, COLOR color, int count, u32 alpha);

INLINE s16 sinLut(int angle);

INLINE void setSpriteTileRow(int index, int y, int n);

extern OBJ_AFFINE* obj_aff_buffer;

#define blendInfo REG_BLDCNT

IWRAM_CODE void memcpy16_fast(void* dest, const void* src, int hwCount);
IWRAM_CODE void memcpy32_fast(void* dest, const void* src, int wCount);
IWRAM_CODE void memset32_fast(void* dest, int word, int wCount);

extern u16 gradientTable[192 + 1];

extern std::map<int,std::string> keyToString;

#define UNBOUND 0

extern void toggleSpriteSorting(bool state);
void setUpLinkUniversal(bool detectEmus);
void testSaveMedia();
