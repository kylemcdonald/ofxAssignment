#include "ofMain.h"
#include "ofxAssignment.h"

class ofApp : public ofBaseApp {
public:
    ofxAssignment solver;
    vector<vector<float>> data, grid;
    void setup() {
        // first, load the data
        // this technique is useful for higher-dimensional matching
        data.clear();
        ofBuffer buffer = ofBufferFromFile("data.tsv");
        for(auto& line : buffer.getLines()) {
            vector<string> chunks = ofSplitString(line, "\t");
            if(chunks.size() == 2) {
                data.push_back(vector<float>{ofToFloat(chunks[0]), ofToFloat(chunks[1])});
            }
        }
        
        // then build the grid
        grid.clear();
        for(const ofVec2f& point : makeGrid(120, 119)) {
            grid.push_back(vector<float>{point.x, point.y});
        }
        
        // resize the data if it's too much
        if(data.size() > grid.size()) {
            data.resize(grid.size());
        }
        
        // and finally, match the grid to the data
        auto start = ofGetElapsedTimeMillis();
        grid = solver.match(data, grid);
        auto stop = ofGetElapsedTimeMillis();
        
        // 1000 points in 497ms
        cout << grid.size() << " points in " <<  (stop - start) << "ms" << endl;
        
        // write to output
        ofFile out("out.tsv", ofFile::WriteOnly);
        for(auto& point : grid) {
            out << point[0] << "\t" << point[1] << endl;
        }
        out.close();
        
        ofBackground(0);
        glPointSize(6);
    }
    void draw() {
        ofMesh mesh;
        mesh.setMode(OF_PRIMITIVE_POINTS);
        int n = data.size();
        float t = ofMap(cos(ofGetElapsedTimef()), -1, 1, 0, 1);
        ofSeedRandom(0);
        for(int i = 0; i < n; i++) {
            mesh.addVertex(ofVec2f(data[i][0] * t + grid[i][0] * (1 - t),
                                   data[i][1] * t + grid[i][1] * (1 - t)));
            mesh.addColor(ofFloatColor(1, data[i][0], data[i][1]));
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
