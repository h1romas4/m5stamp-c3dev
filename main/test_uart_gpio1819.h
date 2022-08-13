#include <stdint.h>

typedef struct
{
    uint8_t num;
    uint8_t elevation;
    uint16_t azimuth;
    uint8_t snr;
} unitgpsgsv_t;

void init_uart_gpio1819(void);
void get_uart_gpsgsv_data(unitgpsgsv_t unitgpsgsv[], uint8_t *satellites);
