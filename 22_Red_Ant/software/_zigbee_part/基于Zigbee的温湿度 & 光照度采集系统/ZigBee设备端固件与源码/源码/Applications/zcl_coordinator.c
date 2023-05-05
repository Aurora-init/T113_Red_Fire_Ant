#include "zcl_coordinator.h"

#include "bdb_interface.h"

#include "zb_port.h"

#include "hal_key.h"

#include "OnBoard.h"

byte zcl_coordinator_taskid;

static void zcl_coordinator_on_incoming_msgs(zclIncomingMsg_t *msg);
static void zcl_coordinator_on_zdo_msg(zdoIncomingMsg_t *msg);
static void zcl_coordinator_on_commissioning(bdbCommissioningModeMsg_t *msg);

static zclGeneral_AppCallbacks_t zcl_coordinator_cmdcallbacks =
{
    NULL                    ,               // Basic Cluster Reset command
    NULL,                                   // Identify Trigger Effect command
    NULL,                                   // On/Off cluster commands
    NULL,                                   // On/Off cluster enhanced command Off with Effect
    NULL,                                   // On/Off cluster enhanced command On with Recall Global Scene
    NULL,                                   // On/Off cluster enhanced command On with Timed Off
#ifdef ZCL_LEVEL_CTRL
    NULL,                                   // Level Control Move to Level command
    NULL,                                   // Level Control Move command
    NULL,                                   // Level Control Step command
    NULL,                                   // Level Control Stop command
#endif
#ifdef ZCL_GROUPS
    NULL,                                   // Group Response commands
#endif
#ifdef ZCL_SCENES
    NULL,                                   // Scene Store Request command
    NULL,                                   // Scene Recall Request command
    NULL,                                   // Scene Response command
#endif
#ifdef ZCL_ALARMS
    NULL,                                   // Alarm (Response) commands
#endif
#ifdef SE_UK_EXT
    NULL,                                   // Get Event Log command
    NULL,                                   // Publish Event Log command
#endif
    NULL,                                   // RSSI Location command
    NULL                                    // RSSI Location Response command
};

void zcl_coordinator_init(byte task_id)
{
    zcl_coordinator_taskid = task_id;

    // This app is part of the Home Automation Profile
    bdb_RegisterSimpleDescriptor(&simpledesc_zha);
    bdb_RegisterSimpleDescriptor(&simpledesc_zll);

    // Register the ZCL General Cluster Library callback functions
    zclGeneral_RegisterCmdCallbacks(ENDPOINT_ZHA, &zcl_coordinator_cmdcallbacks);
    zclGeneral_RegisterCmdCallbacks(ENDPOINT_ZLL, &zcl_coordinator_cmdcallbacks);

    // Register the application's attribute list
    zcl_registerAttrList(ENDPOINT_ZHA, zcl_coordinator_attrs_len, zcl_coordinator_attrs);
    zcl_registerAttrList(ENDPOINT_ZLL, zcl_coordinator_attrs_len, zcl_coordinator_attrs);

    // Register the Application to receive the unprocessed Foundation command/response messages
    zcl_registerForMsgExt(zcl_coordinator_taskid, ENDPOINT_ZHA);
    zcl_registerForMsgExt(zcl_coordinator_taskid, ENDPOINT_ZLL);
    // Private endpint
    endpoint_desc_priv.task_id = &zcl_coordinator_taskid;
    afRegister(&endpoint_desc_priv);

    ZDO_RegisterForZDOMsg(zcl_coordinator_taskid, Mgmt_Leave_req);

    RegisterForKeys(zcl_coordinator_taskid);

    bdb_RegisterCommissioningStatusCB(zcl_coordinator_on_commissioning);

    bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_FORMATION | BDB_COMMISSIONING_MODE_FINDING_BINDING);

    zbport_->init();

    osal_start_timerEx(zcl_coordinator_taskid, ZB_USER_EVENT, ZB_USER_EVENT_PERIOD);
}

uint16 zcl_coordinator_event_loop(uint8 task_id, uint16 events)
{
    afIncomingMSGPacket_t *msg;

    (void)task_id;

    if (events & SYS_EVENT_MSG) {
        while ((msg = (afIncomingMSGPacket_t *)osal_msg_receive(zcl_coordinator_taskid))) {
            switch (msg->hdr.event) {
            case AF_INCOMING_MSG_CMD:
              zbport_->on_priv_msg(msg->srcAddr.addr.shortAddr, msg->clusterId, msg->cmd.Data, msg->cmd.DataLength);
            break;

            case ZCL_INCOMING_MSG:
              zcl_coordinator_on_incoming_msgs((zclIncomingMsg_t *)msg);
            break;

            case ZDO_CB_MSG: zcl_coordinator_on_zdo_msg((zdoIncomingMsg_t *)msg); break;

            case ZDO_STATE_CHANGE: break;

            case KEY_CHANGE: if(((keyChange_t *)msg)->keys & HAL_KEY_SW_6) zbport_->on_click(); break;

            default: break;
            }/*switch*/

            osal_msg_deallocate((uint8 *)msg);
        }/* while */

        return (events ^ SYS_EVENT_MSG);
    }

    if (events & ZB_USER_EVENT) {
        zbport_->on_timer(ZB_USER_EVENT_PERIOD);
        osal_start_timerEx(zcl_coordinator_taskid, ZB_USER_EVENT, ZB_USER_EVENT_PERIOD);

        return (events ^ ZB_USER_EVENT);
    }

    return 0;
}

void zcl_coordinator_on_commissioning(bdbCommissioningModeMsg_t *msg)
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

void zcl_coordinator_on_incoming_msgs(zclIncomingMsg_t *msg)
{
    switch (msg->zclHdr.commandID) {
#ifdef ZCL_READ
        case ZCL_CMD_READ_RSP: break;
#endif

#ifdef ZCL_WRITE
        case ZCL_CMD_WRITE_RSP: break;
#endif

#ifdef ZCL_REPORT
        case ZCL_CMD_CONFIG_REPORT: break;

        case ZCL_CMD_CONFIG_REPORT_RSP: break;

        case ZCL_CMD_READ_REPORT_CFG: break;

        case ZCL_CMD_READ_REPORT_CFG_RSP: break;

        case ZCL_CMD_REPORT: break;
#endif

        case ZCL_CMD_DEFAULT_RSP: break;

#ifdef ZCL_DISCOVER
        case ZCL_CMD_DISCOVER_CMDS_RECEIVED_RSP: break;

        case ZCL_CMD_DISCOVER_CMDS_GEN_RSP: break;

        case ZCL_CMD_DISCOVER_ATTRS_RSP: break;

        case ZCL_CMD_DISCOVER_ATTRS_EXT_RSP: break;
#endif

        default: break;
    }

    if (msg->attrCmd) osal_mem_free(msg->attrCmd);
}

void zcl_coordinator_on_zdo_msg(zdoIncomingMsg_t *msg)
{
}
