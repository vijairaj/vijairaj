#include <stdio.h>
#include "rsa.h"
#include "trace.h"


const RSA_METHOD* swRsaMethod = NULL;

int rsa_pub_enc(int flen,const unsigned char *from, unsigned char *to,
			    RSA *rsa,int padding)
{
	int status;

	trace_fn();

	hexdisplay("from:", from, flen);

	status = swRsaMethod->rsa_pub_enc(flen, from, to, rsa, padding);

	hexdisplay("to:", to, RSA_size(rsa));

	return status;
}

int rsa_pub_dec(int flen,const unsigned char *from, unsigned char *to,
				RSA *rsa,int padding)
{
	int status;

	trace_fn();

	hexdisplay("from:", from, flen);

	status = swRsaMethod->rsa_pub_dec(flen, from, to, rsa, padding);

	hexdisplay("to:", to, RSA_size(rsa));

	return status;
}

int rsa_priv_enc(int flen,const unsigned char *from, unsigned char *to,
				 RSA *rsa,int padding)
{
	int status;

	trace_fn();

	hexdisplay("from:", from, flen);

	status = swRsaMethod->rsa_priv_enc(flen, from, to, rsa, padding);

	hexdisplay("to:", to, RSA_size(rsa));

	return status;
}

int rsa_priv_dec(int flen,const unsigned char *from, unsigned char *to,
				 RSA *rsa,int padding)
{
	int status;

	trace_fn();

	hexdisplay("from:", from, flen);

	status = swRsaMethod->rsa_priv_dec(flen, from, to, rsa, padding);

	hexdisplay("to:", to, RSA_size(rsa));

	return status;
}

int rsa_mod_exp(BIGNUM *r0,const BIGNUM *I,RSA *rsa,BN_CTX *ctx)
{
	int status;

	trace_fn();

	status = swRsaMethod->rsa_mod_exp(r0, I, rsa, ctx);

	return status;
}

int rsa_bn_mod_exp(BIGNUM *r, const BIGNUM *a, const BIGNUM *p, const BIGNUM *m,
			   BN_CTX *ctx, BN_MONT_CTX *m_ctx)
{
	int status;

	trace_fn();

	status = swRsaMethod->bn_mod_exp(r, a, p, m, ctx, m_ctx);

	return status;
}

int rsa_init(RSA *rsa)
{
	trace_fn();

	swRsaMethod = RSA_PKCS1_SSLeay();

	if (swRsaMethod) {
		return 1;
	} else {
		return 0;
	}
}

int rsa_finish(RSA *rsa)
{
	trace_fn();
	return 1;
}

int rsa_sign(int type, const unsigned char *m, unsigned int m_length,
		     unsigned char *sigret, unsigned int *siglen, const RSA *rsa)
{
	int status = 1;

	trace_fn();

	hexdisplay("m:", m, m_length);

	// status =  my_rsa_sign(type, m, m_length, sigret, siglen, rsa);
	
	hexdisplay("sigret:", sigret, *siglen);

	return status;
}

int rsa_verify(int dtype, const unsigned char *m, unsigned int m_length,
		       unsigned char *sigbuf, unsigned int siglen, const RSA *rsa)
{
	trace_fn();
	return 1;
}

int rsa_keygen(RSA *rsa, int bits, BIGNUM *e, BN_GENCB *cb)
{
	trace_fn();
	return 1;
}


