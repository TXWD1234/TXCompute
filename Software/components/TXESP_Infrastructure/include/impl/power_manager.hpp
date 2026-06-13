// Copyright (c) 2026 TXCompute. Licensed under the MIT License.

#pragma once
#include "driver/gpio.h"
#include "esp_timer.h"

namespace tx::esp {
class PowerManager {
public:
	static void init(gpio_num_t powerPin, gpio_num_t signalPin) {
		// init pins
		m_powerPin = powerPin;
		m_signalPin = signalPin;

		gpio_config_t conf_power = {
			.pin_bit_mask = (1ULL << m_powerPin),
			.mode = GPIO_MODE_OUTPUT,
			.pull_up_en = GPIO_PULLUP_DISABLE,
			.pull_down_en = GPIO_PULLDOWN_DISABLE,
			.intr_type = GPIO_INTR_DISABLE,
		};
		gpio_config(&conf_power);

		gpio_config_t conf_signal = {
			.pin_bit_mask = (1ULL << m_signalPin),
			.mode = GPIO_MODE_INPUT,
			.pull_up_en = GPIO_PULLUP_ENABLE,
			.pull_down_en = GPIO_PULLDOWN_DISABLE,
			.intr_type = GPIO_INTR_DISABLE,
		};
		gpio_config(&conf_signal);

		// init power thread
		// xTaskCreate(

		// )

		power_off();
	}

	static void power_on() {
		gpio_set_level(m_powerPin, 1);
	}
	static void power_off() {
		gpio_set_level(m_powerPin, 0);
	}

private:
	inline static gpio_num_t m_powerPin = GPIO_NUM_MAX;
	inline static gpio_num_t m_signalPin = GPIO_NUM_MAX;

	static void powerThread_impl() {
		while (true) {
			bool pressed = gpio_get_level(m_signalPin);
			if ()
		}
	}


}; // namespace tx::esp
} // namespace tx::esp