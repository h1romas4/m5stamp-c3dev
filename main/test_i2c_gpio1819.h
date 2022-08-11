typedef struct
{
    float tmp;
    float hum;
    float pressure;
} unitenv_t;

void init_i2c_gpio1819(void);
void get_i2c_unitenv_data(unitenv_t *unitenv);
