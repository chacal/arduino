#include "StringUtils.h"
#include <string.h>

bool endsWith(const char *string, const char *suffix) {
  if(!string || !suffix)
    return false;

  string = strrchr(string, suffix[0]);

  if(string != NULL)
    return strcmp(string, suffix) == 0;

  return false;
}