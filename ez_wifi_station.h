#pragma once

#include "ez_defines.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"

public class {
    uint8_t ssid[32];
    uint8_t password[64];
    bool_t  is_fully_connected;
    uint16_t poll;
} ez_wifi_station_t;

public ez_wifi_station_t* ez_wifi_station_initialize(char *const ssid, char *const password, uint16_t poll);
public void               ez_wifi_station_run(ez_wifi_station_t *const self);
public bool_t             ez_wifi_station_is_fully_connected(ez_wifi_station_t *const self);
public void               ez_wifi_station_block_until_fully_connected(ez_wifi_station_t *const self);