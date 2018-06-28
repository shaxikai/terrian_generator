#include <QGLViewer/manipulatedCameraFrame.h>
#include <QGridLayout>
#include <QVBoxLayout>
#include "viewer.h"
#include "cameraViewer.h"
#include "mainWidget.h"

mainWidget::mainWidget(QWidget *parent)
    : QWidget( parent )
{
    QGridLayout* layout  = new QGridLayout(this);
    QVBoxLayout* Hlayout = new QVBoxLayout;


    StandardCamera *sc = new StandardCamera();
    Viewer* viewer = new Viewer(sc);
    viewer->setFixedSize(640, 480);
    CameraViewer* cviewer = new CameraViewer(sc);

    QObject::connect(viewer->camera()->frame(), SIGNAL(manipulated()), cviewer,
                     SLOT(updateGL()));
    QObject::connect(viewer->camera()->frame(), SIGNAL(spun()), cviewer,
                     SLOT(updateGL()));
    // Also update on camera change (type or mode)
    QObject::connect(viewer, SIGNAL(cameraChanged()), cviewer,
                     SLOT(updateGL()));

//    viewer.setWindowTitle("standardCamera");
//    cviewer.setWindowTitle("Camera viewer");

    layout->addWidget(cviewer,0,0);
    Hlayout->addWidget(viewer);
    layout->addLayout(Hlayout,0,1);
}
