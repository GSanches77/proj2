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

void Base64Decode(const char *b64message, char *buffer, size_t *length) {
    BIO *bio, *b64;

    bio = BIO_new_mem_buf((void *)b64message, -1);
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    *length = BIO_read(bio, buffer, strlen(b64message));
    if (*length <= 0) {
        printf("Erro na decodificação Base64: %s\n", b64message);
        buffer[0] = '\0';
        *length = 0;
    } else {
        buffer[*length] = '\0';
    }

    BIO_free_all(bio);
}

int main() {
    char palavras[QTD_PALAVRAS][TAM_PALAVRA + 1];
    char senhas_codificadas[QTD_SENHAS][TAM_COD + 1];
    char usuario[TAM_PALAVRA + 1], senha_codificada[TAM_COD + 1], senha_decodificada[TAM_COD + 1];
    char senha_pura[TAM_COD + 1];
    size_t length;

    carregar_palavras(palavras, "palavras.txt");
    carregar_codificadas(senhas_codificadas, "usuarios_senhascodificadas.txt");

    for (int i = 0; i < QTD_SENHAS; i++) {
        remover_nome(senhas_codificadas[i], usuario, senha_codificada);

        // Decodificar a senha
        Base64Decode(senha_codificada, senha_decodificada, &length);
        if (length == 0) {
            printf("Senha decodificada inválida para %s.\n", usuario);
            continue;
        }

        printf("Senha decodificada para %s: %s\n", usuario, senha_decodificada);

        // Gerar combinações e comparar
        for (int p1 = 0; p1 < QTD_PALAVRAS; p1++) {
            for (int p2 = -1; p2 < QTD_PALAVRAS; p2++) {
                if (p2 == -1) {
                    sprintf(senha_pura, "%s", palavras[p1]);
                } else {
                    sprintf(senha_pura, "%s %s", palavras[p1], palavras[p2]);
                }

                // Verificar se a combinação é válida
                if (strlen(senha_pura) == 0) continue;

                printf("Comparando: %s\n", senha_pura);

                if (strcmp(senha_decodificada, senha_pura) == 0) {
                    printf("Senha quebrada para %s: %s\n", usuario, senha_pura);
                    break;
                }
            }
        }
    }

    return 0;
}
