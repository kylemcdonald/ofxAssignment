#pragma once

#include <vector>

namespace CSA {
    // All these implementations return the row solution (lhs solution) to the min-cost problem.
    
    // From a set of N on the left hand side, to a set of M on the right hand side.
    // This can also be interpreted as N rows and M columns.
    struct Arc {
        unsigned int tail; //  [0, N)
        unsigned int head; //  [0, M)
        unsigned long cost; // [0, a large value]
    };
    // Pass a list of arcs. May be sparse or dense, but a perfect assignment must exist.
    std::vector<unsigned int> lap(const std::vector<Arc>& arcs);
    
    // Pass a dense cost matrix.
    template <class T>
    std::vector<unsigned int> lap(const std::vector<std::vector<T>>& costs);
    
    // A 2d point.
    struct Point {
        float x;
        float y;
    };
    // Pass two sets of points, evaluate a subset of the cost matrix.
    std::vector<unsigned int> lap(const std::vector<Point>& a, const std::vector<Point>& b, float percent=0.10);
}
