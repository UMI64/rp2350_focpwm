#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "FreeRTOS.h"
#include "task.h"

#include "focpwm.hpp"

int64_t alarm_callback(alarm_id_t id, void *user_data)
{
    // Put your timeout handler code in here
    return 0;
}

void main_task(__unused void *params)
{
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 1);
    vTaskDelay(pdMS_TO_TICKS(1000));
    gpio_put(25, 0);
    vTaskDelay(pdMS_TO_TICKS(1000));
    gpio_put(25, 1);
    vTaskDelay(pdMS_TO_TICKS(1000));
    gpio_put(25, 0);
    vTaskDelay(pdMS_TO_TICKS(1000));
    // focpwm pwm(25, 26, 27, 50000, 1 * MHZ); // 1MHz 1us, 50K top, p = 1 * 50000 = 50ms
    focpwm pwm(25, 26, 27, 2500, 50 * MHZ); // 50MHz 0.02us, 2.5K top, p = 2 * 0.02 * 2500 = 100us, f = 10KHz
    pwm.set_duty(500, 200, 201);
    pwm.start();
    auto printinfo = [&](focpwm::pwm_hard_t &pwm_hard)
    {
        printf("slice %lu channel %lu div %0.6f, top %lu\n", pwm_hard.slice_num, pwm_hard.channel_num, pwm_hard.div, pwm_hard.top);
    };
    while (true)
    {
        printinfo(pwm.pwm_hard_a);
        printinfo(pwm.pwm_hard_b);
        printinfo(pwm.pwm_hard_c); 
        printf("frequency %0.2f Hz period %0.2f us\n",pwm.frequency,pwm.period);
        vTaskDelay(3000);
    }
}

void vLaunch(void)
{
#define MAIN_TASK_STACK_SIZE (256)
#define MAIN_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
    TaskHandle_t task;

    static StackType_t main_stack[MAIN_TASK_STACK_SIZE];
    static StaticTask_t main_buf;
    task = xTaskCreateStatic(main_task, "MainThread", MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, main_stack, &main_buf);

    // static_assert(configSUPPORT_DYNAMIC_ALLOCATION, "");
    // xTaskCreate(main_task, "MainThread", MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, &task);
    vTaskCoreAffinitySet(task, 1);

    vTaskStartScheduler();
}

int main()
{
    stdio_init_all();

    // Timer example code - This example fires off the callback after 2000ms
    add_alarm_in_ms(2000, alarm_callback, NULL, false);
    // For more examples of timer use see https://github.com/raspberrypi/pico-examples/tree/master/timer

    printf("System Clock Frequency is %d Hz\n", clock_get_hz(clk_sys));
    printf("USB Clock Frequency is %d Hz\n", clock_get_hz(clk_usb));
    // For more examples of clocks use see https://github.com/raspberrypi/pico-examples/tree/master/clocks

    vLaunch();
}
