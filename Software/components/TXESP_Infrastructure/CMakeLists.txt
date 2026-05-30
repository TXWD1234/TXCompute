# Copyright (c) 2026 TXCompute. Licensed under the MIT License.

idf_component_register(
	SRCS
		"include/tx/esp.h"
		"include/impl/ili9488_driver_panel.hpp"
		"include/impl/frame_composer.hpp"
		"include/impl/text_renderer.hpp"
		"include/impl/usb_framework.hpp"
		"include/impl/usb_keyboard_input_handler.hpp"
		"include/impl/buffer.hpp"
	INCLUDE_DIRS
		"include"
	REQUIRES
		"TXLib"
		"driver"
		"esp_driver_gpio"
		"esp_lcd"
		"esp_timer"
		"esp_common"
		"esp_system"
		"espressif__usb"
		"espressif__usb_host_hid"
		"esp_driver_uart"
)