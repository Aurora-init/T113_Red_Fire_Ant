#ifndef __ZB_DATABASE_H__
#define __ZB_DATABASE_H__

#include "zb_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    void (*init)(void);

    bool (*write)(uint8_t *buf, uint8_t len);
    bool (*read)(uint8_t *buf, uint8_t len);
} zb_database_t;

extern const zb_database_t *zbdb_;

#ifdef __cplusplus
}
#endif

#endif /* __ZB_DATABASE_H__ */
