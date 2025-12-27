#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define F_CPU 16000000UL

#define SER PD0
#define R_CLK PD1
#define SR_CLK PD2
#define ANIM_NUM 8

bool sr_write(uint8_t data);
void shift_bit(bool bit);
void tick(void);
bool latch(void);
void clear(void);

volatile bool restart_requested = false;
volatile int mode = 0;

#include <anim.h>

int main(void) {
  DDRD |= (1 << SER) | (1 << R_CLK) | (1 << SR_CLK);
  PORTD |= (1 << PD5) | (1 << PD6) | (1 << PD7);
  PCICR |= (1 << PCIE2);
  PCMSK2 |= (1 << PCINT21) | (1 << PCINT22) | (1 << PCINT23);

  ADCSRA |= (1 << ADEN);
  ADMUX = 0x00;
  ADCSRA |= (1 << ADSC);
  while(ADCSRA & (1 << ADSC));
  srand(ADC);
  
  sei();

  while (true) {
    switch (mode) {
      case 0:
        hello_world();
        break;
      case 1:
        binary_counter();
        break;
      case 2:
        bounce_left_right();
        break;
      case 3:
        left_sign(3);
        break;
      case 4:
        loading_bar();
        break;
      case 5:
        random_spark();
        break;
      case 6:
        waterdrop();
        break;
      case 7:
        heartbeat();
        break;
      default:
        hello_world();
        break;
    }
  }
}

bool sr_write(uint8_t data) {
  for (int i=0; i<8; i++) {
    shift_bit(data & 0x80);
    data <<= 1;
  }
  return latch();
}

void shift_bit(bool bit) {
  if (bit)    PORTD |= (1 << SER);
  else        PORTD &= (~(1 << SER));

  tick();
}

void tick(void) {
  PORTD |= (1 << SR_CLK);
  _delay_ms(5);
  PORTD &= (~(1 << SR_CLK));
}

bool latch(void) {
  if (restart_requested) {
    restart_requested = false;
    return false;
  }

  PORTD |= (1 << R_CLK);
  _delay_ms(50);
  PORTD &= (~(1 << R_CLK));
  return true;
}

void clear(void) {
  sr_write(0x00);
}

ISR (PCINT2_vect) {
  _delay_ms(25);
  if ((~(PIND >> PD5)) & 1) {
    restart_requested = true;
    if (mode) {
      mode--;
    } else {
      mode = ANIM_NUM;
    }

  } else if ((~(PIND >> PD6)) & 1) {
    restart_requested = true;

  } else if ((~(PIND >> PD7)) & 1) {
    restart_requested = true;
    if (mode==(ANIM_NUM-1)) {
      mode = 0;
    } else {
      mode++;
    }
  }
}