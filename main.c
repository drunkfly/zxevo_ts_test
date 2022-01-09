#include <evo.h>
#include "resources.h"

#define PALETTES_PAGE 1
#define GFX_PAGE	  24

void SetPage_0000_3fff(unsigned char index) _naked
{
_asm
    ld      hl, #2
	add     hl, sp
	ld      a, (hl)

    ld      bc, #0x10af      /* memory page 0 port */
    out     (c), a

    ret
_endasm;
}

void SetPage_4000_7fff(unsigned char index) _naked
{
_asm
    ld      hl, #2
	add     hl, sp
	ld      a, (hl)

    ld      bc, #0x11af      /* memory page 1 port */
    out     (c), a

    ret
_endasm;
}

void MapSystemPortsTo0000() _naked
{
_asm
    ld      bc, #0x15AF      /* FMAddr */
    ld      a, #0x10         /* Enable, A15:12 = 0 */
    out     (c), a
    ret
_endasm;
}

void UnmapSystemPorts() _naked
{
_asm
    ld      bc, #0x15AF      /* FMAddr */
    ld      a, #0            /* Disable, A15:12 = 0 */
    out     (c), a
    ret
_endasm;
}

void SetPalette(unsigned dstIndex, unsigned char srcIndex)
{
    const unsigned char* src;
    unsigned short* dst;
    unsigned char i;

    MapSystemPortsTo0000();

    SetPage_4000_7fff(PALETTES_PAGE);

    srcIndex *= 16;
    dstIndex *= 16;

    src = (unsigned char*)(0x4000 + srcIndex);
    dst = (unsigned short*)0x0000;
    for (i = 0; i < 16; i++) {
        unsigned char r = *src & 3;
        unsigned char g = (*src >> 2) & 3;
        unsigned char b = (*src >> 4) & 3;
        ++src;

        r <<= 3;
        g <<= 3;
        b <<= 3;

        *dst++ = b | (g << 5) | (r << 10);
    }

    UnmapSystemPorts();
}

static void DrawTile(unsigned char* p, const char* s)
{
    int x, y;
    for (y = 0; y < 8; y++) {
        for (x = 0; x < 4; x++) {
            unsigned char a, b;
            a = (*s++ == '#' ? 0x10 : 0x00);
            b = (*s++ == '#' ? 0x01 : 0x00);
            *p++ = a | b;
        }
        p += (512/2) - 4;
    }
}

void main(void)
{
    unsigned short* d;
    unsigned char* p;
    int x, y;

    // Configure memory
    _asm
    ld      a, #0xcc     // 11001100    ; RAM @ 0000, RAM write disable
    ld      bc, #0x21af  // MemConfig
    out     (c), a

    ld      a, #0x10     // 16, temp page for tilemap data
    ld      bc, #0x16af  // TMPage
    out     (c), a

    ld      a, #0x18     // 24, GFX_PAGE
    ld      bc, #0x17af  // T0GPage
    out     (c), a

    ld      a, #0x24     // disable sprites & tiles 1, enable tiles 0
    ld      bc, #0x06af  // TSConfig
    out     (c), a
    _endasm;

    SetPalette(0, 0);

    SetPage_4000_7fff(24);
    p = (unsigned char*)0x4000;
    for (y = 0; y < 4; y++) {
        *p++ = 0x11;
        *p++ = 0x11;
        *p++ = 0x22;
        *p++ = 0x22;
        p += (512/2) - 4;
        *p++ = 0x22;
        *p++ = 0x22;
        *p++ = 0x11;
        *p++ = 0x11;
        p += (512/2) - 4;
    }

    p = (unsigned char*)0x4004;
    DrawTile(p,
        "........"
        ".##..##."
        ".##..##."
        ".######."
        ".##..##."
        ".##..##."
        ".##..##."
        "........"
        );

    p = (unsigned char*)0x4008;
    DrawTile(p,
        "........"
        ".######."
        ".##....."
        ".######."
        ".##....."
        ".##....."
        ".######."
        "........"
        );

    p = (unsigned char*)0x400C;
    DrawTile(p,
        "........"
        ".##....."
        ".##....."
        ".##....."
        ".##....."
        ".##....."
        ".######."
        "........"
        );

    p = (unsigned char*)0x4010;
    DrawTile(p,
        "........"
        "..####.."
        ".##..##."
        ".##..##."
        ".##..##."
        ".##..##."
        "..####.."
        "........"
        );

    p = (unsigned char*)0x4014;
    DrawTile(p,
        "........"
        "##.##.##"
        "##.##.##"
        "##.##.##"
        "##.##.##"
        "##.##.##"
        ".##..##."
        "........"
        );

    p = (unsigned char*)0x4018;
    DrawTile(p,
        "........"
        ".#####.."
        ".##..##."
        ".##..##."
        ".#####.."
        ".##..##."
        ".##..##."
        "........"
        );

    p = (unsigned char*)0x401C;
    DrawTile(p,
        "........"
        ".#####.."
        ".##..##."
        ".##..##."
        ".##..##."
        ".##..##."
        ".#####.."
        "........"
        );

    SetPage_4000_7fff(16);
    d = (unsigned short*)0x4000;
    /*
    for (y = 0; y < 256; y++) {
        for (x = 0; x < 256; x++)
            *d++ = 0;
    }
    */

    d = (unsigned short*)0x4000;
    *d++ = 1;
    *d++ = 2;
    *d++ = 3;
    *d++ = 3;
    *d++ = 4;
    d++;
    *d++ = 5;
    *d++ = 4;
    *d++ = 6;
    *d++ = 3;
    *d++ = 7;
}
