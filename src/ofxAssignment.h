#pragma once

#include "ofMain.h"

vector<ofVec2f> makeGrid(int xSteps, int ySteps);
vector<ofVec3f> makeGrid(int xSteps, int ySteps, int zSteps);

class ofxAssignment {
protected:
    vector<unsigned int> assignment;
public:
    vector<unsigned int> sparseSearchRadius;
    
    // note that match() will modify a and b if normalize is true!
    template <class T>
    vector<T> match(vector<T>& a, vector<T>& b, bool normalize = true);
    
    // note that matchSparse() will modify a and b if normalize is true!
    // if subset is 0, match() will be used
    // if subset is >0 and <1, it will be interpreted as a percentage
    // if subset is >=1 it will be interpreted as the number of points per row
    // matchSparse works when all final assignments have similar distances
    // it is good at transforming a circle into a square, but not an L shape into a square
    // because most of the L matches the square exactly, but two sides of the square have no good match
    // with point clouds to grids this means, ideally, the initial point cloud should be
    // uniformly distributed across a square region.
    template <class T>
    vector<T> matchSparse(vector<T>& a, vector<T>& b, float subset = 0.50, bool normalize = true);
    
    // solve is useful for solving a square cost matrix directly
    // note that match() will modify a and b if normalize is true!
    const vector<unsigned int>& solve(vector<vector<double>>& cost, bool normalize = true);
    
    const vector<unsigned int>& getAssignment() const;
};
