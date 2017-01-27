#pragma once

#include "ofMain.h"

vector<ofVec2f> makeGrid(int xSteps, int ySteps);
vector<ofVec3f> makeGrid(int xSteps, int ySteps, int zSteps);

class ofxAssignment {
protected:
    vector<unsigned int> assignment;
public:
    
    // note that match() will modify a and b if normalize is true!
    template <class T>
    vector<T> match(vector<T>& a, vector<T>& b, bool normalize = true);
    
    template <class T>
    vector<T> matchSparse(vector<T>& a, vector<T>& b, float percent = 0.40, bool normalize = true);
    
    // solve is useful for solving a square cost matrix directly
    // note that match() will modify a and b if normalize is true!
    const vector<unsigned int>& solve(vector<vector<double>>& cost, bool normalize = true);
};
