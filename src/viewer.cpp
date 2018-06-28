#include <QKeyEvent>

#include "viewer.h"
#include "quadtree.h"
#include "sky.h"
#include "globalvariables.h"

SKY sky;
QuadTree quadtree;
int mapSize = 1;
float heightScale = 0.25f;


void Viewer::init() {
    restoreStateFromFile();
    setFixedSize(640, 480);
    //DrawInit();

    setSceneCenter(qglviewer::Vec(0.0f, 0.0f, 0.0f));
    setSceneRadius(mapSize);
    //camera()->fitSphere(qglviewer::Vec(0,0,0), 3);
    showEntireScene();

    setKeyDescription(Qt::Key_C, "Create a new random terrain");
    setKeyDescription(Qt::Key_H, "Load height field from file");
    setKeyDescription(Qt::Key_M, "Toggle wireframe mesh display");
    setKeyDescription(Qt::Key_O, "Toggles shadows");
    setKeyDescription(Qt::Key_W, "Toggles water");
    setKeyDescription(Qt::Key_S, "Toggles sky");
    setKeyDescription(Qt::Key_T, "Toggles trees");
    setKeyDescription(Qt::Key_L, "Change light direction");
    setKeyDescription(Qt::Key_X, "Toggles textures");
    setKeyDescription(Qt::Key_P, "Toggles camera type (perspective or orthographic)");
    setKeyDescription(Qt::Key_Q, "Toggles camera mode (standard or QGLViewer)");

    setMouseBindingDescription(
                Qt::ShiftModifier, Qt::MidButton,
                "Change frustum size (for standard camera in orthographic mode)");


    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glClearDepth(1.0);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    //    glDisable(GL_BLEND);

    quadtree.generateRandomHeightMap(HEIGHTFAULT, mapSize, 32, 25, 150, 4);
    quadtree.setHeightScale(heightScale);
    if (!quadtree.loadTile(LOWEST,  "/home/hj/work/trrrain_generator/terrain/Data/lowest.jpg") ||
            !quadtree.loadTile(MEDIUM,  "/home/hj/work/trrrain_generator/terrain/Data/medium.jpg") ||
            !quadtree.loadTile(HIGH,    "/home/hj/work/trrrain_generator/terrain/Data/high.jpg") ||
            !quadtree.loadTile(HIGHEST, "/home/hj/work/trrrain_generator/terrain/Data/highest.jpg")) {
        std::cout << "Base Texture load failed." << std::endl;
        exit(0);
    }

    quadtree.generateTextureMap();

    if (!sky.loadTexture(SKY_FRONT, "/home/hj/work/trrrain_generator/terrain/Data/skyfront.jpg") ||
            !sky.loadTexture(SKY_BACK, "/home/hj/work/trrrain_generator/terrain/Data/skyback.jpg") ||
            !sky.loadTexture(SKY_RIGHT, "/home/hj/work/trrrain_generator/terrain/Data/skyright.jpg") ||
            !sky.loadTexture(SKY_LEFT, "/home/hj/work/trrrain_generator/terrain/Data/skyleft.jpg") ||
            !sky.loadTexture(SKY_TOP, "/home/hj/work/trrrain_generator/terrain/Data/skytop.jpg") ||
            !sky.loadTexture(SKY_BOTTOM, "/home/hj/work/trrrain_generator/terrain/Data/skybottom.jpg")) {
        std::cout << "Can not load sky texture." << std::endl;
        exit(0);
    }


    quadtree.doTexture(true);
    quadtree.setDetailLevel(50.0f / (SIZE / 3));
    quadtree.setMinResolution(10.0f / (SIZE / 3));
    sky.doTexture(true);
    quadtree.doDrawMesh(false);
    sky.doDrawMesh(false);


}

void Viewer::draw(){
    quadtree.init();
    qglviewer::Vec v = camera()->position();
    quadtree.update(v.x, v.y, v.z);

    if(quadtree.isDrawMesh())
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    quadtree.render();
    if (sky.wantSky()) {
        sky.set(0, 0, 0.25f, (float)mapSize*1.5f);
        sky.render();
    }
}


void Viewer::keyPressEvent(QKeyEvent *e) {
    if (e->modifiers() == Qt::NoModifier)
        switch (e->key()) {
        case Qt::Key_C:
        {
            GLint viewPort[4] = {0};
            cv::Mat image;
            qglviewer::Vec v;
            glGetIntegerv(GL_VIEWPORT, viewPort);
            image = cv::Mat(viewPort[3], viewPort[2], CV_8UC3);
            glReadPixels(viewPort[1], viewPort[0], viewPort[2], viewPort[3], GL_BGR, GL_UNSIGNED_BYTE, image.data);
            cv::flip(image, image, 0);
            v = camera()->position();
            cv::imwrite("/home/hj/work/trrrain_generator/terrian_generator/frames/frame"+std::to_string(v.x)+"_"+std::to_string(v.y)+"_"+std::to_string(v.z)+".jpg",image);

            image = cv::Mat(viewPort[3], viewPort[2], CV_32FC1);
            glReadPixels( viewPort[0], viewPort[1], viewPort[2], viewPort[3], GL_DEPTH_COMPONENT,  GL_FLOAT, image.data);
            image.convertTo(image,CV_8UC1,255.);
            cv::flip(image, image, 0);
            cv::imwrite("/home/hj/work/trrrain_generator/terrian_generator/frames/depth"+std::to_string(v.x)+"_"+std::to_string(v.y)+"_"+std::to_string(v.z)+".jpg",image);
        }
            break;
        case Qt::Key_D:
            quadtree.generateRandomHeightMap(CIRCLEALGORITHM, mapSize, 32, 0, 255, 3);
            quadtree.setHeightScale(heightScale);
            quadtree.generateTextureMap();
            //quadtree.CalculateLighting();
            //myTree.initTrees(quadtree, numTrees, waterLevel * mapSize);
            update();
            cameraChanged();
            break;
        case Qt::Key_F:
            quadtree.generateRandomHeightMap(HEIGHTFAULT, mapSize, 32, 0, 255, 3);
            quadtree.setHeightScale(heightScale);
            quadtree.generateTextureMap();
            //quadtree.CalculateLighting();
            //myTree.initTrees(quadtree, numTrees, waterLevel * mapSize);
            update();
            cameraChanged();
            break;
        case Qt::Key_M:
            if (quadtree.isDrawMesh() || sky.isDrawMesh()) {
                quadtree.doDrawMesh(false);
                sky.doDrawMesh(false);
            } else {
                quadtree.doDrawMesh(true);
                sky.doDrawMesh(true);
            }
            update();
            cameraChanged();
            break;
            //        case Qt::Key_O:
            //            if (quadtree.isLighted())
            //                quadtree.DoLighting(false);
            //            else
            //                quadtree.DoLighting(true);
            //            update();
            //            break;
            //        case Qt::Key_W:
            //            myWater.switchWater();
            //            update();
            //            break;
        case Qt::Key_S: // switch sky
            sky.switchSky();
            update();
            cameraChanged();
            break;
            //        case Qt::Key_T: // switch trees
            //            myTree.switchTree();
            //            update();
            //            break;
            //        case Qt::Key_L: // switch direction de lumiere (45 degrees steps)
            //            quadtree.StepLightingDirection();
            //            quadtree.CalculateLighting();
            //            update();
            //            break;
        case Qt::Key_X:
            if (quadtree.isTexture() || sky.isTexture()) {
                quadtree.doTexture(false);
                sky.doTexture(false);
                //quadtree.generateTextureMap(2 * mapSize);
            } else {
                quadtree.doTexture(true);
                sky.doTexture(true);
                //quadtree.generateTextureMap(2 * mapSize);
            }
            update();
            cameraChanged();
            break;
        case Qt::Key_Q :
            ((StandardCamera *)camera())->toggleMode();
            showMessage();
            update();
            break;
        case Qt::Key_P :
            if (camera()->type() == qglviewer::Camera::ORTHOGRAPHIC)
                camera()->setType(qglviewer::Camera::PERSPECTIVE);
            else
                camera()->setType(qglviewer::Camera::ORTHOGRAPHIC);
            showMessage();
            update();
            break;
        default:
            QGLViewer::keyPressEvent(e); // handler de la classe superieure
        }
    else
        QGLViewer::keyPressEvent(e);
}


void Viewer::showMessage() {
    QString std = ((StandardCamera *)camera())->isStandard() ? "Standard camera"
                                                             : "QGLViewer camera";
    QString type =
            camera()->type() == qglviewer::Camera::PERSPECTIVE ? "Perspective" : "Orthographic";
    displayMessage(std + " - " + type);
    Q_EMIT cameraChanged();
}

Viewer::Viewer(StandardCamera *nfc)
{
    qglviewer::Camera *c = camera();
    setCamera(nfc);
    delete c;
}
