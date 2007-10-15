// $Id$

#ifndef _MY__RSA_H_
#define _MY__RSA_H_

#include <openssl/crypto.h>
#include <openssl/dso.h>
#include <openssl/des.h>
#include <openssl/engine.h>


int rsa_pub_enc(int flen,const unsigned char *from, unsigned char *to, RSA *rsa,int padding);
int rsa_pub_dec(int flen,const unsigned char *from, unsigned char *to, RSA *rsa,int padding);
int rsa_priv_enc(int flen,const unsigned char *from, unsigned char *to, RSA *rsa,int padding);
int rsa_priv_dec(int flen,const unsigned char *from, unsigned char *to, RSA *rsa,int padding);
int rsa_mod_exp(BIGNUM *r0,const BIGNUM *I,RSA *rsa,BN_CTX *ctx);
int rsa_bn_mod_exp(BIGNUM *r, const BIGNUM *a, const BIGNUM *p, const BIGNUM *m, BN_CTX *ctx, BN_MONT_CTX *m_ctx);
int rsa_init(RSA *rsa);
int rsa_finish(RSA *rsa);
int rsa_sign(int type, const unsigned char *m, unsigned int m_length, unsigned char *sigret, unsigned int *siglen, const RSA *rsa);
int rsa_verify(int dtype, const unsigned char *m, unsigned int m_length, unsigned char *sigbuf, unsigned int siglen, const RSA *rsa);
int rsa_keygen(RSA *rsa, int bits, BIGNUM *e, BN_GENCB *cb);

#endif

