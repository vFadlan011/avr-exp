#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <ctype.h>
#include <max7219.h>
#include <stdbool.h>

#define F_CPU 16000000UL
#define DEBOUNCE_DELAY 3
#define SCK PB5
#define MOSI PB3
#define LOAD PB2
#define BUTTON_COUNT 3

volatile uint32_t centiseconds = 0;
volatile bool count = true;
volatile bool update_display = true;
volatile uint8_t c = 0;

extern uint64_t IMAGES[];
extern uint8_t IMAGES_LEN;

typedef struct {
  uint8_t *inputreg;
  uint8_t pin_no;
  bool last_state;
  uint8_t delay;
  bool event;
} button_t;

volatile button_t hold; // PD2
volatile button_t next; // PD6
volatile button_t prev; // PD7

volatile button_t *buttons[BUTTON_COUNT] = {
  &hold,
  &next,
  &prev
};

button_t init_button(uint8_t *inputreg, char pin_no);

int main() {
  uint8_t cs, h, m, s;
  uint32_t remaining_seconds;

  TCCR1B = (1 << 3) | 0b010; // ctc mode prescale 8, 2MHz
  OCR1A = 19999; // 10ms each match
  TIMSK1 = (1 << OCIE1A); // Timer Interrupt 1 Mask

  hold = init_button(&PIND, PD2);
  next = init_button(&PIND, PD6);
  prev = init_button(&PIND, PD7);

  PORTD = (1 << hold.pin_no) | (1 << next.pin_no) | (1 << prev.pin_no);

  SPI_init(SCK, MOSI, LOAD);
  display_init(0xFF, 0x07, 7, LOAD, 0, 2);
  display_init(0x00, 0x07, 7, LOAD, 1, 2);

  sei();
  while (1) {
    if (update_display) {
      if (centiseconds >= 8640000) {
        centiseconds = 0;
      }
      
      cs = centiseconds % 100;
      remaining_seconds = centiseconds/100;
      h = remaining_seconds / 3600;
      m = remaining_seconds / 60 % 60;
      s = remaining_seconds % 60; // v

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

    if (hold.event) {
      if ((*(hold.inputreg) >> hold.pin_no) & 1) {
        count = true;
      } else {
        count = false;
      }
      hold.event = false;
    }

    if (next.event) {
      if (!((*(next.inputreg) >> next.pin_no) & 1)) {
        if (c==(IMAGES_LEN-1)) {
          c = 0;
        } else {
          c++;
        }
      }
      next.event = false;
    }

    if (prev.event) {
      if (!((*(prev.inputreg) >> prev.pin_no) & 1)) {
        if (c) {
          c--;
        } else {
          c = IMAGES_LEN-1;
        }
      }
      prev.event = false;
    }
  }
}

ISR (TIMER1_COMPA_vect) {
  if (count) {
    centiseconds++;
    update_display = true;
  }

  for (int i=0; i<BUTTON_COUNT; i++) {
    bool current_state = (*(buttons[i]->inputreg) >> buttons[i]->pin_no) & 1;

    if (current_state != buttons[i]->last_state) {
      if (buttons[i]->delay>=DEBOUNCE_DELAY) {
        buttons[i]->event = true;
        buttons[i]->last_state = current_state;
        buttons[i]->delay = 0;
      } else {
        buttons[i]->delay++;
      }
      update_display = true;
    } else {
      buttons[i]->delay = 0;
    }
  }
}

button_t init_button(uint8_t *inputreg, char pin_no) {
  button_t temp;
  temp.inputreg = inputreg;
  temp.pin_no = pin_no;
  temp.last_state = ((*inputreg) >> pin_no) & 1;
  temp.delay = 0;
  temp.event = false;
  return temp;
}