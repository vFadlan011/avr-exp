#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/boot.h>
#include <util/twi.h>
#include <stdint.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

#define AHT10_ADDR      0x38
#define AHT10_CMD_INIT  0b11100001
#define AHT10_CMD_TRIG  0b10101100
#define AHT10_CMD_SR    0b10111010
#define AHT10_CTRL      0b00110011
#define AHT10_CTRL_NOP  0

#define F_CPU       16000000UL
#define SLA_W       (AHT10_ADDR << 1 | TW_WRITE)
#define SLA_R       (AHT10_ADDR << 1 | TW_READ)

typedef struct {
  uint8_t state;
  uint32_t raw_hum;
  uint32_t raw_temp;
  float relative_hum;
  float temp_celcius;
} aht10_reading_t;

volatile bool transmit = true;

void uart_transmit(unsigned char c);
void uart_print(char *c);

bool twi_wait(void);
void aht_init(void);
void aht_trigger_measurement(void);
void aht_read(aht10_reading_t *reading_data);
void aht_sr(void);

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

  /* Configure I2C (AHT10 Sensor) */
  aht_init();
  aht10_reading_t aht_reading;

  char s[50];

  sei();
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

bool twi_wait() {
  uint16_t timeout = ~0;

  while ((!(TWCR & (1<<TWINT))) && timeout) timeout--;

  return (bool)timeout;
}

void aht_init(void)
{
  bool success = true;
  TWSR &= ~((1 << TWPS0) | (1 << TWPS1)); // prescale 1
  TWBR = 73; // Bitrate: 98.765,43 Hz (max 100khz)
  PORTC |= (1 << PC5) | (1 << PC4);

  /* SEND START BIT */
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // Clear interrupt | Start TWI | Enable TWI
  success = twi_wait();
  if ((TWSR & 0xF8) != TW_START || !success) {
    uart_print("ERROR: START\n");
    success = false;
  }

  /* SEND SLA+W */
  if (success) {
    TWDR = SLA_W;
    TWCR = (1<<TWINT) | (1<<TWEN); // send TWDR
    success = twi_wait();
  }
  if ((TWSR & 0xF8) != TW_MT_SLA_ACK || !success) {
    uart_print("ERROR: INIT SLA+W\n");
    success = false;
  }

  /* SEND INIT BYTE (0xE1) */
  if (success) {
    TWDR = AHT10_CMD_INIT;
    TWCR = (1<<TWINT) | (1<<TWEN);
    success = twi_wait();
  }
  if ((TWSR & 0xF8)!= TW_MT_DATA_ACK || !success) {
    uart_print("ERROR: INIT CMD\n");
    success = false;
  }

  /* SEND CTRL BYTE (0x33) */
  if (success) {
    TWDR = AHT10_CTRL;
    TWCR = (1<<TWINT) | (1<<TWEN);
    success = twi_wait();
  }
  if ((TWSR & 0xF8)!= TW_MT_DATA_ACK || !success) {
    uart_print("ERROR: INIT CTRL\n");
    success = false;
  }

  /* SEND TRL NOP BYTE (0x0) */
  if (success) {
    TWDR = AHT10_CTRL_NOP;
    TWCR = (1<<TWINT) | (1<<TWEN);
    success = twi_wait();
  }
  if ((TWSR & 0xF8)!= TW_MT_DATA_ACK || !success) {
    uart_print("ERROR: INIT CTRL NOP\n");
    success = false;
  }

  /* SEND STOP BYTE */
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO); // send stop
  if (success) 
    uart_print("SUCCESS: TWI CONN\n");
}

void aht_trigger_measurement(void)
{
  /* SEND START BIT */
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // Clear interrupt | Start TWI | Enable TWI
  success = twi_wait();
  if ((TWSR & 0xF8) != TW_START || !success) {
    uart_print("ERROR: START\n");
    success = false;
  }

  /* SEND SLA+W */
  if (success) {
    TWDR = SLA_W;
    TWCR = (1<<TWINT) | (1<<TWEN); // send TWDR
    success = twi_wait();
  }
  if ((TWSR & 0xF8) != TW_MT_SLA_ACK || !success) {
    uart_print("ERROR: TRIG SLA+W\n");
    success = false;
  }

  /* SEND TRIGGER BYTE (0xAC) */
  if (success) {
    TWDR = AHT10_CMD_TRIG;
    TWCR = (1<<TWINT) | (1<<TWEN);
    success = twi_wait();
  }
  if ((TWSR & 0xF8)!= TW_MT_DATA_ACK || !success) {
    uart_print("ERROR: TRIG CMD\n");
    success = false;
  }

  /* SEND CTRL BYTE (0x33) */
  if (success) {
    TWDR = AHT10_CTRL;
    TWCR = (1<<TWINT) | (1<<TWEN);
    success = twi_wait();
  }
  if ((TWSR & 0xF8)!= TW_MT_DATA_ACK || !success) {
    uart_print("ERROR: TRIG CTRL\n");
    success = false;
  }

  /* SEND TRL NOP BYTE (0x0) */
  if (success) {
    TWDR = AHT10_CTRL_NOP;
    TWCR = (1<<TWINT) | (1<<TWEN);
    success = twi_wait();
  }
  if ((TWSR & 0xF8)!= TW_MT_DATA_ACK || !success) {
    uart_print("ERROR: TRIG CTRL NOP\n");
    success = false;
  }

  /* SEND STOP BYTE */
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO); // send stop
  if (success) 
    uart_print("SUCCESS: AHT10 TRIG\n");
}