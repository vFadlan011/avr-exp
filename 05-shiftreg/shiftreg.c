#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

#define SER PD0
#define R_CLK PD1
#define SR_CLK PD2

void write(uint8_t data);
void shiftBit(bool bit);
void tick(void);
void latch(void);
void clear(void);

int main(void) {
  DDRD |= (1 << SER) | (1 << R_CLK) | (1 << SR_CLK);

  char msg[] = "HELLO WORLD";
  size_t msg_len = sizeof(msg) - 1; // exclude '\0'

  while (true) {
    for (size_t i=0; i<msg_len; i++) {
        sr_write(msg[i]);
        _delay_ms(500);
    }
  }
}

void sr_write(uint8_t data) {
  for (int i=0; i<8; i++) {
    shiftBit(data & 0x80);
    data <<= 1;
  }
  latch();
}

void shiftBit(bool bit) {
    if (bit)    PORTD |= (1 << SER);
    else        PORTD &= (~(1 << SER));

    tick();
}

void tick(void) {
    PORTD |= (1 << SR_CLK);
    _delay_ms(50);
    PORTD &= (~(1 << SR_CLK));
}

void latch(void) {
  PORTD |= (1 << R_CLK);
  _delay_ms(50);
  PORTD &= (~(1 << R_CLK));
}

void clear(void) {
    sr_write(0x00);
}