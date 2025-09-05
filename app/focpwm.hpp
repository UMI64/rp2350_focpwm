
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
        uint32_t slice_num;
        uint32_t channel_num;
        uint32_t count;
        float div;
    } pwm_hard_t;
    pwm_hard_t pwm_hard_a, pwm_hard_b, pwm_hard_c;
    uint32_t slice_mask;
    void init_pwm_hard(pwm_hard_t *pwm_hard, uint32_t gpio, uint32_t count, uint32_t freq);

public:
    focpwm(uint32_t pin_a, uint32_t pin_b, uint32_t pin_c, uint32_t count, uint32_t freq);
    ~focpwm();
    void start();
    void stop();
    void set_freq(uint32_t freq);
    void set_duty(uint16_t duty_a, uint16_t duty_b, uint16_t duty_c);
    void set_pin(uint32_t pin, uint16_t level);
};