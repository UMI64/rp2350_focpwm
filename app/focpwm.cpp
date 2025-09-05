#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "FreeRTOS.h"
#include "task.h"

#include "blink.pio.h"

void blink_pin_forever(PIO pio, uint sm, uint offset, uint pin, uint freq)
{
    blink_program_init(pio, sm, offset, pin);
    pio_sm_set_enabled(pio, sm, true);

    printf("Blinking pin %d at %d Hz\n", pin, freq);

    // PIO counter program takes 3 more cycles in total than we pass as
    // input (wait for n + 1; mov; jmp)
    pio->txf[sm] = (125000000 / (2 * freq)) - 3;
}

int64_t alarm_callback(alarm_id_t id, void *user_data)
{
    // Put your timeout handler code in here
    return 0;
}

void main_task(__unused void *params)
{
    while (true)
    {
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

    // PIO Blinking example
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &blink_program);
    printf("Loaded program at %d\n", offset);
    blink_pin_forever(pio, 0, offset, 6, 3);
    // Timer example code - This example fires off the callback after 2000ms
    add_alarm_in_ms(2000, alarm_callback, NULL, false);
    // For more examples of timer use see https://github.com/raspberrypi/pico-examples/tree/master/timer

    printf("System Clock Frequency is %d Hz\n", clock_get_hz(clk_sys));
    printf("USB Clock Frequency is %d Hz\n", clock_get_hz(clk_usb));
    // For more examples of clocks use see https://github.com/raspberrypi/pico-examples/tree/master/clocks

    vLaunch();
}
