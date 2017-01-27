#define QUICK_MIN
#include	<stdio.h>
#include	<stdlib.h>
#include	"csa_types.h"

char	*nomem_msg = "Insufficient memory.\n";

void	st_reset(s)

csa_stack	s;

{
s->top = s->bottom;
}

char	*st_pop(s)

csa_stack	s;

{
s->top--;
return(*(s->top));
}

csa_stack	st_create(size)

unsigned	size;

{
csa_stack	s;
void	exit();

s = (csa_stack) malloc(sizeof(struct csa_stack_st));

if (s == NULL)
  {
  (void) printf("%s", nomem_msg);
  exit(9);
  }
s->bottom = (char **) malloc(size * sizeof(char *));
if (s->bottom == NULL)
  {
  (void) printf("%s", nomem_msg);
  exit(9);
  }
s->top = s->bottom;
return(s);
}
