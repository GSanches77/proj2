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
#define MAX_COMBINACOES 1000000 // Número máximo de combinações permitidas

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

char **gerar_combinacoes(char palavras[][TAM_PALAVRA + 1], int qtd_palavras, int *qtd_combinacoes) {
    *qtd_combinacoes = 0;
    char **combinacoes = malloc(MAX_COMBINACOES * sizeof(char *));
    if (combinacoes == NULL) {
        printf("Erro ao alocar memória para combinações.\n");
        exit(1);
    }

    for (int p1 = 0; p1 < qtd_palavras; p1++) {
        for (int p2 = -1; p2 < qtd_palavras; p2++) {
            for (int p3 = -1; p3 < qtd_palavras; p3++) {
                for (int p4 = -1; p4 < qtd_palavras; p4++) {
                    for (int p5 = -1; p5 < qtd_palavras; p5++) {
                        if (*qtd_combinacoes >= MAX_COMBINACOES) {
                            printf("Número máximo de combinações atingido.\n");
                            return combinacoes;
                        }

                        char combinacao[TAM_COD] = "";
                        strcat(combinacao, palavras[p1]);
                        if (p2 != -1) strcat(strcat(combinacao, " "), palavras[p2]);
                        if (p3 != -1) strcat(strcat(combinacao, " "), palavras[p3]);
                        if (p4 != -1) strcat(strcat(combinacao, " "), palavras[p4]);
                        if (p5 != -1) strcat(strcat(combinacao, " "), palavras[p5]);

                        combinacoes[*qtd_combinacoes] = malloc(strlen(combinacao) + 1);
                        if (combinacoes[*qtd_combinacoes] == NULL) {
                            printf("Erro ao alocar memória para combinação.\n");
                            exit(1);
                        }
                        strcpy(combinacoes[*qtd_combinacoes], combinacao);
                        (*qtd_combinacoes)++;
                    }
                }
            }
        }
    }

    return combinacoes;
}

void comparar_senhas(char senhas_decodificadas[][TAM_COD + 1], char **combinacoes, int qtd_combinacoes, 
                     char usuarios[][TAM_PALAVRA + 1], int qtd_senhas, 
                     char quebradas[][TAM_COD + 1], char nao_quebradas[][TAM_COD + 1], int *qtd_quebradas, int *qtd_nao_quebradas) {
    *qtd_quebradas = 0;
    *qtd_nao_quebradas = 0;

    for (int i = 0; i < qtd_senhas; i++) {
        int encontrada = 0;
        for (int j = 0; j < qtd_combinacoes && !encontrada; j++) {
            if (strcmp(senhas_decodificadas[i], combinacoes[j]) == 0) {
                sprintf(quebradas[*qtd_quebradas], "%s:%s", usuarios[i], combinacoes[j]);
                (*qtd_quebradas)++;
                encontrada = 1;
            }
        }
        if (!encontrada) {
            sprintf(nao_quebradas[*qtd_nao_quebradas], "%s:%s", usuarios[i], senhas_decodificadas[i]);
            (*qtd_nao_quebradas)++;
        }
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

int main() {
    char palavras[QTD_PALAVRAS][TAM_PALAVRA + 1];
    char senhas_codificadas[QTD_SENHAS][TAM_COD + 1];
    char senhas_decodificadas[QTD_SENHAS][TAM_COD + 1];
    char usuarios[QTD_SENHAS][TAM_PALAVRA + 1];
    char quebradas[QTD_SENHAS][TAM_COD + 1], nao_quebradas[QTD_SENHAS][TAM_COD + 1];

    int qtd_combinacoes, qtd_quebradas, qtd_nao_quebradas;

    carregar_palavras(palavras, "palavras.txt");
    carregar_codificadas(senhas_codificadas, "usuarios_senhascodificadas.txt");
    decodificar_senhas(senhas_codificadas, senhas_decodificadas, QTD_SENHAS);
    char **combinacoes = gerar_combinacoes(palavras, QTD_PALAVRAS, &qtd_combinacoes);
    comparar_senhas(senhas_decodificadas, combinacoes, qtd_combinacoes, usuarios, QTD_SENHAS, quebradas, nao_quebradas, &qtd_quebradas, &qtd_nao_quebradas);

    salvar_resultados("senhas_quebradas.txt", quebradas, qtd_quebradas);
    salvar_resultados("senhas_nao_quebradas.txt", nao_quebradas, qtd_nao_quebradas);

    // Liberar memória alocada dinamicamente
    for (int i = 0; i < qtd_combinacoes; i++) {
        free(combinacoes[i]);
    }
    free(combinacoes);

    return 0;
}
