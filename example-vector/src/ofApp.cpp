#include "ofApp.h"

void ofApp::setup()
{
    guiPanel.setup("Picker", "settings.json");
    guiPanel.add(fpsLabel);
    guiPanel.add(funcTime);
    guiPanel.add(primDetail);
    guiPanel.add(mouseDistance);
    guiPanel.add(loopMode);

    eventListeners.push(primDetail.newListener([&](const int& value)
    {
        primitive = ofIcoSpherePrimitive(256, value);
    }));

    primitive = ofIcoSpherePrimitive(256, primDetail);
}

void ofApp::update()
{
    const auto mvpMat = cam.getModelViewProjectionMatrix();
    if (loopMode == 0)
    {
        pickSerial(primitive.getMesh(), mvpMat);
    }
    else if (loopMode == 1)
    {
        pickParallelDirect(primitive.getMesh(), mvpMat);
    }
    else //if (loopMode == 2)
    {
        pickParallelBlockedRange(primitive.getMesh(), mvpMat);
    }

    selectVbo.setVertexData(triangleVerts.data(), triangleVerts.size(), GL_DYNAMIC_DRAW);

    fpsLabel = ofGetFrameRate();
}

void ofApp::draw()
{
    cam.begin();
    ofEnableDepthTest();
    {
        ofSetColor(ofColor::white);
        primitive.drawWireframe();

        ofSetColor(ofColor::gold);
        selectVbo.draw(GL_TRIANGLES, 0, selectVbo.getNumVertices());
    }
    ofDisableDepthTest();
    cam.end();

    guiPanel.draw();
}

glm::vec2 ofApp::worldToScreen(const glm::vec3& pos, const glm::mat4& mvpMat)
{
    // cam.worldToScreen() doesn't work in parallel for some reason, 
    // so we just do it manually using a cached MVP matrix.
    const auto camPos4 = mvpMat * glm::vec4(pos, 1);
    const auto camPos3 = glm::vec3(camPos4) / camPos4.w;
    return glm::vec2(ofMap(camPos3.x, -1, 1, 0, ofGetWidth()), ofMap(camPos3.y, 1, -1, 0, ofGetHeight()));
}

void ofApp::pickSerial(const ofMesh& mesh, const glm::mat4& mvpMat)
{
    const uint64_t startTime = ofGetElapsedTimeMillis();

    triangleVerts.clear();
    const auto& vertices = mesh.getVertices();
    const auto& indices = mesh.getIndices();
    const glm::vec2 mouse = glm::vec2(mouseX, mouseY);
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        const glm::vec3 v0 = vertices[indices[i + 0]];
        const glm::vec3 v1 = vertices[indices[i + 1]];
        const glm::vec3 v2 = vertices[indices[i + 2]];

        const glm::vec2 p0 = worldToScreen(v0, mvpMat);
        const glm::vec2 p1 = worldToScreen(v1, mvpMat);
        const glm::vec2 p2 = worldToScreen(v2, mvpMat);

        if (glm::distance(mouse, p0) < mouseDistance ||
            glm::distance(mouse, p1) < mouseDistance ||
            glm::distance(mouse, p2) < mouseDistance)
        {
            triangleVerts.push_back(v0);
            triangleVerts.push_back(v1);
            triangleVerts.push_back(v2);
        }
    }

    funcTime = ofGetElapsedTimeMillis() - startTime;
}

void ofApp::pickParallelDirect(const ofMesh& mesh, const glm::mat4& mvpMat)
{
    const uint64_t startTime = ofGetElapsedTimeMillis();

    tbb::concurrent_vector<Triangle> triangles;
    const auto& vertices = mesh.getVertices();
    const auto& indices = mesh.getIndices();
    const glm::vec2 mouse = glm::vec2(mouseX, mouseY);
    tbb::parallel_for(size_t(0), indices.size(), size_t(3), [&](size_t i)
    {
        const glm::vec3 v0 = vertices[indices[i + 0]];
        const glm::vec3 v1 = vertices[indices[i + 1]];
        const glm::vec3 v2 = vertices[indices[i + 2]];

        const glm::vec2 p0 = worldToScreen(v0, mvpMat);
        const glm::vec2 p1 = worldToScreen(v1, mvpMat);
        const glm::vec2 p2 = worldToScreen(v2, mvpMat);

        if (glm::distance(mouse, p0) < mouseDistance ||
            glm::distance(mouse, p1) < mouseDistance ||
            glm::distance(mouse, p2) < mouseDistance)
        {
            // Add all points as one object to ensure they stay sequential.
            triangles.push_back(Triangle{ v0, v1, v2 });
        }
    });

    // Copy triangles to points vector.
    triangleVerts.clear();
    for (int i = 0; i < triangles.size(); ++i)
    {
        triangleVerts.push_back(triangles[i].p0);
        triangleVerts.push_back(triangles[i].p1);
        triangleVerts.push_back(triangles[i].p2);
    }

    funcTime = ofGetElapsedTimeMillis() - startTime;
}

void ofApp::pickParallelBlockedRange(const ofMesh& mesh, const glm::mat4& mvpMat)
{
    const uint64_t startTime = ofGetElapsedTimeMillis();

    tbb::concurrent_vector<Triangle> triangles;
    const auto& vertices = mesh.getVertices();
    const auto& indices = mesh.getIndices();
    const glm::vec2 mouse = glm::vec2(mouseX, mouseY);
    // Can't set stride, so set the max to size/3 and adjust index in loop.
    tbb::parallel_for(tbb::blocked_range<size_t>(0, indices.size() / 3), [&](const auto& r)
    {
        for (int i = r.begin(); i < r.end(); ++i)
        {
            const glm::vec3 v0 = vertices[indices[i * 3 + 0]];
            const glm::vec3 v1 = vertices[indices[i * 3 + 1]];
            const glm::vec3 v2 = vertices[indices[i * 3 + 2]];

            const glm::vec2 p0 = worldToScreen(v0, mvpMat);
            const glm::vec2 p1 = worldToScreen(v1, mvpMat);
            const glm::vec2 p2 = worldToScreen(v2, mvpMat);

            if (glm::distance(mouse, p0) < mouseDistance ||
                glm::distance(mouse, p1) < mouseDistance ||
                glm::distance(mouse, p2) < mouseDistance)
            {
                // Add all points as one object to ensure they stay sequential.
                triangles.push_back(Triangle{ v0, v1, v2 });
            }
        }
    });

    // Copy triangles to points vector.
    triangleVerts.clear();
    for (int i = 0; i < triangles.size(); ++i)
    {
        triangleVerts.push_back(triangles[i].p0);
        triangleVerts.push_back(triangles[i].p1);
        triangleVerts.push_back(triangles[i].p2);
    }

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
