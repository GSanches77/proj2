
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <stdint.h>

#define TAM_PALAVRA 10
#define QTD_PALAVRAS 24
#define TAM_COD 150
#define QTD_SENHAS 23

void carregar_palavras(char palavras[][TAM_PALAVRA + 1], const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de palavras.
");
        exit(1);
    }

    for (int i = 0; i < QTD_PALAVRAS; i++) {
        if (fgets(palavras[i], TAM_PALAVRA, arquivo)) {
            palavras[i][strcspn(palavras[i], "\n")] = '\0';  // Remove o caractere de nova linha
        }
    }

    fclose(arquivo);
}

int Base64Encode(const unsigned char *buffer, size_t length, char **b64text) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, buffer, length);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    BIO_set_close(bio, BIO_NOCLOSE);

    *b64text = (char *)malloc(bufferPtr->length + 1);
    memcpy(*b64text, bufferPtr->data, bufferPtr->length);
    (*b64text)[bufferPtr->length] = '\0';

    BIO_free_all(bio);
    return 0; // Sucesso
}

void carregar_codificadas(char senhas_codificadas[][TAM_COD + 1], const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de senhas codificadas.
");
        exit(1);
    }

    for (int i = 0; i < QTD_SENHAS; i++) {
        if (fgets(senhas_codificadas[i], TAM_COD, arquivo)) {
            senhas_codificadas[i][strcspn(senhas_codificadas[i], "\n")] = '\0';  // Remove nova linha
        }
    }

    fclose(arquivo);
}

void remover_nome(char *linha, char *saida) {
    char *p = strchr(linha, ':');
    if (p != NULL) {
        strcpy(saida, p + 1);
    }
}

void gerar_combinacoes(char palavras[QTD_PALAVRAS][TAM_PALAVRA + 1], int nivel, char combinacao[][TAM_COD], int *contador) {
    char buffer[TAM_COD] = "";
    for (int i = 0; i < QTD_PALAVRAS; i++) {
        strcpy(buffer, palavras[i]);
        for (int j = 0; j < QTD_PALAVRAS && nivel > 1; j++) {
            strcat(buffer, " ");
            strcat(buffer, palavras[j]);
            strcpy(combinacao[*contador], buffer);
            (*contador)++;
        }
    }
}

void codificar_combinacao(char *entrada, char *saida) {
    SHA512_CTX ctx;
    unsigned char hash[SHA512_DIGEST_LENGTH];

    SHA512_Init(&ctx);
    SHA512_Update(&ctx, entrada, strlen(entrada));
    SHA512_Final(hash, &ctx);

    char *b64encoded = NULL;
    Base64Encode(hash, SHA512_DIGEST_LENGTH, &b64encoded);
    strcpy(saida, b64encoded);
    free(b64encoded);
}

int main() {
    char palavras[QTD_PALAVRAS][TAM_PALAVRA + 1];
    char senhas_codificadas[QTD_SENHAS][TAM_COD + 1];
    char combinacoes[100000][TAM_COD];
    int contador = 0;

    carregar_palavras(palavras, "palavras.txt");
    carregar_codificadas(senhas_codificadas, "usuarios_senhascodificadas.txt");

    gerar_combinacoes(palavras, 2, combinacoes, &contador);

    for (int i = 0; i < contador; i++) {
        char senha_codificada[TAM_COD];
        codificar_combinacao(combinacoes[i], senha_codificada);

        for (int j = 0; j < QTD_SENHAS; j++) {
            char senha_pura[TAM_COD];
            remover_nome(senhas_codificadas[j], senha_pura);
            if (strcmp(senha_codificada, senha_pura) == 0) {
                printf("Senha quebrada: %s -> %s\n", combinacoes[i], senhas_codificadas[j]);
            }
        }
    }

    return 0;
}
