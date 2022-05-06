// crypto.c
// libhydro helpers
//
// Copyright (C) 2021 Maxim Starodub <maxim.starodub@aris-space.ch>
// SPDX-License-Identifier: MIT

#include <err.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "crypto.h"
#include "libhydrogen/hydrogen.h"

void
crypto_init(void)
{
	if (0 != hydro_init()) {
		errx(EXIT_FAILURE, "unable to initialise libhydrogen crypto");
	}
}

void
crypto_lock(void *addr, size_t len)
{
	if (0 != mlock(addr, len)) {
		err(EXIT_FAILURE, "unable to lock memory");
	}
}

void
crypto_unlock(void *addr, size_t len)
{
	hydro_memzero(addr, len);
	if (0 != munlock(addr, len)) {
		err(EXIT_FAILURE, "unable to unlock memory");
	}
}

uint8_t *
crypto_enc_msg(hydro_kx_session_keypair *kp, void *msg, size_t msg_len)
{
	uint8_t *ciphertext = malloc(msg_len + hydro_secretbox_HEADERBYTES);
	if (NULL == ciphertext) {
		errx(EXIT_FAILURE, "encrypting failed, no buffer");
	}
	hydro_secretbox_encrypt(ciphertext, msg, msg_len, 0, context, kp->tx);
	return ciphertext;
}

void *
crypto_dec_msg(hydro_kx_session_keypair *kp, uint8_t *ciphertext, size_t ciphertext_len)
{
	char *decrypted = malloc(ciphertext_len - hydro_secretbox_HEADERBYTES);
	if (NULL == decrypted) {
		errx(EXIT_FAILURE, "decrypting failed, no buffer");
	}
	if (0 != hydro_secretbox_decrypt(decrypted, ciphertext, ciphertext_len, 0, context, kp->rx)) {
		errx(EXIT_FAILURE, "decrypting failed, message forged");
	}
	return decrypted;
}
