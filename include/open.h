#ifndef OPEN_H
#define OPEN_H

#include <arpa/inet.h>

int open_network_socket_client(const char *address, in_port_t port, int *err);
int open_network_socket_server(const char *address, in_port_t port, int backlog, int *err);
int accept_connection(int server_fd, int *err);

#endif    // OPEN_H
