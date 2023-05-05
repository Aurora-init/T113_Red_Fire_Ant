#include "zb_common.h"

#define MAX3(a,b,c) (((a) > (b)) ? ( ((a) > (c)) ? (a) : (c) ) : ( ((b) > (c)) ? (b) : (c) ))
#define MIN3(a,b,c) (((a) > (b)) ? ( ((b) > (c)) ? (c) : (b) ) : ( ((a) > (c)) ? (c) : (a) ))

static void invert_to_uint8(uint8_t src_char, uint8_t *dest_char)
{ 
    uint8_t tmp = 0;

    uint8_t i;
    for (i = 0; i < 8; i++) if (src_char & (1 << i)) tmp |= 1 << (7-i);

    *dest_char = tmp;
}  

static void invert_to_uint16(uint16_t src_dat, uint16_t *dest_dat)
{  
    uint16_t tmp = 0;

    uint8_t i;
    for (i = 0; i < 16; i++) if (src_dat & (1 << i)) tmp |= 1 << (15 - i);

    *dest_dat = tmp;
}  

uint16_t crc16_x25(const uint8_t *buf, uint16_t buf_len)
{
    uint16_t result = 0xFFFF;
    uint16_t poly   = 0x1021;
    uint8_t  wchar  = 0;  

    while (buf_len--) {
        wchar = *(buf++);
        invert_to_uint8(wchar, &wchar);

        result ^= (wchar << 8);

        uint8_t i;
        for (i = 0; i < 8; i++)
            if(result & 0x8000) result = (result << 1) ^ poly;
            else result = result << 1;
    }

    invert_to_uint16(result, &result);
    return (result ^ 0xFFFF);
}

uint8_t zb_generate_seqnum()
{
    static uint8_t seqnum_ = 0;

    return ++seqnum_;
}
