#include <stdio.h>
#include <stdlib.h>

enum Cor {
    NEGRO,
    RUBRO
}

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

tipoNo* minimo(tipoNo* no) {
    // Encontra o menor valor na subárvore à direita
    while (no->esq != NULL) {
        no = no->esq;
    }
    return no;
}

void substituirNo(tipoArvore *arvore, tipoNo *no, tipoNo *filho) {
    if (no->pai == NULL) {
        // Se o nó removido é a raiz
        arvore->raiz = filho;
    } else if (no == no->pai->esq) {
        no->pai->esq = filho;
    } else {
        no->pai->dir = filho;
    }
    
    if (filho != NULL) {
        filho->pai = no->pai;
    }
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
    while (no != arvore->raiz && no->cor == NEGRO) {
        if (no == no->pai->esq) {
            tipoNo *irmao = no->pai->dir;

            // Caso 1: O irmão do nó é rubro
            if (irmao->cor == RUBRO) {
                irmao->cor = NEGRO;
                no->pai->cor = RUBRO;
                rotacaoEsquerda(arvore, no->pai);
                irmao = no->pai->dir;
            }

            // Caso 2: O irmão e os filhos do irmão são negros
            if (irmao->esq->cor == NEGRO && irmao->dir->cor == NEGRO) {
                irmao->cor = RUBRO;
                no = no->pai;
            } else {
                // Caso 3: O irmão é negro, e o filho esquerdo é rubro
                if (irmao->dir->cor == NEGRO) {
                    irmao->esq->cor = NEGRO;
                    irmao->cor = RUBRO;
                    rotacaoDireita(arvore, irmao);
                    irmao = no->pai->dir;
                }

                // Caso 4: O irmão é negro, e o filho direito é rubro
                irmao->cor = no->pai->cor;
                no->pai->cor = NEGRO;
                irmao->dir->cor = NEGRO;
                rotacaoEsquerda(arvore, no->pai);
                no = arvore->raiz;
            }
        } else {
            // Casos simétricos para o irmão à esquerda
            tipoNo *irmao = no->pai->esq;

            if (irmao->cor == RUBRO) {
                irmao->cor = NEGRO;
                no->pai->cor = RUBRO;
                rotacaoDireita(arvore, no->pai);
                irmao = no->pai->esq;
            }

            if (irmao->esq->cor == NEGRO && irmao->dir->cor == NEGRO) {
                irmao->cor = RUBRO;
                no = no->pai;
            } else {
                if (irmao->esq->cor == NEGRO) {
                    irmao->dir->cor = NEGRO;
                    irmao->cor = RUBRO;
                    rotacaoEsquerda(arvore, irmao);
                    irmao = no->pai->esq;
                }

                irmao->cor = no->pai->cor;
                no->pai->cor = NEGRO;
                irmao->esq->cor = NEGRO;
                rotacaoDireita(arvore, no->pai);
                no = arvore->raiz;
            }
        }
    }

    no->cor = NEGRO;
}


void removerNo(tipoArvore *arvore, int valor) {
    if (arvore->raiz == NULL) {
        printf("Árvore vazia!\n");
        return;
    }

    // Buscar o nó com o valor fornecido
    tipoNo *no = buscarNo(arvore->raiz, valor);

    // Nó não encontrado
    if (no == NULL) {
        printf("Valor %d não encontrado na árvore!\n", valor);
        return;
    }

    // Imprime o valor do nó que será removido
    printf("Nó a ser removido = %d\n", no->valor);

    tipoNo *substituto;
    int corOriginal = no->cor;  // Armazenar a cor original para verificação posterior

    if (no->esq == NULL) {
        // Caso 1: O nó não tem filho esquerdo
        substituto = no->dir;
        substituirNo(arvore, no, substituto);
    } else if (no->dir == NULL) {
        // Caso 2: O nó não tem filho direito
        substituto = no->esq;
        substituirNo(arvore, no, substituto);
    } else {
        // Caso 3: O nó tem dois filhos
        tipoNo *sucessor = minimo(no->dir);  // Encontra o sucessor
        corOriginal = sucessor->cor;         // Atualiza a cor original para o sucessor
        substituto = sucessor->dir;          // O substituto pode ser NULL ou um filho direito
        
        if (sucessor->pai == no) {
            if (substituto != NULL) {
                substituto->pai = sucessor;
            }
        } else {
            substituirNo(arvore, sucessor, substituto);
            sucessor->dir = no->dir;
            sucessor->dir->pai = sucessor;
        }

        substituirNo(arvore, no, sucessor);
        sucessor->esq = no->esq;
        sucessor->esq->pai = sucessor;
        sucessor->cor = no->cor;  // Sucessor herda a cor do nó removido
    }

    printf("removendo (%d) \n\n", no->valor);
    
    free(no);  // Libera a memória do nó removido

    // Somente realizar correções se o nó removido ou o sucessor eram negros
    if (corOriginal == NEGRO && substituto != NULL) {
        verificarCorrecoesRemocao(arvore, substituto);
    }
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