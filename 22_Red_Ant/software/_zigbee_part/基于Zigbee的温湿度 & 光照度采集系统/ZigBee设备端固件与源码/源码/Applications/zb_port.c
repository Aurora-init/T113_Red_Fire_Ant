#include "zb_port.h"

#include "zb_network.h"

#include "zb_msg.h"

#include "hal_lcd.h"

#include "AF.h"

#include "cc2530_ioctl.h"

#include "hal_adc.h"

#include "hal_dht11.h"

#include "hal_uart.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Uart
#define ZCLSAMPLESW_UART_BUF_LEN        128
static uint8 zclSampleSw_UartBuf[ZCLSAMPLESW_UART_BUF_LEN];
static void zclSampleSw_InitUart(void);
static void zclSampleSw_UartCB(uint8 port, uint8 event);

static void zb_on_msg(const uint8_t *data, uint8_t len);

static void zb_port_init(void);
static void zb_port_on_click(void);
static void zb_port_on_priv_msg(uint16_t nwkaddr, uint16_t cluster, const uint8_t *data, uint16_t len);
static void zb_port_on_timer(uint16_t ms);

#if ZG_BUILD_COORDINATOR_TYPE
static void zb_port_coordinator_on_msg(uint8_t temp, uint8_t humi, uint8_t light, uint8_t gas);
#endif

const zb_port_t *zbport_ = &(const zb_port_t){
    .init = zb_port_init,
    .on_click = zb_port_on_click,
    .on_priv_msg = zb_port_on_priv_msg,
    .on_timer = zb_port_on_timer,
};

void zb_on_msg(const uint8_t *data, uint8_t len)
{
#if ZG_BUILD_COORDINATOR_TYPE
    HalLcdWriteString("", 2);
    HalLcdWriteString("sync", 2);
#elif ZG_BUILD_RTRONLY_TYPE
#else
#endif
}

void zb_port_init()
{
#if ZG_BUILD_COORDINATOR_TYPE
    zbmsg_->set_receiver(zb_on_msg);
    zbmsg_->init();

    zbnwk_->permit(60, 0);
#elif ZG_BUILD_RTRONLY_TYPE
#else
#endif
}

void zb_port_on_click(void)
{
#if ZG_BUILD_COORDINATOR_TYPE
#elif ZG_BUILD_RTRONLY_TYPE
#else
#endif
}

void zb_port_on_priv_msg(uint16_t nwkaddr, uint16_t cluster, const uint8_t *data, uint16_t len)
{
#if ZG_BUILD_COORDINATOR_TYPE
    static uint8_t _temp = 25;
    static uint8_t _humi = 54;

    if (cluster == PRIV_CID_MESSAGE) {
        uint8_t temp  = data[0];
        uint8_t humi  = data[1];
        uint8_t light = data[2];

        if (temp != 0xFF) _temp = temp;
        if (humi != 0xFF) _humi = humi;

        char msg[32];
        
        //sprintf(msg, "%d#", _temp);
        //sprintf(msg, "T:%d,H:%d#", _temp, _humi);

        //zbmsg_->send((uint8_t *)msg, strlen(msg) + 1);

        zb_port_coordinator_on_msg(_temp, _humi, light, 0);
    }
#elif ZG_BUILD_RTRONLY_TYPE
#else
#endif
}

void zb_port_on_timer(uint16_t ms)
{
#if ZG_BUILD_COORDINATOR_TYPE
#elif ZG_BUILD_RTRONLY_TYPE
    extern endPointDesc_t endpoint_desc_priv;

    static uint8_t newdev = 0;

    if (!(_NIB.nwkDevAddress != INVALID_NODE_ADDR && _NIB.nwkPanId != INVALID_NODE_ADDR)) {
        if (newdev == 0) newdev = 1;
        return;
    }

    if (newdev != 0) {
        static uint8_t cnt = 0;
        if (cnt < 7) { cnt++; return; }
    }

    uint8_t buf[4];

    static uint8_t _temp = 0xFF, _humi = 0xFF, _light = 0xFF;

    halDHT11Data_t dht11Dat = halDHT11GetData();
    if (dht11Dat.ok && (dht11Dat.temp != 0 || dht11Dat.humi != 0)) {
        buf[0] = dht11Dat.temp;
        buf[1] = dht11Dat.humi;
    }
    else buf[0] = buf[1] = 0xFF;
    
    buf[2] = HalAdcRead(HAL_ADC_CHN_AIN7, HAL_ADC_RESOLUTION_8);
    if (buf[2] >= 125) buf[2] = 0;
    else buf[2] = 125 - buf[2];

    buf[3] = 0;

    if (_temp == buf[0] && _humi == buf[1] && (_light != 0xFF && (_light < buf[2] + 3) && (_light + 3 > buf[2]))) return;

    _temp = buf[0];
    _humi = buf[1];

#if 0
    /* Debug */
    char str[32];
    sprintf(str, "%d %d %d %d", _temp, _humi, buf[2], buf[3]);
    HalLcdWriteString(str, 3);
#endif

    if (_light > buf[2] + 3 || _light + 3 < buf[2]) {
        _light = buf[2];
        zbnwk_->priv_send(0, &endpoint_desc_priv, PRIV_CID_MESSAGE, buf, 4);
    }
    else {
        static uint8_t sendcnt = 0;
        if (++sendcnt > 3) { _light = buf[2]; sendcnt = 0; zbnwk_->priv_send(0, &endpoint_desc_priv, PRIV_CID_MESSAGE, buf, 4); }
    }
#else
#endif
}

#if ZG_BUILD_COORDINATOR_TYPE
void zb_port_coordinator_on_msg(uint8_t temp, uint8_t humi, uint8_t light, uint8_t gas)
{
    zclSampleSw_InitUart();
    
    char tempHumiStr[32];
    char lightGasStr[32];
    char tempx[32];
    char tempxHumixlightx[32];
    
    /*这两段封装了LCD显示的东西*/
    sprintf(tempHumiStr, "T:%d,H:%d", temp, humi);
    sprintf(lightGasStr, "LADC: %d", light);
    HalLcdWriteString(tempHumiStr, 3);
    HalLcdWriteString(lightGasStr, 4);
    
    /*这一段封装了串口打印的数据*/
    
    //sprintf(tempx,"%d\0",temp,humi); 
    //HalUARTWrite(HAL_UART_PORT_0, tempx, 10);
    sprintf(tempxHumixlightx, "%d%d%d\0", temp,humi,light); 
    HalUARTWrite(HAL_UART_PORT_0, tempxHumixlightx, 10);
    //HalUARTWrite(HAL_UART_PORT_0, Humix, 10);
    //HalUARTWrite(HAL_UART_PORT_0, lightx, 10);
}
#endif

static void zclSampleSw_InitUart(void)
{
  halUARTCfg_t uartConfig;

  /* UART Configuration */
  uartConfig.configured           = TRUE;
  uartConfig.baudRate             = HAL_UART_BR_115200;
  uartConfig.flowControl          = FALSE;
  uartConfig.flowControlThreshold = 0;
  uartConfig.rx.maxBufSize        = ZCLSAMPLESW_UART_BUF_LEN;
  uartConfig.tx.maxBufSize        = 0;
  uartConfig.idleTimeout          = 6;
  uartConfig.intEnable            = TRUE;
  uartConfig.callBackFunc         = zclSampleSw_UartCB;

  /* Start UART */
  HalUARTOpen(HAL_UART_PORT_0, &uartConfig);
}

/**
 * @fn      zclSampleSw_UartCB
 *
 * @brief   Uart Callback
 */
static void zclSampleSw_UartCB(uint8 port, uint8 event)
{
  uint8 rxLen = Hal_UART_RxBufLen(HAL_UART_PORT_0);
  
  if(rxLen != 0)
  {
    //读取串口接收到的数据
    HalUARTRead(HAL_UART_PORT_0  ,  zclSampleSw_UartBuf , rxLen);
    //发送开关命令
    //zclSampleSw_OnOffTest();
    //串口发送接收到的数据
    HalUARTWrite(HAL_UART_PORT_0 ,  zclSampleSw_UartBuf , rxLen);
  }
}
