#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

void carregar_codificadas(char senhas_codificadas[][TAM_COD + 1], char usuarios[][TAM_PALAVRA + 1], const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de senhas codificadas.\n");
        exit(1);
    }
    for (int i = 0; i < QTD_SENHAS; i++) {
        char linha[TAM_COD + TAM_PALAVRA];
        if (fgets(linha, sizeof(linha), arquivo)) {
            char *sep = strchr(linha, ':');
            if (sep != NULL) {
                strncpy(usuarios[i], linha, sep - linha);
                usuarios[i][sep - linha] = '\0';
                strcpy(senhas_codificadas[i], sep + 1);
                senhas_codificadas[i][strcspn(senhas_codificadas[i], "\n")] = '\0';  // Remove nova linha
            }
        }
    }
    fclose(arquivo);
}

void Base64Decode(const char *b64message, char *buffer, size_t *length) {
    BIO *bio, *b64;
    bio = BIO_new_mem_buf((void *)b64message, -1);
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    *length = BIO_read(bio, buffer, strlen(b64message));
    buffer[*length] = '\0';

    BIO_free_all(bio);
}

void decodificar_senhas(char senhas_codificadas[][TAM_COD + 1], char senhas_decodificadas[][TAM_COD + 1], int qtd) {
    size_t length;
    for (int i = 0; i < qtd; i++) {
        Base64Decode(senhas_codificadas[i], senhas_decodificadas[i], &length);
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

void comparar_senhas(char palavras[][TAM_PALAVRA + 1], char senhas_decodificadas[][TAM_COD + 1], char usuarios[][TAM_PALAVRA + 1], 
                     int qtd_senhas, int qtd_palavras, char quebradas[][TAM_COD + 1], char nao_quebradas[][TAM_COD + 1], 
                     int *qtd_quebradas, int *qtd_nao_quebradas) {
    *qtd_quebradas = 0;
    *qtd_nao_quebradas = 0;

    for (int i = 0; i < qtd_senhas; i++) {
        int encontrada = 0;
        char tentativa[TAM_COD];

        for (int p1 = 0; p1 < qtd_palavras && !encontrada; p1++) {
            for (int p2 = -1; p2 < qtd_palavras && !encontrada; p2++) {
                for (int p3 = -1; p3 < qtd_palavras && !encontrada; p3++) {
                    for (int p4 = -1; p4 < qtd_palavras && !encontrada; p4++) {
                        for (int p5 = -1; p5 < qtd_palavras && !encontrada; p5++) {
                            snprintf(tentativa, sizeof(tentativa), "%s", palavras[p1]);
                            if (p2 != -1) strcat(strcat(tentativa, " "), palavras[p2]);
                            if (p3 != -1) strcat(strcat(tentativa, " "), palavras[p3]);
                            if (p4 != -1) strcat(strcat(tentativa, " "), palavras[p4]);
                            if (p5 != -1) strcat(strcat(tentativa, " "), palavras[p5]);

                            if (strcmp(senhas_decodificadas[i], tentativa) == 0) {
                                snprintf(quebradas[*qtd_quebradas], TAM_COD, "%s:%s", usuarios[i], tentativa);
                                (*qtd_quebradas)++;
                                encontrada = 1;
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (!encontrada) {
            snprintf(nao_quebradas[*qtd_nao_quebradas], TAM_COD, "%s:%s", usuarios[i], senhas_decodificadas[i]);
            (*qtd_nao_quebradas)++;
        }
    }
}

int main() {
    char palavras[QTD_PALAVRAS][TAM_PALAVRA + 1];
    char senhas_codificadas[QTD_SENHAS][TAM_COD + 1];
    char senhas_decodificadas[QTD_SENHAS][TAM_COD + 1];
    char usuarios[QTD_SENHAS][TAM_PALAVRA + 1];
    char quebradas[QTD_SENHAS][TAM_COD + 1], nao_quebradas[QTD_SENHAS][TAM_COD + 1];
    int qtd_quebradas, qtd_nao_quebradas;

    carregar_palavras(palavras, "palavras.txt");
    carregar_codificadas(senhas_codificadas, usuarios, "usuarios_senhascodificadas.txt");
    decodificar_senhas(senhas_codificadas, senhas_decodificadas, QTD_SENHAS);
    comparar_senhas(palavras, senhas_decodificadas, usuarios, QTD_SENHAS, QTD_PALAVRAS, quebradas, nao_quebradas, &qtd_quebradas, &qtd_nao_quebradas);

    salvar_resultados("senhas_quebradas.txt", quebradas, qtd_quebradas);
    salvar_resultados("senhas_nao_quebradas.txt", nao_quebradas, qtd_nao_quebradas);

    return 0;
}
