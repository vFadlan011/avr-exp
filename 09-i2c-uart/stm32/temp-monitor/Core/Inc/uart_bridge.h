#ifndef UART_BRIDGE_H
#define UART_BRIDGE_H

#include <stdint.h>
#include <stdbool.h>

#define RX_BUF_SIZE 64

extern volatile char rx_buf[RX_BUF_SIZE];
extern volatile uint8_t rx_idx;

extern volatile float aht_temp, aht_hum;
extern volatile bool message_received;

#endif
