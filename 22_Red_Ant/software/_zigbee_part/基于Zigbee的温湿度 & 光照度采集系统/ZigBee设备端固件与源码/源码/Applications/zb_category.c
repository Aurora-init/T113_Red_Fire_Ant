#include "zb_category.h"

#include "zb_database.h"

#include "zb_network.h"

#include "zb_msg.h"

#include "hal_lcd.h"

#include "zcl_general.h"

#include <string.h>

static void zb_category_init(void);
static void zb_category_on_host_msg(uint8_t *payload, uint16_t size);

static void zb_category_on_nwkaddr_rsp(uint8_t *mac, uint16_t nwkaddr);
static void zb_category_on_macaddr_rsp(uint8_t *mac, uint16_t nwkaddr);

static void zb_category_read_db(uint8_t len);

static void zb_category_on_dev_annce(uint8_t *mac, uint16_t nwkaddr);
static void zb_category_on_active_ep_rsp(uint16_t nwkaddr, uint8_t n, uint8_t *eplist);

static void zb_category_on_simple_desc_rsp(uint16_t nwkaddr,
                                           uint8_t endpoint,
                                           uint16_t profile,
                                           uint8_t ver,
                                           uint16_t devtype,
                                           uint8_t incluster_num,
                                           uint16_t *inclusters,
                                           uint8_t outcluster_num,
                                           uint16_t *outclusters);

static void zb_category_on_default_rsp(uint16_t nwkaddr);
static void zb_category_on_report(uint16_t nwkaddr, uint16_t cluster, void *data);

static void zb_category_on_leave(uint16_t nwkaddr);

const zb_category_t *zbcategory_ = &(const zb_category_t){
    .init = zb_category_init,
    .on_host_msg = zb_category_on_host_msg,

    .on_nwkaddr_rsp = zb_category_on_nwkaddr_rsp,
    .on_macaddr_rsp = zb_category_on_macaddr_rsp,

    .on_dev_annce = zb_category_on_dev_annce,
    .on_active_ep_rsp = zb_category_on_active_ep_rsp,
    .on_simple_desc_rsp = zb_category_on_simple_desc_rsp,

    .on_default_rsp = zb_category_on_default_rsp,
    .on_report = zb_category_on_report,

    .on_leave = zb_category_on_leave,
};

void zb_category_init()
{
    //zbdb_->init();

    zbmsg_->init();

    zbnwk_->permit(0, false);
}

void zb_category_on_host_msg(uint8_t *payload, uint16_t size)
{
    uint8_t action = payload[0];

    switch (action) {
    case 0x00: zbnwk_->set_panid_channel((uint16_t)payload[1] << 8 | payload[2], payload[3]); break;

    case 0x01: zbnwk_->nwkaddr_req(&payload[1]); break;

    case 0x02: zbnwk_->macaddr_req((uint16_t)payload[1] << 8 | payload[2]); break;

    case 0x03: zbnwk_->permit(payload[1], true); break;

    case 0x04: zbnwk_->allow_join(&payload[1]); break;

    case 0x05: zbdb_->write(&payload[2], payload[1]); break;

    case 0x06: zb_category_read_db(payload[1]); break;

    case 0x07: zbnwk_->remove(&payload[3], (uint16_t)payload[1] << 8 | payload[2]); break;

    case 0x08: zbnwk_->set_onoff((uint16_t)payload[1] << 8 | payload[2], payload[3], payload[4]); break;

    case 0x09: zbnwk_->set_rgb((uint16_t)payload[1] << 8 | payload[2], payload[3], payload[4], payload[5], payload[6]); break;

    case 0x0A: zbnwk_->set_level((uint16_t)payload[1] << 8 | payload[2], payload[3], payload[4]); break;
    }
}

void zb_category_on_nwkaddr_rsp(uint8_t *mac, uint16_t nwkaddr)
{
    uint8_t payload[] = {
        0x00,
        mac[7], mac[6], mac[5], mac[4], mac[3], mac[2], mac[1], mac[0],
        nwkaddr >> 8, (uint8_t)nwkaddr
    };

    zbmsg_->send(payload, sizeof(payload));
}

void zb_category_on_macaddr_rsp(uint8_t *mac, uint16_t nwkaddr)
{
    uint8_t payload[] = {
        0x01,
        mac[7], mac[6], mac[5], mac[4], mac[3], mac[2], mac[1], mac[0],
        nwkaddr >> 8, (uint8_t)nwkaddr
    };

    zbmsg_->send(payload, sizeof(payload));
}

void zb_category_read_db(uint8_t len)
{
    uint8_t payload[255] = { 0x02 };

    zbdb_->read(&payload[1], len);

    zbmsg_->send(payload, len + 1);
}

void zb_category_on_dev_annce(uint8_t *mac, uint16_t nwkaddr)
{
    zbnwk_->active_ep_req(nwkaddr);

    uint8_t payload[] = {
        0x02,
        nwkaddr >> 8,
        nwkaddr,
        mac[7], mac[6], mac[5], mac[4], mac[3], mac[2], mac[1], mac[0],
    };

    zbmsg_->send(payload, sizeof(payload));
}

void zb_category_on_active_ep_rsp(uint16_t nwkaddr, uint8_t n, uint8_t *eplist)
{
    uint8_t i;
    for (i = 0; i < n; i++) zbnwk_->simple_desc_req(nwkaddr, eplist[i]);

    uint8_t payload[255] = {
        [0] = 0x03,
        [1] = nwkaddr >> 8,
        [2] = nwkaddr,
        [3] = (n < sizeof(payload) - 4) ? n : sizeof(payload) - 4,
    };

    uint8_t j;
    for (j = 0; j < n && j < sizeof(payload) - 4; j++) payload[4 + j] = eplist[j];

    zbmsg_->send(payload, (n < sizeof(payload) - 4) ? n + 4 : sizeof(payload));
}

void zb_category_on_simple_desc_rsp(uint16_t nwkaddr,
                                    uint8_t endpoint,
                                    uint16_t profile,
                                    uint8_t ver,
                                    uint16_t devtype,
                                    uint8_t incluster_num,
                                    uint16_t *inclusters,
                                    uint8_t outcluster_num,
                                    uint16_t *outclusters)
{
    uint8_t payload[255] = {
        [0] = 0x04,
        [1] = nwkaddr >> 8,
        [2] = nwkaddr,
        [3] = endpoint,
        [4] = profile >> 8,
        [5] = profile,
        [6] = ver,
        [7] = devtype >> 8,
        [8] = devtype,
        [9] = incluster_num,
    };

    uint8_t i, n = 10;
    for (i = 0; i < incluster_num; i++) {
        payload[n++] = inclusters[i] >> 8;
        payload[n++] = inclusters[i];
    }

    payload[n++] = outcluster_num;
    for (i = 0; i < outcluster_num; i++) {
        payload[n++] = outclusters[i] >> 8;
        payload[n++] = outclusters[i];
    }

    zbmsg_->send(payload, n);
}

void zb_category_on_default_rsp(uint16_t nwkaddr)
{
    uint8_t payload[] = {
        [0] = 0x05,
        [1] = nwkaddr >> 8,
        [2] = nwkaddr,
    };

    zbmsg_->send(payload, sizeof(payload));
}

void zb_category_on_report(uint16_t nwkaddr, uint16_t cluster, void *data)
{
    uint8_t len;
    uint8_t payload[255];

    zclReportCmd_t *report_cmd = (zclReportCmd_t *)data;

    if (report_cmd->numAttr == 0) return;

    payload[0] = 0x06;
    payload[1] = nwkaddr >> 8;
    payload[2] = nwkaddr;
    payload[3] = cluster >> 8;
    payload[4] = cluster;
    payload[5] = report_cmd->numAttr;

    uint8_t i;
    for (i = 0, len = 6; i < report_cmd->numAttr; i++) {
        payload[len++] = report_cmd->attrList[i].attrID >> 8;
        payload[len++] = report_cmd->attrList[i].attrID;

        payload[len++] = report_cmd->attrList[i].dataType;

        len += zb_copy_attrdata(&payload[len], report_cmd->attrList[i].attrData, report_cmd->attrList[i].dataType);
    }

    zbmsg_->send(payload, len);
}

void zb_category_on_leave(uint16_t nwkaddr)
{
    uint8_t payload[] = {
        [0] = 0x07,
        [1] = nwkaddr >> 8,
        [2] = nwkaddr
    };

    zbmsg_->send(payload, sizeof(payload));
}