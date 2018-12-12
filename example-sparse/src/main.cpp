#include "ofMain.h"
#include "ofxAssignment.h"

class ofApp : public ofBaseApp {
public:
    ofxAssignment solver;
    vector<ofVec2f> initial, grid;
    void setup() {
        int side = 32;
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
//        grid = solver.matchSparse(initial, grid);
        grid = solver.match(initial, grid);
        auto stop = ofGetElapsedTimeMillis();
        
        // at subset=0.5
        // (all have some glitches)
        // 25x25=625 points in 131ms sparse, 102ms dense
        // 32x32=1024 points in 382ms sparse, 277ms dense
        // 64x64=4096 points in 7s sparse, 4s dense
        // 96x96=9216 points in 34s sparse, 23s dense
        // 128x128=16384 points in 130s sparse (lapjv python is 1560s), 88s dense
        // at subset=0.1
        // 320x320=102400 points in 4423s (73m) lots of glitches
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
            mesh.addVertex(ofVec3f(grid[i] * t + initial[i] * (1 - t)));
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
