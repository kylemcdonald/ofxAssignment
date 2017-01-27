#define QUICK_MIN
#if	defined(DEBUG) || defined(CHECK_EPS_OPT)
#include	<stdio.h>
#endif
#include	"csa_types.h"
#include	"csa_defs.h"

/*
extern	double	epsilon, min_epsilon, scale_factor;
*/
extern	double	epsilon, min_epsilon;

#ifdef	USE_PRICE_OUT
extern	double	po_cost_thresh;
#endif
extern	lhs_ptr	head_lhs_node, tail_lhs_node;
#if	defined(DEBUG) || defined(CHECK_EPS_OPT)
extern	rhs_ptr	head_rhs_node;
#endif
#if	defined(USE_PRICE_OUT) || defined(CHECK_EPS_OPT)
extern	unsigned	refines;
#endif
#ifdef	QUICK_MIN
extern	void	best_build();
#endif

int	update_epsilon()

{
#ifdef	USE_PRICE_OUT
double	po_cutoff;
double	thresh;
int	one_priced_in;
#if	defined(QUICK_MIN) && !defined(BACK_PRICE_OUT)
int	need_best_rebuild;
#endif	/* QUICK_MIN && !BACK_PRICE_OUT */
#ifdef	BACK_PRICE_OUT
rl_aptr	b, b_stop;
lhs_ptr	u;
rhs_ptr	w;
#endif	/* BACK_PRICE_OUT */
#endif	/* USE_PRICE_OUT */
#if	defined(USE_PRICE_OUT) || defined(CHECK_EPS_OPT)
double	v_price, red_cost;
lhs_ptr	v;
lr_aptr	a, a_start, a_stop;
#endif	/* USE_PRICE_OUT || CHECK_EPS_OPT */
#ifdef	STRONG_PO
double	fix_in_thresh;
#endif	/* STRONG_PO */
extern	double	scale_factor;

int	fix_in = FALSE;

epsilon /= scale_factor;

if (epsilon < min_epsilon) epsilon = min_epsilon;

#ifdef	USE_PRICE_OUT
po_cutoff = po_cost_thresh * epsilon;
#endif

#if	defined(USE_PRICE_OUT) || defined(CHECK_EPS_OPT)
/*
Now if at least one refine has occurred (and hence all lhs nodes are
matched), check for arcs that should be priced in and price them in,
and check for arcs that should be priced out and price them out.
*/
if (refines > 0)
  {
  for (v = head_lhs_node; v != tail_lhs_node; v++)
    {
    /*
    First, save the location of the first priced-in arc so we don't do
    costly computations for any arc twice.
    */
    a_start = v->first;
    /*
    Determine the price we will assume v has. We choose the price so the
    matching arc will have zero partial reduced cost, and this choice
    enables us to do several things:
    1) Detect epsilon-optimality by checking that partial reduced costs
       of non-matching arcs are at least -epsilon;
    2) Make price-in and price-out decisions in a way that lets us store
       new costs (with incorporated rhs prices) without any backtracking.
       Those arcs with reduced cost close to that of the matching arc
       are priced in, those far away are priced out, and that's that.
    */
    v_price = v->matched->head->p - v->matched->c;
#ifdef	USE_PRICE_OUT
    thresh = po_cutoff - v_price;
    one_priced_in = FALSE;
#if	defined(QUICK_MIN) && !defined(BACK_PRICE_OUT)
    need_best_rebuild = FALSE;
#endif	/* QUICK_MIN && !BACK_PRICE_OUT */
#endif	/* USE_PRICE_OUT */
#ifdef	STRONG_PO
    /*
    Check for arcs to price in.
    */
    fix_in_thresh = -epsilon - v_price;
    for (a = v->priced_out; a != v->first; a++)
      if ((a != v->matched) && ((red_cost = a->c - a->head->p) < thresh))
	{
	price_in_unm_arc(v, a);
	one_priced_in = TRUE;
	/*
	If we have a fix-in, we don't have to unmatch the node here,
	since refine unmatches all nodes initially anyway. Just let
	main know that refine is needed.
	*/
	if (red_cost < fix_in_thresh)
	  fix_in = TRUE;
#if	defined(QUICK_MIN) && !defined(BACK_PRICE_OUT)
	need_best_rebuild = TRUE;
#endif	/* QUICK_MIN && !BACK_PRICE_OUT */
	if (a == v->first) break;
	}
#endif	/* STRONG_PO */
    a_stop = (v+1)->priced_out;
    /*
    For each arc incident to v, decide whether or not to price it out.
    */
    for (a = a_start; a != a_stop; a++)
      {
      if (a != v->matched)
	{
	red_cost = a->c - a->head->p;
#ifdef	USE_PRICE_OUT
	if (red_cost >= thresh)
	  {
	  price_out_unm_arc(v, a);
#if	defined(QUICK_MIN) && !defined(BACK_PRICE_OUT)
	  need_best_rebuild = TRUE;
#endif	/* QUICK_MIN && !BACK_PRICE_OUT */
	  }
	else
	  {
	  one_priced_in = TRUE;
#endif	/* USE_PRICE_OUT */
#ifdef	CHECK_EPS_OPT
	  /*
	  0.01 in the following line because of precision problems
	  that are ultimately OK, although they make the flow look
	  non-epsilon-optimal.
	  */
	  if (v_price + red_cost < -epsilon * (scale_factor + 0.01))
	    {
	    (void) printf("Epsilon optimality violation! c(%ld, %ld)=%lg; epsilon=%lg\n",
			  v - head_lhs_node + 1, a->head - head_rhs_node + 1,
			  v_price + red_cost, epsilon);
	    (void) fflush(stdout);
	    }
#endif	/* CHECK_EPS_OPT */
#ifdef	USE_PRICE_OUT
	  }
#endif	/* USE_PRICE_OUT */
	}
      }
#ifdef	USE_PRICE_OUT
    a = v->matched;
#ifdef	STRONG_PO
    if (one_priced_in)
      {
      if (!a->head->node_info.priced_in)
	{
	/*
	Matching arc is priced out.
	*/
	price_in_mch_arc(v, a);
#if	defined(QUICK_MIN) && !defined(BACK_PRICE_OUT)
	need_best_rebuild = TRUE;
#endif	/* QUICK_MIN && !BACK_PRICE_OUT */
	}
      }
    else
#else	/* !STRONG_PO */
    if (!one_priced_in)
#endif	/* STRONG_PO */
      if (a->head->node_info.priced_in)
	{
	/*
	No arcs are priced in except the matching arc. Price it out,
	too, and if we use back-arc price-outs, price out all the arcs
	incident to its head.
	*/
	price_out_mch_arc(v, a);
#if	defined(QUICK_MIN) && !defined(BACK_PRICE_OUT)
	need_best_rebuild = TRUE;
#endif	/* QUICK_MIN && !BACK_PRICE_OUT */
#ifdef	BACK_PRICE_OUT
	w = a->head;
	b_stop = (w+1)->priced_out;
	for (b = w->back_arcs; b != b_stop; b++)
	  {
	  u = b->tail;
	  a = b->rev;
	  price_out_unm_arc(u, a);
	  }
#endif	/* BACK_PRICE_OUT */
	}
#if	defined(QUICK_MIN) && !defined(BACK_PRICE_OUT)
    /*
    Make sure v->node_info.few_arcs reflects the priced-in degree of v.
    */
    if (a_stop - v->first < NUM_BEST + 1)
      v->node_info.few_arcs = TRUE;
    else
      {
      v->node_info.few_arcs = FALSE;
      if (need_best_rebuild)
	best_build(v);
      }
#endif	/* QUICK_MIN && !BACK_PRICE_OUT */
#endif	/* USE_PRICE_OUT */
    }
#if	defined(QUICK_MIN) && defined(BACK_PRICE_OUT)
  /*
  Rebuild the best list for every node, since back price outs mean we
  can't build them as we do the price outs.
  */
  for (v = head_lhs_node; v != tail_lhs_node; v++)
    if ((v+1)->priced_out - v->first < NUM_BEST + 1)
      v->node_info.few_arcs = TRUE;
    else
      {
      v->node_info.few_arcs = FALSE;
      best_build(v);
      }
#endif
  }
#endif	/* USE_PRICE_OUT || CHECK_EPS_OPT */
return(!fix_in);
}
