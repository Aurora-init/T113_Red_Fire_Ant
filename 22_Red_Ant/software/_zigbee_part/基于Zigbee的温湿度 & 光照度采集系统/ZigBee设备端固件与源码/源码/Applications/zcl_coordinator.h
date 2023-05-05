#ifndef __ZCL_COORDINATOR_H__
#define __ZCL_COORDINATOR_H__

#include "zcl.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern SimpleDescriptionFormat_t simpledesc_zha;

extern SimpleDescriptionFormat_t simpledesc_zll;

extern SimpleDescriptionFormat_t simpledesc_priv;

extern endPointDesc_t endpoint_desc_priv;

extern CONST zclAttrRec_t zcl_coordinator_attrs[];

extern uint8 CONST zcl_coordinator_attrs_len;

 /*
  * Initialization for the task
  */
void zcl_coordinator_init( byte task_id );

/*
 *  Event Process for the task
 */
UINT16 zcl_coordinator_event_loop( byte task_id, UINT16 events );

#ifdef __cplusplus
}
#endif

#endif /* ZCL_COORDINATOR_H */