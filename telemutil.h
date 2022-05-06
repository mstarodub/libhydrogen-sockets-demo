// telemutil.h
//
// Copyright (C) 2021 Maxim Starodub <maxim.starodub@aris-space.ch>
// SPDX-License-Identifier: MIT

#ifndef _TELEMUTIL_H_
#define _TELEMUTIL_H_

#include <stdint.h>
#include <stddef.h>

void tx(int clientsd, uint8_t *data, size_t len);
void rx(int sd, uint8_t *data, size_t len);
int conn_establish_server(void);
int conn_establish_client(void);

#endif // _TELEMUTIL_H_
