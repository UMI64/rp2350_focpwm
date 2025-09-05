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
    vTaskDelay(pdMS_TO_TICKS(10000));
    focpwm pwm(25, 26, 27, 10000, 10000); // 10KHz 0.1ms 10K count, 0.1 * 10000 = 1000ms
    pwm.set_duty(5000, 2000, 3000);
    pwm.set_pin(25, 1);
    vTaskDelay(pdMS_TO_TICKS(3000));
    pwm.set_pin(25, 0);
    vTaskDelay(pdMS_TO_TICKS(3000));
    pwm.set_pin(25, 1);
    vTaskDelay(pdMS_TO_TICKS(3000));
    pwm.set_pin(25, 0);
    vTaskDelay(pdMS_TO_TICKS(3000));
    pwm.start();
    auto printinfo = [&](focpwm::pwm_hard_t &pwm_hard)
    {
        printf("slice %lu channel %lu div %0.6f, count %lu\n", pwm_hard.slice_num, pwm_hard.channel_num, pwm_hard.div, pwm_hard.count);
    };
    while (true)
    {
        printinfo(pwm.pwm_hard_a);
        printinfo(pwm.pwm_hard_b);
        printinfo(pwm.pwm_hard_c);
        printf("Hello, world!\n");
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
