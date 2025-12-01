#include <avr/io.h>
#include <ctype.h>
#define F_CPU 2000000L
#include <util/delay.h>

void dot(uint8_t n);
void dash(uint8_t n);
void sep();
void send_char(char c);
void send(char s[]);

int main() {
  DDRB |= (1 << PB1) | (1 << PB2);
  DDRD |= 1 << PD5;
  PORTD |= (1 << PD6);

  while (1) {
    if ((~(PIND >> PD6)) & 1) {
      sep(); sep(); sep();
      send("SELAMAT PAGI SIANG SORE DAN MALAM");
      _delay_ms(1000);
    } else if ((PIND >> PD7) & 1) {
      sep(); sep(); sep();
      send("HALLO BANGG");
      _delay_ms(1000);
    }
  }
}

void send(char s[]) {
  char c;
  for (int i = 0; (c = s[i]) != '\0'; i++) {
    send_char(c);
  }
}

void send_char(char c) {
  switch (toupper(c)) {
    case 'A': dot(1); dash(1); break;
    case 'B': dash(1); dot(3); break;
    case 'C': dash(1); dot(1); dash(1); dot(1); break;
    case 'D': dash(1); dot(2); break;
    case 'E': dot(1); break;
    case 'F': dot(2); dash(1); dot(1); break;
    case 'G': dash(2); dot(1); break;
    case 'H': dot(4); break;
    case 'I': dot(2); break;
    case 'J': dot(1); dash(3); break;
    case 'K': dash(1); dot(1); dash(1); break;
    case 'L': dot(1); dash(1); dot(2); break;
    case 'M': dash(2); break;
    case 'N': dash(1); dot(1); break;
    case 'O': dash(3); break;
    case 'P': dot(1); dash(2); dot(1); break;
    case 'Q': dash(2); dot(1); dash(1); break;
    case 'R': dot(1); dash(1); dot(1); break;
    case 'S': dot(3); break;
    case 'T': dash(1); break;
    case 'U': dot(2); dash(1); break;
    case 'V': dot(3); dash(1); break;
    case 'W': dot(1); dash(2); break;
    case 'X': dash(1); dot(2); dash(1); break;
    case 'Y': dash(1); dot(1); dash(2); break;
    case 'Z': dash(2); dot(2); break;
    case '0': dash(5); break;
    case '1': dot(1); dash(4); break;
    case '2': dot(2); dash(3); break;
    case '3': dot(3); dash(2); break;
    case '4': dot(4); dash(1); break;
    case '5': dot(5); break;
    case '6': dash(1); dot(4); break;
    case '7': dash(2); dot(3); break;
    case '8': dash(3); dot(2); break;
    case '9': dash(4); dot(1); break;
  }
  sep();
}

void sep() {
    PORTD |= 1 << PD5;
    _delay_ms(300);
    PORTD &= ~(1 << PD5);
    _delay_ms(150);
}

void dot(uint8_t n) {
  for (; n > 0; n--) {
    PORTB |= 1 << PB2;
    _delay_ms(150);
    PORTB &= ~(1 << PB2);
    _delay_ms(150);
  }
}

void dash(uint8_t n) {
  for (; n > 0; n--) {
    PORTB |= 1 << PB1;
    _delay_ms(350);
    PORTB &= ~(1 << PB1);
    _delay_ms(150);
  }
}
