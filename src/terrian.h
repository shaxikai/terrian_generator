#ifndef TERRAIN_H
#define TERRAIN_H
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QGLViewer/qglviewer.h>

#define SIZE 128
#define TILE_SIZE 256
#define texNum 4

enum HIGHTMAPMETHOD {
    HEIGHTFAULT = 0,
    CIRCLEALGORITHM
};

struct HEIGHTMAP {
  std::vector<uchar> arrayHeightMap;
  int sizeHeightMap;
};

enum TEXTURETYPE {
  LOWEST = 0, // 沙
  MEDIUM,     // 草
  HIGH,       // 石
  HIGHEST     // 山顶
};

struct TEXTURE {
  std::vector<int> region;
  std::vector<cv::Mat> data;
  int numTextures;
};

struct Tile {
    unsigned int tileID;
    cv::Mat image;
};

class TileManager {
private:
    std::vector<Tile> tiles;
    int sizeTiles;

public:
    TileManager(){}
    TileManager(int _sizeTiles){
        sizeTiles = _sizeTiles;
        tiles.resize(sizeTiles*sizeTiles);
    }
    ~TileManager(){}

    void setSizeTiles(int& _sizeTiles) {sizeTiles = _sizeTiles;}
    int getSizeTiles() {return sizeTiles;}

    void setTileTex(int& i,int &j, cv::Mat& _image) {
        tiles[i*sizeTiles+j].image = _image;
    }
    cv::Mat& getTileTex(int& i, int&j) {
        return tiles[i*sizeTiles+j].image;
    }

    unsigned int& getTileID(int& i, int&j) {
        return tiles[i*sizeTiles+j].tileID;
    }

};

class Terrian
{
protected:
    HEIGHTMAP heightMap;
    float scaleHeightMap;

    bool paintTextures;
    bool drawMesh;

    TEXTURE textures;
    TileManager tileManager;
    cv::Mat fusionTexture;
    cv::Mat detailTexture;

    void normalizeTerrain(std::vector<float>& heightData);
    void smoothTerrain(std::vector<float>& heightData, int& kernelSize);
    void smoothTerrain2(std::vector<float>& heightData, int& kernelSize);

public:
    int mapSize;
    int sizeHeightMap;
    int mapScale;
    unsigned int fusionTextureID;
    unsigned int detailTextureID;


    inline void doTexture(bool doIt) { paintTextures = doIt; }
    inline bool isTexture() { return paintTextures; }
    inline void doDrawMesh(bool doIt) { drawMesh = doIt; }
    inline bool isDrawMesh() { return drawMesh; }

    inline void setHeightScale(float scale) { scaleHeightMap = scale; }

    void generateRandomHeightMap(HIGHTMAPMETHOD mothod,const int& scale, int iterations, int min, int max, int smoothKernelSize);
    void generateTextureMap();
    void reloadTextureMap();


    inline bool loadTile(TEXTURETYPE type, const std::string filename) {
        textures.data[type] = cv::imread(filename);
        return !textures.data[type].empty();
    }
    inline uchar getTrueHeightAtPoint(int X, int Y) {
        return (heightMap.arrayHeightMap[(Y * mapSize) + X]);
    }
    inline float getScaledHeightAtPoint(int X, int Y) {
        if (X >= mapSize) X = mapSize - 1;
        if (Y >= mapSize) Y = mapSize - 1;
        return ((float)(heightMap.arrayHeightMap[(Y * mapSize) + X]) * scaleHeightMap);
    }

private:
    void getPixelFrameBaseTextures(cv::Mat& image, int& col, int& row, std::array<uchar,3>& pixel);

public:
    Terrian();
    ~Terrian();

private:

};



#endif // TERRAIN_H
