#include "ez_system.h"
#include "ez_wifi_station.h"
#include "ez_socket_client.h"

#include "driver/gpio.h"

private_static uint8_t     ID[]     = { };
private_static uint32_t    GUID_LENGTH = 16;

private_static char *const SSID     = "";
private_static char *const PASSWORD = "";

private_static char *const IP       = "";
private_static uint16_t    PORT     = 42069;
private_static uint16_t    POLL     = 100;

private_static uint16_t    SIZE     = 128;

private_static char *const TAG      = "[PROGRAM]";

private_static gpio_num_t  PIN      = GPIO_NUM_2;

private_static bool_t _socket_client_is_connected = false;

private_static void block_until_fully_connected_to_network(void *const param)
{
    ez_log("waiting to be fully connected");

    ez_wifi_station_t *const wifi_station = (ez_wifi_station_t *const)param;
    ez_wifi_station_block_until_fully_connected(wifi_station);
}

private_static bool_t try_use_socket_client(ez_socket_client_t *const socket_client, void *const param)
{
    uint32_t length = 0;

    if (_socket_client_is_connected != true) // handshake
    {
        ez_log("sending handshake");
        if (ez_socket_client_try_send(socket_client, ID, GUID_LENGTH) != true) { return false; }

        ez_log("receiving handshake");
        if (ez_socket_client_try_receive(socket_client, param, SIZE, &length) != true) { return false; }

        if (length != 2 || ez_compare(param, length, "OK", 2) != true) { return false; }

        ez_log("hanshake succes");

        _socket_client_is_connected = true;
        return true;
    }

    ez_log("receiving command");
    if (ez_socket_client_try_receive(socket_client, param, SIZE, &length) != true) 
    { 
        _socket_client_is_connected = false; 
        return false; 
    }

    if (length == 2 && ez_compare(param, length, "ON", 2))
    {
        gpio_set_level(PIN, 0);
        
        ez_log("light set on");
        if (ez_socket_client_try_send(socket_client, "OK", 2) != true)
        { 
            _socket_client_is_connected = false; 
            return false; 
        }

        return true;
    }

    if (length == 3 && ez_compare(param, length, "OFF", 3))
    {
        gpio_set_level(PIN, 1);
        
        ez_log("light set off");
        if (ez_socket_client_try_send(socket_client, "OK", 2) != true)
        { 
            _socket_client_is_connected = false; 
            return false; 
        }
        
        return true;
    }
    
    ez_log("sending error response");
    if (ez_socket_client_try_send(socket_client, "ERROR", 5) != true)
    { 
        _socket_client_is_connected = false; 
        return false; 
    }

    vTaskDelay(POLL / portTICK_PERIOD_MS);
    return true;
}

void app_main(void)
{
    gpio_reset_pin(PIN);
    gpio_set_direction(PIN, GPIO_MODE_OUTPUT);

    ez_system_initialize();

    ez_wifi_station_t *const wifi_station = ez_wifi_station_initialize(SSID, PASSWORD, POLL);
    ez_wifi_station_run(wifi_station);

    void* buffer = malloc(SIZE);

    ez_socket_client_t *const socket_client = ez_socket_client_inititalize(
        IP, PORT, 
        &block_until_fully_connected_to_network, wifi_station,
        &try_use_socket_client, buffer
    );

    ez_socket_client_run(socket_client);
}
