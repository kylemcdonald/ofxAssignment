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
    template <class T>
    vector<T> matchSparse(vector<T>& a, vector<T>& b, float subset = 0.50, bool normalize = true);
    
    // solve is useful for solving a square cost matrix directly
    // note that match() will modify a and b if normalize is true!
    const vector<unsigned int>& solve(vector<vector<double>>& cost, bool normalize = true);
    
    const vector<unsigned int>& getAssignment() const;
};
