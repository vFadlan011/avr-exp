#include <anim.h>

void draw_pattern(uint8_t pattern, int delay_ms) {
  sr_write(pattern);
  _delay_ms(delay_ms);
}

void binary_counter() {
  for (int i=0; i<256; i++) {
    draw_pattern(i, 150); 
  }
}

void stack_to_end() {
  uint8_t pattern = 0;
  uint8_t carry = 0;
  for (int i=7; i>=0; i--) {
    for (int j=0; j<i; j++) {
      pattern = (1 << j);
      draw_pattern((pattern | carry), 150);
    }
    carry |= pattern;
  }
}

void bounce_left_right() {
  for (int i=1; i<7; i++) {
    if (i==1) {
      shift_bit(1);
    } else {
      shift_bit(0);
    }
    latch();
    _delay_ms(200);
  }
  for (int i=7; i>=0; i--) { 
    sr_write((1 << i));
    _delay_ms(200);
  }
}

void left_sign(int len) {
  for (int i=0; i<8+len; i++) {
    if (i<len) {
      shift_bit(1);
    } else {
      shift_bit(0);
    }
    latch();
    usleep(300 * 1000);
  }
}

void loading_bar() {
  uint8_t pattern = 0;
  for (int i=0; i<=8; i++) {
    draw_pattern(pattern, 500);
    pattern |= (1 << i);
  }
}


void random_spark() {
  uint8_t pattern = 0;
  for (int i=0; i<8; i++) {
    pattern |= ((rand()%2 == 0) << (rand() % 8));
  }
  draw_pattern(pattern, 150);
}

void waterdrop() {
  uint8_t pattern = 0b00011000;
  uint8_t mask = pattern;

  for (int i=0; i<4; i++) {
    draw_pattern(pattern, 300);
    pattern |= ((pattern << 1) | (pattern >> 1));
    pattern &= (~mask);
    mask |= pattern;
  }
}

void heartbeat() {
  uint8_t frames[] = {
    0b00011000,
    0b00111100,
    0b01111110,
    0b11111111,
  };
  for (int i=0; i<4; i++) {
    draw_pattern(frames[i], 300);
  }
  for (int i=2; i>=0; i--) {
    draw_pattern(frames[i], 300);
  }
}