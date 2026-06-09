// Copyright (c) 2026 TXCompute. Licensed under the MIT License.

#pragma once
#include "driver/gpio.h"

namespace tx::esp {
class PowerManager {
public:
	static void init(gpio_num_t powerPin) {
		m_powerPin = powerPin;

		gpio_config_t io_conf = {
			.pin_bit_mask = (1ULL << m_powerPin),
			.mode = GPIO_MODE_OUTPUT,
			.pull_up_en = GPIO_PULLUP_DISABLE,
			.pull_down_en = GPIO_PULLDOWN_DISABLE,
			.intr_type = GPIO_INTR_DISABLE,
		};
		gpio_config(&io_conf);

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
}; // namespace tx::esp
} // namespace tx::esp