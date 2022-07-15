//  Interface du module reader - module implément la lecture mot à mot dans un
//    flot selon des critères configurables.

#ifndef READER__H
#define READER__H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

//  reader_read : lit les len premiers caractères du prochain mot sur le flot
//    contrôlé par f puis le copie dans buf. Si plsp vaut true, alors les carac.
//    de ponctuation joueront le même rôle que les carac. d'espacement. Si uppr
//    vaut true, alors tout caractère minuscule rencontré est transformé en son
//    caractère majuscule. Renvoie zéro si il n'y a plus de mot à lire sur le
//    flot ou si une erreur de lecture est survenue. Renvoie sinon le nombre de
//    caractères lus : compris entre ]0; len] si le mot ne dépasse pas la limite
//    len, sinon len + 1.
extern size_t reader_read(FILE *f, char *buf, size_t len, bool plsp, bool uppr);

#endif
