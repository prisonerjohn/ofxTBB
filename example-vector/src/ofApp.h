#pragma once

#include "tbb/tbb.h"

#include "ofMain.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp
{
public:
    struct Triangle
    {
        glm::vec3 p0;
        glm::vec3 p1;
        glm::vec3 p2;
    };

    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    ofxPanel guiPanel;
    ofParameter<float> fpsLabel{ "FPS", 0.0f, 0.0f, 30.0f };
    ofParameter<int> funcTime{ "Func Time", 0 };
    ofParameter<int> primDetail{ "Prim Detail", 4, 1, 7 };
    ofParameter<int> mouseDistance{ "Mouse Distance", 16, 0, 256 };
    ofParameter<int> loopMode{ "Loop Mode", 0, 0, 2 };

    ofEventListeners eventListeners;

    ofEasyCam cam;
    of3dPrimitive primitive;

    std::vector<glm::vec3> triangleVerts;
    ofVbo selectVbo;

    glm::vec2 worldToScreen(const glm::vec3& pos, const glm::mat4& mvpMat);

    void pickSerial(const ofMesh& mesh, const glm::mat4& mvpMat);
    void pickParallelDirect(const ofMesh& mesh, const glm::mat4& mvpMat);
    void pickParallelBlockedRange(const ofMesh& mesh, const glm::mat4& mvpMat);
};
