#ifndef __ZB_COMMON_H__
#define __ZB_COMMON_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef unsigned char uint8_t;
typedef unsigned int  uint16_t;

/* Endpoints */
#define ENDPOINT_ZHA            0x08
#define ENDPOINT_ZLL            0x0B
#define ENDPOINT_PRIVATE        120

/* Private clusters */
#define PRIV_CID_MESSAGE        0x0000
#define PRIV_CID_SETON          0x0001
#define PRIV_CID_SETOFF         0x0002

/* Events */
#define ZB_USER_EVENT           0x0001
#define ZB_USER_EVENT_PERIOD    1000

uint16_t crc16_x25(const uint8_t *buf, uint16_t buf_len);

uint8_t zb_generate_seqnum(void);

#ifdef __cplusplus
}
#endif

#endif /* __ZB_COMMON_H__ */