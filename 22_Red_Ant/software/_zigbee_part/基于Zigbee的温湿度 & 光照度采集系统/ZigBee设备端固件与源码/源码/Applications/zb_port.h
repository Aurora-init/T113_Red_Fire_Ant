#ifndef __ZB_PORT_H__
#define __ZB_PORT_H__

#include "zb_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    void (*init)(void);
    void (*on_click)(void);
    void (*on_priv_msg)(uint16_t nwkaddr, uint16_t cluster, const uint8_t *data, uint16_t len);
    void (*on_timer)(uint16_t ms);
} zb_port_t;

extern const zb_port_t *zbport_;

#ifdef __cplusplus
}
#endif

#endif /* __ZB_PORT_H__ */
