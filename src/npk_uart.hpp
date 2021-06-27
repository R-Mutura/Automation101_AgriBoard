#ifndef NPk_UART_HPP
#define NPk_UART_HPP

//Serial.begin(115200);

struct nutrients
{
    /* data */
    byte nitrogen_val = 0;
    byte phosphorous_val=0;
    byte potassium_val =0;
};

typedef struct nutrients Nutrient_function;
void NPK_uart_init();
byte nitrogen();
byte phosphorous();
byte potassium();
Nutrient_function  NPK_get_values();

#endif