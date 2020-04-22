#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp
{
public:
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
    ofParameter<int> threshold{ "Threshold", 180, 0, 255 };
    ofParameter<int> loopMode{ "Loop Mode", 0, 0, 2 };

    ofVideoGrabber grabber;
    ofImage image;

    void thresholdSerial(ofPixels& pix);
    void thresholdParallelDirect(ofPixels& pix);
    void thresholdParallelBlockedRange(ofPixels& pix);
};
