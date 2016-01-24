#include "ofMain.h"
#include "ofxAssignment.h"

class ofApp : public ofBaseApp {
public:
    ofEasyCam cam;
    ofxAssignment solver;
    vector<ofVec3f> initial, grid;
    void setup() {
        int side = 12;
        int n = side * side * side;
        
        // first, generate some data on a random walk modulo one
        initial.clear();
        ofVec3f cur;
        ofSeedRandom(ofGetSystemTimeMicros());
        for(int i = 0; i < n; i++) {
            cur += ofVec3f(ofRandomf(), ofRandomf(), ofRandomf()) * .01;
            cur.x = fmodf(cur.x, 1);
            cur.y = fmodf(cur.y, 1);
            cur.z = fmodf(cur.z, 1);
            initial.push_back(cur);
        }
        
        // then build the grid
        grid = makeGrid(side, side, side);

        // and finally, match the grid to the data
        auto start = ofGetElapsedTimeMillis();
        grid = solver.match(initial, grid);
        auto stop = ofGetElapsedTimeMillis();
        
        // 1728 points in 751ms
        // 8000 points in 64432ms
        cout << grid.size() << " points in " <<  (stop - start) << "ms" << endl;
        
        ofBackground(0);
        glPointSize(6);
    }
    void draw() {
        ofMesh mesh;
        mesh.setMode(OF_PRIMITIVE_POINTS);
        int n = initial.size();
        float t = ofMap(cos(ofGetElapsedTimef()), -1, 1, 0, 1);
        ofSeedRandom(0);
        for(int i = 0; i < n; i++) {
            mesh.addVertex(grid[i] * t + initial[i] * (1 - t));
            mesh.addColor(ofFloatColor(initial[i].x, initial[i].y, initial[i].z));
        }
        ofSetColor(255);
        
        cam.begin();
        float scale = ofGetWidth() / 2;
        ofRotateY(ofGetElapsedTimef() * 16.);
        ofTranslate(-scale / 2, -scale / 2, -scale / 2);
        ofScale(scale, scale, scale);
        mesh.draw();
        cam.end();
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
