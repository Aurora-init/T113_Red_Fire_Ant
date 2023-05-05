#ifndef __ZB_MSG_H__
#define __ZB_MSG_H__

#include "zb_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    void (*init)(void);
    int (*send)(const uint8_t *payload, uint8_t size);
    void (*set_receiver)(void (*receiver)(const uint8_t *payload, uint8_t size));
} zb_msg_t;

extern const zb_msg_t *zbmsg_;

#ifdef __cplusplus
}
#endif

#endif /* __ZB_MSG_H__ */
