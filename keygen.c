// keygen.c
// generate pre-shared keys for telemetry
//
// Copyright (C) 2021 Maxim Starodub <maxim.starodub@aris-space.ch>
// SPDX-License-Identifier: MIT

#include <err.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crypto.h"
#include "keyfile.h"
#include "libhydrogen/hydrogen.h"
#include "utest.h"

static void debug_print_kp(hydro_kx_keypair *kp);
static void save_kp(char *keyname, hydro_kx_keypair *kp);
static void save_key(char *fname, uint8_t *key, size_t len);
static void usage(void);
static bool test_rw_roundtrip(hydro_kx_keypair *kp);

static void
debug_print_kp(hydro_kx_keypair *kp)
{
	char skbuf[BUFSIZE];
	char pkbuf[BUFSIZE];
	crypto_lock(skbuf, BUFSIZE);
	crypto_lock(pkbuf, BUFSIZE);

	(void)hydro_bin2hex(skbuf, BUFSIZE, kp->sk, hydro_kx_SECRETKEYBYTES);
	(void)hydro_bin2hex(pkbuf, BUFSIZE, kp->pk, hydro_kx_PUBLICKEYBYTES);
	printf("secret: %s\n", skbuf);
	printf("public: %s\n", pkbuf);

	crypto_unlock(skbuf, BUFSIZE);
	crypto_unlock(pkbuf, BUFSIZE);
}

static void
save_kp(char *keyname, hydro_kx_keypair *kp)
{
	char fname[BUFSIZE];

	// private
	snprintf(fname, strlen(keyname) + strlen(".priv") + 1, "%s.priv", keyname);
	save_key(fname, kp->sk, hydro_kx_SECRETKEYBYTES);

	// public
	snprintf(fname, strlen(keyname) + strlen(".pub") + 1, "%s.pub", keyname);
	save_key(fname, kp->pk, hydro_kx_PUBLICKEYBYTES);
}

static void
save_key(char *fname, uint8_t *key, size_t len)
{
	FILE *fout;
	char fbuf[BUFSIZE];
	crypto_lock(fbuf, BUFSIZE);

	fout = fopen(fname, "w");
	if (NULL == fout) {
		err(EXIT_FAILURE, "cannot access keyfile %s", fname);
	}

	(void)hydro_bin2hex(fbuf, BUFSIZE, key, len);
	fprintf(fout, "%s\n", fbuf);
	crypto_unlock(fbuf, BUFSIZE);
	fclose(fout);
}

static void
usage(void)
{
	errx(EXIT_FAILURE, "usage: keygen new|dump|test");
}

static bool
test_rw_roundtrip(hydro_kx_keypair *kp)
{
	hydro_kx_keypair kp_;
	save_kp("test", kp);
	read_kp("test", &kp_);
	return (0 == memcmp(kp->sk, kp_.sk, hydro_kx_SECRETKEYBYTES)
		&& 0 == memcmp(kp->pk, kp_.pk, hydro_kx_PUBLICKEYBYTES));
}

UTEST_STATE();
UTEST(crypto, rw_roundtrip)
{
	hydro_kx_keypair test_kp;
	hydro_kx_keygen(&test_kp);
	ASSERT_TRUE(test_rw_roundtrip(&test_kp));
}

int
main(int argc, char **argv)
{
	assume(sizeof(context) - 1 == hydro_secretbox_CONTEXTBYTES);
	assume(hydro_kx_PUBLICKEYBYTES == hydro_secretbox_KEYBYTES);
	assume(hydro_kx_SECRETKEYBYTES == hydro_secretbox_KEYBYTES);

	crypto_init();

	hydro_kx_keypair ground_kp;
	hydro_kx_keypair rocket_kp;
	crypto_lock(&ground_kp, sizeof(hydro_kx_keypair));
	crypto_lock(&rocket_kp, sizeof(hydro_kx_keypair));

	if (2 != argc) {
		usage();
	}
	if (0 == strcmp(argv[1], "new")) {
		hydro_kx_keygen(&ground_kp);
		hydro_kx_keygen(&rocket_kp);
		save_kp("ground", &ground_kp);
		save_kp("rocket", &rocket_kp);
	}
	else if (0 == strcmp(argv[1], "dump")) {
		read_kp("ground", &ground_kp);
		read_kp("rocket", &rocket_kp);
		puts("GROUND");
		debug_print_kp(&ground_kp);
		puts("ROCKET");
		debug_print_kp(&rocket_kp);
	}
	else if (0 == strcmp(argv[1], "test")) {
		return utest_main(argc, argv);
	}
	else {
		usage();
	}

	crypto_unlock(&ground_kp, sizeof(hydro_kx_keypair));
	crypto_unlock(&rocket_kp, sizeof(hydro_kx_keypair));
}
