#ifndef VIEWER_H
#define VIEWER_H

#include <QGLViewer/qglviewer.h>
#include <memory>
#include <standardCamera.h>

class Viewer : public QGLViewer
{
    Q_OBJECT

public:
    Viewer(StandardCamera *camera);

public:
Q_SIGNALS:
    void cameraChanged();


protected:
    virtual void draw();
    virtual void init();

    virtual void keyPressEvent(QKeyEvent *e);

private:
  void showMessage();
};

#endif // VIEWER_H
