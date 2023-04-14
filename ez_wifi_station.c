
#include "ez_wifi_station.h"

private        void ez_wifi_station_set_event_handlers(ez_wifi_station_t *const self);
private_static void ez_wifi_station_sta_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
private_static void ez_wifi_station_ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

public ez_wifi_station_t* ez_wifi_station_initialize(char *const ssid, char *const password, uint16_t poll)
{
    ez_wifi_station_t* self = ez_new(ez_wifi_station_t);
    ez_string_copy(self->ssid, ssid);
    ez_string_copy(self->password, password);
    self->poll = poll;
    return self;
}

public void ez_wifi_station_run(ez_wifi_station_t *const self)
{
    esp_netif_create_default_wifi_sta();

    ez_wifi_station_set_event_handlers(self);

    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    ez_assert(esp_wifi_init(&wifi_init_config));

    wifi_config_t wifi_config;
    ez_zero(wifi_config);
    ez_string_copy(wifi_config.sta.ssid, self->ssid);
    ez_string_copy(wifi_config.sta.password, self->password);
    wifi_config.sta.pmf_cfg.capable = true;
    wifi_config.sta.pmf_cfg.required = false;
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    ez_assert(esp_wifi_set_mode(WIFI_MODE_STA));
    ez_assert(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ez_assert(esp_wifi_start());
}

public bool_t ez_wifi_station_is_fully_connected(ez_wifi_station_t *const self)
{
    return self->is_fully_connected;
}

public void ez_wifi_station_block_until_fully_connected(ez_wifi_station_t *const self)
{
    while (self->is_fully_connected != true)
    {
        vTaskDelay(self->poll / portTICK_PERIOD_MS);
    }
}

private_static void ez_wifi_station_set_event_handlers(ez_wifi_station_t *const self)
{
    ez_assert(esp_event_handler_instance_register(
        WIFI_EVENT,
        WIFI_EVENT_STA_START,
        &ez_wifi_station_sta_event_handler,
        self,
        null
    ));

    ez_assert(esp_event_handler_instance_register(
        WIFI_EVENT,
        WIFI_EVENT_STA_DISCONNECTED,
        &ez_wifi_station_sta_event_handler,
        self,
        null
    ));

    ez_assert(esp_event_handler_instance_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &ez_wifi_station_ip_event_handler,
        self,
        null
    ));
}

private_static void ez_wifi_station_sta_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ez_wifi_station_t *const self = arg;

    if (event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        self->is_fully_connected = false;
    }

    if (event_id == WIFI_EVENT_STA_START || event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        ez_assert(esp_wifi_connect());
    }
}

private_static void ez_wifi_station_ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ez_wifi_station_t *const self = arg;
    self->is_fully_connected = true;
}