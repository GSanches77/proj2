#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

#define TAM_PALAVRA 10
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
            palavras[i][strcspn(palavras[i], "\n")] = '\0';
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

    *b64text = (char *)malloc(bufferPtr->length + 1);
    memcpy(*b64text, bufferPtr->data, bufferPtr->length);
    (*b64text)[bufferPtr->length] = '\0';

    BIO_free_all(bio);
    return 0;
}

void carregar_codificadas(char senhas_codificadas[][TAM_COD + 1], const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de senhas codificadas.\n");
        exit(1);
    }
    for (int i = 0; i < QTD_SENHAS; i++) {
        if (fgets(senhas_codificadas[i], TAM_COD, arquivo)) {
            senhas_codificadas[i][strcspn(senhas_codificadas[i], "\n")] = '\0'; 
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

void codificar_combinacao(const char *combinacao, char *saida) {
    unsigned char hash[SHA512_DIGEST_LENGTH];
    SHA512_CTX sha_context;

    SHA512_Init(&sha_context);
    SHA512_Update(&sha_context, combinacao, strlen(combinacao));
    SHA512_Final(hash, &sha_context);

    char *b64text = NULL;
    Base64Encode(hash, SHA512_DIGEST_LENGTH, &b64text);
    strcpy(saida, b64text);
    free(b64text);
}

int main() {
    char palavras[QTD_PALAVRAS][TAM_PALAVRA + 1];
    char senhas_codificadas[QTD_SENHAS][TAM_COD + 1];
    char senhas_quebradas[QTD_SENHAS][TAM_COD + 1];
    char senhas_nao_quebradas[QTD_SENHAS][TAM_COD + 1];

    int qtd_quebradas = 0, qtd_nao_quebradas = 0;

    carregar_palavras(palavras, "palavras.txt");
    carregar_codificadas(senhas_codificadas, "usuarios_senhascodificadas.txt");

    for (int i = 0; i < QTD_SENHAS; i++) {
        char usuario[TAM_PALAVRA + 1], senha_codificada[TAM_COD + 1], senha_pura[TAM_COD + 1];
        remover_nome(senhas_codificadas[i], usuario, senha_codificada);

        int encontrada = 0;
        for (int p1 = 0; p1 < QTD_PALAVRAS && !encontrada; p1++) {
            for (int p2 = -1; p2 < QTD_PALAVRAS && !encontrada; p2++) {
                if (p2 == -1) {
                    sprintf(senha_pura, "%s", palavras[p1]);
                } else {
                    sprintf(senha_pura, "%s %s", palavras[p1], palavras[p2]);
                }
                char senha_codificada_tentativa[TAM_COD + 1];
                codificar_combinacao(senha_pura, senha_codificada_tentativa);

                if (strcmp(senha_codificada, senha_codificada_tentativa) == 0) {
                    sprintf(senhas_quebradas[qtd_quebradas++], "%s:%s", usuario, senha_pura);
                    encontrada = 1;
                }
            }
        }

        if (!encontrada) {
            sprintf(senhas_nao_quebradas[qtd_nao_quebradas++], "%s:%s", usuario, senha_codificada);
        }
    }

    salvar_resultados("senhas_quebradas.txt", senhas_quebradas, qtd_quebradas);
    salvar_resultados("senhas_nao_quebradas.txt", senhas_nao_quebradas, qtd_nao_quebradas);

    return 0;
}
