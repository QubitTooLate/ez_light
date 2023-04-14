
#include "ez_defines.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"

struct _ez_socket_client_t;
typedef struct _ez_socket_client_t ez_socket_client_t;

public typedef void   (*ez_block_until_fully_connected_to_network_t)(void *const param);
public typedef bool_t (*ez_use_socket_client_t)(ez_socket_client_t *const socket_client, void *const param);

public struct _ez_socket_client_t {
    int socket_handle;
    char* ip;
    uint16_t port;
    ez_block_until_fully_connected_to_network_t blocker;
    void* blocker_param;
    ez_use_socket_client_t socket_client_user;
    void* socket_client_user_param;
};

public ez_socket_client_t* ez_socket_client_inititalize(char *const ip, uint16_t port, ez_block_until_fully_connected_to_network_t blocker, void *const blocker_param, ez_use_socket_client_t socket_client_user, void *const socket_client_user_param);
public void                ez_socket_client_run(ez_socket_client_t *const self);
public bool_t              ez_socket_client_try_send(ez_socket_client_t *const self, void* buffer, uint32_t lenght);
public bool_t              ez_socket_client_try_receive(ez_socket_client_t *const self, void* buffer, uint32_t size, uint32_t *const length);