#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------------------------- CONSTANTES ----------------------------

#define HASH_TAM 7  // tamanho da tabela hash (pode ser pequeno para teste)

// ---------------------------- STRUCTS -------------------------------

// Nó da árvore de salas (mapa da mansão)
typedef struct Sala {
    char nome[40];
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// Nó da árvore de pistas (BST)
typedef struct PistaNode {
    char texto[100];
    struct PistaNode *esq;
    struct PistaNode *dir;
} PistaNode;

// Nó da lista de pistas ligada a um suspeito
typedef struct PistaListaNode {
    char texto[100];
    struct PistaListaNode *prox;
} PistaListaNode;

// Nó da lista encadeada da tabela hash de suspeitos
typedef struct SuspeitoNode {
    char nome[40];
    PistaListaNode *pistas;    // lista de pistas associadas a esse suspeito
    int contadorPistas;        // quantas pistas apontam para esse suspeito
    struct SuspeitoNode *prox; // próximo na lista (colisão)
} SuspeitoNode;

// ---------------------- PROTÓTIPOS: SALAS --------------------------

Sala* criarSala(const char* nome);
void explorarSalas(Sala* raiz,
                   PistaNode** arvorePistas,
                   SuspeitoNode* tabelaHash[],
                   int hashTam);
void liberarArvoreSalas(Sala* raiz);

// ---------------------- PROTÓTIPOS: PISTAS (BST) -------------------

PistaNode* inserirPistaBST(PistaNode* raiz, const char* texto);
void inserirPista(PistaNode** raiz, const char* texto);
void emOrdem(PistaNode* raiz);
void listarPistas(PistaNode* raiz);
void liberarArvorePistas(PistaNode* raiz);

// Coleta pista vinculada à sala e registra suspeito
void coletarPistaNaSala(Sala* sala,
                        PistaNode** arvorePistas,
                        SuspeitoNode* tabelaHash[],
                        int hashTam);

// ---------------------- PROTÓTIPOS: HASH SUSPEITOS -----------------

void inicializarHash(SuspeitoNode* tabela[], int tam);
unsigned int funcaoHash(const char* nome);
SuspeitoNode* buscarSuspeito(SuspeitoNode* tabela[], int tam, const char* nome);
void inserirHash(SuspeitoNode* tabela[], int tam,
                 const char* nomeSuspeito, const char* textoPista);
void listarAssociacoes(SuspeitoNode* tabela[], int tam);
SuspeitoNode* suspeitoMaisProvavel(SuspeitoNode* tabela[], int tam);
void liberarHash(SuspeitoNode* tabela[], int tam);

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

    hall->esquerda      = salaEstar;
    hall->direita       = cozinha;

    salaEstar->esquerda = biblioteca;
    salaEstar->direita  = jardim;

    cozinha->esquerda   = despensa;
    cozinha->direita    = garagem;

    biblioteca->esquerda = biblioteca->direita = NULL;
    jardim->esquerda     = jardim->direita     = NULL;
    despensa->esquerda   = despensa->direita   = NULL;
    garagem->esquerda    = garagem->direita    = NULL;

    // Raiz da BST de pistas começa vazia
    PistaNode* arvorePistas = NULL;

    // Tabela hash de suspeitos
    SuspeitoNode* tabelaHash[HASH_TAM];
    inicializarHash(tabelaHash, HASH_TAM);

    // Exploração da mansão (coleta pistas e preenche hash de suspeitos)
    explorarSalas(hall, &arvorePistas, tabelaHash, HASH_TAM);

    // Resumo final
    printf("\n=== RESUMO DAS PISTAS COLETADAS ===\n");
    listarPistas(arvorePistas);

    printf("\n=== SUSPEITOS E SUAS PISTAS ===\n");
    listarAssociacoes(tabelaHash, HASH_TAM);

    SuspeitoNode* principal = suspeitoMaisProvavel(tabelaHash, HASH_TAM);
    if (principal != NULL) {
        printf("\n>>> SUSPEITO MAIS PROVAVEL: %s (%d pista(s)) <<<\n",
               principal->nome, principal->contadorPistas);
    } else {
        printf("\nNenhum suspeito foi associado a pistas.\n");
    }

    // Libera memória
    liberarArvoreSalas(hall);
    liberarArvorePistas(arvorePistas);
    liberarHash(tabelaHash, HASH_TAM);

    return 0;
}

// ====================================================
//            IMPLEMENTAÇÃO - ÁRVORE DE SALAS
// ====================================================

// cria uma sala dinamicamente
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

// explora a mansão, permitindo navegação e coleta de pistas
void explorarSalas(Sala* raiz,
                   PistaNode** arvorePistas,
                   SuspeitoNode* tabelaHash[],
                   int hashTam) {
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
    printf("  u - listar suspeitos e pistas\n");
    printf("  s - sair da mansao\n\n");

    while (1) {
        printf("Voce esta em: %s\n", atual->nome);

        // Coleta pista relacionada à sala (se houver)
        coletarPistaNaSala(atual, arvorePistas, tabelaHash, hashTam);

        printf("Caminhos disponiveis:\n");
        if (atual->esquerda != NULL)
            printf("  e - Ir para a esquerda (%s)\n", atual->esquerda->nome);
        else
            printf("  e - (nenhuma sala a esquerda)\n");

        if (atual->direita != NULL)
            printf("  d - Ir para a direita (%s)\n", atual->direita->nome);
        else
            printf("  d - (nenhuma sala a direita)\n");

        printf("Outras opcoes:\n");
        printf("  p - Revisar pistas coletadas\n");
        printf("  u - Ver suspeitos e pistas\n");
        printf("  s - Sair da mansao\n");

        printf("Escolha (e/d/p/u/s): ");
        if (scanf(" %c", &comando) != 1) {
            printf("Entrada invalida.\n");
            break;
        }
        printf("\n");

        if (comando == 's' || comando == 'S') {
            printf("Voce decidiu encerrar a exploracao.\n");
            break;
        } else if (comando == 'p' || comando == 'P') {
            listarPistas(*arvorePistas);
        } else if (comando == 'u' || comando == 'U') {
            listarAssociacoes(tabelaHash, hashTam);
        } else if (comando == 'e' || comando == 'E') {
            if (atual->esquerda != NULL)
                atual = atual->esquerda;
            else
                printf("Nao ha sala a esquerda daqui.\n\n");
        } else if (comando == 'd' || comando == 'D') {
            if (atual->direita != NULL)
                atual = atual->direita;
            else
                printf("Nao ha sala a direita daqui.\n\n");
        } else {
            printf("Comando invalido. Use e/d/p/u/s.\n\n");
        }

        if (atual->esquerda == NULL && atual->direita == NULL) {
            printf("Voce chegou a um comodo sem mais caminhos (%s).\n", atual->nome);
            printf("Use 'p' para revisar as pistas, 'u' para ver suspeitos ou 's' para sair.\n\n");
        }
    }
}

// libera recursivamente a árvore de salas
void liberarArvoreSalas(Sala* raiz) {
    if (raiz == NULL) return;
    liberarArvoreSalas(raiz->esquerda);
    liberarArvoreSalas(raiz->direita);
    free(raiz);
}

// ====================================================
//       IMPLEMENTAÇÃO - ÁRVORE DE PISTAS (BST)
// ====================================================

// insere uma pista na BST de forma ordenada
PistaNode* inserirPistaBST(PistaNode* raiz, const char* texto) {
    if (raiz == NULL) {
        PistaNode* novo = (PistaNode*)malloc(sizeof(PistaNode));
        if (novo == NULL) {
            printf("Erro ao alocar memoria para a pista.\n");
            exit(1);
        }
        strncpy(novo->texto, texto, sizeof(novo->texto));
        novo->texto[sizeof(novo->texto) - 1] = '\0';
        novo->esq = novo->dir = NULL;
        return novo;
    }

    int cmp = strcmp(texto, raiz->texto);
    if (cmp < 0) {
        raiz->esq = inserirPistaBST(raiz->esq, texto);
    } else if (cmp > 0) {
        raiz->dir = inserirPistaBST(raiz->dir, texto);
    } else {
        // cmp == 0 -> pista duplicada, não insere novamente
    }
    return raiz;
}

void inserirPista(PistaNode** raiz, const char* texto) {
    *raiz = inserirPistaBST(*raiz, texto);
}

// percurso em ordem para exibir pistas em ordem alfabética
void emOrdem(PistaNode* raiz) {
    if (raiz == NULL) return;
    emOrdem(raiz->esq);
    printf("- %s\n", raiz->texto);
    emOrdem(raiz->dir);
}

// lista todas as pistas coletadas
void listarPistas(PistaNode* raiz) {
    printf("--- PISTAS COLETADAS ---\n");
    if (raiz == NULL) {
        printf("Nenhuma pista coletada ate o momento.\n\n");
        return;
    }
    emOrdem(raiz);
    printf("------------------------\n\n");
}

// libera recursivamente a BST de pistas
void liberarArvorePistas(PistaNode* raiz) {
    if (raiz == NULL) return;
    liberarArvorePistas(raiz->esq);
    liberarArvorePistas(raiz->dir);
    free(raiz);
}

// associa certas salas a pistas + suspeitos
void coletarPistaNaSala(Sala* sala,
                        PistaNode** arvorePistas,
                        SuspeitoNode* tabelaHash[],
                        int hashTam) {
    if (sala == NULL) return;

    // Exemplos de associações sala -> (pista, suspeito)
    if (strcmp(sala->nome, "Biblioteca") == 0) {
        const char* texto = "Paginas rasgadas de um diario na Biblioteca.";
        const char* suspeito = "Sr. Black";
        inserirPista(arvorePistas, texto);
        inserirHash(tabelaHash, hashTam, suspeito, texto);
        printf("Voce encontrou uma pista na Biblioteca!\n");
    } else if (strcmp(sala->nome, "Cozinha") == 0) {
        const char* texto = "Faca com manchas suspeitas na Cozinha.";
        const char* suspeito = "Sra. White";
        inserirPista(arvorePistas, texto);
        inserirHash(tabelaHash, hashTam, suspeito, texto);
        printf("Voce encontrou uma pista na Cozinha!\n");
    } else if (strcmp(sala->nome, "Jardim") == 0) {
        const char* texto = "Pegadas recentes no Jardim molhado.";
        const char* suspeito = "Coronel Mostarda";
        inserirPista(arvorePistas, texto);
        inserirHash(tabelaHash, hashTam, suspeito, texto);
        printf("Voce encontrou uma pista no Jardim!\n");
    } else if (strcmp(sala->nome, "Garagem") == 0) {
        const char* texto = "Carro com motor quente na Garagem.";
        const char* suspeito = "Srta. Scarlet";
        inserirPista(arvorePistas, texto);
        inserirHash(tabelaHash, hashTam, suspeito, texto);
        printf("Voce encontrou uma pista na Garagem!\n");
    }
    // Salas sem pista associada não fazem nada aqui.
}

// ====================================================
//       IMPLEMENTAÇÃO - TABELA HASH DE SUSPEITOS
// ====================================================

// inicializa a tabela hash com NULL em todas as posições
void inicializarHash(SuspeitoNode* tabela[], int tam) {
    for (int i = 0; i < tam; i++) {
        tabela[i] = NULL;
    }
}

// função de hashing simples: soma dos valores ASCII do nome % tam
unsigned int funcaoHash(const char* nome) {
    unsigned int soma = 0;
    while (*nome) {
        soma += (unsigned char)(*nome);
        nome++;
    }
    return soma;
}

// busca um suspeito pelo nome dentro da tabela
SuspeitoNode* buscarSuspeito(SuspeitoNode* tabela[], int tam, const char* nome) {
    unsigned int h = funcaoHash(nome) % tam;
    SuspeitoNode* atual = tabela[h];
    while (atual != NULL) {
        if (strcmp(atual->nome, nome) == 0) {
            return atual;
        }
        atual = atual->prox;
    }
    return NULL;
}

// insere uma relação suspeito <-> pista na tabela hash
void inserirHash(SuspeitoNode* tabela[], int tam,
                 const char* nomeSuspeito, const char* textoPista) {

    unsigned int h = funcaoHash(nomeSuspeito) % tam;

    // Tenta encontrar suspeito já existente
    SuspeitoNode* s = buscarSuspeito(tabela, tam, nomeSuspeito);

    // Se não existir, cria novo suspeito e coloca na cabeça da lista
    if (s == NULL) {
        s = (SuspeitoNode*)malloc(sizeof(SuspeitoNode));
        if (s == NULL) {
            printf("Erro ao alocar memoria para suspeito.\n");
            exit(1);
        }
        strncpy(s->nome, nomeSuspeito, sizeof(s->nome));
        s->nome[sizeof(s->nome) - 1] = '\0';
        s->pistas = NULL;
        s->contadorPistas = 0;
        s->prox = tabela[h];
        tabela[h] = s;
    }

    // Verifica se a pista já está associada ao suspeito
    PistaListaNode* p = s->pistas;
    while (p != NULL) {
        if (strcmp(p->texto, textoPista) == 0) {
            // Pista já registrada para esse suspeito, não duplica
            return;
        }
        p = p->prox;
    }

    // Insere nova pista no início da lista de pistas do suspeito
    PistaListaNode* novaP = (PistaListaNode*)malloc(sizeof(PistaListaNode));
    if (novaP == NULL) {
        printf("Erro ao alocar memoria para pista do suspeito.\n");
        exit(1);
    }
    strncpy(novaP->texto, textoPista, sizeof(novaP->texto));
    novaP->texto[sizeof(novaP->texto) - 1] = '\0';
    novaP->prox = s->pistas;
    s->pistas = novaP;

    s->contadorPistas++;
}

// lista todos os suspeitos e suas pistas
void listarAssociacoes(SuspeitoNode* tabela[], int tam) {
    int temAlgum = 0;
    for (int i = 0; i < tam; i++) {
        SuspeitoNode* s = tabela[i];
        while (s != NULL) {
            temAlgum = 1;
            printf("Suspeito: %s (%d pista(s))\n", s->nome, s->contadorPistas);
            PistaListaNode* p = s->pistas;
            while (p != NULL) {
                printf("  - %s\n", p->texto);
                p = p->prox;
            }
            printf("\n");
            s = s->prox;
        }
    }
    if (!temAlgum) {
        printf("Nenhum suspeito associado a pistas ainda.\n\n");
    }
}

// encontra o suspeito mais citado (com maior contadorPistas)
SuspeitoNode* suspeitoMaisProvavel(SuspeitoNode* tabela[], int tam) {
    SuspeitoNode* melhor = NULL;
    for (int i = 0; i < tam; i++) {
        SuspeitoNode* s = tabela[i];
        while (s != NULL) {
            if (melhor == NULL || s->contadorPistas > melhor->contadorPistas) {
                melhor = s;
            }
            s = s->prox;
        }
    }
    return melhor;
}

// libera toda a tabela hash (suspeitos + listas de pistas)
void liberarHash(SuspeitoNode* tabela[], int tam) {
    for (int i = 0; i < tam; i++) {
        SuspeitoNode* s = tabela[i];
        while (s != NULL) {
            SuspeitoNode* proxS = s->prox;

            PistaListaNode* p = s->pistas;
            while (p != NULL) {
                PistaListaNode* proxP = p->prox;
                free(p);
                p = proxP;
            }

            free(s);
            s = proxS;
        }
        tabela[i] = NULL;
    }
}