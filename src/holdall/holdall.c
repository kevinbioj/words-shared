//  Partie implantation du module holdall.

#include "holdall.h"

//  struct holdall, holdall : implantation par liste dynamique simplement
//    chainée. L'insertion a lieu en queue si la macroconstante
//    HOLDALL_INSERT_TAIL est définie, en tête sinon.

typedef struct choldall choldall;

struct choldall {
  void *value;
  choldall *next;
};

struct holdall {
  choldall *head;
#ifdef HOLDALL_INSERT_TAIL
  choldall *tail;
#endif
  size_t count;
};

holdall *holdall_empty(void) {
  holdall *ha = malloc(sizeof *ha);
  if (ha == NULL) {
    return NULL;
  }
  ha->head = NULL;
#ifdef HOLDALL_INSERT_TAIL
  ha->tail = NULL;
#endif
  ha->count = 0;
  return ha;
}

int holdall_put(holdall *ha, void *ptr) {
  choldall *p = malloc(sizeof *p);
  if (p == NULL) {
    return -1;
  }
  p->value = ptr;
#ifdef HOLDALL_INSERT_TAIL
  p->next = NULL;
  if (ha->tail == NULL) {
    ha->head = p;
  } else {
    ha->tail->next = p;
  }
  ha->tail = p;
#else
  p->next = ha->head;
  ha->head = p;
#endif
  ha->count += 1;
  return 0;
}

size_t holdall_count(holdall *ha) {
  return ha->count;
}

int holdall_apply(holdall *ha, int (*fun)(void *)) {
  for (const choldall *p = ha->head; p != NULL; p = p->next) {
    int r = fun(p->value);
    if (r != 0) {
      return r;
    }
  }
  return 0;
}

int holdall_apply_context(holdall *ha,
    void *context, void *(*fun1)(void *context, void *ptr),
    int (*fun2)(void *ptr, void *resultfun1)) {
  for (const choldall *p = ha->head; p != NULL; p = p->next) {
    int r = fun2(p->value, fun1(context, p->value));
    if (r != 0) {
      return r;
    }
  }
  return 0;
}

int holdall_apply_context2(holdall *ha,
    void *context1, void *(*fun1)(void *context1, void *ptr),
    void *context2, int (*fun2)(void *context2, void *ptr, void *resultfun1)) {
  for (const choldall *p = ha->head; p != NULL; p = p->next) {
    int r = fun2(context2, p->value, fun1(context1, p->value));
    if (r != 0) {
      return r;
    }
  }
  return 0;
}

void holdall_dispose(holdall **haptr) {
  if (*haptr == NULL) {
    return;
  }
  choldall *p = (*haptr)->head;
  while (p != NULL) {
    choldall *t = p;
    p = p->next;
    free(t);
  }
  free(*haptr);
  *haptr = NULL;
}

//  choldall__split : divise la liste chainée pointée par cha en deux listes de
//    taille à peu près égales. Si |cha| est impair, alors la première liste
//    contiendra un élément de plus que la seconde. *frontptr se voit affectée
//    la première moitié, et *backptr la seconde moitié de la liste séparée.
//    Si la liste ne contient qu'un seul élément, alors celui-ci sera affecté
//    à la première moitié, et la seconde sera vide.
static void choldall__split(choldall *cha,
    choldall **frontptr, choldall **backptr) {
  if (cha == NULL || cha->next == NULL) {
    *frontptr = cha;
    *backptr = NULL;
    return;
  }
  choldall *s = cha;
  choldall *f = cha->next;
  while (f != NULL) {
    f = f->next;
    if (f != NULL) {
      s = s->next;
      f = f->next;
    }
  }
  *frontptr = cha;
  *backptr = s->next;
  s->next = NULL;
}

//  choldall__merge : fusionne les listes chainées pointées par a et b et
//    affecte à *dest la liste résultante de la fusion. L'insertion est
//    déterminée par le résultat de la fonction pointée par compar sur les
//    têtes des listes a et b.
static void choldall__merge(choldall *a, choldall *b,
    choldall **dest,
    int (*compar)(const void *, const void *)) {
  if (a == NULL) {
    *dest = b;
    return;
  }
  if (b == NULL) {
    *dest = a;
    return;
  }
  if (compar(a->value, b->value) <= 0) {
    *dest = a;
    choldall__merge(a->next, b, &(*dest)->next, compar);
  } else {
    *dest = b;
    choldall__merge(a, b->next, &(*dest)->next, compar);
  }
}

//  choldall__sort : trie la liste chainée pointée par *chaptr selon la méthode
//    du tri fusion et à l'aide de la fonction de comparaison pointée par
//    compar.
static void choldall__sort(choldall **chaptr,
    int (*compar)(const void *, const void *)) {
  if (*chaptr == NULL || (*chaptr)->next == NULL) {
    return;
  }
  choldall *a;
  choldall *b;
  choldall__split(*chaptr, &a, &b);
  choldall__sort(&a, compar);
  choldall__sort(&b, compar);
  choldall__merge(a, b, chaptr, compar);
}

int holdall_sort(holdall *ha, int (*compar)(const void *, const void *)) {
  if (ha->count > 1) {
    choldall__sort(&(ha->head), compar);
  }
  return 0;
}
