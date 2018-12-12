#define	NUM_BEST	3

typedef	struct	lhs_node	{
    struct	{
        /*
         flag used to indicate to
         double_push() that so few arcs
         are incident that best[] is
         useless.
         */
        unsigned	few_arcs : 1;
    }	node_info;
    /*
     list of arcs to consider first in
     calculating the minimum-reduced-cost
     incident arc; if we find it here, we
     need look no further.
     */
    struct	lr_arc	*best[NUM_BEST];
    /*
     bound on the reduced cost of an arc we
     can be certain still belongs among
     those in best[].
     */
    double	next_best;
    
    /*
     first arc in the arc array associated
     with this node.
     */
    struct	lr_arc	*priced_out;
    /*
     first priced-in arc in the arc array
     associated with this node.
     */
    struct	lr_arc	*first;
    /*
     matching arc (if any) associated with
     this node; NULL if this node is
     unmatched.
     */
    struct	lr_arc	*matched;
}	*lhs_ptr;

typedef	struct	rhs_node	{
    struct	{
        /*
         flag to indicate this node's
         matching arc (if any) is
         priced in.
         */
        unsigned	priced_in : 1;
    }	node_info;
    /*
     lhs node this rhs node is matched to.
     */
    lhs_ptr	matched;
    /*
     price of this node.
     */
    double	p;
}	*rhs_ptr;

typedef	struct	lr_arc		{
    /*
     rhs node associated with this arc.
     */
    rhs_ptr	head;
    /*
     arc cost.
     */
    double	c;
}	*lr_aptr;

typedef	struct	csa_stack_st	{
    /*
     Sometimes csa_stacks have lhs nodes, and
     other times they have rhs nodes. So
     there's a little type clash;
     everything gets cast to (char *) so we
     can use the same structure for both.
     */
    char	**bottom;
    char	**top;
}	*csa_stack;

typedef	struct	csa_queue_st	{
    /*
     Sometimes csa_queues have lhs nodes, and
     other times they have rhs nodes. So
     there's a little type clash;
     everything gets cast to (char *) so we
     can use the same structure for both.
     */
    char		**head;
    char		**tail;
    char		**storage;
    char		**end;
    unsigned	max_size;
}	*csa_queue;
