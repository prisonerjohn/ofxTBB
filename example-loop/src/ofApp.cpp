#include "ofApp.h"

#include "tbb/tbb.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    grabber.setDeviceID(1);
    grabber.setup(1280, 720);

    guiPanel.setup("Threshold", "settings.json");
    guiPanel.add(fpsLabel);
    guiPanel.add(funcTime);
    guiPanel.add(threshold);
    guiPanel.add(loopMode);
}

void ofApp::update()
{
    grabber.update();
    if (grabber.isFrameNew())
    {
        image.setFromPixels(grabber.getPixels());

        if (loopMode == 0)
        {
            thresholdSerial(image.getPixels());
        }
        else if (loopMode == 1)
        {
            thresholdParallelDirect(image.getPixels());
        }
        else //if (loopMode == 2)
        {
            thresholdParallelBlockedRange(image.getPixels());
        }

        image.update();
    }

    fpsLabel = ofGetFrameRate();
}

void ofApp::draw()
{
    grabber.draw(0, 0, 640, 360);
    image.draw(0, 360, 640, 360);
    guiPanel.draw();
}

void ofApp::thresholdSerial(ofPixels& pix)
{
    const uint64_t startTime = ofGetElapsedTimeMillis();
    for (size_t y = 0; y < pix.getHeight(); ++y)
    {
        for (size_t x = 0; x < pix.getWidth(); ++x)
        {
            if (pix.getColor(x, y).getBrightness() < threshold)
            {
                pix.setColor(x, y, ofColor::black);
            }
        }
    }
    funcTime = ofGetElapsedTimeMillis() - startTime;
}

void ofApp::thresholdParallelDirect(ofPixels& pix)
{
    const uint64_t startTime = ofGetElapsedTimeMillis();
    tbb::parallel_for(size_t(0), pix.getHeight(), [&](size_t y) 
    {
        tbb::parallel_for(size_t(0), pix.getWidth(), [&](size_t x)
        {
            if (pix.getColor(x, y).getBrightness() < threshold)
            {
                pix.setColor(x, y, ofColor::black);
            }
        });
    });
    funcTime = ofGetElapsedTimeMillis() - startTime;
}

void ofApp::thresholdParallelBlockedRange(ofPixels& pix)
{
    const uint64_t startTime = ofGetElapsedTimeMillis();
    tbb::parallel_for(tbb::blocked_range2d<size_t>(0, pix.getHeight(), 0, pix.getWidth()), [&](const auto& r)
    {
        for (int y = r.rows().begin(); y < r.rows().end(); ++y)
        {
            for (int x = r.cols().begin(); x < r.cols().end(); ++x)
            {
                if (pix.getColor(x, y).getBrightness() < threshold)
                {
                    pix.setColor(x, y, ofColor::black);
                }
            }
        }
    });
    funcTime = ofGetElapsedTimeMillis() - startTime;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
