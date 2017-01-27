#include "ofMain.h"
#include "ofxAssignment.h"

class ofApp : public ofBaseApp {
public:
    ofxAssignment solver;
    vector<ofVec2f> initial, grid;
    void setup() {
        int side = 128;
        int n = side * side;
        
        // first, generate some data on a random walk modulo one
        initial.clear();
        ofVec2f cur;
        ofSeedRandom(0);
        for(int i = 0; i < n; i++) {
            cur += ofVec2f(ofRandomf(), ofRandomf()) * .01;
            cur.x = fmodf(cur.x, 1);
            cur.y = fmodf(cur.y, 1);
            initial.push_back(cur);
        }
        
        // then build the grid
        grid = makeGrid(side, side);

        // and finally, match the grid to the data
        auto start = ofGetElapsedTimeMillis();
        grid = solver.matchSparse(initial, grid);
        auto stop = ofGetElapsedTimeMillis();
        
        // 625 points in 64ms
        // 1024 points in 211ms
        // 4096 points in 8841ms
        cout << grid.size() << " points in " <<  (stop - start) << "ms" << endl;
        
        ofBackground(0);
        glPointSize(1);
    }
    void draw() {
        ofMesh mesh;
        mesh.setMode(OF_PRIMITIVE_POINTS);
        int n = initial.size();
        float t = ofMap(cos(ofGetElapsedTimef()), -1, 1, 0, 1);
        ofSeedRandom(0);
        for(int i = 0; i < n; i++) {
            mesh.addVertex(grid[i] * t + initial[i] * (1 - t));
            mesh.addColor(ofFloatColor(1, initial[i].x, initial[i].y));
        }
        float padding = 128;
        float scale = ofGetWidth() - 2 * padding;
        ofTranslate(padding, padding);
        ofScale(scale, scale);
        ofSetColor(255);
        mesh.draw();
    }
    void keyPressed(int key) {
        if(key == ' ') {
            setup();
        }
    }
};

int main() {
    ofSetupOpenGL(400, 400, OF_WINDOW);
    ofRunApp(new ofApp());
}
