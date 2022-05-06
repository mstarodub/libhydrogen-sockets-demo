// server.c
// rocket telemetry harness
//
// Copyright (C) 2021 Maxim Starodub <maxim.starodub@aris-space.ch>
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include <err.h>
#include <unistd.h>
#include <stdlib.h>

#include "../libhydrogen/hydrogen.h"
#include "../crypto.h"
#include "../keyfile.h"
#include "../telemetry.h"
#include "../telemutil.h"

void
crypto_kx_server(hydro_kx_session_keypair *kp, int sd)
{
	hydro_kx_keypair server_kp;
	read_kp("./rocket", &server_kp);

	uint8_t client_pk[hydro_kx_PUBLICKEYBYTES];
	read_key("./ground.pub", client_pk, hydro_kx_PUBLICKEYBYTES);

	uint8_t packet1[hydro_kx_KK_PACKET1BYTES];
	uint8_t packet2[hydro_kx_KK_PACKET2BYTES];

	rx(sd, packet1, hydro_kx_KK_PACKET1BYTES);
	// second message, after packet1 has been received
	if (0 != hydro_kx_kk_2(kp, packet2, packet1, client_pk, &server_kp)) {
		errx(EXIT_FAILURE, "key exchange failed");
	}
	tx(sd, packet2, hydro_kx_KK_PACKET2BYTES);
}

int
main(void)
{
	// do not lock / zero memory on server (rocket)
	crypto_init();

	// session_keys.tx is key for sending data to client
	// session_keys.rx is key for receiving data from client
	hydro_kx_session_keypair session_keys;

	int sd = conn_establish_server();
	crypto_kx_server(&session_keys, sd);

	// send messages here
	char buffer[] = "test";
	size_t towrite = 4;
	uint8_t *ciphertext = crypto_enc_msg(&session_keys, buffer, towrite);
	tx(sd, ciphertext, towrite + hydro_secretbox_HEADERBYTES);
	free(ciphertext);

	struct testpacket x = {
			.a = 'x',
			.b = 244,
			.c = {1, 2, 3},
	};
	size_t packetsz = sizeof(x);
	uint8_t *ciphertext2 = crypto_enc_msg(&session_keys, &x, packetsz);
	tx(sd, ciphertext2, packetsz + hydro_secretbox_HEADERBYTES);
	free(ciphertext2);

	close(sd);
}
