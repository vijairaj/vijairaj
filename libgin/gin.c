// $Id$

#include <stdio.h>
#include <openssl/crypto.h>
#include <openssl/dso.h>
#include <openssl/des.h>
#include <openssl/engine.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "rsa.h"
#include "trace.h"

static const char engine_id[] = "gin";
static const char engine_description[] = "Sample dynamic engine support";

#if 0
static int rand_bytes(unsigned char *buf, int num)
{
	trace("%s(%d)\n", __func__, num);

	int fd = open("/dev/urandom", O_RDONLY);

	if (fd < 0) {
		return 0;
	} else {
		read(fd, buf, num);
	}

	return 1;
}

static RAND_METHOD rand_method =
{
	NULL,			/* seed */
	rand_bytes,		/* bytes */
	NULL,			/* cleanup */
	NULL,			/* add */
	rand_bytes,		/* pseudorand */
	NULL,			/* status */
};	
#endif

static RSA_METHOD rsa_method =
{
	.name = "SAM implementation of RSA",
	.rsa_pub_enc = rsa_pub_enc,
	.rsa_pub_dec = rsa_pub_dec,
	.rsa_priv_enc = rsa_priv_enc,
	.rsa_priv_dec = rsa_priv_dec,
	.rsa_mod_exp = rsa_mod_exp,
	.bn_mod_exp = rsa_bn_mod_exp,
	.init = rsa_init,
	.finish = rsa_finish,
	.flags = RSA_METHOD_FLAG_NO_CHECK,
	.app_data = NULL,
	.rsa_sign = NULL, // rsa_sign,
	.rsa_verify = NULL, // rsa_verify,
	.rsa_keygen = rsa_keygen
};

static int bind_supported_methods(ENGINE *engine)
{
	if(ENGINE_set_id(engine, engine_id)
			&& ENGINE_set_name(engine, engine_description)
			&& ENGINE_set_RSA(engine, &rsa_method)
	//		&& ENGINE_set_RAND(engine, &rand_method)
	) {
		trace("Engine loaded successfully\n");
		return 1;
	} else {
		trace("Engine failed to load\n");
		return 0;
	}
}
	
static int bind_fn(ENGINE *engine, const char *id)
{
	int status = 1;

	trace_fn();

	do {
	#if 0
		if(id == NULL || strcmp(id, engine_id) != 0) {
			trace("Wrong engine id\n");
			status = 0;
			break;
		}
	#endif

		if(! bind_supported_methods(engine)) {
			trace("Can't bind methods\n");
			return 0;
		}

		trace("Engine successfully initialized\n");		
	} while (0);

	return status;
}

IMPLEMENT_DYNAMIC_CHECK_FN()

IMPLEMENT_DYNAMIC_BIND_FN(bind_fn)

