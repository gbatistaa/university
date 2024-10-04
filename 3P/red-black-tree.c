#include <stdio.h>
#include <stdlib.h>

typedef enum {
  NEGRO,
  RUBRO
} Cor;

typedef struct no {
  int valor;
  struct no *pai, *esq, *dir;
  Cor cor; // Usando enum para a cor
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
  tipoNo *esq = no->esq;
  no->esq = esq->dir;

  if (esq->dir != NULL) {
    esq->dir->pai = no;
  }

  esq->pai = no->pai;

  if (no->pai == NULL) {
    arvore->raiz = esq;
  } else if (no == no->pai->esq) {
    no->pai->esq = esq;
  } else {
    no->pai->dir = esq;
  }

  esq->dir = no;
  no->pai = esq;
}

void verificarCorrecoesInsercao(tipoArvore *arvore, tipoNo *no) {
  if (no == arvore->raiz && no->cor == RUBRO) {
    printf("Raiz (%d) virou negro\n\n", no->valor);
    no->cor = NEGRO;
    return;
  }

  if (no->pai == NULL || no->pai->cor == NEGRO) {
    printf("Não precisa de correção!\n");
    return;
  }

  tipoNo *avo = no->pai->pai;
  if (avo == NULL) {
    return;
  }

  tipoNo *tio = (no->pai == avo->esq) ? avo->dir : avo->esq;

  if (tio != NULL && tio->cor == RUBRO) {
    printf("O pai (%d) de (%d) virou negro\n", no->pai->valor, no->valor);
    no->pai->cor = NEGRO;

    printf("O tio (%d) de (%d) virou negro\n", tio->valor, no->valor);
    tio->cor = NEGRO;

    printf("O avô (%d) de (%d) virou rubro\n\n", avo->valor, no->valor);
    avo->cor = RUBRO;

    verificarCorrecoesInsercao(arvore, avo);
  } else {
    if (no == no->pai->dir && no->pai == avo->esq) {
      printf("Rotacionando pai (%d) para esquerda\n", no->pai->valor);
      rotacaoEsquerda(arvore, no->pai);
      no = no->esq;
    } else if (no == no->pai->esq && no->pai == avo->dir) {
      printf("Rotacionando pai (%d) para direita\n", no->pai->valor);
      rotacaoDireita(arvore, no->pai);
      no = no->dir;
    }

    if (no == no->pai->esq) {
      printf("Rotacionando avô (%d) para direita\n", avo->valor);
      rotacaoDireita(arvore, avo);
    } else {
      printf("Rotacionando avô (%d) para esquerda\n", avo->valor);
      rotacaoEsquerda(arvore, avo);
    }

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
}

tipoNo* maximo(tipoNo* no) {
  while (no->dir != NULL) {
    no = no->dir;
  }
  return no;
}

tipoNo* minimo(tipoNo* no) {
  while (no->esq != NULL) {
    no = no->esq;
  }
  return no;
}

void substituirNo(tipoArvore *arvore, tipoNo *noVelho, tipoNo *noNovo) {
  if (noNovo == NULL) {
    if (noVelho->pai == NULL) {
      arvore->raiz = NULL;
      printf("A raiz (%d) foi excluída!\n\n", noVelho->valor);
      return;
    }

    if (noVelho == noVelho->pai->esq) {
      noVelho->pai->esq = NULL;
    } else {
      noVelho->pai->dir = NULL;
    }
    printf("Substituindo (%d) por nulo!\n\n", noVelho->valor);
    return;
  }

  if (noVelho->pai == NULL) {
    arvore->raiz = noNovo;
    noNovo->pai = NULL;
    return;
  }

  if (noVelho == noVelho->pai->esq) {
    noVelho->pai->esq = noNovo;
  } else {
    noVelho->pai->dir = noNovo;
  }

  noNovo->esq = noVelho->esq;
  noNovo->dir = noVelho->dir;
  noNovo->pai = noVelho->pai;

  if (noNovo == noVelho->esq) {
    noNovo->esq = NULL;
  }

  if (noNovo == noVelho->dir) {
    noNovo->dir = NULL;
  }

  printf("Substituindo (%d) por (%d)!\n\n", noVelho->valor, noNovo->valor);
}

tipoNo* buscarNo(tipoNo *raiz, int valor) {
  if (raiz == NULL) {
    return NULL;
  }

  if (valor < raiz->valor) {
    return buscarNo(raiz->esq, valor);
  } else if (valor > raiz->valor) {
    return buscarNo(raiz->dir, valor);
  }

  return raiz;
}

void verificarCorrecoesRemocao(tipoArvore *arvore, tipoNo *no) {
  while (no != arvore->raiz && no->cor == NEGRO) {
    if (no == no->pai->esq) {
      tipoNo *irmao = no->pai->dir;

      if (irmao->cor == RUBRO) {
        irmao->cor = NEGRO;
        no->pai->cor = RUBRO;
        rotacaoEsquerda(arvore, no->pai);
        irmao = no->pai->dir;
      }

      if ((irmao->esq == NULL || irmao->esq->cor == NEGRO) &&
          (irmao->dir == NULL || irmao->dir->cor == NEGRO)) {
        irmao->cor = RUBRO;
        no = no->pai;
      } else {
        if (irmao->dir == NULL || irmao->dir->cor == NEGRO) {
          if (irmao->esq != NULL) irmao->esq->cor = NEGRO;
          irmao->cor = RUBRO;
          rotacaoDireita(arvore, irmao);
          irmao = no->pai->dir;
        }

        irmao->cor = no->pai->cor;
        no->pai->cor = NEGRO;

        if (irmao->dir != NULL) irmao->dir->cor = NEGRO;
        rotacaoEsquerda(arvore, no->pai);
        no = arvore->raiz;
      }
    } else {
      tipoNo *irmao = no->pai->esq;

      if (irmao->cor == RUBRO) {
        irmao->cor = NEGRO;
        no->pai->cor = RUBRO;
        rotacaoDireita(arvore, no->pai);
        irmao = no->pai->esq;
      }

      if ((irmao->dir == NULL || irmao->dir->cor == NEGRO) &&
          (irmao->esq == NULL || irmao->esq->cor == NEGRO)) {
        irmao->cor = RUBRO;
        no = no->pai;
      } else {
        if (irmao->esq == NULL || irmao->esq->cor == NEGRO) {
          if (irmao->dir != NULL) irmao->dir->cor = NEGRO;
          irmao->cor = RUBRO;
          rotacaoEsquerda(arvore, irmao);
          irmao = no->pai->esq;
        }

        irmao->cor = no->pai->cor;
        no->pai->cor = NEGRO;

        if (irmao->esq != NULL) irmao->esq->cor = NEGRO;
        rotacaoDireita(arvore, no->pai);
        no = arvore->raiz;
      }
    }
  }
  no->cor = NEGRO;
}

void removerNo(tipoArvore *arvore, tipoNo *no) {
  tipoNo *noSubstituto = NULL;
  tipoNo *noRemovido = no;

  Cor corOriginal = noRemovido->cor;

  if (no->esq == NULL) {
    noSubstituto = no->dir;
    substituirNo(arvore, noRemovido, no->dir);
  } else if (no->dir == NULL) {
    noSubstituto = no->esq;
    substituirNo(arvore, noRemovido, no->esq);
  } else {
    noRemovido = maximo(no->esq);
    corOriginal = noRemovido->cor;
    noSubstituto = noRemovido->esq;

    if (noRemovido->pai != no) {
      substituirNo(arvore, noRemovido, noRemovido->esq);
      noRemovido->esq = no->esq;
      noRemovido->esq->pai = noRemovido;
    }

    substituirNo(arvore, no, noRemovido);
    noRemovido->dir = no->dir;
    noRemovido->dir->pai = noRemovido;
    noRemovido->cor = no->cor;
  }

  if (corOriginal == NEGRO) {
    verificarCorrecoesRemocao(arvore, noSubstituto);
  }

  free(no);
}

int main() {
  tipoArvore arvore;
  inicializarArvore(&arvore);

  criarNo(&arvore, 10);
  criarNo(&arvore, 20);
  criarNo(&arvore, 30);
  criarNo(&arvore, 15);
  criarNo(&arvore, 25);
  criarNo(&arvore, 5);
  criarNo(&arvore, 1);

  // Remoção de um nó
  tipoNo *n = buscarNo(arvore.raiz, 15);
  if (n) {
    printf("Removendo nó com valor: %d\n", n->valor);
    removerNo(&arvore, n);
  }

  return 0;
}
