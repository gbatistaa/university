#include <stdio.h>
#include <stdlib.h>

#define NEGRO 0
#define RUBRO 1

typedef struct no {
    int valor;
    struct no *pai, *esq, *dir;
    // cor == 1 para rubro e 0 para negro
    int cor;
} tipoNo;

typedef struct arvore {
    tipoNo *raiz;
    int quant;
} tipoArvore;

void inicializarArvore(tipoArvore *arvore) {
    arvore->raiz = NULL;
}

void rotacaoEsquerda(tipoArvore *arvore, tipoNo *no) {
    tipoNo *dir = no->dir;
    no->dir = dir->esq;
    
    if (dir->esq != NULL) {
        dir->esq->pai = no;
    }
    
    dir->pai = no->pai;
    
    if (no->pai == NULL) {
        arvore->raiz = dir;
    } else if (no == no->pai->esq) {
        no->pai->esq = dir;
    } else {
        no->pai->dir = dir;
    }
    
    dir->esq = no;
    no->pai = dir;
}

void rotacaoDireita(tipoArvore *arvore, tipoNo *no) {
    tipoNo *esq = no->esq; // Filho da esquerda do nó
    
    no->esq = esq->dir; // Filho direito de esq vira o filho esquerdo do nó

    // Se o filho direito de esq existir, atualize seu pai para nó
    if (esq->dir != NULL) {
        esq->dir->pai = no;
    }

    esq->pai = no->pai; // pai de esq vira pai de nó

    // Se nó for a raiz da árvore, esq se torna raiz
    if (no->pai == NULL) {
        arvore->raiz = esq;
    }
    else if (no == no->pai->esq) {
        no->pai->esq = esq; // Se nó era o filho esquerdo, agora o filho esquerdo será esq
    }
    else {
        no->pai->dir = esq; // Se nó era o filho direito, agora o filho direito será esq
    }

    // Nó vira filho direito de esq
    esq->dir = no;
    
    // e esq vira pai de no
    no->pai = esq;
}


void verificarCorrecoesInsercao(tipoArvore *arvore, tipoNo *no) {
    // Caso raiz seja Rubro, vira negra
    if (no == arvore->raiz && no->cor == RUBRO) {
        printf("Raiz (%d) virou negro\n\n", no->valor);
        no->cor = 0;
        return;
    }

    // Caso 1: Se o pai do nó é negro, não há violação
    if (no->pai == NULL || no->pai->cor == NEGRO) {
        printf("Não precisa de correção!\n");
        return;
    }

    // Caso 2 e 3: Verificar o tio
    tipoNo *avo = no->pai->pai;

    // Não tem avo, o pai do nó é a raiz
    if (avo == NULL) {
        return;
    }

    // se o pai do nó é o filho esquerdo do avo, o tio é o filho direito do avo
    // se o pai do nó é o filho direito do avo, o tio é o filho esquerdo do avo
    tipoNo *tio = (no->pai == avo->esq) ? avo->dir : avo->esq;

    // Caso 2: O pai e o tio são rubros
    if (tio != NULL && tio->cor == RUBRO) {
        printf("O pai (%d) de (%d) virou negro\n", no->pai->valor, no->valor);
        no->pai->cor = NEGRO; // o pai vira negro

        printf("O tio (%d) de (%d) virou negro\n", tio->valor, no->valor);
        tio->cor = NEGRO; // o tio vira negro

        printf("O avô (%d) de (%d) virou rubro\n\n", avo->valor, no->valor);
        avo->cor = RUBRO; // o avo vira rubro

        verificarCorrecoesInsercao(arvore, avo); // Continuar verificando o avô
    } else {
        // Caso 3: O tio é negro (ou NULL)

        // nó é filho da direita do pai e
        // pai é filho da esquerda do avo
        if (no == no->pai->dir && no->pai == avo->esq) {
            // Rotação para a esquerda no pai
            printf("Rotacionando pai (%d) para esquerda\n", no->pai->valor);
            rotacaoEsquerda(arvore, no->pai);
            no = no->esq;  // Ajuste do novo nó
        } else if (no == no->pai->esq && no->pai == avo->dir) {
            // Rotação para a direita no pai
            printf("Rotacionando pai (%d) para direita\n", no->pai->valor);
            rotacaoDireita(arvore, no->pai);
            no = no->dir;  // Ajuste do novo nó
        }

        // Caso 3 (continuação): Rotação simples
        if (no == no->pai->esq) {
            // se o nó é filho da esquerda do pai, rotação para a direita no avo
            printf("Rotacionando avô (%d) para direita\n", avo->valor);
            rotacaoDireita(arvore, avo);
        } else {
            // se o nó é filho da direita do pai, rotação para a esquerda no avo
            printf("Rotacionando avô (%d) para esquerda\n", avo->valor);
            rotacaoEsquerda(arvore, avo);
        }

        // Ajustar as cores após a rotação
        printf("Pai (%d) virou negro\n", no->pai->valor);
        no->pai->cor = NEGRO;

        printf("Avô (%d) virou rubro\n", avo->valor);
        avo->cor = RUBRO;
    }
}

tipoNo* inserirNo(tipoNo* raiz, tipoNo* novo) {
    if (raiz == NULL) {
        return novo;
    }

    if (novo->valor < raiz->valor) {
        printf("Inserindo (%d) a esquerda de (%d)\n", novo->valor, raiz->valor);
        raiz->esq = inserirNo(raiz->esq, novo);
        raiz->esq->pai = raiz;
    } else if (novo->valor > raiz->valor) {
        printf("Inserindo (%d) a direita de (%d)\n", novo->valor, raiz->valor);
        raiz->dir = inserirNo(raiz->dir, novo);
        raiz->dir->pai = raiz;
    }

    return raiz;
}

void criarNo(tipoArvore *arvore, int valor) {
    tipoNo *novo = (tipoNo*) malloc(sizeof(tipoNo));
    novo->valor = valor;
    novo->esq = NULL;
    novo->dir = NULL;
    novo->cor = RUBRO;
    novo->pai = NULL;

    printf("\n\nCriando nó (%d) RUBRO\n", valor);

    if (arvore->raiz == NULL) {
        printf("Nó (%d) é raiz\n", novo->valor);
    }

    arvore->raiz = inserirNo(arvore->raiz, novo);

    verificarCorrecoesInsercao(arvore, novo);

    return;
}

tipoNo* maximo(tipoNo* no) {
    printf("Entrando no nó (%d)\n", no->valor);
    // Encontra o maior valor na subárvore à esquerda
    while (no->dir != NULL) {
        no = no->dir;
    }
    return no;
}

tipoNo* minimo(tipoNo* no) {
    printf("Entrando no nó (%d)\n", no->valor);
    // Encontra o menor valor na subárvore à direita
    while (no->esq != NULL) {
        no = no->esq;
    }
    return no;
}

void substituirNo(tipoArvore *arvore, tipoNo *noVelho, tipoNo *noNovo) {
    // caso 1: substituir por nulo
    if (noNovo == NULL) {
        // se noVelho é raiz
        if (noVelho->pai == NULL) {
            arvore->raiz = NULL;
            printf("A raiz (%d) foi excluída!\n\n", noVelho->valor);
            return;
        }

        if (noVelho == noVelho->pai->esq) {
            // no velho é filho esquerdo do pai
            noVelho->pai->esq = NULL;
        } else {
            // no velho é filho direito do pai
            noVelho->pai->dir = NULL;
        }
        printf("Substituindo (%d) por nulo!\n\n", noVelho->valor);
        return;
    }

    // caso 2: no velho é raiz
    if (noVelho->pai == NULL) {
        arvore->raiz = noNovo;
        noNovo->pai = NULL;
        noNovo->esq = NULL;
        noNovo->dir = NULL;
        printf("Substituindo (%d) por (%d)!\n\n", noVelho->valor, noNovo->valor);
        return;
    }

    // caso 3: no Velho é filho esquerdo do pai
    if (noVelho == noVelho->pai->esq) {
        noVelho->pai->esq = noNovo;
    }
    // caso 4: no velho é filho direito do pai
    else if (noVelho == noVelho->pai->dir) {
        noVelho->pai->dir = noNovo;
    }

    noNovo->esq = noVelho->esq;
    noNovo->dir = noVelho->dir;
    noNovo->pai = noVelho->pai;

    // verificações de no nulo
    if (noNovo == noVelho->esq) {
        noNovo->esq = NULL;
    }

    if (noNovo == noVelho->dir) {
        noNovo->dir = NULL;
    }

    printf("Substituindo (%d) por (%d)!\n\n", noVelho->valor, noNovo->valor);
}

tipoNo* buscarNo(tipoNo *raiz, int valor) {
    // Se o nó for NULL, o valor não está na árvore
    if (raiz == NULL) {
        return NULL;
    }

    // Se o valor procurado for menor, vá para a subárvore esquerda
    if (valor < raiz->valor) {
        return buscarNo(raiz->esq, valor);
    }
    // Se o valor procurado for maior, vá para a subárvore direita
    else if (valor > raiz->valor) {
        return buscarNo(raiz->dir, valor);
    }

    // Se for igual, o nó foi encontrado
    return raiz;
}

void verificarCorrecoesRemocao(tipoArvore *arvore, tipoNo *no) {
    if (no == NULL && no == arvore->raiz) {
        printf("A árvore foi zerada\n");
        return;
    }

    while (no != arvore->raiz && no->cor == NEGRO) {
        // Nó é filho esquerdo
        if (no == no->pai->esq) {
            printf("O substituto é filho esquerdo\n");
            tipoNo *irmao = no->pai->dir;

            // Caso 1: O irmão do nó é rubro
            if (irmao->cor == RUBRO) {
                printf("O irmão (%d) de (%d) é rubro\n", irmao->valor, no->valor);
                printf("O irmão vira negro\n");
                printf("O pai vira rubro\n");
                irmao->cor = NEGRO;
                no->pai->cor = RUBRO;
                printf("Rotacionando o pai (%d) para esquerda\n", no->pai->valor);
                rotacaoEsquerda(arvore, no->pai);
                irmao = no->pai->dir;
                printf("irmao virou (%d)\n", irmao->valor);
            }

            // Caso 2: Os filhos do irmão são negros
            if ((irmao->esq == NULL || irmao->esq->cor == NEGRO) &&
                (irmao->dir == NULL || irmao->dir->cor == NEGRO)) {
                printf("Os dois filhos do irmão são negros\n");
                printf("Irmao virou rubro\n");
                irmao->cor = RUBRO;
                no = no->pai;
                printf("Atualizou nó para (%d)\n", no->valor);
            } else {
                // Caso 3: O filho direito é negro
                if (irmao->dir == NULL || irmao->dir->cor == NEGRO) {
                    printf("O filho direito do irmão é negro\n");
                    if (irmao->esq != NULL) irmao->esq->cor = NEGRO;
                    printf("Filho esquerdo virou negro\n");
                    printf("Irmão virou rubro\n");
                    irmao->cor = RUBRO;
                    printf("Rotacionando irmão para direita\n");
                    rotacaoDireita(arvore, irmao);
                    irmao = no->pai->dir;
                    printf("Atualizou imão para (%d)\n", irmao->valor);
                }

                // Caso 4: Trocar cores
                irmao->cor = no->pai->cor;
                printf("Irmão copia a cor do pai (%s)\n", irmao->cor == NEGRO ? "negro" : "rubro");
                no->pai->cor = NEGRO;
                printf("Pai vira negro");
                printf("Filho direito do irmão vira negro");
                if (irmao->dir != NULL) irmao->dir->cor = NEGRO;
                printf("Rotacionando pai para esquerda\n");
                rotacaoEsquerda(arvore, no->pai);
                no = arvore->raiz;
                printf("Atualizou nó para raiz");
            }
        } else {
            printf("O substituto é filho Direito");
            // Casos simétricos para o irmão à esquerda
            tipoNo *irmao = no->pai->esq;

            if (irmao->cor == RUBRO) {
                printf("O irmão (%d) de (%d) é rubro\n", irmao->valor, no->valor);
                printf("O irmão vira negro\n");
                printf("O pai vira rubro\n");
                irmao->cor = NEGRO;
                no->pai->cor = RUBRO;
                printf("Rotacionando o pai (%d) para direita\n", no->pai->valor);
                rotacaoDireita(arvore, no->pai);
                irmao = no->pai->esq;
                printf("irmao virou (%d)\n", irmao->valor);
            }

            if ((irmao->esq == NULL || irmao->esq->cor == NEGRO) &&
                (irmao->dir == NULL || irmao->dir->cor == NEGRO)) {
                printf("Os dois filhos do irmão são negros\n");
                printf("Irmao virou rubro\n");
                irmao->cor = RUBRO;
                no = no->pai;
                printf("Atualizou nó para (%d)\n", no->valor);
            } else {
                if (irmao->esq == NULL || irmao->esq->cor == NEGRO) {
                    printf("O filho direito do irmão é negro\n");
                    if (irmao->dir != NULL) irmao->dir->cor = NEGRO;
                    printf("Filho direito virou negro\n");
                    printf("Irmão virou rubro\n");
                    irmao->cor = RUBRO;
                    printf("Rotacionando irmão para esquerda\n");
                    rotacaoEsquerda(arvore, irmao);
                    irmao = no->pai->esq;
                    printf("Atualizou imão para (%d)\n", irmao->valor);
                }

                irmao->cor = no->pai->cor;
                printf("Irmão copia a cor do pai (%s)\n", irmao->cor == NEGRO ? "negro" : "rubro");
                no->pai->cor = NEGRO;
                printf("Pai vira negro");
                printf("Filho esquerdo do irmão vira negro");
                if (irmao->esq != NULL) irmao->esq->cor = NEGRO;
                printf("Rotacionando pai para direita\n");
                rotacaoDireita(arvore, no->pai);
                no = arvore->raiz;
                printf("Atualizou nó para raiz");
            }
        }
    }

    if (no != NULL) {
        no->cor = NEGRO;
    }
    
    printf("Fim das verificações\n\n");
}

tipoNo* criarNoNegroNulo(tipoArvore *arvore) {
    tipoNo *nuloNegro = (tipoNo*) malloc(sizeof(tipoNo));
    nuloNegro->cor = NEGRO;
    nuloNegro->valor = -1;  // Valor fictício para representar nó nulo
    nuloNegro->pai = NULL;
    nuloNegro->esq = NULL;
    nuloNegro->dir = NULL;
    return nuloNegro;
}

void removerNo(tipoArvore *arvore, int valor) {
    if (arvore->raiz == NULL) {
        printf("Árvore vazia!\n");
        return;
    }

    // Buscar o nó com o valor fornecido
    tipoNo *noRemovido = buscarNo(arvore->raiz, valor);

    // Nó não encontrado
    if (noRemovido == NULL) {
        printf("Valor %d não encontrado na árvore!\n", valor);
        return;
    }

    // Imprime o valor do nó que será removido
    printf("\nNó a ser removido = %d\n", noRemovido->valor);

    // Caso 1: O nó removido é folha e vermelho
    if (noRemovido->esq == NULL && noRemovido->dir == NULL && noRemovido->cor == RUBRO) {
        printf("(%d) é folha e vermelho\n", noRemovido->valor);
        substituirNo(arvore, noRemovido, NULL);
        free(noRemovido);
        return;
    }

    // Caso 2: O nó removido é folha e negro
    if (noRemovido->esq == NULL && noRemovido->dir == NULL) {
        printf("(%d) não tem filhos\n", noRemovido->valor);
        printf("(%d) é folha e negro\n", noRemovido->valor);
        tipoNo *noFantasma = criarNoNegroNulo(arvore);
        noFantasma->pai = noRemovido->pai;
        noFantasma->esq = noRemovido->esq;
        noFantasma->dir = noRemovido->dir;
        noFantasma->cor = NEGRO;
        substituirNo(arvore, noRemovido, noFantasma);
        verificarCorrecoesRemocao(arvore, noFantasma);
        noFantasma->esq = NULL;
        noFantasma->dir = NULL;
        if (noFantasma->pai->esq == noFantasma) {
            noFantasma->pai->esq = NULL;
        } else {
            noFantasma->pai->dir = NULL;
        }
        noFantasma->pai = NULL;
        free(noFantasma);
        free(noRemovido);
        return;
    }

    tipoNo *substituto;
    int corOriginal = noRemovido->cor;  // Armazenar a cor original para verificação posterior

    // Caso 3: O nó removido tem apenas filho direito
    if (noRemovido->esq == NULL) {
        printf("(%d) não tem filho esquerdo\n", noRemovido->valor);
        printf("Procurando na arvore o menor valor da subarvore à direita\n");
        substituto = minimo(noRemovido->dir);
    }
    // Caso 4: O nó removido tem filho esquerdo
    else if (noRemovido->esq != NULL) {
        printf("(%d) tem filho esquerdo\n", noRemovido->valor);
        printf("Procurando na arvore o maior valor da subarvore à esquerda\n");
        substituto = maximo(noRemovido->esq);
    }

    printf("Substituto = %d\n", substituto->valor);
    substituirNo(arvore, noRemovido, substituto);

    if (corOriginal == NEGRO && substituto->cor == RUBRO) {
        substituto->cor = NEGRO;
    } else if (corOriginal == NEGRO) {
        verificarCorrecoesRemocao(arvore, substituto);
    }

    free(noRemovido);
}

// Mostrando no sentido LNR
void mostrarNo(tipoArvore *arvore, tipoNo* no, int linha) {
    if (no != NULL) {
        mostrarNo(arvore, no->esq, linha+1);  // Exibir a subárvore esquerda
        printf("Valor: %d\n", no->valor);
        printf("Cor: %s\n", no->cor == RUBRO ? "RUBRO" : "NEGRO");
        printf("Linha: %d\n", linha);
        if (no == arvore->raiz){
            printf("Raiz!\n");
        }
        printf("\n\n");
        mostrarNo(arvore, no->dir, linha+1);  // Exibir a subárvore direita
    }
}

void mostrar(tipoArvore *arvore) {
    if (arvore->raiz == NULL) {
        printf("Árvore vazia!\n");
    } else {
        int linha = 0;
        mostrarNo(arvore, arvore->raiz, linha);
    }
}

int main() {
    tipoArvore arvore;
    inicializarArvore(&arvore);

    int opc, valor;

    do {
        printf("** Árvore Rubro-Negra **\n");
        printf("  1 - Inserir\n");
        printf("  2 - Remover\n");
        printf("  3 - Mostrar\n");
        printf("  4 - Sair\n");
        printf("  Escolha uma opção: ");
        scanf("%d", &opc);
        printf("\n\n");

        switch (opc) {
            case 1:
                printf("Digite o valor a ser inserido: ");
                scanf("%d", &valor);
                criarNo(&arvore, valor);
                break;

            case 2:
                printf("Digite o valor a ser removido: ");
                scanf("%d", &valor);
                removerNo(&arvore, valor);
                break;

            case 3:
                mostrar(&arvore);
                break;

            case 4:
                printf("Saindo...\n");
                break;

            default:
                printf("Opção inválida!\n");
        }

        printf("\n");
    } while (opc != 4);

    return 0;
}