#include	<stdio.h>
#include	"csa_types.h"
#include	"csa_defs.h"

#define	sort_insert(best, size, a, a_prc, nsize) \
\
{\
unsigned	si_i, si_j;\
\
if (size == 0)\
  best[0] = a;\
else\
  {\
  si_j = size;\
  for (si_i = 0; si_i < size; si_i++)\
    if (a_prc < best[si_i]->c - best[si_i]->head->p)\
      {\
      si_j = si_i;\
      for (si_i = nsize - 1; si_i > si_j; si_i--)\
	best[si_i] = best[si_i - 1];\
      break;\
      }\
  best[si_j] = a;\
  }\
}

extern	lhs_ptr		head_lhs_node, tail_lhs_node;
extern	double		epsilon;
extern	unsigned	total_e;
extern	ACTIVE_TYPE	active;
extern	double		po_cost_thresh;
extern	char		*st_pop(), *deq();

void	best_build(lhs_ptr v) {
    unsigned	i;
    lr_aptr		a, a_stop;
    double		red_cost, save_max;
    
    for (i = 0, a = v->first; i < NUM_BEST; i++, a++)
    {
        red_cost = a->c - a->head->p;
        sort_insert(v->best, i, a, red_cost, i + 1);
    }
    /*
     Calculate initial next_best by looking at the next arc in the
     adjacency list.
     */
    if ((v->next_best = a->c - a->head->p) <
        (red_cost = v->best[NUM_BEST - 1]->c -
         v->best[NUM_BEST - 1]->head->p))
    {
        sort_insert(v->best, NUM_BEST, a, v->next_best, NUM_BEST);
        v->next_best = red_cost;
    }
    a++;
    /*
     Now go through remaining arcs in adjacency list and place each one
     at the appropriate place in best[], if any.
     */
    a_stop = (v+1)->priced_out;
    for (; a != a_stop; a++)
    {
        if ((red_cost = a->c - a->head->p) < v->next_best)
        {
            if (red_cost < (save_max = v->best[NUM_BEST - 1]->c -
                            v->best[NUM_BEST - 1]->head->p))
            {
                sort_insert(v->best, NUM_BEST, a, red_cost, NUM_BEST);
                v->next_best = save_max;
            }
            else
                v->next_best = red_cost;
        }
    }
}

/* Assume v has excess (is unassigned) and do a double push from v. */

void	double_push(lhs_ptr v) {
    double	v_pref, v_second, red_cost, adm_gap;
    lr_aptr	a, a_stop, adm;
    rhs_ptr	w;
    lhs_ptr	u;
    unsigned	i;
    lr_aptr		*check_arc;
    
    /*
     Begin part I: Compute the following:
     o adm, the minimum-reduced-cost arc incident to v,
     o adm_gap, the amount by which the reduced cost of adm must be
     increased to make it equal in reduced cost to another arc incident
     to v, or enough to price the arc out if it is the only incident
     arc.
     */
    
    if (v->node_info.few_arcs)
    {
        
        /*
         If the input problem is feasible, it is never the case that
         (a_stop == a) after the following two lines because we never get
         excess at a node with no incident arcs.
         */
        a_stop = (v+1)->priced_out;
        a = v->first;
        v_pref = a->c - a->head->p;
        v_second = v_pref + epsilon * (po_cost_thresh + 1.0);
        adm = a;
        /*
         After this loop, v_pref is the minimum reduced cost of an edge out of
         v, and v_second is the second-to-minimum such reduced cost.
         */
        for (a++; a != a_stop; a++)
            if (v_pref > (red_cost = a->c - a->head->p))
            {
                v_second = v_pref;
                v_pref = red_cost;
                adm = a;
            }
            else if (v_second > red_cost)
                v_second = red_cost;
        
    }
    else
    {
        /*
         Find the minimum and second-minimum edges listed in the node's
         best[] array, and check whether their present partial reduced
         costs are below the node's bound as stored in next_best. If they
         are, we calculate adm_gap and are done with part I. If not, we
         rebuild the best[] array and the next_best bound, and calculate the
         required information.
         */
        adm = v->best[1];
        v_second = adm->c - adm->head->p;
        adm = v->best[0];
        v_pref = adm->c - adm->head->p;
        if (v_pref > v_second)
        {
            adm = v->best[1];
            red_cost = v_second;
            v_second = v_pref;
            v_pref = red_cost;
        }
        for (i = NUM_BEST - 2, check_arc = &v->best[2]; i > 0; i--, check_arc++)
        {
            a = *check_arc;
            if (v_pref > (red_cost = a->c - a->head->p))
            {
                v_second = v_pref;
                v_pref = red_cost;
                adm = a;
            }
            else if (v_second > red_cost)
                v_second = red_cost;
        }
        if (v_second > v->next_best)
        {
            /*
             Rebuild the best[] array and recalculate next_best.
             !v->node_info.few_arcs, so we know there are enough incident arcs
             to fill up best[] initially and have one left over for next_best.
             */
            best_build(v);
            adm = v->best[1];
            v_second = adm->c - adm->head->p;
            adm = v->best[0];
            v_pref = adm->c - adm->head->p;
        }
    }
    
    adm_gap = v_second - v_pref;
    
    /*
     Begin part II: Using the information computed in part I,
     o match v to w, adm's head, and
     o unmatch the node (if any) formerly matched to w.
     In the case where w's current matching arc is priced out, we do not
     change the matching, but we reset the value of adm_gap so that the
     (v, w) arc will be priced out.
     */
    w = adm->head;
    if ((u = w->matched))
    /*
     If w's matched arc is priced in, go ahead and unmatch (u, w) and
     match (v, w). If w's matched arc is priced out, abort the double
     push and relabel w so v no longer prefers w.
     */
        if (w->node_info.priced_in)
        {
            u->matched = NULL;
            make_active(u);
            v->matched = adm;
            w->matched = v;
        }
        else
        {
            adm_gap = epsilon * po_cost_thresh;
            make_active(v);
        }
        else
        {
            total_e--;
            v->matched = adm;
            w->matched = v;
        }
    
    /*
     Relabel w: v's price is chosen to make the implicit reduced cost of
     v's new preferred arc (v_pref + adm_gap) equal to zero. Then w's price
     is chosen so that the arc just matched has implicit reduced cost
     -epsilon.
     */
    w->p -= adm_gap + epsilon;
}

void	refine() {
    lhs_ptr	v;
    
    /*
     Saturate all negative arcs: Negative arcs are exactly those
     right-to-left matching arcs with negative reduced cost, and there is
     an interpretation of the implicit price function on the left that
     admits all right-to-left matching arcs. This interpretation is
     always consistent with the stored prices of lhs nodes in the case
     of EXPLICIT_PRICES.
     */
    total_e = 0;
    for (v = head_lhs_node; v != tail_lhs_node; v++)
    {
        if (v->matched && v->matched->head->node_info.priced_in)
        {
            v->matched->head->matched = NULL;
            v->matched = NULL;
        }
        if (v->matched == NULL)
        {
            total_e++;
            make_active(v);
        }
    }
    
    while (total_e > EXCESS_THRESH)
    {
        get_active_node(v);
        double_push(v);
    }
}
