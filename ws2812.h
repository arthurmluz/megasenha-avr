#include <stdint.h>

typedef struct rgb_s {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_s;

void ws2812_init();
void ws2812_display(rgb_s *colors, uint32_t len);