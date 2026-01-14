#include <stdint.h>
#include <ctype.h>

void SPI_init(int sck, int mosi, int load);
void SPI_transmit(uint8_t data);
void send_instruction(uint16_t instruction, uint8_t load_pin, uint8_t target_id);
void display_init(uint8_t decode_mode, uint8_t scan_limit, uint8_t brightness, uint8_t load_pin, uint8_t id, uint8_t total_display);
void display_codeB(uint8_t digit, uint8_t code, uint8_t load_pin, uint8_t id);
void display_char(int c, uint8_t load_pin, uint8_t id);