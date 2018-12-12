#define	TRUE	1
#define	FALSE	0
#define	DEFAULT_SCALE_FACTOR	10
#define	EXCESS_THRESH	0
#define	ACTIVE_TYPE	csa_stack
#define	create_active(size)	active = st_create(size)
#define	make_active(v)		st_push(active, (char *) v)
#define	get_active_node(v)	v = (lhs_ptr) st_pop(active)

#define	st_push(s, el) \
{\
    *(s->top) = (char *) el;\
    s->top++;\
}

#define	st_empty(s)	(s->top == s->bottom)

#define	enq(q, el) \
{\
    *(q->tail) = el;\
    if (q->tail == q->end) q->tail = q->storage;\
    else q->tail++;\
}

#define q_empty(q) (q->head == q->tail ? 1 : 0)

#define	insert_list(node, head) \
{\
    node->next = (*(head));\
    (*(head))->prev = node;\
    (*(head)) = node;\
    node->prev = tail_rhs_node;\
}

#define	delete_list(node, head) \
{\
    if (node->prev == tail_rhs_node) (*(head)) = node->next;\
    node->prev->next = node->next;\
    node->next->prev = node->prev;\
}
