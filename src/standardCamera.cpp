#include "standardCamera.h"
#include "globalvariables.h"

using namespace qglviewer;

StandardCamera::StandardCamera() {
    standard = true;
    orthoSize = 1.0;
}

float StandardCamera::zNear() const {
    if (standard)
        return 0.1;
    else
        return Camera::zNear();
}

float StandardCamera::zFar() const {
    if (standard)
        return mapSize;
    else
        return Camera::zFar();
}


void StandardCamera::getOrthoWidthHeight(GLdouble &halfWidth,
                                         GLdouble &halfHeight) const {
    if (standard) {
        halfHeight = orthoSize;
        halfWidth = aspectRatio() * orthoSize;
    } else
        Camera::getOrthoWidthHeight(halfWidth, halfHeight);
}
