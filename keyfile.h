// keyfile.h
//
// Copyright (C) 2021 Maxim Starodub <maxim.starodub@aris-space.ch>
// SPDX-License-Identifier: MIT

#ifndef _KEYFILE_H_
#define _KEYFILE_H_

#include <stdint.h>
#include <stddef.h>

#include "libhydrogen/hydrogen.h"

void read_kp(char *keyname, hydro_kx_keypair *kp);
void read_key(char *fname, uint8_t *buf, size_t len);

enum { BUFSIZE = 500, };

#endif // _KEYFILE_H_
