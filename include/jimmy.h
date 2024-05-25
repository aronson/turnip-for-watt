
//{{BLOCK(jimmy)

//======================================================================
//
//	jimmy, 64x64@8, 
//	+ palette 256 entries, not compressed
//	+ 29 tiles (t|f reduced) not compressed
//	+ regular map (flat), not compressed, 4x4 
//	Metatiled by 2x2
//	Total size: 512 + 1856 + 80 + 32 = 2480
//
//	Time-stamp: 2024-05-25, 02:32:04
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_JIMMY_H
#define GRIT_JIMMY_H

#define jimmyTilesLen 1856
extern const unsigned int jimmyTiles[464];

#define jimmyMetaTilesLen 80
extern const unsigned short jimmyMetaTiles[40];

#define jimmyMetaMapLen 32
extern const unsigned short jimmyMetaMap[16];

#define jimmyPalLen 512
extern const unsigned short jimmyPal[256];

#endif // GRIT_JIMMY_H

//}}BLOCK(jimmy)
