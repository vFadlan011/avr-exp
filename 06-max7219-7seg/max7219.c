#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#define F_CPU 16000000UL

#define SCK PB5
#define MOSI PB3
#define LOAD PB2

#define MAX_TEST 0xF
#define MAX_SHUTDOWN 0xC
#define MAX_SCAN 0xB
#define MAX_INTENSITY 0xA
#define MAX_MODE 0x9

void SPI_init(void);
void SPI_transmit(uint8_t);
void send_instruction(uint16_t);
void display_test(void);
void display_init(uint8_t, uint8_t, uint8_t);
void display(uint8_t, uint8_t);
void dot(uint8_t);
void display_str(char[]);

int main(void) {
  SPI_init();
  display_init(0xFF, 0x07, 4);

  while (1) {
    display_str("HELP1234");
  }
}

void SPI_init(void) {
  DDRB |= (1 << SCK) | (1 << MOSI) | (1 << LOAD);
  SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
  PORTB |= (1 << LOAD);
}

void SPI_transmit(uint8_t data) {
  SPDR = data;
  while (!(SPSR & (1 << SPIF)));
}

void send_instruction(uint16_t instruction) {
  SPI_transmit(instruction >> 8);
  SPI_transmit(instruction & 0xFF);
  PORTB &= (~(1 << LOAD));
  _delay_us(50);
  PORTB |= (1 << LOAD);
}

void display_test(void) {
  send_instruction((MAX_TEST << 8) | 1);
  _delay_ms(100);
  send_instruction((MAX_TEST << 8));
}

void display_init(uint8_t decode_mode, uint8_t scan_limit, uint8_t intensity) {
  send_instruction((MAX_SHUTDOWN << 8) | 1);
  display_test();
  send_instruction((MAX_MODE << 8) | decode_mode);
  send_instruction((MAX_SCAN<< 8) | (scan_limit & 0x7));
  send_instruction((MAX_INTENSITY<< 8) | (intensity & 0xF));
}

void display(uint8_t digit, uint8_t code) {
  send_instruction(((digit & 0xF) << 8) | code);
}

void dot(uint8_t digit) {
  send_instruction(((digit & 0xF) << 8) | (1 << 7));
}

void display_str(char msg[]) {
  for (int i=0; i<8; i++) {
    char c;
    switch (msg[i]) {
      case '-':
        c = 10;
        break;
      case 'E':
        c = 11;
        break;
      case 'H':
        c = 12;
        break;
      case 'L':
        c = 13;
        break;
      case 'P':
        c = 14;
        break;
      case ' ':
        c = 15;
        break;
      case '\0':
        c = 15;
        break;
      default:
        c = msg[i] - '0';
        break;
    }
    display(8-i, c);
  }
}