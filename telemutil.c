// telemutil.c
// helpers for testing send and receive over BSD sockets
//
// Copyright (C) 2021 Maxim Starodub <maxim.starodub@aris-space.ch>
// SPDX-License-Identifier: MIT

#include <arpa/inet.h>
#include <err.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

void
tx(int clientsd, uint8_t *data, size_t len)
{
	size_t written = 0;
	ssize_t w = 0;
	while (written < len) {
		w = write(clientsd, data + written, len - written);
		if (-1 == w) {
			if (errno != EINTR) {
				err(1, "write failed");
			}
		}
		else {
			written += (size_t)w;
		}
	}
}

void
rx(int sd, uint8_t *data, size_t len)
{
	ssize_t r = -1;
	size_t rc = 0;

	while ((0 != r) && rc < len) {
		r = read(sd, data + rc, len - rc);
		if (-1 == r) {
			if (errno != EINTR) {
				err(1, "read failed");
			}
		} else {
			rc += (size_t)r;
		}
	}
}

int
conn_establish_server(void)
{
	int sd, clientsd;
	struct sockaddr_in client, sockname = {
		.sin_family = AF_INET,
		.sin_port = htons(1444),
		.sin_addr.s_addr = htonl(INADDR_ANY)
	};
	unsigned int clientlen = sizeof(&client);
	if (-1 == (sd = socket(AF_INET, SOCK_STREAM, 0))) {
		err(EXIT_FAILURE, "socket failed");
	}
	int option = 1;
	if (-1 == setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option))) {
		err(EXIT_FAILURE, "setsockopt failed");
	}
	if (-1 == bind(sd, (struct sockaddr *)&sockname, sizeof(sockname))) {
		err(EXIT_FAILURE, "bind failed");
	}
	if (-1 == listen(sd, 3)) {
		err(EXIT_FAILURE, "listen failed");
	}
	if (-1 == (clientsd = accept(sd, (struct sockaddr *)&client, &clientlen))) {
		err(1, "accept failed");
	}

	return clientsd;
}

int
conn_establish_client(void)
{
	int sd;
	struct sockaddr_in server_sa = {
		.sin_family = AF_INET,
		.sin_port = htons(1444),
		.sin_addr.s_addr = inet_addr("127.0.0.1")
	};
	if (INADDR_NONE == server_sa.sin_addr.s_addr) {
		errx(EXIT_FAILURE, "invalid addr");
	}
	if (-1 == (sd = socket(AF_INET, SOCK_STREAM, 0))) {
		err(EXIT_FAILURE, "socket failed");
	}
	int option = 1;
	if (-1 == setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option))) {
		err(EXIT_FAILURE, "setsockopt failed");
	}
	if (-1 == connect(sd, (struct sockaddr *)&server_sa, sizeof(server_sa))) {
		err(EXIT_FAILURE, "connect failed");
	}
	return sd;
}
