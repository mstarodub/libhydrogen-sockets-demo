// keyfile.c
// helpers for reading keys from files
//
// Copyright (C) 2021 Maxim Starodub <maxim.starodub@aris-space.ch>
// SPDX-License-Identifier: MIT

#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crypto.h"
#include "keyfile.h"
#include "libhydrogen/hydrogen.h"

void
read_kp(char *keyname, hydro_kx_keypair *kp)
{
	char fname[BUFSIZE];

	// private
	snprintf(fname, strlen(keyname) + strlen(".priv") + 1, "%s.priv", keyname);
	read_key(fname, kp->sk, hydro_kx_SECRETKEYBYTES);

	// public
	snprintf(fname, strlen(keyname) + strlen(".pub") + 1, "%s.pub", keyname);
	read_key(fname, kp->pk, hydro_kx_PUBLICKEYBYTES);
}

void
read_key(char *fname, uint8_t *key, size_t len)
{
	char buf[BUFSIZE];
	crypto_lock(buf, BUFSIZE);

	FILE *fin = fopen(fname, "r");
	if (NULL == fin) {
		err(EXIT_FAILURE, "cannot open keyfile %s", fname);
	}
	if (-1 == fseek(fin, 0, SEEK_END)) {
		err(EXIT_FAILURE, "cannot process keyfile %s", fname);
	}
	int flen = (int)ftell(fin);
	if (-1 == flen) {
		err(EXIT_FAILURE, "cannot process keyfile %s", fname);
	}
	// contains a newline, two hex bytes per uint8_t
	if (len != (size_t)((flen - 1) / 2) || flen > BUFSIZE) {
		errx(EXIT_FAILURE, "incomplete or malformed keyfile %s", fname);
	};
	if (-1 == fseek(fin, 0, SEEK_SET)) {
		err(EXIT_FAILURE, "cannot process keyfile %s", fname);
	}
	if (flen != (int)fread(buf, 1, flen, fin) && 0 != ferror(fin)) {
		err(EXIT_FAILURE, "cannot read keyfile %s", fname);
	}
	if (-1 == hydro_hex2bin(key, len, buf, len*2, "", NULL)) {
		errx(EXIT_FAILURE, "incomplete or malformed keyfile %s", fname);
	}
	crypto_unlock(buf, BUFSIZE);
	if (0 != fclose(fin)) {
		err(EXIT_FAILURE, "cannot close keyfile %s", fname);
	}
}
