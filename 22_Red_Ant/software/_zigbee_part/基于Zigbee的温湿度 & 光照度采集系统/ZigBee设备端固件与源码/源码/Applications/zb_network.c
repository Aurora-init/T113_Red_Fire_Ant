#include "zb_network.h"

#include "ZDProfile.h"

#include "zcl_general.h"

static void zb_nwk_permit(uint8_t timeout, uint8_t broadcast);

static void zb_nwk_remove(uint8_t *mac, uint16_t nwkaddr);

static void zb_nwk_priv_send(uint16_t nwkaddr, void *endpoint_desc, uint16_t cluster, const uint8_t *data, uint16_t len);

static void zb_nwk_set_onoff(uint16_t nwkaddr, uint8_t endpoint, uint8_t on);
static void zb_nwk_set_level(uint16_t nwkaddr, uint8_t endpoint, uint8_t level);

const zb_network_t *zbnwk_ = &(const zb_network_t){
    .permit = zb_nwk_permit,

    .remove = zb_nwk_remove,

    .priv_send = zb_nwk_priv_send,

    .set_onoff = zb_nwk_set_onoff,
    .set_level = zb_nwk_set_level,
};

void zb_nwk_permit(uint8_t timeout, uint8_t broadcast)
{
    zAddrType_t addr = {
        .addrMode = AddrBroadcast,
        .addr.shortAddr = NWK_BROADCAST_SHORTADDR_DEVZCZR,
    };

    if (broadcast) ZDP_MgmtPermitJoinReq(&addr, timeout, TRUE, FALSE);

    NLME_PermitJoiningRequest(timeout);
}

void zb_nwk_remove(uint8_t *mac, uint16_t nwkaddr)
{
    zAddrType_t addr = {
        .addrMode = Addr16Bit,
        .addr.shortAddr = nwkaddr,
    };

    ZDP_MgmtLeaveReq(&addr, mac, 0, FALSE, 0);
}

void zb_nwk_priv_send(uint16_t nwkaddr, void *endpoint_desc, uint16_t cluster, const uint8_t *data, uint16_t len)
{
    static uint8_t zb_nwk_priv_id = 0;

    afAddrType_t dest = {
        .addrMode = (nwkaddr < 0xFFFC) ? afAddr16Bit : afAddrBroadcast,
        .addr.shortAddr = nwkaddr,
        .endPoint = ((endPointDesc_t *)endpoint_desc)->endPoint,
    };

    zb_nwk_priv_id++;
    AF_DataRequest(&dest, (endPointDesc_t *)endpoint_desc, cluster, len, (uint8_t *)data, &zb_nwk_priv_id, AF_DISCV_ROUTE, AF_DEFAULT_RADIUS);
}

void zb_nwk_set_onoff(uint16_t nwkaddr, uint8_t endpoint, uint8_t on)
{
    afAddrType_t addr = {
        .addrMode = afAddr16Bit,
        .addr.shortAddr = nwkaddr,
        .endPoint = endpoint,
    };

    if (on == 0) zclGeneral_SendOnOff_CmdOff(ENDPOINT_ZHA, &addr, FALSE, zb_generate_seqnum());
    else zclGeneral_SendOnOff_CmdOn(ENDPOINT_ZHA, &addr, FALSE, zb_generate_seqnum());
}

void zb_nwk_set_level(uint16_t nwkaddr, uint8_t endpoint, uint8_t level)
{
    afAddrType_t dstAddr = {
        .addrMode = afAddr16Bit,
        .addr.shortAddr = nwkaddr,
        .endPoint = endpoint,
    };

    zclGeneral_SendLevelControlMoveToLevel(ENDPOINT_ZHA, &dstAddr, level, 5, false, zb_generate_seqnum());
}
