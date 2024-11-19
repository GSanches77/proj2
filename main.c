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
#define qtd_palavras 28
#define TAM_COD 150
#define qtd_senhas 23

void carregar_palavras(char palavras[][TAM_PALAVRA + 1], const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de palavras.\n");
        exit(1);
    }

    int i = 0;

    for(i = 0; i < qtd_palavras; i++){
        fgets(palavras[i], TAM_PALAVRA, arquivo);
        palavras[i][strcspn(palavras[i], "\n")] = '\0';
    }
    
    fclose(arquivo);
}

int Base64Encode(const unsigned char* buffer, size_t length, char** b64text) { 
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
	BIO_free_all(bio);

	*b64text=(*bufferPtr).data;

	return (0); //success
}

//bool comparar(char combinacao_palavras[TAM_PALAVRA*5+5]){

//}

void combinacao(char palavras[qtd_palavras][TAM_PALAVRA + 1], char combinacao_palavras[TAM_PALAVRA*5+5]){
	int i;
	int j;
	int k;
	int l;
	int m;
	
    for(i = 0; i < qtd_palavras; i++){
        strcpy(combinacao_palavras, palavras[i]);
        for(j = 0; j <= qtd_palavras; j++){
            if(j < qtd_palavras){
                strcat(combinacao_palavras, " ");  
                strcat(combinacao_palavras, palavras[j]);

            } 
            for(k = 0; k <= qtd_palavras; k++){
                if(k < qtd_palavras){
                    strcat(combinacao_palavras, " ");  
                    strcat(combinacao_palavras, palavras[j]);
                } 
                for(l = 0; l <= qtd_palavras; l++){
                    if(l < qtd_palavras){
                        strcat(combinacao_palavras, " ");  
                        strcat(combinacao_palavras, palavras[j]);
                    } 
                    for(m = 0; m <= qtd_palavras; m++){
                        if(m < qtd_palavras){
                            strcat(combinacao_palavras, " ");  
                            strcat(combinacao_palavras, palavras[j]);

                            combinacao_palavras[0] = '\0';
                        } 
                    }
                }
            }
        }
    }

}


void carregar_codificadas(char senhas_codificadas[][TAM_COD + 1], const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de palavras.\n");
        exit(1);
    }

    int i = 0;

    for(i = 0; i < qtd_senhas; i++){
        char senha = fgets(senhas_codificadas[i], TAM_COD, arquivo);
    	//senhas_codificadas[i][strcspn(senhas_codificadas[i], "\n")] = "\0";
    }
    
    fclose(arquivo);
}

void remover_nome(char *str) {
    char *p = strchr(str, ':');
    if (p != NULL) {
        printf("%s", p + 1);
    }
}

int main(){
	int i;
    char palavras[qtd_palavras][TAM_PALAVRA + 1];
    int tam_combinacoes = TAM_PALAVRA*5+5;
    char combinacao_palavras[tam_combinacoes];
    char senhas_codificadas[qtd_senhas][TAM_COD + 1];

    SHA512_CTX ctx;
    unsigned char buffer[512];

    char *str = "torta pizza sorvete";
    int len = strlen(str);
    strcpy(buffer, str);
    SHA512_Init(&ctx);
    SHA512_Update(&ctx, buffer, len);
    SHA512_Final(buffer, &ctx);

    char *base64encoded;
    Base64Encode(buffer, 64, &base64encoded);


    carregar_codificadas(senhas_codificadas, "usuarios_senhascodificadas.txt");


    carregar_palavras(palavras, "palavras.txt");
    
    /*for (int i = 0; i < qtd_palavras; i++) {
        printf("%s", palavras[i]);
    }*/
    
    for (i = 0; i <qtd_senhas; i++) {
        remover_nome(senhas_codificadas[i]);
    }

    combinacao(palavras, combinacao_palavras);

    printf("%s", combinacao_palavras);

    return 0;

}
    char *base64encoded;
    Base64Encode(buffer, 64, &base64encoded);
    printf("Senha codificada: %s\n", base64encoded);
    return 0;
}
