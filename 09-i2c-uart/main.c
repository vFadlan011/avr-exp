#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/boot.h>
#include <stdint.h>
#include <ctype.h>
#include <stdbool.h>

#define F_CPU 16000000UL

volatile bool transmit = true;

void ts_get(float *ts);

void uart_transmit(unsigned char c);
void uart_print(char *c);

int main() 
{
  /* Configure Timer */
  TCCR1B = (1 << 3) | 0b100; // CTC mode | prescale 256: 62.5KHz
  OCR1A = 15624; // 250ms each match
  OCR1B = 15624;
  TIMSK1 = (1 << OCIE1B); // Timer Interrupt 1 Mask

  /* Configure Internal Temp Sensor ADC */
  ADMUX = (1 << REFS1) | (1 << REFS0) | (1 << MUX3); // Use internal 1.1v reference | select temp sensor channel
  ADCSRA |= (1 << ADEN) | (1 << ADATE); // Enable ADC | auto trigger ADC
  ADCSRB |= 0b101; // Trigger on counter1 match b
  
  float ts;

  /* Configure UART
  8-bit word (default)
  1 stop bit (default)
  Even parity
  250.000 bps
  */
  UCSR0B |= (1 << TXEN0); // Transmit mode
  UCSR0C |= (1 << UPM01); // Even parity
  UBRR0 = 3; // 250.000 bps
  sei();

  while (1) {
    if (transmit) {
      ts_get(&ts);

      transmit = false;
    }
  }
}

ISR (TIMER1_COMPA_vect)
{
  transmit = true;
}

void ts_get(float *ts)
{
  while (!(ADCSRA & (1 << ADIF)));
  uint16_t ts_raw = ADCL | (ADCH << 8);
  ADCSRA |= (1 << ADIF);

  *t = (ts_raw - 324.31) / 1.22;
}

void uart_transmit(unsigned char c)
{
  while (!(UCSR0A & (1<<UDRE0)))
    ;
  UDR0 = c;
}

void uart_print(char *c) {
  while (*c) {
    uart_transmit(*c);
    *c++;
  }
}