#include <ctype.h>
#include <stdio.h>
#include "reader.h"

#define READER_SOP(c, p) (isspace(c) || (p && ispunct(c)))

size_t reader_read(FILE *f, char *buf, size_t len, bool plsp, bool uppr) {
  size_t k = 0;
  int c;
  while ((c = fgetc(f)) != EOF) {
    if (READER_SOP(c, plsp)) {
      if (k == 0) {
        continue;
      }
      buf[k] = '\0';
      break;
    }
    buf[k++] = (char) ((uppr && islower(c)) ? toupper(c) : c);
    if (k > len) {
      buf[len] = '\0';
      while ((c = fgetc(f)) != EOF && !READER_SOP(c, plsp));
      if (c != EOF && ungetc(c, f) == EOF) {
        k = 0;
      }
      break;
    }
  }
  if (c == EOF && !feof(f)) {
    return 0;
  }
  return k;
}
