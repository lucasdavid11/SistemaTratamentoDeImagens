/*============================================================================*/
/* FUNÇÕES PROJETO 3 FUNDAMENTOS DE PROGRAMAÇÃO*/
/*----------------------------------------------------------------------------*/
/* Autores:
   2582732 - Lucas David Roscziniak Costa
   2582716 - Gustavo Bueno da Costa
                                                     */
/*============================================================================*/

#include "trabalho3.h"
#include "imagem.h"
#include "gerador_de_testes.h"

#define MEDIA 35
#define CANAL_R 0 /* Constante usada para se referir ao canal vermelho. */
#define CANAL_G 1 /* Constante usada para se referir ao canal verde. */
#define CANAL_B 2 /* Constante usada para se referir ao canal azul. */

void removeFundo (Imagem3C* img, Imagem3C* bg, int contagem[N_TIPOS_DE_VEICULOS]);
void divide_as_pistas (Imagem3C* img);
void transforma_veiculos (Imagem3C* img);
void defineTipo_e_armazena(Imagem3C* img, int contagem[N_TIPOS_DE_VEICULOS]);

int contaVeiculos (Imagem3C* img, Imagem3C* bg, int contagem [N_TIPOS_DE_VEICULOS])
{
    removeFundo (img, bg, contagem);
    divide_as_pistas (img);
    transforma_veiculos (img);
    defineTipo_e_armazena(img, contagem);

    int retorno = 0, i;
    for (i = 0 ; i < N_TIPOS_DE_VEICULOS ; i++)
        retorno = retorno + contagem[i];
    return retorno;
}

/** ====== removeFundo ===== **
*
*   percorre toda a matriz vendo se os pontos estão abaixo da media e
*   - se sim zera as cores do ponto
*   - se nao pinta o ponto de vermelho, um ponto de interesse
*   a flag é levantada quando encontra um ponto de interesse
*
*/

void removeFundo (Imagem3C* img, Imagem3C* bg, int contagem[N_TIPOS_DE_VEICULOS])
{
    unsigned long i, j;
    int flag = 0;
    for(i = 0 ; i < img -> altura ; i++){
        for (j = 0 ; j < img -> largura; j++)
        {
            /** ====== Pinta de preto ======*/

            if (bg->dados[CANAL_R][i][j] < img ->dados[CANAL_R][i][j] + MEDIA && bg -> dados[CANAL_R][i][j] > img->dados[CANAL_R][i][j] - MEDIA)
                if (bg -> dados[CANAL_G][i][j] < img -> dados[CANAL_G][i][j] + MEDIA && bg -> dados[CANAL_G][i][j] > img->dados[CANAL_G][i][j] - MEDIA)
                    if (bg -> dados[CANAL_B][i][j] < img -> dados[CANAL_B][i][j] + MEDIA && bg -> dados[CANAL_B][i][j] > img-> dados[CANAL_B][i][j] - MEDIA)
                    {
                        img -> dados[CANAL_R][i][j] = 0;
                        img -> dados[CANAL_G][i][j] = 0;
                        img -> dados[CANAL_B][i][j] = 0;
                    }
                    else
                        flag = 1;
                else
                    flag = 1;
            else
                flag = 1;

            /** ====== pinta os carros de vermelho ======*/

            if (flag)
            {
                img -> dados[CANAL_R][i][j] = 150;
                img -> dados[CANAL_G][i][j] = 0;
                img -> dados[CANAL_B][i][j] = 0;
                flag = 0;
            }
        }
     }
}

/** ====== Divide as pistas =====
*
*   divide a imagem em 8 pistas com linhas azuis
*   como todos os pontos azuis estao zerados, qualquer ponto azul agora serve
*   como "parede" para as pistas
*   96 é a altura de cada  pista
*   o for mais interno engrossa a linha para n cortar retrovisores de carros ao meio
*
*/
void divide_as_pistas (Imagem3C* img)
{
        unsigned long i, j, k;
        for (j = 0 ; j < img -> largura; j++)
            {
            for (i = 1 ; i <= 7 ; i++)
            {
                for(k = 0 ; k < 5 ; k++)
                {
                    img -> dados[CANAL_R][i * 96 - 3 + k][j] = 0;
                    img -> dados[CANAL_B][i * 96 - 3 + k][j] = 150;
                }
            }
        }
}

/**====== Transforma veiculos ======
*
*   percorre toda a matriz, transformando pontos vermelhos em colunas verdes
*
*   k = i - (i % 96) + 5 // i - (i % 96) vai fazer i se tornar um numero divizivel por 96, logo um ponto onde tem linha azul
*   ja o + 5 é por conta da linha azul ter 5 de espessura (tem +2 só pra garantir)
*   assim o k pinta a coluna dentro da faixa azul delimitada
*
*   depois de tudo transformado em colunas verdes as colunas
*   que nao estiverem acompanhados por mais 5 colunas verdes a sua frente, vão ser
*   vao desaparecer, para limpar parte do ruido gerado
*
*/

void transforma_veiculos (Imagem3C* img)
{
        unsigned long i, j, k;
        for(i = 96 ; i < img -> altura - 96 ; i++){
            for (j = 0 ; j < img -> largura; j++)
            {
                if((img -> dados[CANAL_R][i][j]) == 150)
                {
                    for(k = i - (i % 96) + 5 ; img -> dados[CANAL_B][k][j] != 150 ; k++){
                        img -> dados[CANAL_R][k][j] = 0;
                        img -> dados[CANAL_G][k][j] = 150;
                    }
                }
            }
        }

        int filtro, l;
        for(i = 1 ; i < 7 ; i++){
            for (j = 0 ; j < img -> largura; j += 4){
                for  (filtro = 0, l = 0 ; l < 5 ; l++)
                    if(img -> dados[CANAL_G][i*96 + 5][j + l] != 150)
                        filtro = 1;
                if (filtro)
                {
                    for(k = i*96 + 5 ; img -> dados[CANAL_B][k][j] != 150 ; k++)
                    {
                        img -> dados[CANAL_G][k][j] = 0;
                        img -> dados[CANAL_G][k][j+1] = 0;
                        img -> dados[CANAL_G][k][j+2] = 0;
                        img -> dados[CANAL_G][k][j+3] = 0;
                    }
                }
            }
        }
}

/** ===== Define tipo e armazena =====
*
*   percorre a matriz contabilizando a largura de cada quadrado verde
*   classifica cada um de acordo com a largura
*   e incrementa a o vetor contagem de acordo com a classificacao
*
*   i começa em 144 para iniciar no meio de cada estrada, por isso vai incrementando de 96 em 96
*   j termina com 100 a menos pois a parte do telhado da imagem gera muito ruido, ent ela é evitada na contagem
*
*/

void defineTipo_e_armazena(Imagem3C* img, int contagem[N_TIPOS_DE_VEICULOS])
{
    int tamanhoVeiculo, i, j;

    for (i = 0 ; i < 4 ; i++)
    contagem[i] = 0;

    for(i = 144 ; i < img -> altura ; i += 96)
    {
        for(j = 0 ; j < img -> largura - 100 ; j++)
        {
            if(img -> dados[CANAL_G][i][j] == 150)
                tamanhoVeiculo++;

            if(img -> dados[CANAL_G][i][j+1] == 0) //fim do veiculo
            {
                if (tamanhoVeiculo >= 50 && tamanhoVeiculo <= 80) // MOTO
                    contagem[0]++;
                else if (tamanhoVeiculo>= 81 && tamanhoVeiculo <= 150) //CARROS E VANS
                    contagem[1]++;
                else if (tamanhoVeiculo >= 151 && tamanhoVeiculo <= 300) // ONIBUS E CAMINHOES MENORES
                    contagem[2]++;
                else if (tamanhoVeiculo >= 301) // CAMINHOES MAIORES
                    contagem[3]++;

                tamanhoVeiculo = 0;
            }

        }

    }
}
