#include "zcl_general.h"
#include "zcl_ha.h"

#include "zcl_router.h"

#include "zb_common.h"

/* currently all cluster implementations are according to ZCL6, 
   which has revision #1. In the future it is possible that different 
   clusters will have different revisions, so they will have to use
   separate attribute variables. */
const uint16 cluster_revision_all = 0x0001;

/* Clusters for the private endpoint. */
const cId_t zcl_router_private_clusters[] = {
    PRIV_CID_MESSAGE,
    PRIV_CID_SETON,
    PRIV_CID_SETOFF,
};
#define ZCLROUTER_MAX_OUTCLUSTERS_PRIV   (sizeof(zcl_router_private_clusters) / sizeof(zcl_router_private_clusters[0]))

/* Private simple description. */
SimpleDescriptionFormat_t simpledesc_priv =
{
  ENDPOINT_PRIVATE,
  ZCL_HA_PROFILE_ID,
  ZCL_HA_DEVICEID_TEST_DEVICE,
  0,
  0,
  ZCLROUTER_MAX_OUTCLUSTERS_PRIV,
  (cId_t *)zcl_router_private_clusters,
  ZCLROUTER_MAX_OUTCLUSTERS_PRIV,
  (cId_t *)zcl_router_private_clusters,
};

endPointDesc_t endpoint_desc_priv = {
  .endPoint = ENDPOINT_PRIVATE,
  .simpleDesc = &simpledesc_priv,
  .latencyReq = (afNetworkLatencyReq_t)0,
};