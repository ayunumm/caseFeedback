#include "wifi.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "lcd.h"
#include "communication.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

void init_system(void);
int read_buttons(void);
void update_lcd(int rating);

int main() {
    sleep_ms(3000); 
    init_system(); 
    
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("How was the");
    lcd_set_cursor(1, 0);
    lcd_print("lession? :)");

    int rating = 0;
    while(1) {
        rating = read_buttons();
        if (rating > 0) {
            printf("Rating registered: %d\n", rating);
            send_to_thingspeak(rating);
            update_lcd(rating);
            absolute_time_t unlock_time = delayed_by_us(get_absolute_time(), 27 * 1000000);
            while(absolute_time_diff_us(get_absolute_time(), unlock_time) > 0) {
                int64_t remaining_time = absolute_time_diff_us(get_absolute_time(), unlock_time);
                int remaining_seconds = remaining_time / 1000000;

                lcd_clear();
                lcd_set_cursor(0, 0);

                char buf[16];
                snprintf(buf, sizeof(buf), "Please wait: %d sec...", remaining_seconds);
                lcd_print(buf);

                sleep_ms(1000);
            }

            lcd_clear();
            lcd_set_cursor(0, 0);
            lcd_print("Next person!");
        } 

        sleep_ms(50); // Save power/CPU cycles
    }
    return 0;
}

int read_buttons() {
    if (!gpio_get(6)) return 1; 
    if (!gpio_get(7)) return 2;
    if (!gpio_get(8)) return 3;

    return 0;
}

void update_lcd(int rating) {
    lcd_clear();
    lcd_set_cursor(0, 0);

    if (rating == 1) {
        lcd_print("Bad");
    } else if (rating == 2){
        lcd_print("OK");
    } else if (rating == 3) {
        lcd_print("Good");
    }

    lcd_set_cursor(1, 0);
    lcd_print("Thank you!");

    sleep_ms(3000);
    lcd_clear();
    lcd_set_cursor(0, 0);
}

void init_system() {
    stdio_init_all();

    gpio_init(6);
    gpio_set_dir(6, GPIO_IN);
    gpio_pull_up(6);

    gpio_init(7);
    gpio_set_dir(7, GPIO_IN);
    gpio_pull_up(7);

    gpio_init(8);
    gpio_set_dir(8, GPIO_IN);
    gpio_pull_up(8);

    i2c_init(i2c0, 400000); 
    gpio_set_function(0, GPIO_FUNC_I2C); 
    gpio_set_function(1, GPIO_FUNC_I2C); 
    gpio_pull_up(0);
    gpio_pull_up(1);

    lcd_init();
    lcd_clear();
    lcd_set_cursor(0, 0);

    wifi_init();
    lcd_print("System Ready");
}
