#include "focpwm.hpp"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#define BIT(x) (1ul << (x))
focpwm::focpwm(uint32_t pin_a, uint32_t pin_b, uint32_t pin_c, uint16_t top, uint32_t freq)
{
    irq_set_exclusive_handler(PWM_IRQ_WRAP_1, irq_handler);
    irq_set_enabled(PWM_IRQ_WRAP_1, true);
    init_pwm_hard(&pwm_hard_a, pin_a, top, freq);
    init_pwm_hard(&pwm_hard_b, pin_b, top, freq);
    init_pwm_hard(&pwm_hard_c, pin_c, top, freq);
    slice_mask = BIT(pwm_hard_a.slice_num) | BIT(pwm_hard_b.slice_num) | BIT(pwm_hard_c.slice_num);
}
focpwm::~focpwm()
{
    pwm_set_mask_enabled(~slice_mask);
}

void focpwm::start()
{
    pwm_set_mask_enabled(slice_mask);
}
void focpwm::stop()
{
    pwm_set_mask_enabled(~slice_mask);
}
void focpwm::set_duty(uint16_t duty_a, uint16_t duty_b, uint16_t duty_c)
{
    pwm_set_chan_level(pwm_hard_a.slice_num, pwm_hard_a.channel_num, duty_a);
    pwm_set_chan_level(pwm_hard_b.slice_num, pwm_hard_b.channel_num, duty_b);
    pwm_set_chan_level(pwm_hard_c.slice_num, pwm_hard_c.channel_num, duty_c);
}
void focpwm::set_pin(uint32_t pin, uint16_t level)
{
    pwm_set_gpio_level(pin, level);
}

void focpwm::init_pwm_hard(pwm_hard_t *pwm_hard, uint32_t gpio, uint16_t top, uint32_t freq)
{
    pwm_hard->gpio = gpio;
    pwm_hard->slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_hard->channel_num = pwm_gpio_to_channel(gpio);
    pwm_hard->div = (float)clock_get_hz(clk_sys) / freq;
    pwm_hard->div = pwm_hard->div > 256 ? 256 : pwm_hard->div;
    pwm_hard->top = top;
    period = (float)top / freq * 2 * 1000000;
    frequency = (float)clock_get_hz(clk_sys) / pwm_hard->div / top / 2;
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    pwm_clear_irq(pwm_hard->slice_num);
    pwm_irqn_set_slice_enabled(PWM_IRQ_WRAP_1, pwm_hard->slice_num, true);
    pwm_config c = pwm_get_default_config();
    pwm_config_set_clkdiv(&c, pwm_hard->div);
    pwm_config_set_wrap(&c, pwm_hard->top);
    pwm_config_set_phase_correct(&c, true);
    pwm_init(pwm_hard->slice_num, &c, false);
}

void focpwm::irq_handler(void)
{
}