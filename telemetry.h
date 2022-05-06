// telemetry.h
//
// Copyright (C) 2021 Maxim Starodub <maxim.starodub@aris-space.ch>
// SPDX-License-Identifier: MIT

#ifndef _TELEMETRY_H_
#define _TELEMETRY_H_

#include <stdint.h>

struct testpacket {
	char a;
	uint8_t b;
	uint32_t c[3];
} __attribute__((packed));

#endif // _TELEMETRY_H_
