//  Implantation du module shword - le motif d'occurrences est déterminé par un
//    entier dont le nombre de bits (8 * sizeof(type)) conditionne le nombre
//    maximal de fichiers pouvant être pris en charge par ladite structure.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shword.h"

#define FLAG_HAS(d, f) ((d & (1 << f)) == (1 << f))
#define FLAG_SET(d, f) (d |= (1 << f))

struct shword {
  const char *w;
  SHW_PATTERN_TYPE pat;
  SHW_OCCURRENCES_TYPE occ;
  size_t fcount;
};

shword *shword_create(const char *w) {
  shword *p = malloc(sizeof *p);
  if (p == NULL) {
    return NULL;
  }
  p->w = malloc(strlen(w) + 1);
  if (p->w == NULL) {
    free(p);
    return NULL;
  }
  strcpy((char *) p->w, w);
  p->pat = 0;
  p->occ = 0;
  p->fcount = 0;
  return p;
}

int shword_increment(shword *shw, size_t idx) {
  if (idx >= SHW_PATTERN_MAX) {
    return 2;
  }
  if (!FLAG_HAS(shw->pat, idx)) {
    FLAG_SET(shw->pat, idx);
    ++shw->fcount;
  }
  if (shw->occ == SHW_OCCURRENCES_MAX) {
    return 1;
  }
  ++shw->occ;
  return 0;
}

SHW_OCCURRENCES_TYPE shword_occurrences(const shword *shw) {
  return shw->occ;
}

bool shword_occursin(const shword *shw, size_t idx) {
  if (idx >= SHW_PATTERN_MAX) {
    return false;
  }
  return FLAG_HAS(shw->pat, idx);
}

size_t shword_filecount(const shword *shw) {
  return shw->fcount;
}

const char *shword_word(const shword *shw) {
  return shw->w;
}

int shword_compare(const shword *shw1, const shword *shw2) {
  size_t filecount = shword_filecount(shw2) - shword_filecount(shw1);
  if (filecount) {
    return (int) filecount;
  }
  SHW_OCCURRENCES_TYPE occurrences = shword_occurrences(shw2)
      - shword_occurrences(shw1);
  if (occurrences) {
    return (int) occurrences;
  }
  return strcmp(shword_word(shw1), shword_word(shw2));
}

int shword_display(const shword *shw, const struct print_race *pr) {
  if (pr == NULL) {
    return 1;
  }
  char pat[pr->inputcnt];
  for (size_t k = 0; k < pr->inputcnt; ++k) {
    pat[k] = shword_occursin(shw, k) ? 'x' : '-';
  }
  int r = SHW_OCCURRENCES_MAX == shword_occurrences(shw)
      ? printf("%.*s\t" SHW_OCCURRENCES_MANY "\t%s\n", (int) pr->inputcnt, pat,
          shword_word(shw))
      : printf("%.*s\t%lu\t%s\n", (int) pr->inputcnt, pat,
          shword_occurrences(shw), shword_word(shw));
  return r < 0 ? EOF : 0;
}

struct print_race *shword_predisplay(struct print_race *pr, const shword *shw) {
  if (shword_filecount(shw) < 2) {
    return NULL;
  }
  if (pr->remaining > 0) {
    --pr->remaining;
    pr->last = shw;
    return pr;
  }
  if (pr->samenumbers && pr->last != NULL) {
    if (shword_filecount(shw) == shword_filecount(pr->last)
        && shword_occurrences(shw) == shword_occurrences(pr->last)) {
      pr->last = shw;
      return pr;
    }
  }
  return NULL;
}

int shword_destroy(shword *shw) {
  if (shw != NULL) {
    free((char *) shw->w);
    free(shw);
  }
  return 0;
}
