#pragma once

#include "hardware/adc.h"
#include "pico/stdlib.h"
#include <cstdint>

class Joystick {
public:
    Joystick(uint x_gpio, uint y_gpio, uint sw_gpio)
        : x_gpio_(x_gpio), y_gpio_(y_gpio), sw_gpio_(sw_gpio) {}

    void init() {
        adc_init();

        adc_gpio_init(x_gpio_);
        adc_gpio_init(y_gpio_);

        gpio_init(sw_gpio_);
        gpio_set_dir(sw_gpio_, GPIO_IN);
        gpio_pull_up(sw_gpio_);
    }

    uint16_t xRaw() const {
        return readAdcGpio(x_gpio_);
    }

    uint16_t yRaw() const {
        return readAdcGpio(y_gpio_);
    }

    bool pressed() const {
        return gpio_get(sw_gpio_) == 0;
    }

    int xCentered() const {
        return static_cast<int>(xRaw()) - center_;
    }

    int yCentered() const {
        return static_cast<int>(yRaw()) - center_;
    }

private:
    static constexpr int center_ = 2048; // Pico ADC is 12-bit: 0..4095

    uint x_gpio_;
    uint y_gpio_;
    uint sw_gpio_;

    static uint16_t readAdcGpio(uint gpio) {
        uint channel = gpio - 26; // GP26=ADC0, GP27=ADC1, GP28=ADC2
        adc_select_input(channel);
        return adc_read();
    }
};