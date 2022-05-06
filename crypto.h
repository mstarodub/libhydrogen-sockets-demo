// crypto.h
//
// Copyright (C) 2021 Maxim Starodub <maxim.starodub@aris-space.ch>
// SPDX-License-Identifier: MIT

#ifndef _CRYPTO_H_
#define _CRYPTO_H_

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "libhydrogen/hydrogen.h"

#define assume(expr)                                                           \
	__extension__({                                                            \
		static_assert(__builtin_constant_p(expr) ? (expr) : true, #expr);      \
		assert(__builtin_choose_expr(__builtin_constant_p(expr), true, expr)); \
	})

void crypto_init(void);
void crypto_lock(void *addr, size_t len);
void crypto_unlock(void *addr, size_t len);
uint8_t *crypto_enc_msg(hydro_kx_session_keypair *kp, void *msg, size_t msg_len);
void *crypto_dec_msg(hydro_kx_session_keypair *kp, uint8_t *ciphertext, size_t ciphertext_len);

static char context[] = "pccrd_rf";

#endif // _CRYPTO_H_
