#ifndef __ZCL_ROUTER_H__
#define __ZCL_ROUTER_H__

#include "zcl.h"

#ifdef __cplusplus
extern "C"
{
#endif

// Rejoin event
#define ROUTER_REJOIN_EVT          0x0080
#define ROUTER_REJOIN_PERIOD       1000

extern SimpleDescriptionFormat_t simpledesc_priv;

extern endPointDesc_t endpoint_desc_priv;

/*
 * Initialization for the task
 */
void zcl_router_init( byte task_id );

/*
 *  Event Process for the task
 */
UINT16 zcl_router_event_loop( byte task_id, UINT16 events );

#ifdef __cplusplus
}
#endif

#endif /* __ZCL_ROUTER_H__ */