#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "omp.h"

int main ()
{
  {
    int x = 0;
  }
  printf ("No scope for %d\n", x);

  return 0;
}
