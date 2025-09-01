#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disciplina.h"

// Removido array global. Todas operações são feitas diretamente em arquivo.

void cadastrarDisciplina(FILE *arq) {

    Disciplina disciplina;
    printf("\nCadastrar Disciplina\n");
    printf("Codigo: ");
    scanf("%d", &disciplina.codigo);
    printf("Nome: ");
    scanf("%s", disciplina.nome);
    printf("Professor Responsável: ");
    scanf("%s", disciplina.professor);

    // Escreve a nova disciplina no arquivo
    fwrite(&disciplina, sizeof(Disciplina), 1, arq);

    printf("Disciplina cadastrada com sucesso!\n");
}



void listarDisciplinas(FILE *arq) {
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo de disciplinas.\n");
        return;
    }

    Disciplina disciplina;
    int count = 0;
    printf("\n+==========================================================================+\n");
    printf("|                         LISTA DE DISCIPLINAS                            |\n");
    printf("+==========================================================================+\n");
    printf("| %-4s | %-25s | %-25s |\n", "Cod", "Nome", "Professor");
    printf("+------+---------------------------+---------------------------+\n");
    while (fread(&disciplina, sizeof(Disciplina), 1, arq)) {
        printf("| %-4d | %-25s | %-25s |\n", disciplina.codigo, disciplina.nome, disciplina.professor);
        count++;
    }
    printf("+==========================================================================+\n");
    printf("Total de disciplinas listadas: %d\n", count);
}



void salvarDisciplinas(FILE *arq) {
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo de disciplinas.\n");
        return;
    }

    // Função desnecessária: dados já são gravados diretamente no arquivo nas operações de cadastro/edição.
    printf("Função salvarDisciplinas não é mais necessária. Dados já são persistidos no arquivo.\n");
}


FILE *abrirArquivoDisciplina(char *nomeArquivo, char *modo) {
    FILE *arq = fopen(nomeArquivo, modo);
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }
    return arq;
}

// Função para trocar o professor responsável por uma disciplina
void trocarProfessorDisciplina(int codigoDisciplina, const char *novoProfessor, FILE *arq) {
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo de disciplinas.\n");
        return;
    }

    Disciplina disciplina;
    long pos = 0;

    // Percorre todas as disciplinas no arquivo
    while (fread(&disciplina, sizeof(Disciplina), 1, arq)) {
        if (disciplina.codigo == codigoDisciplina) {
            // Atualiza o professor diretamente no registro
            strncpy(disciplina.professor, novoProfessor, sizeof(disciplina.professor) - 1);
            disciplina.professor[sizeof(disciplina.professor) - 1] = '\0';
            // Volta o ponteiro para a posição da disciplina e atualiza no arquivo
            fseek(arq, pos * sizeof(Disciplina), SEEK_SET);
            fwrite(&disciplina, sizeof(Disciplina), 1, arq);
            printf("Professor responsável pela disciplina %d alterado com sucesso para %s.\n", codigoDisciplina, novoProfessor);
            return;
        }
        pos++;
    }
    printf("Disciplina com código %d não encontrada.\n", codigoDisciplina);
}


void carregarDisciplinas(FILE *arq) {
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo de disciplinas.\n");
        return;
    }

    // Função desnecessária: dados são lidos diretamente do arquivo nas operações de listagem/busca.
    printf("Função carregarDisciplinas não é mais necessária. Dados já são lidos do arquivo.\n");
}


void bubbleSortDisciplina(FILE *arq) {
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo de disciplinas.\n");
        return;
    }

    // Calcula o número total de disciplinas no arquivo
    fseek(arq, 0, SEEK_END);  // Move o ponteiro para o final do arquivo
    long tamanhoArquivo = ftell(arq);  // Obtém o tamanho do arquivo
    int qtdDisciplinas = tamanhoArquivo / sizeof(Disciplina);  // Calcula o número de disciplinas no arquivo
    fseek(arq, 0, SEEK_SET);  // Volta o ponteiro para o início do arquivo

    // Ordenação Bubble Sort diretamente no arquivo
    Disciplina d1, d2;
    for (int i = 0; i < qtdDisciplinas - 1; i++) {
        for (int j = 0; j < qtdDisciplinas - 1 - i; j++) {
            // Lê a disciplina na posição j
            fseek(arq, j * sizeof(Disciplina), SEEK_SET);
            fread(&d1, sizeof(Disciplina), 1, arq);
            // Lê a disciplina na posição j+1
            fread(&d2, sizeof(Disciplina), 1, arq);
            if (d1.codigo > d2.codigo) {
                // Troca as disciplinas de posição no arquivo
                fseek(arq, j * sizeof(Disciplina), SEEK_SET);
                fwrite(&d2, sizeof(Disciplina), 1, arq);
                fwrite(&d1, sizeof(Disciplina), 1, arq);
            }
        }
    }
    printf("\nBase de dados de disciplinas ordenada com sucesso!\n");
}


