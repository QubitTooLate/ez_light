
#include "ez_system.h"

public_static void ez_system_initialize()
{
    ez_assert(nvs_flash_init());
    ez_assert(esp_netif_init());
    ez_assert(esp_event_loop_create_default());
}
