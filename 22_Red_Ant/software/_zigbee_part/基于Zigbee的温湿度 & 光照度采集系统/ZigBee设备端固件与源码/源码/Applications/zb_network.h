#ifndef __ZB_NETWORK_H__
#define __ZB_NETWORK_H__

#include "zb_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    void (*permit)(uint8_t timeout, uint8_t broadcast);

    void (*remove)(uint8_t *mac, uint16_t nwkaddr);

    void (*priv_send)(uint16_t nwkaddr, void *endpoint_desc, uint16_t cluster, const uint8_t *data, uint16_t len);

    void (*set_onoff)(uint16_t nwkaddr, uint8_t endpoint, uint8_t on);

    void (*set_level)(uint16_t nwkaddr, uint8_t endpoint, uint8_t level);
} zb_network_t;

extern const zb_network_t *zbnwk_;

#ifdef __cplusplus
}
#endif

#endif /* __ZB_NETWORK_H__ */
