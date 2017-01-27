#include	<stdio.h>
#include	"csa_types.h"
#include	"csa_defs.h"

extern	lhs_ptr	head_lhs_node, tail_lhs_node;
extern	rhs_ptr	head_rhs_node, tail_rhs_node;
extern	lr_aptr	head_arc;
extern	double	scale_factor, epsilon;

void	show_lhs_node(lhs_id)

int	lhs_id;

{
lhs_ptr	v = &head_lhs_node[lhs_id - 1];
int	rhs_id;
rhs_ptr	w;
lr_aptr	a;
double	v_price, this_price;

(void) printf("Lhs node %d ", lhs_id);
if (v->matched)
  {
  w = v->matched->head;
  rhs_id = w - head_rhs_node + 1;
  (void) printf("matched thru stored cost %lg to rhs node %d",
		v->matched->c, rhs_id);
  if (w->matched == v)
    (void) putchar('\n');
  else
    {
    lhs_id = w->matched - head_lhs_node + 1;
    (void) printf(", matched back to lhs node %d\n", lhs_id);
    }
  (void) printf("\tMatching arc is priced ");
  if (v->matched->head->node_info.priced_in)
    (void) printf("in\n");
  else
    (void) printf("out\n");
  }
else
  (void) printf("unmatched\n");
(void) printf("\t%ld arcs priced out, %ld arcs priced in\n",
	      v->first - v->priced_out, (v+1)->priced_out - v->first);
if ((v+1)->priced_out - v->first > 0)
  {
  (void) printf("\tPriced in arcs:\n");
  for (a = v->first; a == v->matched; a++);
  if (a == (v+1)->priced_out)
    v_price = 0.0;
  else
    {
    v_price = a->head->p - a->c;
    for (a++; a != (v+1)->priced_out; a++)
      if ((a != v->matched) &&
	  (v_price < (this_price = a->head->p - a->c)))
	v_price = this_price;
    }
  for (a = v->first; a != (v+1)->priced_out; a++)
    {
    rhs_id = a->head - head_rhs_node + 1;
    (void) printf("\t\t(%d, %d) stored cost %lg, cmp cost %lg\n",
		  lhs_id, rhs_id, a->c, v_price + a->c - a->head->p);
    }
  }
}

void	show_rhs_node(rhs_id)

int	rhs_id;

{
rhs_ptr	v = &head_rhs_node[rhs_id - 1];
int	lhs_id;
#ifdef	USE_P_UPDATE
rl_aptr	b;
#endif

(void) printf("Rhs node %d, p %lg ",
	      rhs_id, v->p);
if (v->matched)
  {
  lhs_id = v->matched - head_lhs_node + 1;
  if (v->matched->matched->head == v)
    (void) printf("matched thru stored cost %lg to lhs node %d\n",
		  v->matched->matched->c, lhs_id);
  else
    (void) printf("matched inconsistently to lhs node %d\n", lhs_id);
  }
else
  (void) printf("unmatched\n");
#ifdef	USE_P_UPDATE
for (b = v->priced_out; b != v->back_arcs; b++)
  {
  lhs_id = b->tail - head_lhs_node + 1;
  (void) printf("Arc (%d, %d) back stored cost %lg (priced out)\n",
		lhs_id, rhs_id, b->c);
  }
for (; b != (v+1)->priced_out; b++)
  {
  lhs_id = b->tail - head_lhs_node + 1;
  (void) printf("Arc (%d, %d) back stored cost %lg (priced in) cmp cost %lg\n",
		lhs_id, rhs_id, b->c, b->c - v->p);
  }
#endif
}

void	show_lhs()

{
int	id;

for (id = 1; id <= tail_lhs_node - head_lhs_node; id++)
  show_lhs_node(id);
}

void	show_rhs()

{
int	id;

for (id = 1; id <= tail_rhs_node - head_rhs_node; id++)
  show_rhs_node(id);
}

int	check_e_o_node(v, epsilon)

lhs_ptr	v;
double	epsilon;

{
lr_aptr	a;
double	match_rc;
int	ret = FALSE;

if (v->matched)
  {
  match_rc = v->matched->c - v->matched->head->p;
  for (a = v->first; a != (v+1)->priced_out; a++)
    {
    if (a->c - a->head->p - match_rc < -epsilon * 1.01)
      {
      (void) printf("Violated epsilon optimality: c(%ld, %ld)=%lg; matched to %ld; eps=%lg\n",
		    v - head_lhs_node + 1,
		    a->head - head_rhs_node + 1,
		    a->c - a->head->p - match_rc,
		    v->matched->head - head_rhs_node + 1,
		    epsilon);
      ret = TRUE;
      }
    }
  }
return(ret);
}

void	check_e_o(epsilon)

double	epsilon;

{
lhs_ptr	v;

for (v = head_lhs_node; v != tail_lhs_node; v++)
  (void) check_e_o_node(v, epsilon);
}
