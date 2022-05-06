// client.c
// ground station telemetry harness
//
// Copyright (C) 2021 Maxim Starodub <maxim.starodub@aris-space.ch>
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h>

#include "../libhydrogen/hydrogen.h"
#include "../crypto.h"
#include "../keyfile.h"
#include "../telemetry.h"
#include "../telemutil.h"

void
crypto_kx_client(hydro_kx_session_keypair *kp, int sd)
{
	hydro_kx_state st_client;

	hydro_kx_keypair client_kp;
	crypto_lock(&client_kp, sizeof(hydro_kx_keypair));
	read_kp("./ground", &client_kp);

	uint8_t server_pk[hydro_kx_PUBLICKEYBYTES];
	read_key("./rocket.pub", server_pk, hydro_kx_PUBLICKEYBYTES);

	uint8_t packet1[hydro_kx_KK_PACKET1BYTES];
	uint8_t packet2[hydro_kx_KK_PACKET2BYTES];

	// first message, initiate connection
	hydro_kx_kk_1(&st_client, packet1, server_pk, &client_kp);
	tx(sd, packet1, hydro_kx_KK_PACKET1BYTES);

	rx(sd, packet2, hydro_kx_KK_PACKET2BYTES);
	// third message, after packet2 has been received
	if (0 != hydro_kx_kk_3(&st_client, kp, packet2, &client_kp)) {
		errx(EXIT_FAILURE, "key exchange failed");
	}

	crypto_unlock(&client_kp, sizeof(hydro_kx_keypair));
}

int
main(void)
{
	crypto_init();

	// session_keys.tx is key for sending data to server
	// session_keys.rx is key for receiving data from server
	hydro_kx_session_keypair session_keys;
	crypto_lock(&session_keys, sizeof(hydro_kx_session_keypair));

	int sd = conn_establish_client();
	crypto_kx_client(&session_keys, sd);

	// send messages here
	uint8_t buffer[4 + hydro_secretbox_HEADERBYTES];
	size_t len = sizeof(buffer);
	rx(sd, buffer, len);
	char *decrypted = crypto_dec_msg(&session_keys, buffer, len);
	fwrite(decrypted, sizeof(char), 4, stdout);
	free(decrypted);

	uint8_t buffer2[sizeof(struct testpacket) + hydro_secretbox_HEADERBYTES];
	size_t len2 = sizeof(buffer2);
	rx(sd, buffer2, len2);
	struct testpacket *decrypted2 = crypto_dec_msg(&session_keys, buffer2, len2);
	// for testing
	printf("\na: %c\nb: %u\nc1: %u, c2: %u, c3: %u\n", decrypted2->a,
		decrypted2->b, decrypted2->c[0], decrypted2->c[1], decrypted2->c[2]);
	free(decrypted2);

	close(sd);
	crypto_unlock(&session_keys, sizeof(hydro_kx_session_keypair));
}
