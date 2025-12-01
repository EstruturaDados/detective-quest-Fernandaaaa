#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ----------------------------------------------------
// Struct Sala (nó da árvore da mansão)
// ----------------------------------------------------
typedef struct Sala {
    char nome[40];
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// ----------------------------------------------------
// Struct PistaNode (nó da árvore de pistas - BST)
// Cada nó contém o texto de uma pista e ponteiros
// para esquerda e direita.
// ----------------------------------------------------
typedef struct PistaNode {
    char texto[100];
    struct PistaNode *esq;
    struct PistaNode *dir;
} PistaNode;

// ----------------------------------------------------
// Protótipos das funções relacionadas às salas
// ----------------------------------------------------
Sala* criarSala(const char* nome);
void explorarSalas(Sala* raiz, PistaNode** arvorePistas);
void liberarArvoreSalas(Sala* raiz);

// ----------------------------------------------------
// Protótipos das funções relacionadas às pistas (BST)
// ----------------------------------------------------
PistaNode* inserirPistaBST(PistaNode* raiz, const char* texto);
void inserirPista(PistaNode** raiz, const char* texto);
void emOrdem(PistaNode* raiz);
void listarPistas(PistaNode* raiz);
void liberarArvorePistas(PistaNode* raiz);

// Função auxiliar: coleta pista ao entrar em certas salas
void coletarPistaNaSala(Sala* sala, PistaNode** arvorePistas);

// ====================================================
//                         MAIN
// ====================================================
int main(void) {
    // Montagem do mapa da mansão

    Sala* hall         = criarSala("Hall de Entrada");
    Sala* salaEstar    = criarSala("Sala de Estar");
    Sala* cozinha      = criarSala("Cozinha");
    Sala* biblioteca   = criarSala("Biblioteca");
    Sala* jardim       = criarSala("Jardim");
    Sala* despensa     = criarSala("Despensa");
    Sala* garagem      = criarSala("Garagem");

    // Ligando os ponteiros (criando a árvore binária)
    hall->esquerda      = salaEstar;
    hall->direita       = cozinha;

    salaEstar->esquerda = biblioteca;
    salaEstar->direita  = jardim;

    cozinha->esquerda   = despensa;
    cozinha->direita    = garagem;

    biblioteca->esquerda = NULL;
    biblioteca->direita  = NULL;

    jardim->esquerda = NULL;
    jardim->direita  = NULL;

    despensa->esquerda = NULL;
    despensa->direita  = NULL;

    garagem->esquerda = NULL;
    garagem->direita  = NULL;

    // Raiz da árvore de pistas começa vazia
    PistaNode* arvorePistas = NULL;

    // Inicia exploração a partir do Hall de Entrada
    explorarSalas(hall, &arvorePistas);

    // Ao final, mostra todas as pistas coletadas
    printf("\n=== RESUMO DAS PISTAS COLETADAS ===\n");
    listarPistas(arvorePistas);

    // Libera memórias
    liberarArvoreSalas(hall);
    liberarArvorePistas(arvorePistas);

    return 0;
}

// ====================================================
//            IMPLEMENTAÇÃO - ÁRVORE DE SALAS
// ====================================================

// criarSala()
//  - Cria dinamicamente uma sala da mansão.
//  - Copia o nome informado e zera ponteiros esquerda/direita.
Sala* criarSala(const char* nome) {
    Sala* nova = (Sala*)malloc(sizeof(Sala));
    if (nova == NULL) {
        printf("Erro ao alocar memoria para a sala.\n");
        exit(1);
    }

    strncpy(nova->nome, nome, sizeof(nova->nome));
    nova->nome[sizeof(nova->nome) - 1] = '\0';

    nova->esquerda = NULL;
    nova->direita  = NULL;
    return nova;
}

// explorarSalas()
//  - Permite a navegação do jogador pela árvore de salas.
//  - Comandos:
//       'e' -> ir para a sala à esquerda
//       'd' -> ir para a sala à direita
//       'p' -> listar pistas coletadas até o momento
//       's' -> sair da mansão
//  - Ao entrar em certas salas, coleta automaticamente uma pista.
void explorarSalas(Sala* raiz, PistaNode** arvorePistas) {
    if (raiz == NULL) {
        printf("Nao ha salas na mansao.\n");
        return;
    }

    Sala* atual = raiz;
    char comando;

    printf("\n=== DETECTIVE QUEST - EXPLORACAO DA MANSAO ===\n");
    printf("Comandos:\n");
    printf("  e - ir para a esquerda\n");
    printf("  d - ir para a direita\n");
    printf("  p - revisar pistas coletadas\n");
    printf("  s - sair da mansao\n\n");

    while (1) {
        // Exibe a sala atual
        printf("Voce esta em: %s\n", atual->nome);

        // Ao entrar na sala atual, coleta pista se existir para ela
        coletarPistaNaSala(atual, arvorePistas);

        // Caminhos disponíveis
        printf("Caminhos disponiveis:\n");
        if (atual->esquerda != NULL) {
            printf("  e - Ir para a esquerda (%s)\n", atual->esquerda->nome);
        } else {
            printf("  e - (nenhuma sala a esquerda)\n");
        }

        if (atual->direita != NULL) {
            printf("  d - Ir para a direita (%s)\n", atual->direita->nome);
        } else {
            printf("  d - (nenhuma sala a direita)\n");
        }

        printf("Outras opcoes:\n");
        printf("  p - Revisar pistas coletadas\n");
        printf("  s - Sair da mansao\n");

        printf("Escolha (e/d/p/s): ");
        if (scanf(" %c", &comando) != 1) {
            printf("\nEntrada invalida. Encerrando.\n");
            break;
        }
        printf("\n");

        if (comando == 's' || comando == 'S') {
            printf("Voce decidiu encerrar a exploracao.\n");
            break;
        } else if (comando == 'p' || comando == 'P') {
            listarPistas(*arvorePistas);
        } else if (comando == 'e' || comando == 'E') {
            if (atual->esquerda != NULL) {
                atual = atual->esquerda;
            } else {
                printf("Nao ha sala a esquerda daqui. Tente outro caminho.\n\n");
            }
        } else if (comando == 'd' || comando == 'D') {
            if (atual->direita != NULL) {
                atual = atual->direita;
            } else {
                printf("Nao ha sala a direita daqui. Tente outro caminho.\n\n");
            }
        } else {
            printf("Comando invalido. Use 'e', 'd', 'p' ou 's'.\n\n");
        }

        // Se a sala atual for folha (sem esquerda e direita), informamos
        if (atual->esquerda == NULL && atual->direita == NULL) {
            printf("Voce chegou a um comodo sem mais caminhos (%s).\n", atual->nome);
            printf("Use 'p' para revisar as pistas ou 's' para sair.\n\n");
        }
    }
}

// liberarArvoreSalas()
//  - Libera a memória de todos os nós da árvore de salas (recursivo).
void liberarArvoreSalas(Sala* raiz) {
    if (raiz == NULL) {
        return;
    }
    liberarArvoreSalas(raiz->esquerda);
    liberarArvoreSalas(raiz->direita);
    free(raiz);
}

// ====================================================
//       IMPLEMENTAÇÃO - ÁRVORE DE PISTAS (BST)
// ====================================================

// inserirPistaBST()
//  - Insere uma nova pista em uma árvore de busca binária.
//  - Organização alfabética pelo texto da pista (strcmp).
//  - Não insere duplicatas (se texto igual já existir).
PistaNode* inserirPistaBST(PistaNode* raiz, const char* texto) {
    if (raiz == NULL) {
        PistaNode* novo = (PistaNode*)malloc(sizeof(PistaNode));
        if (novo == NULL) {
            printf("Erro ao alocar memoria para a pista.\n");
            exit(1);
        }
        strncpy(novo->texto, texto, sizeof(novo->texto));
        novo->texto[sizeof(novo->texto) - 1] = '\0';
        novo->esq = NULL;
        novo->dir = NULL;
        return novo;
    }

    int cmp = strcmp(texto, raiz->texto);
    if (cmp < 0) {
        raiz->esq = inserirPistaBST(raiz->esq, texto);
    } else if (cmp > 0) {
        raiz->dir = inserirPistaBST(raiz->dir, texto);
    } else {
        // cmp == 0 -> pista já existe, não insere duplicata
    }
    return raiz;
}

// inserirPista()
//  - Função de conveniência: recebe &raiz e chama inserirPistaBST.
void inserirPista(PistaNode** raiz, const char* texto) {
    *raiz = inserirPistaBST(*raiz, texto);
}

// emOrdem()
//  - Percorre a BST em ordem (esq, raiz, dir),
//    exibindo as pistas em ordem alfabética.
void emOrdem(PistaNode* raiz) {
    if (raiz == NULL) return;
    emOrdem(raiz->esq);
    printf("- %s\n", raiz->texto);
    emOrdem(raiz->dir);
}

// listarPistas()
//  - Exibe todas as pistas já coletadas, em ordem alfabética.
void listarPistas(PistaNode* raiz) {
    printf("\n--- PISTAS COLETADAS ---\n");
    if (raiz == NULL) {
        printf("Nenhuma pista coletada ate o momento.\n\n");
        return;
    }
    emOrdem(raiz);
    printf("------------------------\n\n");
}

// liberarArvorePistas()
//  - Libera recursivamente todos os nós da BST de pistas.
void liberarArvorePistas(PistaNode* raiz) {
    if (raiz == NULL) return;
    liberarArvorePistas(raiz->esq);
    liberarArvorePistas(raiz->dir);
    free(raiz);
}

// coletarPistaNaSala()
//  - Associa algumas salas a pistas específicas.
//  - Quando o jogador entra na sala, a pista é inserida na BST.
//  - Se a pista já existir, nada muda (por causa da lógica da BST).
void coletarPistaNaSala(Sala* sala, PistaNode** arvorePistas) {
    if (sala == NULL) return;

    // Exemplo de associação simples de sala -> pista:
    if (strcmp(sala->nome, "Biblioteca") == 0) {
        inserirPista(arvorePistas, "Paginas rasgadas de um diario na Biblioteca.");
        printf("Voce encontrou uma pista na Biblioteca!\n");
    } else if (strcmp(sala->nome, "Cozinha") == 0) {
        inserirPista(arvorePistas, "Faca com manchas suspeitas na Cozinha.");
        printf("Voce encontrou uma pista na Cozinha!\n");
    } else if (strcmp(sala->nome, "Jardim") == 0) {
        inserirPista(arvorePistas, "Pegadas recentes no Jardim molhado.");
        printf("Voce encontrou uma pista no Jardim!\n");
    } else if (strcmp(sala->nome, "Garagem") == 0) {
        inserirPista(arvorePistas, "Carro com motor quente na Garagem.");
        printf("Voce encontrou uma pista na Garagem!\n");
    }
    // Demais salas podem não ter pista associada.
}

    // 🧠 Nível Mestre: Relacionamento de Pistas com Suspeitos via Hash
    //
    // - Crie uma struct Suspeito contendo nome e lista de pistas associadas.
    // - Crie uma tabela hash (ex: array de ponteiros para listas encadeadas).
    // - A chave pode ser o nome do suspeito ou derivada das pistas.
    // - Implemente uma função inserirHash(pista, suspeito) para registrar relações.
    // - Crie uma função para mostrar todos os suspeitos e suas respectivas pistas.
    // - Adicione um contador para saber qual suspeito foi mais citado.
    // - Exiba ao final o “suspeito mais provável” baseado nas pistas coletadas.
    // - Para hashing simples, pode usar soma dos valores ASCII do nome ou primeira letra.
    // - Em caso de colisão, use lista encadeada para tratar.
    // - Modularize com funções como inicializarHash(), buscarSuspeito(), listarAssociacoes().


