#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "esp_log.h"

#define public_static /* public static */
#define private_static /* private static */ static
#define public /* public */
#define private /* private */ static
#define class typedef struct 
#define bool_t int8_t
#define true 1
#define false 0
#define null 0
#define ez_log(message) ESP_LOGI(TAG, message)
#define ez_assert(result) ESP_ERROR_CHECK(result)
#define ez_zero(t) memset(&t, 0, sizeof(t))
#define ez_string_copy(destination, source) strcpy((char*)(destination), (char*)(source))
#define ez_new(t) (t*)calloc(sizeof(t), 1);
#define ez_compare(a, a_length, b, b_length) (a_length == b_length && memcmp(a, b, a_length) == 0)