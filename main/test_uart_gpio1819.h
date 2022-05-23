typedef struct
{
    float tmp;
    float hum;
    float pressure;
} unitgps_t;

void init_uart_gpio1819(void);
void get_i2c_unitgps_data();
