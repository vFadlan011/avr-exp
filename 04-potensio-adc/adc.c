#include <avr/io.h>
#include <ctype.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <math.h>

void itoa(int n, char s[]);
void reverse(char s[], int len);
void dot(uint8_t n);
void dash(uint8_t n);
void sep();
void send_char(char c);
void send(char s[]);
void delay_ms(int ms);

double speed = 1.0;
const double ADC_MAX = 1023.0;
const double SPEED_MIN = 0.01;

int main() {
  uint16_t ain = 0;
  char ains[50];
  DDRB |= (1 << PB1) | (1 << PB2);
  DDRD |= 1 << PD5;
  PORTD |= (1 << PD6);

  ADMUX |= (1 << REFS0);
  ADCSRA |= 1 << ADEN;
  ADCSRA &= (~(1 << ADIE));

  while (1) {
    ADCSRA |= 1 << ADSC;
    while ((~(ADCSRA >> ADIF)) & 1) { _delay_ms(1); }
    
    // ASSIGN result;
    ain = 0;
    ain |= ADCL;
    ain |= (ADCH << 8);

    // set to clear ADIF
    ADCSRA |= (1 << ADIF);
    
    if ((~(PIND >> PD6)) & 1) {
      sep(); sep(); sep();
      send("HELLO WORLD");
      _delay_ms(1000);
    } else if ((PIND >> PD7) & 1) {
      sep(); sep(); sep();
      itoa(ain, ains);
      send(ains);
      _delay_ms(1000);
    }
  }
}

void itoa(int n, char s[]) {
  int i, sign;

  if ((sign = n) < 0) {
    n = -n;
  }

  i = 0;

  do {
    s[i++] = n % 10 + '0';
  } while ((n /= 10) > 0);

  if (sign < 0) {
    s[i++] = '-';
  }
  s[i] = '\0';
  reverse(s, i+1);
}

void reverse(char s[], int len) {
  int i = 0;
  int j = len - 2;
  char tempC;

  while (i < j) {
    tempC = s[i];
    s[i] = s[j];
    s[j] = tempC;
    ++i;
    --j;
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
    delay_ms(250);
    PORTD &= ~(1 << PD5);
    delay_ms(150);
}

void dot(uint8_t n) {
  for (; n > 0; n--) {
    PORTB |= 1 << PB2;
    delay_ms(150);
    PORTB &= ~(1 << PB2);
    delay_ms(150);
  }
}

void dash(uint8_t n) {
  for (; n > 0; n--) {
    PORTB |= 1 << PB1;
    delay_ms(350);
    PORTB &= ~(1 << PB1);
    delay_ms(150);
  }
}

void delay_ms(int ms) {
  for (int i = 0; i < ms; i++) {
    _delay_ms(1);
  }
}
