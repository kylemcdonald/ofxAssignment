#include "lap.h"

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "csa_types.h"
#include "csa_defs.h"

using namespace std;
using namespace CSA;

extern "C" {
    int update_epsilon();
    void refine();
    csa_stack st_create(unsigned n);
    void best_build(lhs_ptr v);
}

/* ------------------------- Problem size variables -------------------- */
unsigned	n, m;

/* --------------- Data structures describing the problem -------------- */
lhs_ptr	head_lhs_node, tail_lhs_node;
rhs_ptr	head_rhs_node, tail_rhs_node;
lr_aptr	head_lr_arc, tail_lr_arc;

/* ------------------- Bookkeeping/profiling variables ----------------- */
unsigned	double_pushes = 0,
pushes = 0,
relabelings = 0,
refines = 0,
refine_time = 0;
unsigned	rebuilds = 0,
scans = 0,
non_scans = 0;

/* ------------------------- Tunable variables ------------------------- */
/*
 Cost threshhold for pricing out: used even when price-outs are
 switched off to make bounding reduced-cost differences easier in
 double_push(). In principle this is not necessary, but it makes the
 code better.
 */
double		po_cost_thresh;
double		scale_factor;	/* scaling factor */

/*
 Processing variables.
 */
double		epsilon;	/* scaling parameter */
double		min_epsilon;	/* snap to this value when epsilon small */
unsigned	total_e;	/* total excess */
ACTIVE_TYPE	active;		/* list of active nodes */

double	compute_cost()

{
    double	cost = 0.0;
    lhs_ptr	v;
    
    for (v = head_lhs_node; v != tail_lhs_node; v++)
        if (v->matched)
            cost += v->matched->c;
    
    return cost;
}

typedef	struct	temp_arc	{
    lhs_ptr	tail;
    rhs_ptr	head;
    long	cost;
}	*ta_ptr;

unsigned long parse_arcs(const vector<Arc>& arcs, unsigned int lhs_n, unsigned int rhs_n) {
    unsigned int tail, head;
    long cost, *lhs_degree;
    unsigned long max_cost = 0;
    
    lr_aptr a;
    ta_ptr temp_a, temp_arcs;
    lhs_ptr l_v;
    rhs_ptr r_v;
    
    n = lhs_n + rhs_n;
    m = arcs.size();
    
    head_lr_arc = (lr_aptr) malloc((m + 1) * sizeof(struct lr_arc));
    tail_lr_arc = head_lr_arc + m;
    
    bool swap = false;
    if (lhs_n > n - lhs_n) {
        lhs_n = n - lhs_n;
        swap = true;
    }
    head_lhs_node = (lhs_ptr) malloc((lhs_n + 1) *
                                     sizeof(struct lhs_node));
    tail_lhs_node = head_lhs_node + lhs_n;
    head_rhs_node = (rhs_ptr) malloc((n - lhs_n + 1) *
                                     sizeof(struct rhs_node));
    tail_rhs_node = head_rhs_node + n - lhs_n;
    lhs_degree = (long *) malloc(lhs_n * sizeof(long));
    
    temp_arcs = (ta_ptr) malloc(m * sizeof(struct temp_arc));
    if ((head_lhs_node == NULL) || (head_lr_arc == NULL) ||
        (lhs_degree == NULL) || (temp_arcs == NULL)) {
        cerr << "No memory." << endl;
        return 0;
    }
    temp_a = temp_arcs;
    for (tail = 0; tail < lhs_n; tail++)
        lhs_degree[tail] = 0;
    
    for(const Arc& arc : arcs) {
        tail = arc.tail;
        head = arc.head;
        cost = arc.cost;
        
        if (swap) std::swap(head, tail);
        
        if ((tail >= lhs_n) || (head >= n - lhs_n)) {
            cerr << "Out of range arc: " << arc.head << " -> " << arc.tail << endl;
            return 0;
        }
        
        temp_a->head = head_rhs_node + head;
        temp_a->tail = head_lhs_node + tail;
        temp_a->cost = cost;
        if ((cost = abs((int) cost)) > max_cost) {
            max_cost = cost;
        }
        temp_a++;
        lhs_degree[tail]++;
        
    }
    
    a = head_lr_arc;
    for (tail = 0, l_v = head_lhs_node; l_v != tail_lhs_node; l_v++, tail++) {
        l_v->priced_out = l_v->first = a;
        l_v->matched = NULL;
        a += lhs_degree[tail];
        if (lhs_degree[tail] < NUM_BEST + 1) {
            l_v->node_info.few_arcs = true;
        } else {
            l_v->node_info.few_arcs = false;
        }
    }
    tail_lhs_node->priced_out = a;
    
    for (r_v = head_rhs_node; r_v != tail_rhs_node; r_v++) {
        r_v->node_info.priced_in = true;
        r_v->matched = NULL;
        r_v->p = 0.0;
    }
    
    for (temp_a--; temp_a != temp_arcs - 1; temp_a--) {
        a = temp_a->tail->first + (--lhs_degree[temp_a->tail - head_lhs_node]);
        a->head = temp_a->head;
        a->c = (double) temp_a->cost;
    }
    
    (void) free((char *) temp_arcs);
    (void) free((char *) lhs_degree);
    
    return(max_cost);
}

vector<unsigned int> lap(const vector<Arc>& arcs, unsigned int lhs_n, unsigned int rhs_n) {
    //cout << "using lhs_n " << lhs_n << " and rhs_n " << rhs_n << endl;
    //cout << "total arcs count: " << arcs.size() << endl;
    
    lhs_ptr	l_v;
    
    //cout << "parsing arcs" << endl;
    epsilon = parse_arcs(arcs, lhs_n, rhs_n);
    //cout << "epsilon (max cost) is " << epsilon << endl;
    
    scale_factor = DEFAULT_SCALE_FACTOR;
    po_cost_thresh = 2.0 * (double) n * (scale_factor + 1);
    
    //cout << "finding best_build" << endl;
    create_active(n);
    for (l_v = head_lhs_node; l_v != tail_lhs_node; l_v++)
        if (!l_v->node_info.few_arcs)
            best_build(l_v);
    rebuilds = 0;
    
    //cout << "refining" << endl;
    min_epsilon = 2.0 / (double) (n + 1);
    while (epsilon > min_epsilon) {
        (void) update_epsilon();
        //cout << "\tepsilon: " << epsilon << " min_epsilon:" << min_epsilon << endl;
        refine();
    }
    
    //cout << "copying solution" << endl;
    vector<unsigned int> lhs_sol(lhs_n);
    for (lhs_ptr v = head_lhs_node; v != tail_lhs_node; v++) {
        unsigned int lhs = v - head_lhs_node;
        unsigned int rhs = v->matched->head - head_rhs_node + tail_lhs_node - head_lhs_node - lhs_n;
        double cost = v->matched->c;
        lhs_sol[lhs] = rhs;
    }
    
    return lhs_sol;
}

void get_totals(const vector<Arc>& arcs, unsigned int& lhs_n, unsigned int& rhs_n) {
    lhs_n = 0;
    rhs_n = 0;
    for(const Arc& arc : arcs) {
        if(arc.tail > lhs_n) lhs_n = arc.tail;
        if(arc.head > rhs_n) rhs_n = arc.head;
    }
    lhs_n++;
    rhs_n++;
}

vector<unsigned int> CSA::lap(const vector<Arc>& arcs) {
    unsigned int lhs_n, rhs_n;
    get_totals(arcs, lhs_n, rhs_n);
    return ::lap(arcs, lhs_n, rhs_n);
}

template <class T>
vector<Arc> build_arcs(const vector<vector<T>>& costs, unsigned int& lhs_n, unsigned int& rhs_n) {
    lhs_n = costs.size();
    rhs_n = costs[0].size();
    vector<Arc> arcs(lhs_n * rhs_n);
    auto itr = arcs.begin();
    unsigned int tail = 0;
    for(const auto& row : costs) {
        unsigned int head = 0;
        for(const auto& cost: row) {
            Arc& arc = *itr;
            arc.tail = tail;
            arc.head = head;
            arc.cost = cost;
            head++;
            itr++;
        }
        tail++;
    }
    return arcs;
}

template vector<unsigned int> CSA::lap(const vector<vector<double>>& costs);
template vector<unsigned int> CSA::lap(const vector<vector<unsigned long>>& costs);

template <class T>
vector<unsigned int> CSA::lap(const vector<vector<T>>& costs) {
    unsigned int lhs_n, rhs_n;
    vector<Arc> arcs = build_arcs(costs, lhs_n, rhs_n);
    return ::lap(arcs, lhs_n, rhs_n);
}

float distance_squared(const CSA::Point& a, const CSA::Point& b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return dx * dx + dy * dy;
}

struct ReducedCol {
    float reduced_cost;
    unsigned int col;
};

struct reduced_col_compare {
    bool operator() (const ReducedCol& a, const ReducedCol& b) const {
        return (a.reduced_cost < b.reduced_cost);
    }
};
// todo: pick a more adaptive scaling factor for the costs
// todo: better way to pick percentage/subset value
// todo: parallelize the things that are easy (reduced cost, partial sort)
// todo: better technique for guaranteeing perfect matching, e.g. random iterative pairing with available nearest neighbors
vector<unsigned int> CSA::lap(const vector<CSA::Point>& a, const vector<CSA::Point>& b, float amount, std::vector<unsigned int>* search_radius) {
    float (*distance_func)(const CSA::Point&, const CSA::Point&);
    distance_func = distance_squared;
    
    unsigned int rows = a.size();
    unsigned int cols = b.size();
    
    // get minimum across rows
    //cout << "getting minimum across rows" << endl;
    vector<double> row_minimum(rows, numeric_limits<float>::infinity());
    for(unsigned int row = 0; row < rows; row++) {
        for(unsigned int col = 0; col < cols; col++) {
            float cost = distance_func(a[row], b[col]);
            if(cost < row_minimum[row]) {
                row_minimum[row] = cost;
            }
        }
    }
    
    // get reduced minimum acros cols
    //cout << "get reduced minimum acros cols" << endl;
    vector<double> col_minimum(cols, numeric_limits<float>::infinity());
    for(unsigned int row = 0; row < rows; row++) {
        for(unsigned int col = 0; col < cols; col++) {
            float cost = distance_func(a[row], b[col]);
            cost -= row_minimum[row]; // reduce by row minimum
            if(cost < col_minimum[col]) {
                col_minimum[col] = cost;
            }
        }
    }
    
    float scale_cost = 100000000; // what should this be?
    
    unsigned int subset = amount;
    if(amount < 1) {
        // convert % to degree
        subset = cols * amount;
    } else if (amount >= a.size()) {
            amount = a.size();
        }
    //cout << "degree is " << subset << endl;
    
    // sort each row by reduced cost and create arcs from the lowest %
    //cout << "sort each row by reduced cost and create arcs from the lowest %" << endl;
    vector<CSA::Arc> arcs((rows * subset) + rows);
    auto itr = arcs.begin();
    vector<ReducedCol> reduced_cols(cols);
    for(unsigned int row = 0; row < rows; row++) {
        for(unsigned int col = 0; col < cols; col++) {
            // get reduced cost
            double cost = distance_func(a[row], b[col]);
            cost -= row_minimum[row] - col_minimum[col];
            cost *= scale_cost;
            
            ReducedCol& reduced_col = reduced_cols[col];
            reduced_col.col = col;
            reduced_col.reduced_cost = cost;
            
            // add diagonal: guarantee that perfect matching exists
            if(row == col) {
                Arc& arc = *itr;
                arc.tail = row;
                arc.head = col;
                arc.cost = cost;
                itr++;
            }
        }
        
        // partial sort to get `subset` lowest cost arcs
        partial_sort(reduced_cols.begin(), reduced_cols.begin() + subset, reduced_cols.end(), reduced_col_compare());
        for(unsigned int i = 0; i < subset; i++) {
            const ReducedCol& reduced_col = reduced_cols[i];
            unsigned int col = reduced_col.col;
            if(col != row) { // ignore the diagonal (already added)
                Arc& arc = *itr;
                arc.tail = row;
                arc.head = col;
                arc.cost = reduced_col.reduced_cost;
                itr++;
            }
        }
    }
    
    // remove any unused elements at the end of arcs
    arcs.resize(distance(arcs.begin(), itr));
    
    // do lap
    vector<unsigned int> assignment = ::lap(arcs, rows, cols);
    
    // check what has been assigned
    if(search_radius != nullptr) {
        search_radius->resize(rows);
        for(unsigned int row = 0; row < rows; row++) {
            for(unsigned int col = 0; col < cols; col++) {
                // get reduced cost
                double cost = distance_func(a[row], b[col]);
                cost -= row_minimum[row] - col_minimum[col];
                cost *= scale_cost;
                
                ReducedCol& reduced_col = reduced_cols[col];
                reduced_col.col = col;
                reduced_col.reduced_cost = cost;
            }
            
            // partial sort and find assigned index in sorted array
            partial_sort(reduced_cols.begin(), reduced_cols.begin() + subset, reduced_cols.end(), reduced_col_compare());
            for(unsigned int i = 0; i < subset; i++) {
                const ReducedCol& reduced_col = reduced_cols[i];
                if(reduced_col.col == assignment[row]) {
                    (*search_radius)[row] = i;
                    break;
                }
            }
        }
    }
    
    return assignment;
}
