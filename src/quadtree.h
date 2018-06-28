#ifndef QUADTREE_H
#define QUADTREE_H
#include <terrian.h>

class QuadTree : public Terrian
{
public:
    QuadTree();
    ~QuadTree(){}

    inline void setDetailLevel(float detail) { detailLevel = detail; }
    inline void setMinResolution(float res) { minResolution = res; }

    void init();
    void update(float x, float y, float z);
    void render();


private:
    void propagateRoughness();


    void refineNode(float x, float y, int edgeLength);
    void renderNode(float x, float y, int edgeLength, bool detail);
    void renderVertex(float x, float z, float u, float v);


    inline unsigned char getQuadMatrixData(int X, int Y) {
      if ((X > mapSize) || (X < 0) || (Y > mapSize) || (Y < 0)) {
        printf("Matrix limits exceeded: %d,%d\n", X, Y);
        return 255;
      }
      return quadMatrix[(Y * mapSize) + X];
    }

private:
    std::vector<uchar> quadMatrix;

    float pX, pY, pZ;
    float detailLevel;
    float minResolution;
};

#endif // QUADTREE_H
