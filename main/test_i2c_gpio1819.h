typedef struct {
    float tmp;
    float hum;
    float pressure;
} unitenv_t;

typedef struct {
    float distance;
} unit_ultrasonic_t;

void init_i2c_gpio1819(void);
void get_i2c_unit_data(unitenv_t *unitenv, unit_ultrasonic_t *ultrasonic);
