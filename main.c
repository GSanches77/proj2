#include <stdio.h>
#include <openssl/sha.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <stdint.h>

/**
 * The MIT License (MIT)
 * Copyright (c) 2013 Barry Steyn
 */
int Base64Encode(const unsigned char* buffer, size_t length, char** b64text) { //Encodes a binary safe base 64 string
	BIO *bio, *b64;
	BUF_MEM *bufferPtr;

	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line
	BIO_write(bio, buffer, length);
	BIO_flush(bio);
	BIO_get_mem_ptr(bio, &bufferPtr);
	BIO_set_close(bio, BIO_NOCLOSE);
	BIO_free_all(bio);

	*b64text=(*bufferPtr).data;

	return (0); //success
}

int main() {
    SHA512_CTX ctx;
    unsigned char buffer[512];

    char *str = "torta pizza sorvete";
    int len = strlen(str);
    strcpy(buffer, str);
    SHA512_Init(&ctx);
    SHA512_Update(&ctx, buffer, len);
    SHA512_Final(buffer, &ctx);
    // fwrite(&buffer, 64, 1, stdout);

    char *base64encoded;
    Base64Encode(buffer, 64, &base64encoded);
    printf("Senha codificada: %s\n", base64encoded);
    return 0;
}