#include <QGLViewer/camera.h>

class StandardCamera : public qglviewer::Camera {
public:
    StandardCamera();

    virtual float zNear() const;
    virtual float zFar() const;

    void toggleMode() { standard = !standard; }
    bool isStandard() { return standard; }

    virtual void getOrthoWidthHeight(GLdouble &halfWidth,
                                     GLdouble &halfHeight) const;

private:
    bool standard;
    float orthoSize;
};
