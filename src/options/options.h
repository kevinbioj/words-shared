//  Interface du module options - module gérant la récupération et le traitement
//    des éléments de la ligne de commande.

#ifndef OPTIONS__H
#define OPTIONS__H

#include <stdlib.h>
#include "shword.h"

//  PRNAME : renvoie le nom du programme.
#ifdef PRNAME
#undef PRNAME
#endif
#if defined(__APPLE__)
#define PRNAME getprogname()
#elif defined(__unix__)
extern char *program_invocation_short_name;
#define PRNAME program_invocation_short_name
#else
#define PRNAME "ws"
#endif

//  ERROR, ERRORA : macrofonctions simplifiant l'affichage d'erreurs.
#define ERROR(STR) fprintf(stderr, "%s: " STR "\n", PRNAME)
#define ERRORA(STR, ...) fprintf(stderr, "%s: " STR "\n", PRNAME, __VA_ARGS__)

//  INPUT_MAX : le nombre maximal de sources de fichier pouvant être prises en
//    compte par le programme.
#define INPUT_MAX SHW_PATTERN_MAX

//  FLAG_HAS, FLAG_SET : macrofonctions utilitaires sur la gestion des drapeaux.
#define FLAG_HAS(d, f) (((d) & (1 << (f))) == (1 << (f)))
#define FLAG_SET(d, f) ((d) |= (1 << (f)))

//  enum OPTIONS_FLAGS : énumérateur des différents drapeaux pouvant être
//    déclenchés via la ligne de commande à l'aide d'options.
enum OPTIONS_FLAGS {
  FLAG_HELP,
  FLAG_USAG,
  FLAG_VERS,
  FLAG_PLSP,
  FLAG_SNUM,
  FLAG_UPPR,
};

//  struct options, options : structure regroupant les données fournissables par
//    l'utilisateur via la ligne de commande. La conformité du contenu de la
//    structure n'est garantie qu'après une initialisation aux valeurs par
//    défaut (à l'aide de options_defaults) et la récupération en bonne et due
//    forme de l'entrée utilisateur (à l'aide de options_parse).
typedef struct options {
  int flags;        //  Options à drapeaux (8 options max avec le type char).
  size_t charcnt;   //  Nb. de caractères significatifs d'un mot.
  size_t wordcnt;   //  Nb. de mots à produire sur la sortie standard.
  const char *input[INPUT_MAX]; //  Tableau des sources d'entrées.
  size_t inputcnt;  //  Taille utile du tableau input.
} options;

//  options_defaults : affecte à la structure associée à o les valeurs par
//    défaut pour chaque option et initialise la taille du tableau de sources à
//    zéro.
extern void options_defaults(options *o);

//  options_parse : augmente la structure associée à o des options et sources
//    fournies par l'utilisateur via le tableau d'arguments de longueur argc
//    pointé par argv. La fonction est susceptible de dévier le programme vers
//    l'aide, l'usage ou la version : le programme terminera alors sans retour.
//    Renvoie une valeur négative si une erreur de traitement est survenue.
//    Renvoie sinon zéro.
extern int options_parse(int argc, char *argv[], options *o);

#endif
