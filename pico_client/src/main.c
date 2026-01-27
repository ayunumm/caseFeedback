#include "wifi.h"
#include "lcd.h"
#include "tcp_communication.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include <stdio.h>

void init_system(void);
int read_buttons(void);
void update_lcd(int rating);

int main() {
    init_system(); 
    
    while(1) {
        int rating = read_buttons();

        if (rating > 0) {
            printf("Rating registered: %d\n", rating);

             // 2. Send rating to database/ThingSpeak
            pico_send_to_thingspeak("BZ3WIHTNRIFABJUO", rating);
             // 3. Update status on LCD Screen
            update_lcd(rating);
            
             // Debounce delay to avoid registering the same press multiple times
            sleep_ms(30000); 
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
    lcd_print("System Ready");
    lcd_set_cursor(1, 0);

    wifi_init();
}
