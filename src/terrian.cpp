#include <iostream>

#include "terrian.h"

#define SQR(number) ((number) * (number))


Terrian::Terrian() : mapSize(SIZE), sizeHeightMap(SIZE), paintTextures(true), scaleHeightMap(0.25f), mapScale(1)
{
    heightMap.arrayHeightMap.assign(SQR(sizeHeightMap), 0);
    heightMap.sizeHeightMap = sizeHeightMap;

    textures.numTextures = texNum;
    textures.region.resize(texNum);
    textures.data.resize(texNum);
    fusionTexture = cv::Mat(256, 256, CV_8UC3);

    tileManager = TileManager((int)ceil((float)mapSize / sizeHeightMap));

}

Terrian::~Terrian()
{

}

void Terrian::normalizeTerrain(std::vector<float>& heightData) {
    float min = heightData[0];
    float max = heightData[0];

    for (int i = 1; i < SQR(mapSize); i++) {
        if (heightData[i] > max)
            max = heightData[i];

        else if (heightData[i] < min)
            min = heightData[i];
    }

    if (max < min) {
        std::cout << "Max height < min height in normalize terrian.";
        exit(0);
    } else if (max == min) {
        if (max > 255.0f) heightData.assign(SQR(mapSize), 255.0f);
        else if (max < 0) heightData.assign(SQR(mapSize), 0);
    } else {
        float height = max - min;

        for (int i = 0; i < SQR(mapSize); i++)
            heightData[i] = ((heightData[i] - min) / height) * 255.0f;
    }

}

void Terrian::smoothTerrain(std::vector<float>& heightData, int& kernelSize) {
    std::vector<float> tem(mapSize*mapSize);
    int halfKernelSizeLT = kernelSize / 2;
    int halfKernelSizeRB = halfKernelSizeLT - int(kernelSize%2==0);

    for (int j=0; j<mapSize; ++j) {
        for (int i=0; i<mapSize; ++i) {
            int _halfKernelSizeLT = 0;
            int _halfKernelSizeRB = 0;
            if (i>=j && i+j<=mapSize-1 && j<halfKernelSizeLT) {
                _halfKernelSizeLT = j;
                _halfKernelSizeRB = j;
            } else if (i>=j && i+j>mapSize-1 && i>mapSize-1-halfKernelSizeRB) {
                _halfKernelSizeLT = mapSize-1-i;
                _halfKernelSizeRB = _halfKernelSizeLT;
            } else if (i<j && i+j<=mapSize-1 && i<halfKernelSizeLT) {
                _halfKernelSizeLT = i;
                _halfKernelSizeRB = i;
            } else if (i<j && i+j>mapSize-1 && j>mapSize-1-halfKernelSizeRB) {
                _halfKernelSizeLT = mapSize-1-j;
                _halfKernelSizeRB = _halfKernelSizeLT;
            } else if (i>=halfKernelSizeLT && j>=halfKernelSizeLT &&
                       i<=mapSize-1-halfKernelSizeRB && j<=mapSize-1-halfKernelSizeRB) {
                _halfKernelSizeLT = halfKernelSizeLT;
                _halfKernelSizeRB = halfKernelSizeRB;
            } else {
                std::cout << "Some hight data is not getten when smooth terrian." << std::endl;
                exit(0);
            }

            int idx = j*mapSize+i, count = 0;
            float sum = 0;
            for (int n=j-_halfKernelSizeLT; n<=j+_halfKernelSizeRB; ++n) {
                for (int m=i-_halfKernelSizeLT; m<=i+_halfKernelSizeRB; ++m) {
                    int kernelIdx = n*mapSize+m;
                    sum += heightData[kernelIdx];
                    ++count;
                }
            }

            if (count == SQR(_halfKernelSizeLT+_halfKernelSizeRB+1)) {
                tem[idx] = sum / count;
            } else {
                std::cout << "Count do not equal current kernel size.";
                exit(0);
            }

        }
    }
    heightData = tem;
}

void Terrian::smoothTerrain2(std::vector<float>& heightData, int& kernelSize)
{
    std::vector<float> temp(mapSize*mapSize);
    int center = kernelSize / 2;
    for (int i=0; i<mapSize; ++i) {
        for (int j=0; j<mapSize; ++j) {

            float sum = 0;
            int effectSize = 0;
            int baseX = j - center, baseY = i - center;

            for (int k=0; k<kernelSize; ++k) {
                for (int l=0; l<kernelSize; ++l) {
                    int posX = baseX + l, posY = baseY + k;
                    if (posX>=0 && posX<mapSize && posY>=0 && posY<mapSize) {
                        sum += heightData[posY*mapSize+posX];
                        ++effectSize;
                    }
                }
            }
            temp[i*mapSize+j] = (float)sum / effectSize;
        }
    }
    heightData = temp;
}

void Terrian::generateRandomHeightMap(HIGHTMAPMETHOD method,const int& scale, int iterations, int min,
                                      int max, int smoothKernelSize)
{
    mapScale = scale;
    mapSize = scale * SIZE;
    heightMap.arrayHeightMap.assign(mapSize*mapSize, 0);

    tileManager = TileManager((int)ceil((float)mapSize / sizeHeightMap));

    std::vector<float> tempBuffer(mapSize*mapSize, 0);
    if (method == HEIGHTFAULT) {
        for (int it=0; it<iterations*mapScale; ++it) {
            float height = max - ((max - min) * it) / iterations;

            int randX1 = rand() % mapSize;
            int randY1 = rand() % mapSize;
            int randX2, randY2;
            do {
                randX2 = rand() % mapSize;
                randY2 = rand() % mapSize;
            } while (randX2 == randX1 && randY2 == randY1);

//                    randX1 = 0; randY1 = 0;
//                    randX2 = 127; randY2 = 127;

            int dirX1 = randX2 - randX1;
            int dirY1 = randY2 - randY1;
            int dirX2, dirY2;
            for (int y = 0; y < mapSize; y++) {
                for (int x = 0; x < mapSize; x++) {
                    dirX2 = x - randX1;
                    dirY2 = y - randY1;
                    if ((dirX2 * dirY1 - dirX1 * dirY2) < 0)
                        tempBuffer[(y * mapSize) + x] += height;
                }
            }
            //smoothTerrain(tempBuffer, smoothKernelSize);
            smoothTerrain2(tempBuffer, smoothKernelSize);
        }
    } else if (method == CIRCLEALGORITHM){
        for (int it=0; it<SQR(mapScale)*iterations; ++it) {
            int randX = rand() % mapSize,
                randY = rand() % mapSize,
                randR = (rand() % (sizeHeightMap)) + 10;
            float displacement = (rand() % sizeHeightMap)+1;
            for (int y = 0; y < mapSize; y++) {
                for (int x = 0; x < mapSize; x++) {
                    float distance = sqrt(SQR(x-randX)+SQR(y-randY)),
                          radius = 2 * distance / randR;
                    if (fabs(radius)<=1.0) {
                        tempBuffer[(y* mapSize) +x] +=
                        (displacement/2.0) + (cos(radius*3.14)*(displacement/2.0));
                    }
                }
            }
        }
    } else {
        std::cout << "Do not support extra generate hight map method." << std::endl;
        exit(0);
    }

    normalizeTerrain(tempBuffer);
    for (int i=0; i<SQR(mapSize); ++i) {
        heightMap.arrayHeightMap[i] = (uchar)tempBuffer[i];
    }
}

void Terrian::generateTextureMap(){
    int perLevel=ceil((float)TILE_SIZE/texNum);
    for (int i=0, l=perLevel; i<texNum; ++i, l+=perLevel) {
        textures.region[i] = l;
    }

    for (int j=0, imgtype = textures.data[0].type(); j<tileManager.getSizeTiles(); ++j) {
        for (int i=0; i<tileManager.getSizeTiles(); ++i) {

            int tileLTx = i * TILE_SIZE, tileLTy = j * TILE_SIZE;

            cv::Mat image = cv::Mat::zeros(TILE_SIZE, TILE_SIZE, imgtype);
            float mapRation = 1.0f * SIZE / TILE_SIZE;
            for (int r=0; r<TILE_SIZE; ++r) {
                for (int c=0; c<TILE_SIZE; ++c) {

                    int pixelR = tileLTy + r,
                        pixelC = tileLTx + c;
                    float scaledR = pixelR * mapRation;
                    float scaledC = pixelC * mapRation;
                    int _scaledR = (int)scaledR;
                    int _scaledC = (int)scaledC;

                    uchar heightLB, heightLT, heightRB, heightRT, height;
                    heightLB = getTrueHeightAtPoint(_scaledC, _scaledR);
                    if (_scaledC==scaledC && _scaledR==scaledR) {
                        height = heightLB;
                        //return heightLB;
                    } else if (_scaledC!=scaledC && _scaledR==scaledR) {
                        if (_scaledC+1>=mapSize) {
                            height = heightLB;
                            //return heightLB;
                        } else {
                            heightRB = getTrueHeightAtPoint(_scaledC+1, _scaledR);
                            height = heightLB*((float)_scaledC+1-scaledC)+heightRB*(scaledC-(float)_scaledC);
                            //return height;
                        }
                    } else if (_scaledC==scaledC && _scaledR!=scaledR) {
                        if (_scaledR+1>=mapSize) {
                            height = heightLB;
                            //return heightLB;
                        }
                        else {
                            heightLT = getTrueHeightAtPoint(_scaledC, _scaledR+1);
                            height = heightLB*((float)_scaledR+1-scaledR)+heightLT*(scaledR-(float)_scaledR);
                            //return height;
                        }
                    } else if (_scaledC!=scaledC && _scaledR!=scaledR) {
                        if (_scaledC+1>=mapSize && _scaledR+1>=mapSize) {
                            height = heightLB;
                            //return heightLB;
                        } else if (_scaledC+1<mapSize && _scaledR+1>=mapSize) {
                            heightRB = getTrueHeightAtPoint(_scaledC+1, _scaledR);
                            height = heightLB*((float)_scaledC+1-scaledC)+heightRB*(scaledC-(float)_scaledC);
                            //return height;
                        } else if (_scaledC+1>=mapSize && _scaledR+1<mapSize) {
                            heightLT = getTrueHeightAtPoint(_scaledC, _scaledR+1);
                            height = heightLB*((float)_scaledR+1-scaledR)+heightLT*(scaledR-(float)_scaledR);
                            //return height;
                        } else if (_scaledC+1<mapSize && _scaledR+1<mapSize) {
                            heightRB = getTrueHeightAtPoint(_scaledC+1, _scaledR);
                            heightLT = getTrueHeightAtPoint(_scaledC, _scaledR+1);
                            heightRT = getTrueHeightAtPoint(_scaledC+1, _scaledR+1);
                            float disLB = sqrt(SQR(scaledC-(float)_scaledC)   + SQR(scaledR-(float)_scaledR));
                            float disLT = sqrt(SQR((float)_scaledC-scaledC)   + SQR(scaledR+1-(float)_scaledR));
                            float disRB = sqrt(SQR((float)_scaledC+1-scaledC) + SQR(scaledR-(float)_scaledR));
                            float disRT = sqrt(SQR((float)_scaledC+1-scaledC) + SQR(scaledR+1-(float)_scaledR));
                            height = (disLB*heightLB+disLT*heightLT+disRB*heightRB+disRT*heightRT) /
                                    (disLB + disLT +disRB + disRT);
                            //                    std::cout << "heightRB = " << (int)heightRB << std::endl;
                            //                    std::cout << "heightLT = " << (int)heightLT << std::endl;
                            //                    std::cout << "heightRT = " << (int)heightRT << std::endl;
                            //return height;
                        } else {
                            std::cout << "Can not get height." << std::endl;
                            exit(0);
                        }
                    } else {
                        std::cout << "Can not get height." << std::endl;
                        exit(0);
                    }

                    int Blevel = height / perLevel;
                    std::array<uchar,3>  pixel;
                    if (Blevel == 0) {
                        getPixelFrameBaseTextures(textures.data[0], c, r, pixel);
                    } else if (Blevel>0 && Blevel<texNum) {
                        std::array<uchar,3> _pixel;
                        float percent = 0;
                        int Tlevel = Blevel - 1;
                        getPixelFrameBaseTextures(textures.data[Tlevel], c, r, _pixel);
                        percent = (float)(textures.region[Blevel]-height)/perLevel;
                        for (std::array<uchar,3>::iterator it = pixel.begin(), _it = _pixel.begin();
                             it!=pixel.end()&&_it!=_pixel.end(); ++it,++_it)
                            *it = *_it * percent;
                        getPixelFrameBaseTextures(textures.data[Blevel], c, r, _pixel);
                        percent = (float)(height-textures.region[Tlevel])/perLevel;
                        for (std::array<uchar,3>::iterator it = pixel.begin(), _it = _pixel.begin();
                             it!=pixel.end()&&_it!=_pixel.end(); ++it,++_it)
                            *it += *_it * percent;
                    } else if (Blevel == texNum) {
                        getPixelFrameBaseTextures(textures.data[texNum-1], c, r, pixel);
                    } else {
//                        std::cout << r << " " << c << "\t" << Blevel << std::endl;
//                        std::cout << (int)height << std::endl;
                        std::cout << "Can not get level in textures region." << std::endl;
                        exit(0);
                    }

                    std::array<uchar,3>* pdst = (std::array<uchar,3>*)image.data;
                    pdst += r * image.cols + c;
                    *pdst = pixel;
                }
            }

            tileManager.setTileTex(j, i, image);

            cv::imwrite("/home/hj/work/trrrain_generator/terrian_generator/Data/fusionTexture"+std::to_string(j)+"_"+std::to_string(i)+".jpg", tileManager.getTileTex(j,i));
            //myTexture.save("/home/hj/work/trrrain_generator/terrain/texture.bmp", "BMP");

        }
    }
}

void Terrian::reloadTextureMap() {
    for (int j=0; j<tileManager.getSizeTiles(); ++j) {
        for (int i=0; i<tileManager.getSizeTiles(); ++i) {
            int type, _type;
            if (tileManager.getTileTex(j,i).type()==CV_8UC3) {
                type  = GL_RGB;
                _type = GL_BGR_EXT;
            } else if (tileManager.getTileTex(j,i).type()==CV_8UC4) {
                type  = GL_RGBA;
                _type = GL_BGRA_EXT;
            }
            else {
                std::cout << "Do not support extra formats when generate texture map." << std::endl;
                exit(0);
            }

            //glGenTextures(1, &tileManager.getTileid(j,i));
            glBindTexture(GL_TEXTURE_2D, tileManager.getTileID(j,i));

            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, type, tileManager.getTileTex(j,i).cols,
                         tileManager.getTileTex(j,i).rows, 0, _type, GL_UNSIGNED_BYTE,
                         tileManager.getTileTex(j,i).data);

        }
    }

}

void Terrian::getPixelFrameBaseTextures(cv::Mat& image, int& col, int& row, std::array<uchar,3>& pixel) {
    std::array<uchar,3>* psrc = (std::array<uchar,3>*) image.data;
    int idx = row * image.cols + col;
    if (idx>image.cols*image.rows) {
        std::cout << "Out of image when get pixel from base textures. " << std::endl;
        exit(0);
    }
    psrc += idx;
    pixel = * psrc;
}

















