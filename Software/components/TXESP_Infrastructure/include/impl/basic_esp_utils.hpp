// Copyright (c) 2026 TXCompute. Licensed under the MIT License.

#pragma once
#include "freertos/FreeRTOS.h"
#include "tx/math.h"

namespace tx::esp {
// @param duration ms
inline void delay_impl(u32 duration) {
	vTaskDelay(pdMS_TO_TICKS(duration));
}
}