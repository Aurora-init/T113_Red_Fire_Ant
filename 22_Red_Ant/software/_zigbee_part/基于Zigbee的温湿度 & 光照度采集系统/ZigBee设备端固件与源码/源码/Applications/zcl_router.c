#include "zcl_router.h"

#include "bdb_interface.h"

#include "zb_port.h"

#include "hal_key.h"

#include "OnBoard.h"

byte zcl_router_taskid;

static void zcl_router_on_incoming_msgs(zclIncomingMsg_t *msg);
static void zcl_router_on_zdo_msg(zdoIncomingMsg_t *msg);
static void zcl_router_on_commissioning(bdbCommissioningModeMsg_t *msg);

void zcl_router_init(byte task_id)
{
    zcl_router_taskid = task_id;

    // Private endpint
    endpoint_desc_priv.task_id = &zcl_router_taskid;
    afRegister(&endpoint_desc_priv);

    RegisterForKeys(zcl_router_taskid);

    bdb_RegisterCommissioningStatusCB(zcl_router_on_commissioning);

    bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_STEERING | BDB_COMMISSIONING_MODE_FINDING_BINDING);

    zbport_->init();

    osal_start_timerEx(zcl_router_taskid, ZB_USER_EVENT, ZB_USER_EVENT_PERIOD);
}

uint16 zcl_router_event_loop(uint8 task_id, uint16 events)
{
    afIncomingMSGPacket_t *msg;

    (void)task_id;

    if (events & SYS_EVENT_MSG) {
        while ((msg = (afIncomingMSGPacket_t *)osal_msg_receive(zcl_router_taskid))) {
            switch (msg->hdr.event) {
            case AF_INCOMING_MSG_CMD:
              zbport_->on_priv_msg(msg->srcAddr.addr.shortAddr, msg->clusterId, msg->cmd.Data, msg->cmd.DataLength);
            break;

            case ZCL_INCOMING_MSG:
              zcl_router_on_incoming_msgs((zclIncomingMsg_t *)msg);
            break;

            case ZDO_CB_MSG: zcl_router_on_zdo_msg((zdoIncomingMsg_t *)msg); break;

            case ZDO_STATE_CHANGE: break;

            case KEY_CHANGE: if(((keyChange_t *)msg)->keys & HAL_KEY_SW_6) zbport_->on_click(); break;

            default: break;
            }/*switch*/

            osal_msg_deallocate((uint8 *)msg);
        }/* while */

        return (events ^ SYS_EVENT_MSG);
    }

    if (events & ROUTER_REJOIN_EVT) {
        bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_STEERING | BDB_COMMISSIONING_MODE_FINDING_BINDING );

        return (events ^ ROUTER_REJOIN_EVT);
    }

    if (events & ZB_USER_EVENT) {
        zbport_->on_timer(ZB_USER_EVENT_PERIOD);
        osal_start_timerEx(zcl_router_taskid, ZB_USER_EVENT, ZB_USER_EVENT_PERIOD);

        return (events ^ ZB_USER_EVENT);
    }

    return 0;
}

void zcl_router_on_commissioning(bdbCommissioningModeMsg_t *msg)
{
    switch(msg->bdbCommissioningMode) {
    case BDB_COMMISSIONING_FORMATION:
        if(msg->bdbCommissioningStatus == BDB_COMMISSIONING_SUCCESS) {
            //After formation, perform nwk steering again plus the remaining commissioning modes that has not been processed yet
            bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_STEERING | msg->bdbRemainingCommissioningModes);
        }
        else {
            //Want to try other channels?
            //try with bdb_setChannelAttribute
        }
    break;

    case BDB_COMMISSIONING_NWK_STEERING:
        if(msg->bdbCommissioningStatus == BDB_COMMISSIONING_SUCCESS) {
            //YOUR JOB:
            //We are on the nwk, what now?
        }
        else {
            //See the possible errors for nwk steering procedure
            //No suitable networks found
            //Want to try other channels?
            //try with bdb_setChannelAttribute
            osal_start_timerEx(zcl_router_taskid, ROUTER_REJOIN_EVT, ROUTER_REJOIN_PERIOD);
        }
    break;

    case BDB_COMMISSIONING_FINDING_BINDING:
        if(msg->bdbCommissioningStatus == BDB_COMMISSIONING_SUCCESS) {
            //YOUR JOB:
        }
        else {
            //YOUR JOB:
            //retry?, wait for user interaction?
        }
    break;

    case BDB_COMMISSIONING_INITIALIZATION:
        //Initialization notification can only be successful. Failure on initialization 
        //only happens for ZED and is notified as BDB_COMMISSIONING_PARENT_LOST notification
      
        //YOUR JOB:
        //We are on a network, what now?
    break;
  }
}

void zcl_router_on_incoming_msgs(zclIncomingMsg_t *msg)
{
}

void zcl_router_on_zdo_msg(zdoIncomingMsg_t *msg)
{
}
