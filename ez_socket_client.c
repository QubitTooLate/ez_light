
#include "ez_socket_client.h"

typedef void* (*ez_func_t)(ez_socket_client_t *const self);

private_static void* ez_socket_client_allocate(ez_socket_client_t *const self);
private_static void* ez_socket_client_connect(ez_socket_client_t *const self);
private_static void* ez_socket_client_release(ez_socket_client_t *const self);
private_static void* ez_socket_client_communicate(ez_socket_client_t *const self);

public ez_socket_client_t* ez_socket_client_inititalize(char *const ip, uint16_t port, ez_block_until_fully_connected_to_network_t blocker, void *const blocker_param, ez_use_socket_client_t socket_client_user, void *const socket_client_user_param)
{
    ez_socket_client_t* self = ez_new(ez_socket_client_t);
    self->ip = ip;
    self->port = port;
    self->blocker = blocker;
    self->blocker_param = blocker_param;
    self->socket_client_user = socket_client_user;
    self->socket_client_user_param = socket_client_user_param;
    return self;
}

public void ez_socket_client_run(ez_socket_client_t *const self)
{
    ez_func_t func = (ez_func_t)&ez_socket_client_allocate;
    while (true) 
    {
        func = (ez_func_t)func(self);
    }
}

public bool_t ez_socket_client_try_send(ez_socket_client_t *const self, void* buffer, uint32_t length)
{
    int result = send(self->socket_handle, buffer, length, 0);

    if (result < length) { return false; }

    return true;
}

public bool_t ez_socket_client_try_receive(ez_socket_client_t *const self, void* buffer, uint32_t size, uint32_t* const length)
{
    *length = recv(self->socket_handle, buffer, size, 0);

    if (*length <= 0) { return false; }

    return true;
}

private_static void* ez_socket_client_allocate(ez_socket_client_t *const self)
{
    self->socket_handle = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    if (self->socket_handle < 0) { return &ez_socket_client_release; }

    return &ez_socket_client_connect;
}

private_static void* ez_socket_client_connect(ez_socket_client_t *const self)
{
    if (self->blocker != null) 
    {
        self->blocker(self->blocker_param);
    }

    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(self->ip);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(self->port);
    int result = connect(self->socket_handle, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in6));

    if (result != 0) { return &ez_socket_client_release; }

    return &ez_socket_client_communicate;
}

private_static void* ez_socket_client_release(ez_socket_client_t *const self)
{
    if (self->socket_handle >= 0)
    {
        shutdown(self->socket_handle, 0);
        close(self->socket_handle);
    }

    return &ez_socket_client_allocate;
}

private_static void* ez_socket_client_communicate(ez_socket_client_t *const self)
{
    if (self->socket_client_user(self, self->socket_client_user_param) != true) { return &ez_socket_client_release; }

    return &ez_socket_client_communicate;
}