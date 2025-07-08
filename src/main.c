#include "esp_log.h"
#include "esp_task.h"
#include "stdbool.h"
#include "driver/gpio.h"

#define RED_PIN GPIO_NUM_25
#define YELLOW_PIN GPIO_NUM_26
#define GREEN_PIN GPIO_NUM_27

#define HIGH 1
#define LOW 0

#define SCALE 1

typedef enum {
    RED,
    YELLOW,
    GREEN
} LightState;

LightState curr_state = RED;
LightState prev_state = RED;

static void register_led(short pin){
    gpio_config_t gpio_pin = {
        .intr_type = GPIO_INTR_DISABLE,
        .pin_bit_mask = 1ULL << pin,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .mode = GPIO_MODE_OUTPUT
    };

    gpio_config(&gpio_pin);
}

void blink_green(){
    gpio_set_level(GREEN_PIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(1000*SCALE));
    gpio_set_level(GREEN_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(1000*SCALE));
}

void green_blink_controller(){
    short blink = 0;
    while(blink < 3){
        blink_green();
        blink++;
    }
}

void app_main() {
    register_led(RED_PIN);
    register_led(YELLOW_PIN);
    register_led(GREEN_PIN);

    while(1){
        switch(curr_state){
            case RED:
                prev_state = RED;
                curr_state = YELLOW;
                gpio_set_level(RED_PIN, HIGH);
                gpio_set_level(YELLOW_PIN, LOW);
                gpio_set_level(GREEN_PIN, LOW);
                vTaskDelay(pdMS_TO_TICKS(5000*SCALE));
                break;
            case YELLOW:
                curr_state = (prev_state == RED) ? GREEN : RED;
                prev_state = YELLOW;
                gpio_set_level(RED_PIN, LOW);
                gpio_set_level(YELLOW_PIN, HIGH);
                gpio_set_level(GREEN_PIN, LOW);
                vTaskDelay(pdMS_TO_TICKS(2000*SCALE));
                break;
            case GREEN:
                curr_state = YELLOW;
                prev_state = GREEN;
                gpio_set_level(RED_PIN, LOW);
                gpio_set_level(YELLOW_PIN, LOW);
                gpio_set_level(GREEN_PIN, HIGH);
                vTaskDelay(pdMS_TO_TICKS(5000*SCALE));
                green_blink_controller();
                break;
            default:
                prev_state = RED;
                curr_state = YELLOW;
                gpio_set_level(RED_PIN, HIGH);
                gpio_set_level(YELLOW_PIN, LOW);
                gpio_set_level(GREEN_PIN, LOW);
                break;

        }
    }
    return;
}