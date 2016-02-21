#include "ofxAssignment.h"
#include "lap.h"

float getCost(const ofVec2f& a, const ofVec2f& b) {
    return a.squareDistance(b);
}

float getCost(const ofVec3f& a, const ofVec3f& b) {
    return a.squareDistance(b);
}

float getCost(const vector<float>& a, const vector<float>& b) {
    if(a.size() != b.size()) throw;
    int n = a.size();
    float sum = 0;
    for(int i = 0; i < n; i++) {
        float diff = a[i] - b[i];
        sum += diff * diff;
    }
    return sum;
}

template <class T>
vector<vector<double>> getCost(const vector<T>& a, const vector<T>& b) {
    if(a.size() != b.size()) throw;
    int n = a.size();
    vector<vector<double>> costs(n, vector<double>(n));
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            costs[i][j] = getCost(a[i], b[j]);
        }
    }
    return costs;
}

ofVec2f min(const ofVec2f& a, const ofVec2f& b) {
    return ofVec2f(MIN(a.x, b.x),
                   MIN(a.y, b.y));
}

ofVec2f max(const ofVec2f& a, const ofVec2f& b) {
    return ofVec2f(MAX(a.x, b.x),
                   MAX(a.y, b.y));
}

ofVec2f normalize(ofVec2f& x, const ofVec2f& mi, const ofVec2f& ma) {
    x.x -= mi.x;
    x.y -= mi.y;
    x.x /= (ma.x - mi.x);
    x.y /= (ma.y - mi.y);
    return x;
}

ofVec3f min(const ofVec3f& a, const ofVec3f& b) {
    return ofVec3f(MIN(a.x, b.x),
                   MIN(a.y, b.y),
                   MIN(a.z, b.z));
}

ofVec3f max(const ofVec3f& a, const ofVec3f& b) {
    return ofVec3f(MAX(a.x, b.x),
                   MAX(a.y, b.y),
                   MAX(a.z, b.z));
}

ofVec3f normalize(ofVec3f& x, const ofVec3f& mi, const ofVec3f& ma) {
    x.x -= mi.x;
    x.y -= mi.y;
    x.z -= mi.z;
    x.x /= (ma.x - mi.x);
    x.y /= (ma.y - mi.y);
    x.z /= (ma.z - mi.z);
    return x;
}

template <class T>
vector<T> min(const vector<T>& a, const vector<T>& b) {
    int n = a.size();
    vector<T> result(n);
    for(int i = 0; i < n; i++) {
        result[i] = MIN(a[i], b[i]);
    }
    return result;
}

template <class T>
vector<T> max(const vector<T>& a, const vector<T>& b) {
    int n = a.size();
    vector<T> result(n);
    for(int i = 0; i < n; i++) {
        result[i] = MAX(a[i], b[i]);
    }
    return result;
}

template <class T>
vector<T> normalize(const vector<T>& x, const vector<T>& mi, const vector<T>& ma) {
    int n = x.size();
    vector<T> result(n);
    for(int i = 0; i < n; i++) {
        result[i] = (x[i] - mi[i]) / (ma[i] - mi[i]);
    }
    return result;
}

template <class T>
void normalizeToLimits(vector<T>& x) {
    T xmin, xmax;
    bool first = true;
    for(const T& point : x) {
        if(first) {
            xmin = point;
            xmax = point;
            first = false;
        } else {
            xmin = min(xmin, point);
            xmax = max(xmax, point);
        }
    }
    for(T& point : x) {
        point = normalize(point, xmin, xmax);
    }
}

template <class T>
float getMax(const vector<vector<T>>& m) {
    float max = 0;
    for(auto& row : m) {
        for(auto& e : row) {
            max = MAX(max, e);
        }
    }
    return max;
}

vector<ofVec2f> makeGrid(int xSteps, int ySteps) {
    vector<ofVec2f> grid;
    for(int y = 0; y < ySteps; y++) {
        for(int x = 0; x < xSteps; x++) {
            grid.push_back(ofVec2f((float) x / (xSteps - 1),
                                   (float) y / (ySteps - 1)));
        }
    }
    return grid;
}

vector<ofVec3f> makeGrid(int xSteps, int ySteps, int zSteps) {
    vector<ofVec3f> grid;
    for(int y = 0; y < ySteps; y++) {
        for(int x = 0; x < xSteps; x++) {
            for(int z = 0; z < zSteps; z++) {
                grid.push_back(ofVec3f((float) x / (xSteps - 1),
                                       (float) y / (ySteps - 1),
                                       (float) z / (zSteps - 1)));
            }
        }
    }
    return grid;
}

// these are the available implementations
template vector<ofVec2f> ofxAssignment::match<ofVec2f>(vector<ofVec2f>& a, vector<ofVec2f>& b, bool normalize);
template vector<ofVec3f> ofxAssignment::match<ofVec3f>(vector<ofVec3f>& a, vector<ofVec3f>& b, bool normalize);
template vector<vector<float>> ofxAssignment::match<vector<float>>(vector<vector<float>>& a, vector<vector<float>>& b, bool normalize);

template <class T>
vector<T> ofxAssignment::match(vector<T>& a, vector<T>& b, bool normalize) {
    if(a.size() != b.size()) throw;
    int n = a.size();
    if(normalize) {
        normalizeToLimits(a);
        normalizeToLimits(b);
    }
    vector<vector<double>> cost = getCost(a, b);
    solve(cost);
    vector<T> matched(n);
    for(int i = 0; i < n; i++) {
        int match = assignment[i];
        matched[i] = b[match];
    }
    return matched;
}

const vector<int>& ofxAssignment::solve(const vector<vector<double>>& cost) {
    int dim = cost.size();
    cost_t** assigncost = new cost_t*[dim];
    float scale = BIG / getMax(cost);
    for (int i = 0; i < dim; i++) {
        assigncost[i] = new cost_t[dim];
        for(int j = 0; j < dim; j++) {
            assigncost[i][j] = scale * cost[i][j];
        }
    }
    
    row_t *rowsol = new row_t[dim];
    col_t *colsol = new col_t[dim];
    cost_t *u = new cost_t[dim];
    cost_t *v = new cost_t[dim];
    lap(dim, assigncost, rowsol, colsol, u, v);
    
    assignment = vector<int>(dim);
    for (int i = 0; i < dim; i++) {
        assignment[i] = rowsol[i];
    }
    
    delete [] rowsol;
    delete [] colsol;
    delete [] u;
    delete [] v;
    
    return assignment;
}