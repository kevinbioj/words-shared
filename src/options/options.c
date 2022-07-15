#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "options.h"

#define STR(s) #s
#define XSTR(s) STR(s)

//  DEF_* : valeurs par défaut pour une option donnée.
#define DEF_INIT 63
#define DEF_TOP 10

//  DESC_* : description succinte de l'option et de ses conséquences. Affiché
//    dans l'aide du programme.
#define DESC_INIT "\tThe number of significant characters in a word. If the"   \
  " word read is longer than the given value, then it is cut. Default is"      \
  " " XSTR(DEF_INIT) "."
#define DESC_PLSP "Treats punctuation characters as space characters."
#define DESC_SNUM "\tDisplays words that share the same number of"             \
  " occurrences or presence as the last word displayed in the limit."
#define DESC_TOP  "\t\tDetermines the number of words to produce on the"       \
  " standard output. 0 means all the words. Default is " XSTR(DEF_TOP) "."
#define DESC_UPPR "\tConverts all read lowercase characters to their"          \
  " uppercase form."
#define DESC_HELP "\t\tDisplays this help message and exits."
#define DESC_USAG "\t\tDisplays the expected syntax and exits."
#define DESC_VERS "\t\tDisplays version information and exits."

//  struct option : structure regroupant les informations d'une option : ses
//    identificateurs, une description, son type (à argument ou non), sa valeur
//    par défaut (ignorée si !has_arg) ainsi que l'emplacement où affecter sa
//    valeur.
struct option {
  int short_id;         //  Identificateur court de l'option.
  const char *long_id;  //  Identificateur long de l'option.
  const char *desc;     //  Description succinte de l'option.
  bool has_arg;         //  true : option à valeur | false : drapeau
  size_t default_value; //  si has_arg, la valeur par défaut de l'option.
  size_t offset;        //  has_arg
                        //    ? décalage du champ par rapport à options.
                        //    : emplacement du bit du drapeau dans flags.
};

//  optlist : tableau des options traitables par ce module. Chaque élément du
//    tableau est de type struct option. La fin du tableau est marquée par une
//    option sans identificateur court ni identificateur long.
const struct option optlist[] = {
    {'i', "initial", DESC_INIT, true, DEF_INIT, offsetof(options, charcnt)},
    {'p', "punctuation-like-space", DESC_PLSP, false, 0, FLAG_PLSP},
    {'s', "same-numbers", DESC_SNUM, false, 0, FLAG_SNUM},
    {'t', "top", DESC_TOP, true, DEF_TOP, offsetof(options, wordcnt)},
    {'u', "uppercasing", DESC_UPPR, false, 0, FLAG_UPPR},
    {'?', "help", DESC_HELP, false, 0, FLAG_HELP},
    {0, "usage", DESC_USAG, false, 0, FLAG_USAG},
    {0, "version", DESC_VERS, false, 0, FLAG_VERS},
    {0, NULL, NULL, false, 0, 0},
};

//  OPTLIST_END : détermine si p pointe sur le dernier élément du tableau
//    optlist.
#define OPTLIST_END(p) ((p)->short_id == 0 && (p)->long_id == NULL)

void options_defaults(options *o) {
  o->flags = 0;
  for (const struct option *p = optlist; !OPTLIST_END(p); ++p) {
    if (!p->has_arg) {
      continue;
    }
    memcpy((char *) o + p->offset, &(p->default_value), sizeof(size_t));
  }
  o->inputcnt = 0;
}

//  options_find_byshort : recherche l'option ayant un identificateur court égal
//    à c dans le tableau d'options c. Renvoie NULL si une telle option n'a pas
//    pu être trouvée. Renvoie sinon un pointeur vers cette option.
static const struct option *options_find_byshort(int c) {
  for (const struct option *p = optlist; !OPTLIST_END(p); ++p) {
    if (p->short_id == c) {
      return p;
    }
  }
  return NULL;
}

//  options_find_bylong : recherche l'option ayant un identificateur long égal
//    à la chaine pointée par s pour les len premiers caractères. Renvoie NULL
//    si une telle option n'a pas pu être trouvée. Renvoie sinon un pointeur
//    vers cette option.
static const struct option *options_find_bylong(const char *s, size_t len) {
  for (const struct option *p = optlist; !OPTLIST_END(p); ++p) {
    if (strncmp(p->long_id, s, len) == 0) {
      return p;
    }
  }
  return NULL;
}

#define HELP_VALIDSYNTAX "Usage: %s [OPTION]... FILES"
#define HELP_DESCRIPTION "%s — Prints a list of shared words between text files"
#define HELP_FILESNUMBER "Between 2 and %lu files are expected."
#define HELP_OCCURRENCES "If a word occurs more than %lu times, many is shown" \
  " instead."
#define HELP_VERSIONINFO "%s — Compiled on " __DATE__ " at " __TIME__ "."

//  options_version : affiche des informations sur la version de l'exécutable
//    puis termine le programme avec le code EXIT_SUCCESS.
static void options_version() {
  printf(HELP_VERSIONINFO "\n", PRNAME);
  exit(EXIT_SUCCESS);
}

//  options_usage : affiche la syntaxe attendue par l'exécutable puis termine
//    avec le code EXIT_SUCCESS.
static void options_usage() {
  printf(HELP_VALIDSYNTAX "\n", PRNAME);
  exit(EXIT_SUCCESS);
}

static void options_help() {
  printf(HELP_VALIDSYNTAX "\n", PRNAME);
  printf(HELP_DESCRIPTION "\n\n", PRNAME);
  for (const struct option *p = optlist; !OPTLIST_END(p); ++p) {
    putchar('\t');
    if (p->short_id != '\0') {
      printf("-%c, ", p->short_id);
    } else {
      printf("    ");
    }
    printf("--%s", p->long_id);
    if (p->has_arg) {
      printf("=VALUE");
    }
    printf("\t%s\n", p->desc);
  }
  printf("\n" HELP_FILESNUMBER "\n" HELP_OCCURRENCES "\n",
         INPUT_MAX, SHW_OCCURRENCES_MAX - 1);
  exit(EXIT_SUCCESS);
}

#define IS_INPUT(s) (*(s) != '-' || (*(s) == '-' && *((s) + 1) == '\0'))
#define IS_FILEOPT(s) (*((s) + 1) == '-' && *((s) + 2) == '\0')
#define IS_SHORT(s) (*((s) + 1) != '\0'                                        \
  && (*((s) + 2) == '\0' || *((s) + 2) == '='))

#define ENOFILE "Missing filename: '%s'."
#define EFILEUN "At least 2 files are expected."
#define EFILEOV "Number of files exceeding capacity."
#define EUKNOPT "Unrecognized option '%s'."
#define EMISARG "Missing argument '%s'."
#define EINVARG "Invalid argument '%s'."
#define EOVEARG "Overflowing argument '%s'."
#define ENOAARG "No argument allowed '%s'."

//  options_parse : augmente la structure associée à o des options et sources
//    fournies par l'utilisateur via le tableau d'arguments de longueur argc
//    pointé par argv. La fonction est susceptible de dévier le programme vers
//    l'aide, l'usage ou la version : le programme terminera alors sans retour.
//    Renvoie une valeur négative si une erreur de traitement est survenue.
//    Renvoie sinon zéro.
int options_parse(int argc, char *argv[], options *o) {
  int idx = 1;
  while (idx < argc) {
    const char *optstr = argv[idx++];
    if (IS_INPUT(optstr)) {
      if (o->inputcnt == INPUT_MAX) {
        ERROR(EFILEOV);
        return -1;
      }
      o->input[o->inputcnt++] = strcmp(optstr, "-") == 0 ? NULL : optstr;
      continue;
    }
    if (IS_FILEOPT(optstr)) {
      const char *filename = argv[idx++];
      if (filename == NULL) {
        ERRORA(ENOFILE, optstr);
        return -1;
      }
      if (o->inputcnt == INPUT_MAX) {
        ERROR(EFILEOV);
        return -1;
      }
      o->input[o->inputcnt++] = filename;
      continue;
    }
    char *eq = strchr(optstr, '=');
    size_t len = (eq != NULL)
        ? (size_t) (eq - optstr - 2)
        : strlen(optstr + 2);
    const struct option *curopt = IS_SHORT(optstr)
        ? options_find_byshort(*(optstr + 1))
        : options_find_bylong(optstr + 2, len);
    if (curopt == NULL) {
      ERRORA(EUKNOPT, optstr);
      return -1;
    }
    if (curopt->has_arg) {
      const char *value = (eq != NULL) ? (eq + 1) : argv[idx++];
      if (value == NULL) {
        ERRORA(EMISARG, optstr);
        return -1;
      }
      if (*value == '\0' || strchr(value, '-')) {
        ERRORA(EINVARG, optstr);
        return -1;
      }
      char *end;
      unsigned long n = strtoul(value, &end, 10);
      if (*end != '\0') {
        ERRORA(EINVARG, optstr);
        return -1;
      }
      if (n > SIZE_MAX) {
        ERRORA(EOVEARG, optstr);
        return -1;
      }
      memcpy((char *) o + curopt->offset, &n, sizeof(size_t));
    } else {
      if (strchr(optstr + 2, '=') != NULL) {
        ERRORA(ENOAARG, optstr);
        return -1;
      }
      switch (curopt->offset) {
        case FLAG_HELP:
          options_help();
          break;
        case FLAG_VERS:
          options_version();
          break;
        case FLAG_USAG:
          options_usage();
      }
      o->flags |= (1 << curopt->offset);
    }
  }
  if (o->inputcnt < 2) {
    ERROR(EFILEUN);
    return -1;
  }
  return 0;
}
