#include "csa_types.h"
#include "csa_defs.h"

extern double epsilon, min_epsilon;
extern lhs_ptr head_lhs_node, tail_lhs_node;
extern void best_build();

int	update_epsilon() {
    extern	double scale_factor;
    int	fix_in = FALSE;
    epsilon /= scale_factor;
    if (epsilon < min_epsilon) {
        epsilon = min_epsilon;
    }
    return(!fix_in);
}
