#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <stdint.h>

#define DI_PIN 19
#define DO_PIN 5
#define CLK_PIN 18
#define CS_PIN 22

gpio_config_t io_conf = {
    .pin_bit_mask = (1ULL << DI_PIN) | (1ULL << CLK_PIN) | (1ULL << CS_PIN),
    .mode = GPIO_MODE_OUTPUT,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .pull_up_en = GPIO_PULLDOWN_ENABLE,
    .intr_type = GPIO_INTR_DISABLE
};

gpio_config_t do_io_conf = {
    .pin_bit_mask = (1ULL << DO_PIN),
    .mode = GPIO_MODE_INPUT,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .pull_up_en = GPIO_PULLUP_ENABLE,
    .intr_type = GPIO_INTR_DISABLE
};

static uint8_t SPI_transfer(uint8_t data)
{
    uint8_t received_data = 0;

    for(int i = 7; i >= 0; i--)
    {
        gpio_set_level((gpio_num_t)DI_PIN, (data >> i) & 0x01);
        esp_rom_delay_us(5);
        
        gpio_set_level((gpio_num_t)CLK_PIN, 1);
        esp_rom_delay_us(5);

        if(gpio_get_level((gpio_num_t)DO_PIN))
        {
            received_data |= (1 << i);
        }

        gpio_set_level((gpio_num_t)CLK_PIN, 0);
        esp_rom_delay_us(5);
    }

    return received_data;
}

static void read_flash_data(uint32_t addr, uint8_t *buff, uint8_t len)
{
    // Enables CS
    gpio_set_level((gpio_num_t)CS_PIN, 0);

    esp_rom_delay_us(1);

    // Read command
    SPI_transfer(0x03);

    // Send address
    SPI_transfer((addr >> 16) & 0xFF);
    esp_rom_delay_us(1);

    SPI_transfer((addr >> 8) & 0xFF);
    esp_rom_delay_us(1);
    
    SPI_transfer(addr & 0xFF);
    esp_rom_delay_us(1);

    for(int i = 0; i < len; i++)
    {
        buff[i] = SPI_transfer(0x00);
    }

    esp_rom_delay_us(1);

    // Disables CS
    gpio_set_level((gpio_num_t)CS_PIN, 1);
}

void vTaskCode(void *pvParameters)
{
    gpio_set_level((gpio_num_t)CS_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(2000));

    uint32_t flash_size = 1024 * 1024;
    uint8_t chunk[16];

    for(uint32_t addr = 0; addr < flash_size; addr += 16)
    {   
        read_flash_data(addr, chunk, 16);

        printf("%06X: ", (unsigned int) addr);
        for(int i = 0; i < 16; i++)
        {
            printf("%02X ", chunk[i]);
        }
        printf("\n");

        if (addr % 1024 == 0) {
            vTaskDelay(pdMS_TO_TICKS(5));
        }
    }

    gpio_set_level((gpio_num_t)CS_PIN, 0);
    
    /*
    // JEDEC ID command
    SPI_transfer(0x9F);

    uint8_t m_id = SPI_transfer(0x00);
    uint8_t type = SPI_transfer(0x00);
    uint8_t cap = SPI_transfer(0x00);
    
    gpio_set_level((gpio_num_t)CS_PIN, 1);

    printf("Fabricant ID: %02X, Type: %02X, Capacity: %02X\n", m_id, type, cap);
    */

    printf("Finished!\n");
    vTaskDelete(NULL);
}

void app_main(void)
{
    gpio_config(&io_conf);
    gpio_config(&do_io_conf);

    xTaskCreate(vTaskCode, "DUMP", 4096, NULL, 5, NULL);
}