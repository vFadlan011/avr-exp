#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <ctype.h>
#include <max7219.h>
#include <stdbool.h>

#define F_CPU 16000000UL

#define SCK PB5
#define MOSI PB3
#define LOAD PB2

uint8_t cs = 0;
uint8_t s = 0;
uint8_t m = 0;
uint8_t h = 0;

volatile bool count = true;
volatile bool update_display = true;
volatile int c = 0;

extern uint8_t EMOJI_NUM;
extern uint64_t IMAGES[];
extern uint8_t IMAGES_LEN;

int main() {
  TCCR1B = (1 << 3) | 0b010; // ctc mode prescale 8
  OCR1A = 19999;
  TIMSK1 = (1 << OCIE1A);
  PORTD = (1 << PD4) | (1 << PD6) | (1 << PD7);
  EICRA = (1 << ISC00);
  EIMSK = 1;
  PCICR |= (1 << PCIE2);
  PCMSK2 |= (1 << PCINT22) | (1 << PCINT23);


  SPI_init(SCK, MOSI, LOAD);
  display_init(0xFF, 0x07, 7, LOAD, 0, 2);
  display_init(0x00, 0x07, 7, LOAD, 1, 2);

  sei();
  while (1) {
    if (update_display) {
      display_codeB(1, ((cs % 10) | (1 << 7)), LOAD, 0);
      display_codeB(2, (cs / 10), LOAD, 0);

      display_codeB(3, ((s % 10) | (1 << 7)), LOAD, 0);
      display_codeB(4, (s / 10), LOAD, 0);

      display_codeB(5, ((m % 10) | (1 << 7)), LOAD, 0);
      display_codeB(6, (m / 10), LOAD, 0);

      display_codeB(7, ((h % 10) | (1 << 7)), LOAD, 0);
      display_codeB(8, (h / 10), LOAD, 0);

      display_char(c, LOAD, 1);

      update_display = false;
    }
  }
}

ISR (INT0_vect) {
  if ((!(PIND >> PD4)) & 1) {
    count = false;
  } else if (((PIND >> PD4) & 1)) {
    count = true;
  }
}

ISR (PCINT2_vect) {
  if ((!(PIND >> PD7)) & 1) {
    if (c) {
      c--;
    } else {
      c = IMAGES_LEN-1;
    }

  } else if ((!(PIND >> PD6)) & 1) {
    if (c==(IMAGES_LEN-1)) {
      c = 0;
    } else {
      c++;
    }
  }
}

ISR (TIMER1_COMPA_vect) {
  if (cs < 99) {
    cs++;
  } else {
    cs = 0;
    if (s < 59) {
      s++;
    } else {
      s = 0;
      if (m < 59) {
        m++;
      } else {
        m = 0;
        if (h < 23) {
          h++;
        } else {
          h = 0;
        }
      }
    }
  }

  update_display = true;
}

/*
todo:
off-load timer isr: only increment cs, calculate hms on main loop
off-load int isr: only set event flag and debounce
off-load pcint isr: only set event flag and debounce
*/