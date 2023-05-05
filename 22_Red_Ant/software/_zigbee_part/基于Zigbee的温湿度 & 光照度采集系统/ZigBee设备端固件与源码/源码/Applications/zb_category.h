#ifndef __ZB_CATEGORY_H__
#define __ZB_CATEGORY_H__

#include "zb_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    void (*init)(void);
    void (*on_host_msg)(uint8_t *payload, uint16_t size);

    void (*on_nwkaddr_rsp)(uint8_t *mac, uint16_t nwkaddr);
    void (*on_macaddr_rsp)(uint8_t *mac, uint16_t nwkaddr);

    void (*on_dev_annce)(uint8_t *mac, uint16_t nwkaddr);
    void (*on_active_ep_rsp)(uint16_t nwkaddr, uint8_t n, uint8_t *eplist);

    void (*on_simple_desc_rsp)(uint16_t nwkaddr,
                               uint8_t endpoint,
                               uint16_t profile,
                               uint8_t ver,
                               uint16_t devtype,
                               uint8_t incluster_num,
                               uint16_t *inclusters,
                               uint8_t outcluster_num,
                               uint16_t *outclusters);

    void (*on_default_rsp)(uint16_t nwkaddr);
    void (*on_report)(uint16_t nwkaddr, uint16_t cluster, void *data);

    void (*on_leave)(uint16_t nwkaddr);
} zb_category_t;

extern const zb_category_t *zbcategory_;

#ifdef __cplusplus
}
#endif

#endif /* __ZB_CATEGORY_H__ */
