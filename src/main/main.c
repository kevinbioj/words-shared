#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "holdall.h"
#include "options.h"
#include "reader.h"
#include "shword.h"

#define EFIL "'%s': %s."
#define EMEM "Not enough memory."
#define ETRU "Word '%s...' was truncated in file '%s'."
#define EDIS "Failed to display shared words: %s."
#define EMOR "Try '%s --help' for more information."

//  str_hashfun : calcule la somme de hachage de la chaine de caractères pointée
//    par s.
static size_t str_hashfun(const char *s);

int main(int argc, char *argv[]) {
  options opts;
  options_defaults(&opts);
  switch (options_parse(argc, argv, &opts)) {
    case -1:
      fprintf(stderr, EMOR "\n", PRNAME);
      exit(EXIT_FAILURE);
    case 1: exit(EXIT_SUCCESS);
  }
  int r = EXIT_SUCCESS;
  hashtable *ht = hashtable_empty((int (*)(const void *, const void *))strcmp,
      (size_t (*)(const void *))str_hashfun);
  holdall *ha = holdall_empty();
  char *buf = malloc(opts.charcnt + 1);
  if (ht == NULL || ha == NULL || buf == NULL) {
    goto error_capacity;
  }
  for (size_t k = 0; k < opts.inputcnt; ++k) {
    bool isstdin = opts.input[k] == NULL;
    FILE *f = isstdin ? stdin : fopen(opts.input[k], "r");
    const char *filename = isstdin ? "stdin" : opts.input[k];
    if (f == NULL) {
      ERRORA(EFIL, filename, strerror(errno));
      goto error;
    }
    size_t rcount;
    while ((rcount = reader_read(f, buf, opts.charcnt,
        FLAG_HAS(opts.flags, FLAG_PLSP),
        FLAG_HAS(opts.flags, FLAG_UPPR))) > 0) {
      if (rcount == opts.charcnt + 1) {
        ERRORA(ETRU, buf, filename);
      }
      shword *shw = (shword *) hashtable_search(ht, buf);
      if (shw == NULL) {
        shw = shword_create(buf);
        if (shw == NULL) {
          goto error_capacity;
        }
        if (holdall_put(ha, shw) != 0) {
          shword_destroy(shw);
          goto error_capacity;
        }
        if (hashtable_add(ht, shword_word(shw), shw) == NULL) {
          goto error_capacity;
        }
      }
      //  Inutile de vérifier la valeur de retour puisque k sera toujours borné
      //    à INPUT_MAX lui-même borné par SHW_PATTERN_MAX, et que si le nb.
      //    d'occ. a été atteint on ignore juste le retour puisque de toute
      //    manière le compteur ne bougera plus.
      shword_increment(shw, k);
    }
    if (!feof(f)) {
      ERRORA(EFIL, filename, strerror(errno));
      goto error;
    }
    if (isstdin) {
      clearerr(f);
    } else {
      fclose(f);
    }
  }
  if (holdall_sort(ha,
      (int (*)(const void *, const void *))shword_compare) != 0) {
    goto error_capacity;
  }
  struct print_race pr = {
      .inputcnt = opts.inputcnt,
      .last = NULL,
      .remaining = opts.wordcnt > 0 ? opts.wordcnt : holdall_count(ha),
      .samenumbers = FLAG_HAS(opts.flags, FLAG_SNUM),
  };
  if (holdall_apply_context(ha, &pr,
      (void *(*)(void *, void *))shword_predisplay,
      (int (*)(void *, void *))shword_display) < 0) {
    ERRORA(EDIS, strerror(errno));
    goto error;
  }

  goto dispose;
  error_capacity:
  ERROR(EMEM);
  goto error;
  error:
  r = EXIT_FAILURE;
  dispose:
  hashtable_dispose(&ht);
  if (ha != NULL) {
    holdall_apply(ha, (int (*)(void *))shword_destroy);
  }
  holdall_dispose(&ha);
  free(buf);
  return r;
}

size_t str_hashfun(const char *s) {
  size_t h = 0;
  for (const unsigned char *p = (const unsigned char *) s; *p != '\0'; ++p) {
    h = 37 * h + *p;
  }
  return h;
}
