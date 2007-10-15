// $Id$

#include <stdio.h>
#include <unistd.h>
#include <netdb.h>

#include <openssl/ssl.h>
#include <openssl/engine.h>


#define ENGINE_PATH	"./libgin.so"

#define HOME		"./"
#define CERT_FILE	HOME "server.pem"
#define PRIV_FILE	HOME "dummy.prv.pem"


ENGINE* setup_engine(const char *engine_name)
{
	ENGINE* engine = NULL;
	int status = 1;

	do {
		if (engine_name ==	NULL) { 
			break;
		}

		ENGINE_load_dynamic();

		engine = ENGINE_by_id("dynamic");
		if (engine == NULL) {
			break;
		}

		status = ENGINE_ctrl_cmd_string(engine, "SO_PATH", engine_name, 0);
		if (status <= 0) {
			break;
		}

		status = ENGINE_ctrl_cmd_string(engine, "LOAD", NULL, 0);
		if (status <= 0) {
			break;
		}

		status = ENGINE_set_default(engine, ENGINE_METHOD_ALL);
		if (status <= 0) {
			break;
		}
	} while (0);

	if (engine) {
		// Free our "structural" reference
		ENGINE_free(engine);

		if (status <= 0) {
			engine = NULL;
		}
	}

	return engine;
}

int main ()
{
	int ret = 0;
	int serverSock = -1;
	int clientSock = -1;
	struct sockaddr_in serverAddress;
	struct sockaddr_in clientAddress;
	size_t clientLen;
	char buf[4096];

	char* sslStr = NULL;
	SSL_METHOD *sslMethods = NULL;
	ENGINE* engine = NULL;
	SSL_CTX* ctx = NULL;
	SSL* ssl = NULL;
	X509* clientCert = NULL;


	SSL_load_error_strings();

	SSL_library_init();

	do {
		engine = setup_engine(ENGINE_PATH);
		if (engine == NULL) {
			ret = -1;
			break;
		}

		sslMethods = SSLv23_server_method();
		if (sslMethods == NULL) {
			ret = -2;
			break;
		}

		ctx = SSL_CTX_new(sslMethods);
		if (!ctx) {
			ret = -3;
			break;
		}

		ret = SSL_CTX_use_certificate_file(ctx, CERT_FILE, SSL_FILETYPE_PEM);
		if (ret <= 0) {
			ret = -4;
			break;
		}

		ret = SSL_CTX_use_PrivateKey_file(ctx, PRIV_FILE, SSL_FILETYPE_PEM);
		if (ret <= 0) {
			ret = -5;
			break;
		}

		serverSock = socket(AF_INET, SOCK_STREAM, 0);
		if (serverSock < 0) {
			ret = -6;
			break;
		}

		ret = 1;
		setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &ret, sizeof(ret));

		memset(&serverAddress, 0, sizeof(serverAddress));
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = INADDR_ANY;
		serverAddress.sin_port = htons(4433);

		ret = bind(serverSock, (struct sockaddr*) &serverAddress,
							   sizeof(serverAddress));
		if (ret < 0) {
			ret = -7;
			break;
		}

		ret = listen(serverSock, 5);
		if (ret < 0) {
			ret = -8;
			break;
		}

		clientLen = sizeof(clientAddress);
		clientSock = accept(serverSock, (struct sockaddr*)&clientAddress,
										&clientLen);
		if (clientSock < 0) {
			ret = -9;
			break;
		}

		close(serverSock);

		fprintf(stderr, "Connection from %lx, port %x\n",
				(unsigned long int) clientAddress.sin_addr.s_addr,
				clientAddress.sin_port);

		ssl = SSL_new(ctx);
		if (ssl == NULL) {
			ret = -10;
			break;
		}

		SSL_set_fd(ssl, clientSock);

		ret = SSL_accept(ssl);
		if (ret <= 0) {
			ret = -11;
			break;
		}

		printf("SSL connection using %s\n", SSL_get_cipher(ssl));

		clientCert = SSL_get_peer_certificate(ssl);
		if (clientCert != NULL) {
			printf("Client certificate:\n");

			sslStr = X509_NAME_oneline(X509_get_subject_name(clientCert), 0, 0);
			if (sslStr == NULL) {
				ret = -12;
				break;
			}

			printf("\tsubject: %s\n", sslStr);
			OPENSSL_free(sslStr);

			sslStr = X509_NAME_oneline(X509_get_issuer_name	(clientCert), 0, 0);
			if (sslStr == NULL) {
				ret = -13;
				break;
			}

			printf("\tissuer: %s\n", sslStr);
			OPENSSL_free(sslStr);
		} else {
			printf("Client does not have certificate.\n");
		}

		ret = SSL_read(ssl, buf, sizeof(buf) - 1);
		if (ret <= 0) {
			ret = -13;
			break;
		}

		buf[ret] = '\0';
		printf("Got %d chars:'%s'\n", ret, buf);

		ret = SSL_write(ssl, "Hello from server", 17);
		if (ret <= 0) {
			ret = -14;
			break;
		}
	} while (0);

	if (ret != 0) {
		perror("Error"); 
		ERR_print_errors_fp(stderr);
	}

	if (clientCert != NULL) {
		X509_free(clientCert);
	}

	if (clientSock >= 0) {
		close(clientSock);
	}

	if (ssl != NULL) {
		SSL_free(ssl);
	}

	if (ctx != NULL) {
		SSL_CTX_free(ctx);
	}

	return 0;
}

