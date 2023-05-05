#include "zb_msg.h"

#include "hal_uart.h"
#include "OnBoard.h"

#include <string.h>

/* Buffer size. */
#define ZB_MSG_RXBUF_MAX             (MT_UART_RX_BUFF_MAX + 32)
#define ZB_MSG_TXBUF_MAX             (MT_UART_TX_BUFF_MAX + 32)

static void (*zb_msg_receiver)(const uint8_t *, uint8_t) = 0;

static void zb_msg_init(void);
static int zb_msg_send(const uint8_t *payload, uint8_t size);
static void zb_msg_set_receiver(void (*receiver)(const uint8_t *payload, uint8_t size));
static void zb_msg_incoming(uint8_t port, uint8_t event);

const zb_msg_t *zbmsg_ = &(const zb_msg_t){
    .init = zb_msg_init,
    .send = zb_msg_send,
    .set_receiver = zb_msg_set_receiver,
};

void zb_msg_init()
{
	halUARTCfg_t config = {
        .configured = TRUE,
        .baudRate = HAL_UART_BR_9600,
        .flowControl = FALSE,
        .flowControlThreshold = 0,
        .rx.maxBufSize = ZB_MSG_RXBUF_MAX,
        .tx.maxBufSize = ZB_MSG_TXBUF_MAX,
        .idleTimeout = MT_UART_IDLE_TIMEOUT,
        .intEnable = TRUE,
        .callBackFunc = zb_msg_incoming,
    };

    HalUARTOpen(HAL_UART_PORT_0, &config);
}

int zb_msg_send(const uint8_t *payload, uint8_t size)
{
    HalUARTWrite(HAL_UART_PORT_0, (uint8_t *)payload, size);

    return 0;
}

void zb_msg_set_receiver(void (*receiver)(const uint8_t *, uint8_t))
{
    zb_msg_receiver = receiver;
}

void zb_msg_incoming(uint8_t port, uint8_t event)
{
    static uint8_t rxbuf[ZB_MSG_RXBUF_MAX] = { 0 };

    uint8_t len = Hal_UART_RxBufLen(HAL_UART_PORT_0);

    if (len == 0) return;

    HalUARTRead(HAL_UART_PORT_0, rxbuf, len);

    if (zb_msg_receiver == 0) return;

    zb_msg_receiver(rxbuf, len);
}
