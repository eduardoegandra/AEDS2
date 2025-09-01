
#include <stdio.h>              // Biblioteca padrão de entrada/saída
#include <stdlib.h>             // Biblioteca padrão para funções utilitárias
#include <string.h>             // Biblioteca para manipulação de strings
#include <time.h>               // Biblioteca para manipulação de tempo
#include "professor.h"         // Header das funções de professor
#include "aluno.h"             // Header das funções de aluno
#include "disciplina.h"        // Header das funções de disciplina
#include "main.h"              // Header principal do projeto


// Variáveis auxiliares globais para operações do menu
FILE *arq;                      // Ponteiro genérico para arquivos
int codigoDisciplina;           // Código da disciplina para operações
char novoProfessor[100];        // Nome do novo professor para troca

// Declarações externas (se existirem em outros arquivos)
extern Professor professores[]; // Array de professores (se usado)
extern int qtdProfessores;      // Quantidade de professores
extern Disciplina disciplinas[];// Array de disciplinas (se usado)
extern int qtdDisciplinas;      // Quantidade de disciplinas


// Função para embaralhar um vetor de inteiros (Fisher-Yates)
void embaralhar(int *vet, int tam) {
    srand(time(NULL));
    for (int i = tam - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = vet[i];
        vet[i] = vet[j];
        vet[j] = temp;
    }
}


// Função para criar a base de dados inicial (alunos, professores, disciplinas)
// Gera dados aleatórios e embaralhados para simular um ambiente real
void criarBase(int tam) {
    printf("Criando base de dados com %d alunos (HASH e DAT)...\n", tam);
    clock_t inicio = clock();

    // Inicializa o arquivo hash encadeado exterior
    FILE *arqHash = fopen("alunos_hash.dat", "wb+");
    if (arqHash == NULL) {
        printf("Erro ao criar arquivo de hash encadeado.\n");
        return;
    }
    inicializarTabelaHashEncArquivo(arqHash);

    // Inicializa o arquivo tradicional de alunos
    FILE *arqDat = fopen("alunos.dat", "wb+");
    if (arqDat == NULL) {
        printf("Erro ao criar arquivo alunos.dat.\n");
        fclose(arqHash);
        return;
    }

    int i;
    // Vetor para embaralhar alunos
    int *vet = (int*)malloc(tam * sizeof(int));
    if (vet == NULL) {
        printf("Erro ao alocar memoria para embaralhamento.\n");
        fclose(arqHash);
        fclose(arqDat);
        return;
    }

    // Preenche e embaralha o vetor de alunos
    for (i = 0; i < tam; i++) vet[i] = i;
    embaralhar(vet, tam);

    // Nomes aleatórios para professores, alunos e disciplinas
    char nomesProf[10][20] = {"Ana","Bruno","Carlos","Diana","Eduardo","Fernanda","Gustavo","Helena","Igor","Julia"};
    char nomesAlu[10][20] = {"Lucas","Marina","Pedro","Rafaela","Thiago","Beatriz","Vinicius","Sofia","Caio","Lara"};
    char nomesDisc[10][20] = {"Matematica","Portugues","Historia","Geografia","Fisica","Quimica","Biologia","Ingles","Artes","EdFisica"};

    // Criação de professores aleatórios
    int *vetProf = (int*)malloc(tam * sizeof(int));
    for (i = 0; i < tam; i++) vetProf[i] = i;
    embaralhar(vetProf, tam);
    FILE *arqProf = fopen("professores.dat", "wb+");
    if (arqProf != NULL) {
        for (i = 0; i < tam; i++) {
            Professor prof;
            int idx = vetProf[i];
            snprintf(prof.nome, sizeof(prof.nome), "%s%d", nomesProf[rand()%10], rand()%1000);
            prof.matricula = 5000 + idx;
            snprintf(prof.disciplina, sizeof(prof.disciplina), "%s", nomesDisc[rand()%10]);
            prof.salario = 3000.0f + (rand()%1000);
            fwrite(&prof, sizeof(Professor), 1, arqProf);
        }
        fclose(arqProf);
        printf("Base de professores criada com sucesso!\n");
    } else {
        printf("Erro ao criar professores.dat\n");
    }
    free(vetProf);

    // Criação de disciplinas aleatórias
    int *vetDisc = (int*)malloc(tam * sizeof(int));
    for (i = 0; i < tam; i++) vetDisc[i] = i;
    embaralhar(vetDisc, tam);
    FILE *arqDisc = fopen("disciplinas.dat", "wb+");
    if (arqDisc != NULL) {
        for (i = 0; i < tam; i++) {
            Disciplina disc;
            int idx = vetDisc[i];
            disc.codigo = 100 + idx;
            snprintf(disc.nome, sizeof(disc.nome), "%s%d", nomesDisc[rand()%10], rand()%100);
            snprintf(disc.professor, sizeof(disc.professor), "%s%d", nomesProf[rand()%10], rand()%1000);
            fwrite(&disc, sizeof(Disciplina), 1, arqDisc);
        }
        fclose(arqDisc);
        printf("Base de disciplinas criada com sucesso!\n");
    } else {
        printf("Erro ao criar disciplinas.dat\n");
    }
    free(vetDisc);

    // Criação de alunos aleatórios
    for (i = 0; i < tam; i++) {
        Aluno aluno;
        int idx = vet[i];
        snprintf(aluno.nome, sizeof(aluno.nome), "%s%d", nomesAlu[rand()%10], rand()%1000);
        aluno.matricula = 1000 + idx;
        snprintf(aluno.disciplina, sizeof(aluno.disciplina), "%s", nomesDisc[rand()%10]);
        snprintf(aluno.email, sizeof(aluno.email), "%s%d@dominio.com", nomesAlu[rand()%10], rand()%1000);
        aluno.qtdDisciplinas = 1;
        aluno.disciplinas[0] = 100 + (rand()%tam);
        // Grava no hash encadeado exterior
        inserirAlunoHashEncArquivo(arqHash, aluno);
        // Grava no arquivo tradicional e obtém a posição
        long posicaoArquivo = ftell(arqDat);
        int posicaoAluno = posicaoArquivo / sizeof(Aluno);
        fwrite(&aluno, sizeof(Aluno), 1, arqDat);
        int slot = hashDivisaoEncadeado(aluno.matricula);
        printf("Gaveta %d: %s | Matricula: %d | Email: %s | Posicao .dat: %d\n", slot, aluno.nome, aluno.matricula, aluno.email, posicaoAluno);
    }
    free(vet);
    fclose(arqHash);
    fclose(arqDat);

    // Estatísticas de tempo e log
    clock_t fim = clock();
    double tempoGasto = (double)(fim - inicio) / CLOCKS_PER_SEC;
    printf("Base de dados (HASH e DAT) criada com sucesso!\n");
    printf("Total de alunos: %d\n", tam);
    printf("Tempo de criacao: %.2f ms\n", tempoGasto * 1000);
    printf("Arquivos: alunos_hash.dat e alunos.dat\n");
    char logMsg[256];
    snprintf(logMsg, sizeof(logMsg), "Criacao Base HASH+DAT: %d alunos, %.2f ms", tam, tempoGasto * 1000);
    registrarLog(logMsg);
}
// Menu de operações para a tabela hash encadeada exterior
void menuHash() {
    int op;
    // Loop principal do menu hash
    do {
        printf("\nMENU HASH\n");
        printf("1. Inicializar Hash Encadeado (Arquivo)\n");
        printf("2. Inserir Aluno (Hash Encadeado/Arquivo)\n");
        printf("3. Buscar Aluno (Hash Encadeado/Arquivo)\n");
        printf("4. Remover Aluno (Hash Encadeado/Arquivo)\n");
        printf("5. Listar Alunos (Hash Encadeado/Arquivo)\n");
        printf("0. Voltar\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &op);
    // Switch para tratar cada opção do menu hash
    switch (op) {
            case 1: { // Inicializar hash encadeado exterior
                char confirma;
                printf("ATENCAO: Esta operacao vai apagar TODOS os alunos do hash! Deseja continuar? (s/n): ");
                scanf(" %c", &confirma);
                if (confirma == 's' || confirma == 'S') {
                    FILE *arqHash = fopen("alunos_hash.dat", "wb+");
                    if (arqHash != NULL) {
                        inicializarTabelaHashEncArquivo(arqHash);
                        fclose(arqHash);
                        printf("Arquivo de hash encadeado inicializado com sucesso!\n");
                    } else {
                        printf("Erro ao criar arquivo de hash encadeado.\n");
                    }
                } else {
                    printf("Operacao cancelada. O hash NAO foi apagado.\n");
                }
                break;
            }
            case 2: { // Inserir aluno na tabela hash encadeada exterior
                Aluno aluno;
                printf("Nome: ");
                scanf("%s", aluno.nome);
                printf("Matricula: ");
                scanf("%d", &aluno.matricula);
                printf("Disciplina: ");
                scanf("%s", aluno.disciplina);
                printf("Email: ");
                scanf("%s", aluno.email);
                aluno.qtdDisciplinas = 0;
                
                // Primeiro grava no arquivo .dat para obter a posição
                FILE *arqDat = fopen("alunos.dat", "ab");
                long posicaoArquivo = -1;
                int posicaoAluno = -1;
                if (arqDat != NULL) {
                    posicaoArquivo = ftell(arqDat);
                    posicaoAluno = posicaoArquivo / sizeof(Aluno);
                    fwrite(&aluno, sizeof(Aluno), 1, arqDat);
                    fclose(arqDat);
                }
                
                // Depois insere no hash
                FILE *arqHash = fopen("alunos_hash.dat", "rb+");
                if (arqHash != NULL) {
                    inserirAlunoHashEncArquivo(arqHash, aluno);
                    int slot = hashDivisaoEncadeado(aluno.matricula);
                    printf("Gaveta %d: %s | Matricula: %d | Email: %s | Posicao .dat: %d\n", 
                           slot, aluno.nome, aluno.matricula, aluno.email, posicaoAluno);
                    fclose(arqHash);
                } else {
                    printf("Arquivo de hash encadeado nao encontrado. Inicialize primeiro!\n");
                }
                break;
            }
            case 3: { // Buscar aluno na tabela hash encadeada exterior
                int matricula;
                printf("Digite a matricula do aluno para buscar: ");
                scanf("%d", &matricula);
                FILE *arqHash = fopen("alunos_hash.dat", "rb");
                if (arqHash != NULL) {
                    Aluno aluno;
                    if (buscarAlunoHashEncArquivo(arqHash, matricula, &aluno)) {
                        int slot = hashDivisaoEncadeado(matricula);
                        int posicaoArquivo = encontrarPosicaoAlunoArquivo(matricula);
                        printf("Aluno encontrado na gaveta %d: %s | Email: %s | Posicao .dat: %d\n", 
                               slot, aluno.nome, aluno.email, posicaoArquivo);
                    } else {
                        printf("Aluno nao encontrado na tabela hash encadeada (arquivo).\n");
                    }
                    fclose(arqHash);
                } else {
                    printf("Arquivo de hash encadeado nao encontrado. Inicialize primeiro!\n");
                }
                break;
            }
            case 4: { // Remover aluno da tabela hash encadeada exterior
                int matricula;
                printf("Digite a matricula do aluno para remover: ");
                scanf("%d", &matricula);
                FILE *arqHash = fopen("alunos_hash.dat", "rb+");
                if (arqHash != NULL) {
                    removerAlunoHashEncArquivo(arqHash, matricula);
                    fclose(arqHash);
                } else {
                    printf("Arquivo de hash encadeado nao encontrado. Inicialize primeiro!\n");
                }
                break;
            }
            case 5: { // Listar todos os alunos da tabela hash encadeada exterior
                FILE *arqHash = fopen("alunos_hash.dat", "rb");
                if (arqHash != NULL) {
                    printf("\n--- Listagem de alunos na tabela hash encadeada (arquivo) ---\n");
                    for (int i = 0; i < TAMANHO_TABELA_HASH_ENC; i++) {
                        long head;
                        fseek(arqHash, i * sizeof(long), SEEK_SET);
                        fread(&head, sizeof(long), 1, arqHash);
                        long atual = head;
                        NoAluno no;
                        if (atual != OFFSET_INVALIDO) {
                            printf("Gaveta %d:\n", i);
                            while (atual != OFFSET_INVALIDO) {
                                fseek(arqHash, atual, SEEK_SET);
                                fread(&no, sizeof(NoAluno), 1, arqHash);
                                int posicaoArquivo = encontrarPosicaoAlunoArquivo(no.aluno.matricula);
                                printf("  Nome: %s | Matricula: %d | Email: %s | Posicao .dat: %d\n", 
                                       no.aluno.nome, no.aluno.matricula, no.aluno.email, posicaoArquivo);
                                atual = no.prox;
                            }
                        }
                    }
                    fclose(arqHash);
                } else {
                    printf("Arquivo de hash encadeado nao encontrado. Inicialize primeiro!\n");
                }
                break;
            }
            case 0: { // Voltar ao menu principal
                printf("Voltando ao menu principal...\n");
                break;
            }
            default: // Opção inválida
                printf("Opcao invalida!\n");
        }
    } while (op != 0);
}

// Função principal do programa
int main() {
    int escolha;
    int M = 50;

    // Loop principal do menu do sistema
    do {
        printf("\nMENU\n");
        printf("1. Cadastrar Professor\n");
        printf("2. Cadastrar Aluno\n");
        printf("3. Cadastrar Disciplina\n");
        printf("4. Listar Professores\n");
        printf("5. Listar Alunos\n");
        printf("6. Listar Disciplinas\n");
        printf("7. Editar Professor\n");
        printf("8. Editar Aluno\n");
        printf("9. Buscar Professor com busca binaria\n");
        printf("10. Buscar Aluno com busca binaria\n");
        printf("11. Buscar Professor com busca sequencial\n"); 
        printf("12. Buscar Aluno com busca sequencial\n");
        printf("13. Ordenar Alunos pelo Bubble Sort\n");
        printf("14. Ordenar Professores pelo Bubble Sort\n");
        printf("15. Ordenar Disciplina pelo Bubble Sort\n");
        printf("16. Trocar Professor de Disciplina\n");
        printf("17. Cancelar Matricula de Aluno\n"); 
        printf("18. Matricular Aluno em uma ou mais disciplina\n"); 
        printf("19. Criar Base de Dados HASH\n");
        printf("20. Gerar Particoes com Selecao Natural\n");
        printf("21. Verificar Conteudo das Particoes\n");
        printf("22. Intercalacao Otima\n");
        printf("23. Verificar Arquivo Final Ordenado\n");
        printf("24. Gerar Relatorio Detalhado\n");
        printf("25. Limpar Arquivo de Log\n");
        printf("26. MENU HASH\n");
        printf("0. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &escolha);
    // Switch para tratar cada opção do menu principal
    switch (escolha) {
            case 26: // Menu hash encadeado exterior
                menuHash();
                break;
            case 19: { // Criar base de dados inicial
                int qtd;
                printf("Digite a quantidade de alunos para a base: ");
                scanf("%d", &qtd);
                criarBase(qtd);
                break;
            }
            case 1: { // Cadastrar professor
                FILE *arqProf = fopen("professores.dat", "ab");
                if (arqProf != NULL) {
                    cadastrarProfessor(arqProf);
                    fclose(arqProf);
                } else {
                    printf("Erro ao abrir professores.dat\n");
                }
                break;
            }
            case 2: { // Cadastrar aluno
                FILE *arqAlunos = fopen("alunos.dat", "ab");
                if (arqAlunos != NULL) {
                    cadastrarAluno(arqAlunos);
                    fclose(arqAlunos);
                } else {
                    printf("Erro ao abrir alunos.dat\n");
                }
                break;
            }
            case 3: { // Cadastrar disciplina
                FILE *arqDisc = fopen("disciplinas.dat", "ab");
                if (arqDisc != NULL) {
                    cadastrarDisciplina(arqDisc);
                    fclose(arqDisc);
                } else {
                    printf("Erro ao abrir alunos.dat\n");
                }
                break;
            }
            case 4: { // Listar professores
                FILE *arqProf = fopen("professores.dat", "rb");
                if (arqProf != NULL) {
                    listarProfessores(arqProf);
                    fclose(arqProf);
                } else {
                    printf("Erro ao abrir professores.dat\n");
                }
                break;
            }
            case 5:{ // Listar alunos
                FILE *arqAlunos = fopen("alunos.dat", "rb");
                if (arqAlunos != NULL) {
                    listarAlunos(arqAlunos);
                    fclose(arqAlunos);
                } else {
                    printf("Erro ao abrir alunos.dat\n");
                }
                break;
            }
            case 6:{ // Listar disciplinas
                FILE *arqDisc = fopen("disciplinas.dat", "rb");
                if (arqDisc != NULL) {
                    listarDisciplinas(arqDisc);
                    fclose(arqDisc);
                } else {
                    printf("Erro ao abrir disciplinas.dat\n");
                }
                break;
            }
            case 7: { // Editar professor
                int matricula;
                printf("Digite a matricula do professor para editar: ");
                scanf("%d", &matricula);
                FILE *arqProf = fopen("professores.dat", "r+b");
                if (arqProf != NULL) {
                    editarProfessor(arqProf, matricula);
                    fclose(arqProf);
                } else {
                    printf("Erro ao abrir Professor.dat\n");
                }
                break;
            }
            case 8: { // Editar aluno
                int matricula;
                printf("Digite a matricula do aluno para editar: ");
                scanf("%d", &matricula);
                FILE *arqAlunos = fopen("alunos.dat", "r+b");
                if (arqAlunos != NULL) {
                    editarAluno(arqAlunos, matricula);
                    fclose(arqAlunos);
                } else {
                    printf("Erro ao abrir alunos.dat\n");
                }
                break;
            }
            case 9: { // Buscar professor (binária)
                int matricula;
                printf("Digite a matricula do professor para buscar: ");
                scanf("%d", &matricula);
                FILE *arqProf = fopen("professores.dat", "rb");
                if (arqProf != NULL) {
                    int indice = buscaBinariaProfessor(arqProf, matricula);
                    if (indice != -1) {
                        // Lê o professor encontrado do arquivo
                        Professor prof;
                        fseek(arqProf, indice * sizeof(Professor), SEEK_SET);
                        fread(&prof, sizeof(Professor), 1, arqProf);
                        printf("Professor encontrado: %s\n", prof.nome);
                    } else {
                        printf("Professor nao encontrado.\n");
                    }
                    fclose(arqProf);
                } else {
                    printf("Erro ao abrir professores.dat\n");
                }
                break;
            }
            case 10: { // Buscar aluno (binária)
                int matricula;
                printf("Digite a matricula do aluno para buscar: ");
                scanf("%d", &matricula);
                FILE *arqAlunos = fopen("alunos.dat", "rb");
                if (arqAlunos != NULL) {
                    int indice = buscaBinariaAluno(arqAlunos, matricula);
                    if (indice != -1) {
                        // Lê o aluno encontrado do arquivo
                        Aluno aluno;
                        fseek(arqAlunos, indice * sizeof(Aluno), SEEK_SET);
                        fread(&aluno, sizeof(Aluno), 1, arqAlunos);
                        printf("Aluno encontrado: %s\n", aluno.nome);
                    } else {
                        printf("Aluno nao encontrado.\n");
                    }
                    fclose(arqAlunos);
                } else {
                    printf("Erro ao abrir alunos.dat\n");
                }
                break;
            }
            case 11: { // Buscar professor (sequencial)
                int matricula;
                printf("Digite a matricula do professor para buscar: ");
                scanf("%d", &matricula);
                FILE *arqProf = fopen("professores.dat", "rb");
                if (arqProf != NULL) {
                    int indice = buscaSequencialProfessor(arqProf, matricula);
                    if (indice != -1) {
                        // Lê o professor encontrado do arquivo
                        Professor prof;
                        fseek(arqProf, indice * sizeof(Professor), SEEK_SET);
                        fread(&prof, sizeof(Professor), 1, arqProf);
                        printf("Professor encontrado: %s\n", prof.nome);
                    } else {
                        printf("Professor nao encontrado.\n");
                    }
                    fclose(arqProf);
                } else {
                    printf("Erro ao abrir professores.dat\n");
                }
                break;
            }  
                
            case 12: { // Buscar aluno (sequencial)
                int matricula;
                printf("Digite a matricula do aluno para buscar: ");
                scanf("%d", &matricula);
                FILE *arqAlunos = fopen("alunos.dat", "rb");
                if (arqAlunos != NULL) {
                    int indice = buscaSequencialAluno(arqAlunos, matricula);
                    if (indice != -1) {
                        // Lê o aluno encontrado do arquivo
                        Aluno aluno;
                        fseek(arqAlunos, indice * sizeof(Aluno), SEEK_SET);
                        fread(&aluno, sizeof(Aluno), 1, arqAlunos);
                        printf("Aluno encontrado: %s\n", aluno.nome);
                    } else {
                        printf("Aluno nao encontrado.\n");
                    }
                    fclose(arqAlunos);
                } else {
                    printf("Erro ao abrir alunos.dat\n");
                }
                break;
            }
            case 13: { // Ordenar alunos pelo Bubble Sort
                FILE *arqAlunos = fopen("alunos.dat", "r+b");
                if (arqAlunos != NULL) {
                    bubbleSortAlunos(arqAlunos);  // Ordena a base de dados dos alunos
                    fclose(arqAlunos);
                    printf("Base de alunos ordenada com sucesso!\n");
                } else {
                    printf("Erro ao abrir alunos.dat\n");
                }
                break;
            }
            case 14: { // Ordenar professores pelo Bubble Sort
                FILE *arqProf = fopen("professores.dat", "r+b");
                if (arqProf != NULL) {
                    bubbleSortProfessores(arqProf);  // Ordena a base de dados dos professores
                    fclose(arqProf);
                    printf("Base de professores ordenada com sucesso!\n");
                } else {
                    printf("Erro ao abrir professores.dat\n");
                }
                break;
            }
            case 15: { // Ordenar disciplinas pelo Bubble Sort
                FILE *arq = fopen("disciplinas.dat", "r+b");
                if (arq != NULL) {
                    bubbleSortDisciplina(arq);  // Ordena a base de dados das disciplinas
                    fclose(arq);
                    printf("Base de disciplinas ordenada com sucesso!\n");
                } else {
                    printf("Erro ao abrir disciplinas.dat\n");
                }
                break;
            }   
            case 16: { // Trocar professor de disciplina
                FILE *arqDisc = abrirArquivoDisciplina("disciplinas.dat", "r+b");
                if (arqDisc != NULL) {
                    // Solicita o código da disciplina e o nome do novo professor
                    printf("Digite o codigo da disciplina: ");
                    scanf("%d", &codigoDisciplina);
                    printf("Digite o nome do novo professor: ");
                    getchar();  // Limpa o buffer
                    fgets(novoProfessor, sizeof(novoProfessor), stdin);
                    novoProfessor[strcspn(novoProfessor, "\n")] = '\0';  // Remove o '\n' do final da string

                    // Chama a função para trocar o professor responsável pela disciplina
                    trocarProfessorDisciplina(codigoDisciplina, novoProfessor, arqDisc);

                    // Fecha o arquivo após a operação
                    fclose(arqDisc);
                } else {
                    printf("Erro ao abrir disciplinas.dat\n");
                }
                break;
            }                
            case 17: {  // Cancelar matrícula de aluno
                int matricula;
                printf("Digite a matrícula do aluno para cancelar: ");
                scanf("%d", &matricula);
                FILE *arqAlunos = fopen("alunos.dat", "r+b");
                if (arqAlunos != NULL) {
                    cancelarMatriculaAluno(arqAlunos, matricula);
                    fclose(arqAlunos);
                } else {
                    printf("Erro ao abrir alunos.dat\n");
                }
                break;
            } // Nova opção no menu   
            case 18: {  // Matricular aluno em uma ou mais disciplinas
                int matriculaAluno, qtdDisciplinas;
                printf("Digite a matrícula do aluno para matricular: ");
                scanf("%d", &matriculaAluno);

                printf("Digite o número de disciplinas para matricular: ");
                scanf("%d", &qtdDisciplinas);

                int codigosDisciplinas[qtdDisciplinas];

                for (int i = 0; i < qtdDisciplinas; i++) {
                    printf("Digite o código da disciplina %d: ", i + 1);
                    scanf("%d", &codigosDisciplinas[i]);
                }

                FILE *arqAlunos = fopen("alunos.dat", "r+b");
                if (arqAlunos != NULL) {
                    matricularAlunoEmDisciplina(arqAlunos, matriculaAluno, codigosDisciplinas, qtdDisciplinas);
                    fclose(arqAlunos);
                } else {
                    printf("Erro ao abrir alunos.dat\n");
                }
                break;
            }
            case 20: {  // Gerar partições com seleção natural
                int tamanhoMemoria;
                printf("Digite o tamanho da memória (número de alunos por partição): ");
                scanf("%d", &tamanhoMemoria);
                
                FILE *arqAlunos = fopen("alunos.dat", "rb");
                if (arqAlunos != NULL) {
                    gerarParticoesSelecaoNatural(arqAlunos, tamanhoMemoria);
                    fclose(arqAlunos);
                } else {
                    printf("Erro ao abrir alunos.dat\n");
                }
                break;
            }
            case 21: {  // Verificar conteúdo das partições
                verificarParticoes();
                break;
            }
            case 22: {  // Intercalação ótima
                // Criar arquivo temporário para a intercalação
                FILE *arqTemp = fopen("alunos_temp.dat", "wb");
                if (arqTemp != NULL) {
                    intercalacaoOtima(arqTemp);
                    fclose(arqTemp);
                    
                    // Substituir o arquivo original pelo arquivo ordenado
                    if (remove("alunos.dat") == 0) {
                        if (rename("alunos_temp.dat", "alunos.dat") == 0) {
                            printf("Arquivo alunos.dat foi substituido pela versao ordenada!\n");
                        } else {
                            printf("Erro ao renomear arquivo temporario.\n");
                        }
                    } else {
                        printf("Erro ao remover arquivo original.\n");
                    }
                } else {
                    printf("Erro ao criar arquivo temporario para intercalacao.\n");
                }
                break;
            }
            case 23: {  // Verificar arquivo final ordenado
                verificarArquivoOrdenado();
                break;
            }
            case 24: {  // Gerar relatório detalhado
                gerarRelatorioOperacoes();
                break;
            }
            case 25: {  // Limpar arquivo de log
                limparLog();
                break;
            }
            // Removido o case 26 do menu principal. Inicialização do hash agora é feita pelo menuHash().
            // Removidos os cases 27-30 do menu principal. Agora tudo é pelo menuHash().
            case 0:{ // Sair do programa
                printf("Saindo do programa...\n");
                break;    
            }      
        }
    } while (escolha != 0);
    // Fim do programa
    return 0;
}
