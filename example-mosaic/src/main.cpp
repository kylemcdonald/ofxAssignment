#include "ofMain.h"

ofColor getAverage(const ofPixels& pix, int x, int y, int w, int h) {
    float r = 0, g = 0, b = 0;
    for(int j = y; j < y + h; j++) {
        for(int i = x; i < x + w; i++) {
            const ofColor& cur = pix.getColor(i, j);
            r += cur.r;
            g += cur.g;
            b += cur.b;
        }
    }
    float n = w * h;
    return ofColor(r / n, g / n, b / n);
}

class Tile : public ofVec2f {
public:
    int side;
    float brightness, hue;
    ofColor average;
    vector<ofColor> grid;
    Tile(int x, int y, int side, const vector<ofColor>& grid)
    :ofVec2f(x, y)
    ,side(side)
    ,grid(grid) {
    }
    static vector<Tile> buildTiles(const ofPixels& pix, int side) {
        // we could do this with resizing but OF doesn't have a good downsampling method
        float third = side / 3.;
        int w = pix.getWidth(), h = pix.getHeight();
        int nx = w / side, ny = h / side;
        vector<Tile> tiles;
        for(int y = 0; y < h; y+=side) {
            for(int x = 0; x < w; x+=side) {
                vector<ofColor> grid;
                for(int ky = 0; ky < 3; ky++) {
                    for(int kx = 0; kx < 3; kx++) {
                        grid.push_back(getAverage(pix, x+kx*third, y+ky*third, third, third));
                    }
                }
                tiles.emplace_back(x, y, side, grid);
            }
        }
        return tiles;
    }
};

float sqdistance(const ofColor& c1, const ofColor& c2) {
    long rmean = ((long) c1.r + (long) c2.r) / 2;
    long r = (long) c1.r - (long) c2.r;
    long g = (long) c1.g - (long) c2.g;
    long b = (long) c1.b - (long) c2.b;
    return ((((512 + rmean)*r*r)>>8) + 4*g*g + (((767-rmean)*b*b)>>8));
}

#include "ofxAssignment.h"

class ofApp : public ofBaseApp {
public:
    ofVideoGrabber video;
    ofImage leftImage, rightImage;
    vector<Tile> leftTiles, rightTiles;
    ofxAssignment solver;
    int side = 20;
    void setup() {
//        ofSetFrameRate(5);
        video.initGrabber(1280, 720);
    }
    void update() {
        video.update();
        if(video.isFrameNew()) {
            int w = video.getWidth();
            int h = video.getHeight();
            ofPixels& pix = video.getPixels();
            leftImage.allocate(w / 2, h, OF_IMAGE_COLOR);
            rightImage.allocate(w / 2, h, OF_IMAGE_COLOR);
            pix.cropTo(leftImage.getPixels(), 0, 0, w / 2, h);
            pix.cropTo(rightImage.getPixels(), w / 2, 0, w / 2, h);
            leftImage.update();
            rightImage.update();
            leftTiles = Tile::buildTiles(leftImage, side);
            rightTiles = Tile::buildTiles(rightImage, side);
            
            int n = leftTiles.size();
            vector<vector<double>> cost(n, vector<double>(n));
            for(int i = 0; i < n; i++) {
                Tile& leftTile = leftTiles[i];
                for(int j = 0; j < n; j++) {
                    Tile& rightTile = rightTiles[j];
                    double& dist = cost[i][j];
                    int m = leftTile.grid.size();
                    dist = 0;
                    for(int k = 0; k < m; k++) {
                        dist += sqdistance(leftTile.grid[k], rightTile.grid[k]);
                    }
                }
            }
            
            const vector<int>& assignment = solver.solve(cost);
            vector<Tile> after;
            for(int i : assignment) {
                after.push_back(rightTiles[i]);
            }
            rightTiles = after;
        }
    }
    void draw() {
        int w = video.getWidth();
        int n = leftTiles.size();
        for(int i = 0; i < n; i++) {
            Tile& left = leftTiles[i];
            Tile& right = rightTiles[i];
            rightImage.drawSubsection(left.x, left.y, side, side, right.x, right.y);
            leftImage.drawSubsection(w / 2 + right.x, right.y, side, side, left.x, left.y);
        }
    }
};
int main() {
    ofSetupOpenGL(1280, 720, OF_WINDOW);
    ofRunApp(new ofApp());
}
