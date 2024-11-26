//gcc main.c -o main -lcrypto -lssl -I/usr/include/openssl -L/usr/lib/x86_64-linux-gnu
//./main

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

#define TAM_PALAVRA 11
#define QTD_PALAVRAS 24
#define TAM_COD 150
#define QTD_SENHAS 23

void carregar_palavras(char palavras[][TAM_PALAVRA + 1], const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de palavras.\n");
        exit(1);
    }
    for (int i = 0; i < QTD_PALAVRAS; i++) {
        if (fgets(palavras[i], TAM_PALAVRA, arquivo)) {
            palavras[i][strcspn(palavras[i], "\n")] = '\0';  // Remove nova linha
        }
    }
    fclose(arquivo);
}

void carregar_codificadas(char senhas_codificadas[][TAM_COD + 1], const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de senhas codificadas.\n");
        exit(1);
    }
    for (int i = 0; i < QTD_SENHAS; i++) {
        if (fgets(senhas_codificadas[i], TAM_COD, arquivo)) {
            senhas_codificadas[i][strcspn(senhas_codificadas[i], "\n")] = '\0';  // Remove nova linha
        }
    }
    fclose(arquivo);
}

void remover_nome(char *linha, char *usuario, char *senha) {
    char *p = strchr(linha, ':');
    if (p != NULL) {
        strncpy(usuario, linha, p - linha);
        usuario[p - linha] = '\0';
        strcpy(senha, p + 1);
    }
}

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


void salvar_resultados(const char *arquivo, char resultados[][TAM_COD + 1], int qtd) {
    FILE *file = fopen(arquivo, "w");
    if (file == NULL) {
        printf("Erro ao criar o arquivo %s\n", arquivo);
        exit(1);
    }
    for (int i = 0; i < qtd; i++) {
        fprintf(file, "%s\n", resultados[i]);
    }
    fclose(file);
}

int main() {
    char palavras[QTD_PALAVRAS][TAM_PALAVRA + 1];
    char senhas_codificadas[QTD_SENHAS][TAM_COD + 1];
    char senhas_quebradas[QTD_SENHAS][TAM_COD + 1];
    char senhas_nao_quebradas[QTD_SENHAS][TAM_COD + 1];
    char usuario[TAM_PALAVRA + 1], senha_codificada[TAM_COD + 1], senha_decodificada[TAM_COD + 1];
    char senha_pura[TAM_COD + 1];
    size_t length;
    int qtd_quebradas = 0, qtd_nao_quebradas = 0;

    carregar_palavras(palavras, "palavras.txt");
    carregar_codificadas(senhas_codificadas, "usuarios_senhascodificadas.txt");

    for (int i = 0; i < QTD_SENHAS; i++) {
        remover_nome(senhas_codificadas[i], usuario, senha_codificada);

        // Decodificar a senha
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
        printf("Senha decodificada para %s: %s\n", usuario, buffer); // Exibe a senha decodificada

        // Gerar combinações e comparar
        int encontrada = 0;
        for (int p1 = 0; p1 < QTD_PALAVRAS && !encontrada; p1++) {
            for (int p2 = p1; p2 < QTD_PALAVRAS && !encontrada; p2++) {
                if (p1 == p2) {
                    // Senha com uma única palavra
                    sprintf(senha_pura, "%s", palavras[p1]);
                } else {
                    // Senha com duas ou mais palavras
                    sprintf(senha_pura, "%s %s", palavras[p1], palavras[p2]);
                }

                // Exibe as possíveis senhas geradas
                //printf("Possível senha gerada: %s\n", senha_pura);

                if (strcmp(senha_decodificada, senha_pura) == 0) {
                    sprintf(senhas_quebradas[qtd_quebradas++], "%s:%s", usuario, senha_pura);
                    encontrada = 1;
                }
            }
        }

        if (!encontrada) {
            sprintf(senhas_nao_quebradas[qtd_nao_quebradas++], "%s:%s", usuario, senha_codificada);
        }
    }

    // Salvar resultados
    salvar_resultados("senhas_quebradas.txt", senhas_quebradas, qtd_quebradas);
    salvar_resultados("senhas_nao_quebradas.txt", senhas_nao_quebradas, qtd_nao_quebradas);

    return 0;
}
