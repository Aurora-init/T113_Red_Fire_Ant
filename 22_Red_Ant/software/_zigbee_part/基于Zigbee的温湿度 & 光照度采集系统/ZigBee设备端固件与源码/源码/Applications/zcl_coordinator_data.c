#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ha.h"

#include "zcl_coordinator.h"

#include "zb_common.h"

/* Basic Cluster¡£ */
const uint8 hardware_revision = 0;
const uint8 zcl_version = 0;
const uint8 manufacture[] = { 9, 'B','y','t','e','H','o','u','s','e' }; // ByteHouse
const uint8 model[] = { 14, 'c', 'o', 'o', 'r', 'd', 'i', 'n', 'a', 't', 'o', 'r', '.', 'v', '1'}; // coordinator.v1
const uint8 power_source = POWER_SOURCE_DC;

/* currently all cluster implementations are according to ZCL6, 
   which has revision #1. In the future it is possible that different 
   clusters will have different revisions, so they will have to use
   separate attribute variables. */
const uint16 cluster_revision_all = 0x0001;

/* Attributes */
CONST zclAttrRec_t zcl_coordinator_attrs[] =
{
    {
        ZCL_CLUSTER_ID_GEN_BASIC,
        {
            ATTRID_BASIC_ZCL_VERSION,
            ZCL_DATATYPE_UINT8,
            ACCESS_CONTROL_READ,
            (void *)&zcl_version
        }
    },

    {
        ZCL_CLUSTER_ID_GEN_BASIC,
        {
            ATTRID_BASIC_HW_VERSION,
            ZCL_DATATYPE_UINT8,
            ACCESS_CONTROL_READ,
            (void *)&hardware_revision
        }
    },

    {
        ZCL_CLUSTER_ID_GEN_BASIC,
        {
            ATTRID_BASIC_MANUFACTURER_NAME,
            ZCL_DATATYPE_CHAR_STR,
            ACCESS_CONTROL_READ,
            (void *)manufacture
        }
    },

    {
        ZCL_CLUSTER_ID_GEN_BASIC,
        {
            ATTRID_BASIC_MODEL_ID,
            ZCL_DATATYPE_CHAR_STR,
            ACCESS_CONTROL_READ,
            (void *)model
        }
    },

    {
        ZCL_CLUSTER_ID_GEN_BASIC,
        {
            ATTRID_BASIC_POWER_SOURCE,
            ZCL_DATATYPE_ENUM8,
            ACCESS_CONTROL_READ,
            (void *)&power_source
        }
    },

    {
        ZCL_CLUSTER_ID_GEN_BASIC,
        {
            ATTRID_CLUSTER_REVISION,
            ZCL_DATATYPE_UINT16,
            ACCESS_CONTROL_READ,
            (void *)&cluster_revision_all
        }
    },  
};

uint8 CONST zcl_coordinator_attrs_len = (sizeof(zcl_coordinator_attrs) / sizeof(zcl_coordinator_attrs[0]));

/* In-clusters */
const cId_t zcl_coordinator_incluster_list[] = {
    ZCL_CLUSTER_ID_GEN_BASIC,
};
#define ZCLCOORDINATOR_MAX_INCLUSTERS    (sizeof(zcl_coordinator_incluster_list) / sizeof(zcl_coordinator_incluster_list[0]))

/* Out-clusters */
const cId_t zcl_coordinator_outcluster_list[] = {
    ZCL_CLUSTER_ID_GEN_IDENTIFY,
    ZCL_CLUSTER_ID_GEN_ON_OFF,
    ZCL_CLUSTER_ID_GEN_GROUPS,

    ZCL_CLUSTER_ID_GEN_LEVEL_CONTROL,
    ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL,
    ZCL_CLUSTER_ID_TOUCHLINK,
};
#define ZCLCOORDINATOR_MAX_OUTCLUSTERS   (sizeof(zcl_coordinator_outcluster_list) / sizeof(zcl_coordinator_outcluster_list[0]))

/* Clusters for the private endpoint. */
const cId_t zcl_coordinator_private_clusters[] = {
    PRIV_CID_MESSAGE,
    PRIV_CID_SETON,
    PRIV_CID_SETOFF,
};
#define ZCLCOORDINATOR_MAX_OUTCLUSTERS_PRIV   (sizeof(zcl_coordinator_private_clusters) / sizeof(zcl_coordinator_private_clusters[0]))

/* ZHA simple description. */
SimpleDescriptionFormat_t simpledesc_zha =
{
  ENDPOINT_ZHA,
  ZCL_HA_PROFILE_ID,
  ZCL_HA_DEVICEID_HOME_GATEWAY,
  0,
  0,
  ZCLCOORDINATOR_MAX_INCLUSTERS,
  (cId_t *)zcl_coordinator_incluster_list,
  ZCLCOORDINATOR_MAX_OUTCLUSTERS,
  (cId_t *)zcl_coordinator_outcluster_list,
};

/* ZLL simple description. */
SimpleDescriptionFormat_t simpledesc_zll =
{
  ENDPOINT_ZLL,
  ZCL_HA_PROFILE_ID,
  ZCL_HA_DEVICEID_HOME_GATEWAY,
  0,
  0,
  ZCLCOORDINATOR_MAX_INCLUSTERS,
  (cId_t *)zcl_coordinator_incluster_list,
  ZCLCOORDINATOR_MAX_OUTCLUSTERS,
  (cId_t *)zcl_coordinator_outcluster_list,
};

/* Private simple description. */
SimpleDescriptionFormat_t simpledesc_priv =
{
  ENDPOINT_PRIVATE,
  ZCL_HA_PROFILE_ID,
  ZCL_HA_DEVICEID_HOME_GATEWAY,
  0,
  0,
  ZCLCOORDINATOR_MAX_OUTCLUSTERS_PRIV,
  (cId_t *)zcl_coordinator_private_clusters,
  ZCLCOORDINATOR_MAX_OUTCLUSTERS_PRIV,
  (cId_t *)zcl_coordinator_private_clusters,
};

endPointDesc_t endpoint_desc_priv = {
  .endPoint = ENDPOINT_PRIVATE,
  .epType = 0,
  .simpleDesc = &simpledesc_priv,
  .latencyReq = (afNetworkLatencyReq_t)0,
};