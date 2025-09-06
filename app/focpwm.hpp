
#pragma once
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
class focpwm
{
public:
    // private:
    typedef struct
    {
        uint32_t gpio;
        uint8_t slice_num;
        uint8_t channel_num;
        uint16_t top;
        float div;
    } pwm_hard_t;
    pwm_hard_t pwm_hard_a, pwm_hard_b, pwm_hard_c;
    uint32_t slice_mask;

private:
    static void irq_handler(void);
    void init_pwm_hard(pwm_hard_t *pwm_hard, uint32_t gpio, uint16_t top, uint32_t freq);

public:
    float period;
    float frequency;

public:
    focpwm(uint32_t pin_a, uint32_t pin_b, uint32_t pin_c, uint16_t top, uint32_t freq);
    ~focpwm();
    void start();
    void stop();
    void set_duty(uint16_t duty_a, uint16_t duty_b, uint16_t duty_c);
    void set_pin(uint32_t pin, uint16_t level);
};