#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include <QGLViewer/vec.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define SKY_SIDES 6

#define SKY_FRONT 0
#define SKY_BACK 1
#define SKY_RIGHT 2
#define SKY_LEFT 3
#define SKY_TOP 4
#define SKY_BOTTOM 5

class SKY {
private:
  cv::Mat textures[SKY_SIDES];
  unsigned int texIDs[SKY_SIDES];
  qglviewer::Vec vecMin, vecMax, vecCenter;

  bool iwantsky;
  bool paintTextures;
  bool drawMesh;

public:
  SKY() { iwantsky = false; }

  bool loadTexture(int side, const std::string filename);
  void set(float X, float Y, float Z, float size) {
    vecCenter.setValue(X, Y, Z);
    vecMin.setValue(X-0.5f*size, Y-0.5f*size, Z-0.5f*size);
    vecMax.setValue(X+0.5f*size, Y+0.5f*size, Z+0.5f*size);
  }
  void render();

  void switchSky() { iwantsky = !iwantsky; }
  bool wantSky() { return iwantsky; }

  inline void doTexture(bool doIt) { paintTextures = doIt; }
  inline bool isTexture() { return paintTextures; }
  inline void doDrawMesh(bool doIt) { drawMesh = doIt; }
  inline bool isDrawMesh() { return drawMesh; }
};

#endif
