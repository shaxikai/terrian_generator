#include <QGLViewer/qglviewer.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class CameraViewer : public QGLViewer {
public:
    CameraViewer(qglviewer::Camera *camera);

protected:
    virtual void draw();
    virtual void init();


private:
    qglviewer::Camera *c;
};
