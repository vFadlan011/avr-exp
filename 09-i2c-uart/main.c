#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/boot.h>
#include <stdint.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

#define F_CPU 16000000UL

volatile bool transmit = true;

void uart_transmit(unsigned char c);
void uart_print(char *c);

void aht_init(void);
void aht_measure(float *temp, float *hum);

int main() 
{
  /* Configure Timer */
  TCCR1B = (1 << 3) | 0b100; // CTC mode | prescale 256: 62.5KHz
  OCR1A = 31249; // 500ms each match
  TIMSK1 = (1 << OCIE1A); // Timer Interrupt 1 Mask

  /* Configure UART
  8-bit word (default)
  1 stop bit (default)
  Even parity
  250.000 bps
  */
  UCSR0B |= (1 << TXEN0); // Transmit mode
  UCSR0C |= (1 << UPM01); // Even parity
  UBRR0 = 3; // 250.000 bps
  DDRD |= (1 << PD1);
  sei();

  /* Configure I2C (AHT10 Sensor) */
  TWCR |= (1 << TWEA) | (1 << TWEN); // Enable acknowledge bit | Enable TWI
  TWAR = 0x38 // AHT10 address
  float aht_temp, aht_hum;

  char s[50];

  while (1) {
    if (transmit) {
      snprintf(s, sizeof(s), "%.3f\t%.3f\n", aht_temp, aht_hum);
      uart_print(s);
      
      transmit = false;
    }
  }
}

ISR (TIMER1_COMPA_vect)
{
  transmit = true;
}

void uart_transmit(unsigned char c)
{
  while (!(UCSR0A & (1<<UDRE0)))
    ;
  UDR0 = c;
}

void uart_print(char *c)
{
  while (*c) {
    uart_transmit(*c);
    c++;
  }
}