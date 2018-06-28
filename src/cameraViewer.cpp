#include "cameraViewer.h"
#include "globalvariables.h"



using namespace qglviewer;

CameraViewer::CameraViewer(Camera *camera) : c(camera){}

void CameraViewer::draw() {
    quadtree.init();
    qglviewer::Vec v = camera()->position();
    quadtree.update(v.x, v.y, v.z);

    // Draws the other viewer's camera
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(4.0);
    glColor4f(1.0, 1.0, 1.0, 0.5);
    c->draw();

    if(quadtree.isDrawMesh())
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glLineWidth(1.0);
    quadtree.render();

    if (sky.wantSky()) {
        sky.set(0, 0, 0.25f, (float)mapSize*1.5f);
        sky.render();
    }


}

void CameraViewer::init() {
    restoreStateFromFile();

    setSceneRadius(mapSize);

    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDisable(GL_LIGHTING);
}

