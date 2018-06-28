#include <qapplication.h>
#include <QGLViewer/manipulatedCameraFrame.h>
#include "mainWidget.h"
#include "viewer.h"
#include "cameraViewer.h"

int main(int argc, char **argv) {
    QApplication application(argc, argv);

    mainWidget w;
    w.setWindowTitle("Window");
    w.setMinimumSize(1440,960);
    w.show();
    return  application.exec();

}
