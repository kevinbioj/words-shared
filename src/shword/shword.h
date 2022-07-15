//  Interface du module shword - module implémentant la gestion de mots partagés
//    entre diverses sources de fichiers.

#ifndef SHWORD__H
#define SHWORD__H

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

//  SHW_PATTERN_TYPE, SHW_PATTERN_MAX : respectivement le type utilisé pour la
//    gestion du motif d'occurrences d'un mot, ainsi que le nombre maximal de
//    fichiers pouvant être pris en charge par ce motif. Le nombre de fichiers
//    maximal découle directement du type utilisé.
#define SHW_PATTERN_TYPE long
#define SHW_PATTERN_MAX (8 * sizeof(SHW_PATTERN_TYPE))

//  SHW_OCCURRENCES_TYPE, SHW_OCCURRENCES_MAX, SHW_OCCURRENCES_MANY :
//    respectivement le type utilisé pour la gestion du nombre maximal
//    d'occurrences d'un mot dans les fichiers lus, le nombre maximal
//    d'occurrences qu'un mot peut avoir, ainsi que le mot affiché en place du
//    nombre d'occurrences si ce dernier est égal à la limite. Le nombre maximal
//    d'occurrences découle directement du type utilisé.
#define SHW_OCCURRENCES_TYPE unsigned long
#define SHW_OCCURRENCES_MAX ULONG_MAX
#define SHW_OCCURRENCES_MANY "many"

//  struct shword, shword : structure regroupant les données d'un mot partagé.
//    La création de la structure est confiée à la fonction shword_create.
typedef struct shword shword;

//  struct print_race : structure regroupant les informations utiles à
//    l'affichage des mots partagés par la fonction shword_display.
struct print_race {
  size_t inputcnt;    //  nombre d'entrées prises en charge.
  const shword *last; //  dernier mot affiché par shword_display.
  size_t remaining;   //  nombre de mots restants à afficher.
  bool samenumbers;   //  afficher ou non les mots "égaux" au dernier de la lim.
};

//  shword_create : crée une instance de mot partagé ciblant le mot dénoté par
//    la chaine pointée par w. Renvoie NULL en cas de dépassement de capacité,
//    ou si w vaut NULL. Renvoie sinon un pointeur vers l'objet associé au mot.
extern shword *shword_create(const char *w);

//  shword_increment : marque une occurrence du mot partagé associé à shw dans
//    le fichier d'indice idx. Renvoie une valeur non nulle si le mot a atteint
//    la limite d'occurrences SHW_OCCURRENCES_MAX ou si idx est supérieur ou
//    égal à SHW_PATTERN_MAX. Renvoie sinon zéro.
extern int shword_increment(shword *shw, size_t idx);

//  shword_occurrences : renvoie le nombre d'occurrences du mot partagé associé
//    à shw.
extern SHW_OCCURRENCES_TYPE shword_occurrences(const shword *shw);

//  shword_occursin : renvoie true ou false selon si le mot partagé associé à
//    shw a été déclaré présent dans le fichier d'indice idx. Renvoie false si
//    idx est supérieur ou égal à SHW_PATTERN_MAX.
extern bool shword_occursin(const shword *shw, size_t idx);

//  shword_filecount : renvoie le nombre de fichiers dans lequel le mot partagé
//    associé à shw a été déclaré présent.
extern size_t shword_filecount(const shword *shw);

//  shword_word : renvoie la chaine de caractères dénotant du mot partagé
//    associé à shw.
extern const char *shword_word(const shword *shw);

//  shword_compare : compare les mots partagés associés à shw1 et shw2 selon le
//    schéma suivant.
//  * Clé primaire : nombre de fichiers dans lequel le mot partagé apparait.
//  * Clé secondaire : nombre total d'occurrences du mot partagé.
//  * Clé ternaire : comparaison de la chaine de caractères via strcmp.
//    Renvoie une valeur négative si shw1 est inférieur à shw2, une valeur
//    positive si shw1 est supérieur à shw2, ou zéro si shw1 et shw2 sont égaux.
extern int shword_compare(const shword *shw1, const shword *shw2);

//  shword_display : si pr ne vaut pas NULL, affiche sur la sortie standard le
//    motif d'occurrences du mot partagé associé à shw, son nombre total d'occu
//    -rrences ainsi que le mot avant un retour à la ligne. La fonction renvoie
//    une valeur positive si pr vaut NULL, EOF en cas d'erreur d'écriture sur
//    la sortie standard. Renvoie sinon zéro.
extern int shword_display(const shword *shw, const struct print_race *pr);

//  shword_predisplay : détermine si le mot partagé par shw doit être affiché ou
//    non par shword_display selon les règles fournies par la structure de
//    données associée à pr. Renvoie pr si le mot est affichable, NULL sinon.
extern struct print_race *shword_predisplay(struct print_race *pr,
    const shword *shw);

//  shword_destroy : si shw ne vaut pas NULL, désalloue les composants du mot
//    partagé associé à shw (sa chaine de caractères associée et la structure en
//    elle-même). Renvoie zéro dans tous les cas.
extern int shword_destroy(shword *shw);

#endif
