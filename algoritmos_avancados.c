#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Sala {
    char nome[40];
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// ----------------------------------------------------
// Protótipos das funções
// ----------------------------------------------------
Sala* criarSala(const char* nome);
void conectarSalas(Sala* pai, Sala* esquerda, Sala* direita);
void explorarSalas(Sala* raiz);
void liberarArvore(Sala* raiz);

int main(void) {
    // Criação dos nós da árvore

    Sala* hall        = criarSala("Hall de Entrada");
    Sala* biblioteca  = criarSala("Biblioteca");
    Sala* cozinha     = criarSala("Cozinha");
    Sala* escritorio  = criarSala("Escritorio");
    Sala* salaSecreta = criarSala("Sala Secreta");
    Sala* despensa    = criarSala("Despensa");
    Sala* sotao       = criarSala("Sotao");

    // Conectando as salas na estrutura de árvore binária
    conectarSalas(hall, biblioteca, cozinha);
    conectarSalas(biblioteca, escritorio, salaSecreta);
    conectarSalas(cozinha, despensa, sotao);
    conectarSalas(escritorio, NULL, NULL);
    conectarSalas(salaSecreta, NULL, NULL);
    conectarSalas(despensa, NULL, NULL);
    conectarSalas(sotao, NULL, NULL);

    // Iniciar exploração a partir do Hall de Entrada
    explorarSalas(hall);

    // Liberar memória da árvore (opcional, mas boa prática)
    liberarArvore(hall);

    return 0;
}

// ----------------------------------------------------
// criarSala
//  - Aloca dinamicamente uma nova sala
//  - Copia o nome informado
//  - Inicializa os ponteiros esquerda/direita com NULL
// ----------------------------------------------------
Sala* criarSala(const char* nome) {
    Sala* nova = (Sala*)malloc(sizeof(Sala));
    if (!nova) {
        printf("Erro ao alocar memoria para a sala.\n");
        exit(1);
    }
    strncpy(nova->nome, nome, sizeof(nova->nome));
    nova->nome[sizeof(nova->nome) - 1] = '\0';
    nova->esquerda = NULL;
    nova->direita  = NULL;
    return nova;
}

// ----------------------------------------------------
// conectarSalas
//  - Define quais salas ficam à esquerda e à direita
//    de uma sala pai.
// ----------------------------------------------------
void conectarSalas(Sala* pai, Sala* esquerda, Sala* direita) {
    if (pai == NULL) return;
    pai->esquerda = esquerda;
    pai->direita  = direita;
}

// ----------------------------------------------------
// explorarSalas
//  - Permite que o jogador navegue pela árvore da mansão.
//  - Controles:
//        e -> ir para a sala à esquerda
//        d -> ir para a sala à direita
//        s -> sair da exploração
//  - Exibe o nome da sala a cada movimento.
// ----------------------------------------------------
void explorarSalas(Sala* raiz) {
    Sala* atual = raiz;
    char comando;

    if (atual == NULL) {
        printf("Nao ha salas na mansao.\n");
        return;
    }

    printf("\n=== DETECTIVE QUEST - EXPLORACAO DA MANSAO ===\n");
    printf("Use 'e' (esquerda), 'd' (direita) e 's' (sair).\n\n");

    do {
        // Exibe sala atual
        printf("Voce esta em: %s\n", atual->nome);

        // Indica caminhos disponíveis a partir da sala atual
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

        printf("  s - Sair da mansao\n");
        printf("Escolha (e/d/s): ");
        if (scanf(" %c", &comando) != 1) {
            return;
        }

        switch (comando) {
            case 'e':
            case 'E':
                if (atual->esquerda != NULL) {
                    atual = atual->esquerda;
                } else {
                    printf(">> Nao ha sala a esquerda daqui.\n");
                }
                break;

            case 'd':
            case 'D':
                if (atual->direita != NULL) {
                    atual = atual->direita;
                } else {
                    printf(">> Nao ha sala a direita daqui.\n");
                }
                break;

            case 's':
            case 'S':
                printf("\nVoce decidiu encerrar a exploracao da mansao.\n");
                break;

            default:
                printf("Comando invalido. Use 'e', 'd' ou 's'.\n");
        }

        printf("\n");

    } while (comando != 's' && comando != 'S');
}

// ----------------------------------------------------
// liberarArvore
//  - Libera recursivamente todos os nós (salas) da árvore.
// ----------------------------------------------------
void liberarArvore(Sala* raiz) {
    if (raiz == NULL) return;
    liberarArvore(raiz->esquerda);
    liberarArvore(raiz->direita);
    free(raiz);
}

    // 🔍 Nível Aventureiro: Armazenamento de Pistas com Árvore de Busca
    //
    // - Crie uma struct Pista com campo texto (string).
    // - Crie uma árvore binária de busca (BST) para inserir as pistas coletadas.
    // - Ao visitar salas específicas, adicione pistas automaticamente com inserirBST().
    // - Implemente uma função para exibir as pistas em ordem alfabética (emOrdem()).
    // - Utilize alocação dinâmica e comparação de strings (strcmp) para organizar.
    // - Não precisa remover ou balancear a árvore.
    // - Use funções para modularizar: inserirPista(), listarPistas().
    // - A árvore de pistas deve ser exibida quando o jogador quiser revisar evidências.

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
