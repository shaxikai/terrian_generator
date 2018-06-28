#include "quadtree.h"

QuadTree::QuadTree()
{
    quadMatrix.assign(mapSize*mapSize, 1);
    detailLevel = 2.5f;
    minResolution = 1.2f;
}

void QuadTree::init() {
    quadMatrix.assign(mapSize*mapSize, 0);
    //propagateRoughness();
}

void QuadTree::propagateRoughness() {
    int edgeLength = 2;
    while (edgeLength <= mapSize) {
        int edgeOffset = (edgeLength) >> 1;
        int childOffset = (edgeLength) >> 2;
        for (int z = edgeOffset; z < mapSize - edgeOffset; z += (edgeLength)) {
            for (int x = edgeOffset; x < mapSize - edgeOffset; x += (edgeLength)) {
                int localD2 = (int)ceil(
                            (int)(abs(((getTrueHeightAtPoint(x - edgeOffset, z + edgeOffset) +
                                        getTrueHeightAtPoint(x + edgeOffset, z + edgeOffset)) >>
                                       1) -
                                      getTrueHeightAtPoint(x, z + edgeOffset))));
                int dH = (int)ceil(
                            abs(((getTrueHeightAtPoint(x + edgeOffset, z + edgeOffset) +
                                  getTrueHeightAtPoint(x + edgeOffset, z - edgeOffset)) >>
                                 1) -
                                getTrueHeightAtPoint(x + edgeOffset, z)));
                localD2 = MAX(localD2, dH);


                dH = (int)ceil(
                            abs(((getTrueHeightAtPoint(x - edgeOffset, z - edgeOffset) +
                                  getTrueHeightAtPoint(x + edgeOffset, z - edgeOffset)) >>
                                 1) -
                                getTrueHeightAtPoint(x, z - edgeOffset)));
                localD2 = MAX(localD2, dH);

                dH = (int)ceil(
                            abs(((getTrueHeightAtPoint(x - edgeOffset, z + edgeOffset) +
                                  getTrueHeightAtPoint(x - edgeOffset, z - edgeOffset)) >>
                                 1) -
                                getTrueHeightAtPoint(x - edgeOffset, z)));
                localD2 = MAX(localD2, dH);

                dH = (int)ceil(
                            abs(((getTrueHeightAtPoint(x - edgeOffset, z - edgeOffset) +
                                  getTrueHeightAtPoint(x + edgeOffset, z + edgeOffset)) >>
                                 1) -
                                getTrueHeightAtPoint(x, z)));
                localD2 = MAX(localD2, dH);

                dH = (int)ceil(
                            abs(((getTrueHeightAtPoint(x + edgeOffset, z - edgeOffset) +
                                  getTrueHeightAtPoint(x - edgeOffset, z + edgeOffset)) >>
                                 1) -
                                getTrueHeightAtPoint(x, z)));
                localD2 = MAX(localD2, dH);
                localD2 = (int)ceil((localD2 * 3.0f) / edgeLength);

                int d2, localH;
                if (edgeLength == 2) {
                    d2 = localD2;
                    localH = getTrueHeightAtPoint(x + edgeOffset, z + edgeOffset);
                    localH = MAX(localH, getTrueHeightAtPoint(x + edgeOffset, z));
                    localH = MAX(localH, getTrueHeightAtPoint(x + edgeOffset, z - edgeOffset));
                    localH = MAX(localH, getTrueHeightAtPoint(x, z - edgeOffset));
                    localH = MAX(localH, getTrueHeightAtPoint(x - edgeOffset, z - edgeOffset));
                    localH = MAX(localH, getTrueHeightAtPoint(x - edgeOffset, z));
                    localH = MAX(localH, getTrueHeightAtPoint(x - edgeOffset, z + edgeOffset));
                    localH = MAX(localH, getTrueHeightAtPoint(x, z + edgeOffset));
                    localH = MAX(localH, getTrueHeightAtPoint(x, z));
                    quadMatrix[z*mapSize+x+1] = localH;
                }

                else {
                    float upperBound = 1.0f * minResolution / (2.0f * (minResolution - 1.0f));
                    d2 = (int)ceil( MAX(upperBound * (float)getQuadMatrixData(x, z), (float)localD2));
                    d2 = (int)ceil( MAX(upperBound * (float)getQuadMatrixData(x - edgeOffset, z), (float)d2));
                    d2 = (int)ceil( MAX(upperBound * (float)getQuadMatrixData(x + edgeOffset, z),  (float)d2));
                    d2 = (int)ceil( MAX(upperBound * (float)getQuadMatrixData(x, z + edgeOffset), (float)d2));
                    d2 = (int)ceil( MAX(upperBound * (float)getQuadMatrixData(x, z - edgeOffset), (float)d2));

                    localH = getTrueHeightAtPoint(x + childOffset, z + childOffset);
                    localH = MAX(localH, getTrueHeightAtPoint(x + childOffset, z - childOffset));
                    localH = MAX(localH, getTrueHeightAtPoint(x - childOffset, z - childOffset));
                    localH = MAX(localH, getTrueHeightAtPoint(x - childOffset, z + childOffset));

                    quadMatrix[z*mapSize+x+1] = localH;
                }

                quadMatrix[z*mapSize+x] = d2;
                quadMatrix[x*mapSize+x-1] = d2;

                quadMatrix[x - edgeOffset + mapSize * (z - edgeOffset)] =
                        MAX(getQuadMatrixData(x - edgeOffset, z - edgeOffset), d2);
                quadMatrix[x - edgeOffset + mapSize * (z + edgeOffset)] =
                        MAX(getQuadMatrixData(x - edgeOffset, z + edgeOffset), d2);
                quadMatrix[x + edgeOffset + mapSize * (z + edgeOffset)] =
                        MAX(getQuadMatrixData(x + edgeOffset, z + edgeOffset), d2);
                quadMatrix[x + edgeOffset + mapSize * (z - edgeOffset)] =
                        MAX(getQuadMatrixData(x + edgeOffset, z - edgeOffset), d2);
            }
        }
        edgeLength = (edgeLength << 1);
    }
}

void QuadTree::update(float x, float y, float z) {
    pX = x, pY = y, pZ= z;
    float center = mapSize / 2.0f;
    refineNode(center, center, mapSize);

    center = sizeHeightMap / 2.0f;
    for (int j=0; j<tileManager.getSizeTiles(); ++j) {
        for (int i=0; i<tileManager.getSizeTiles(); ++i) {
            quadMatrix[((j*sizeHeightMap)+center)*mapSize+i*sizeHeightMap+center] = 1;
        }
    }
}

void QuadTree::refineNode(float x, float y, int edgeLength) {
    float viewDistance = (float)(fabs(pX - (x / mapSize-0.5f)*mapScale) +
                                 fabs(pY - (y / mapSize-0.5f)*mapScale) +
                                 fabs(pZ - getScaledHeightAtPoint((int)x, (int)y) / mapSize * mapScale));

    float f = viewDistance / ((float)edgeLength * minResolution *
                              MAX((float)getQuadMatrixData((int)x - 1, (int)y) / 3 * detailLevel, 1.0f));

    int blend;
    if (f < 1.0f) blend = 255;
    else blend = 0;
    quadMatrix[y*mapSize+x] = blend;

    if (blend != 0) {
        if (edgeLength <= 2)
            return;
        else {
            float childOffset = (float)((edgeLength) >> 2);
            int childEdgeLength = (edgeLength) >> 1;

            refineNode(x - childOffset, y - childOffset, childEdgeLength);
            refineNode(x + childOffset, y - childOffset, childEdgeLength);
            refineNode(x - childOffset, y + childOffset, childEdgeLength);
            refineNode(x + childOffset, y + childOffset, childEdgeLength);
            return;
        }
    }
}

void QuadTree::render() {

    //quadMatrix.assign(mapSize*mapSize, 1);
    float center = (sizeHeightMap) / 2.0f;

    //glDisable(GL_CULL_FACE);

    if (paintTextures) {
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

                glEnable(GL_TEXTURE_2D);
                glGenTextures(1, &tileManager.getTileID(j,i));
                glBindTexture(GL_TEXTURE_2D, tileManager.getTileID(j,i));

                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                glTexImage2D(GL_TEXTURE_2D, 0, type, tileManager.getTileTex(j,i).cols,
                             tileManager.getTileTex(j,i).rows, 0, _type, GL_UNSIGNED_BYTE,
                             tileManager.getTileTex(j,i).data);
                glBindTexture(GL_TEXTURE_2D, tileManager.getTileID(j,i));
                renderNode(i*sizeHeightMap+center, j*sizeHeightMap+center, sizeHeightMap, false);;
                glDisable(GL_TEXTURE_2D);
            }
        }

    } else {
        glDisable(GL_TEXTURE_2D);
        for (int j=0; j<tileManager.getSizeTiles(); ++j) {
            for (int i=0; i<tileManager.getSizeTiles(); ++i) {
                renderNode(i*sizeHeightMap+center, j*sizeHeightMap+center, sizeHeightMap, false);
            }
        }
    }

}

void QuadTree::renderNode(float x, float y, int edgeLength, bool detail){

    int iX = (int)x;
    int iY = (int)y;

    int edgeOffset = (edgeLength) / 2.0f;
    int adjOffset = edgeLength;

    float texLeft = ((float)fabs(x - edgeOffset) / sizeHeightMap);
    float texBottom = ((float)fabs(y - edgeOffset) / sizeHeightMap);
    float texRight = ((float)fabs(x + edgeOffset) / sizeHeightMap);
    float texTop = ((float)fabs(y + edgeOffset) / sizeHeightMap);

    float midX = ((texLeft + texRight) / 2.0f);
    float midY = ((texBottom + texTop) / 2.0f);

    if (getQuadMatrixData(iX, iY) > 0) {
        if (edgeLength<=2) {
            glBegin(GL_TRIANGLE_FAN);

            renderVertex(x, y, midX, midY);
            renderVertex(x - edgeOffset, y - edgeOffset, texLeft, texBottom);

            if (((iY - adjOffset) <= 0) || getQuadMatrixData(iX, iY - adjOffset) != 0)
                renderVertex(x, y - edgeOffset, midX, texBottom);

            renderVertex(x + edgeOffset, y - edgeOffset, texRight, texBottom);

            if (((iX + adjOffset) >= mapSize) || getQuadMatrixData(iX + adjOffset, iY) != 0)
                renderVertex(x + edgeOffset, y, texRight, midY);

            renderVertex(x + edgeOffset, y + edgeOffset, texRight, texTop);

            if (((iY + adjOffset) >= mapSize) || getQuadMatrixData(iX, iY + adjOffset) != 0)
                renderVertex(x, y + edgeOffset, midX, texTop);

            renderVertex(x - edgeOffset, y + edgeOffset, texLeft, texTop);

            if (((iX - adjOffset) <= 0) || getQuadMatrixData(iX - adjOffset, iY) != 0)
                renderVertex(x - edgeOffset, y, texLeft, midY);

            renderVertex(x - edgeOffset, y - edgeOffset, texLeft, texBottom);
            glEnd();
            return;
        } else {
            float ichildOffset = (edgeLength) / 4;
            float childOffset = (float)ichildOffset;
            float childEdgeLength = (edgeLength) / 2;

            int suiteStart[]  = {-1, 3, 0, 3, 1, -1, 0, 3, 2, 2, -1, 2, 1, 1, 0, -1};
            int suiteLength[] = {-1, 3, 3, 2, 3, -1, 2, 1, 3, 2, -1, 1, 2, 1, 1, -1};
            int code = (getQuadMatrixData(iX + ichildOffset, iY + ichildOffset) != 0) * 8;
            code |= (getQuadMatrixData(iX - ichildOffset, iY + ichildOffset) != 0) * 4;
            code |= (getQuadMatrixData(iX - ichildOffset, iY - ichildOffset) != 0) * 2;
            code |= (getQuadMatrixData(iX + ichildOffset, iY - ichildOffset) != 0);
            int start=suiteStart[code];
            switch (code) {
            case 15 :
                renderNode(x - childOffset, y - childOffset, childEdgeLength, false);
                renderNode(x + childOffset, y - childOffset, childEdgeLength, false);
                renderNode(x - childOffset, y + childOffset, childEdgeLength, false);
                renderNode(x + childOffset, y + childOffset, childEdgeLength, false);
                return;
                break;

            case 5 :
                glBegin(GL_TRIANGLE_FAN);
                renderVertex(x, y, midX, midY);
                renderVertex(x + edgeOffset, y, texRight, midY);
                renderVertex(x + edgeOffset, y + edgeOffset, texRight, texTop);
                renderVertex(x, y + edgeOffset, midX, texTop);
                glEnd();

                glBegin(GL_TRIANGLE_FAN);
                renderVertex(x, y, midX, midY);
                renderVertex(x - edgeOffset, y, texLeft, midY);
                renderVertex(x - edgeOffset, y - edgeOffset, texLeft, texBottom);
                renderVertex(x, y - edgeOffset, midX, texBottom);
                glEnd();

                renderNode(x - childOffset, y + childOffset, childEdgeLength, false);
                renderNode(x + childOffset, y - childOffset, childEdgeLength, false);
                return;
                break;

            case 10 :
                glBegin(GL_TRIANGLE_FAN);
                renderVertex(x, y, midX, midY);
                renderVertex(x, y + edgeOffset, midX, texTop);
                renderVertex(x - edgeOffset, y + edgeOffset, texLeft, texTop);
                renderVertex(x - edgeOffset, y, texLeft, midY);
                glEnd();

                glBegin(GL_TRIANGLE_FAN);
                renderVertex(x, y, midX, midY);
                renderVertex(x, y - edgeOffset, midX, texBottom);
                renderVertex(x + edgeOffset, y - edgeOffset, texRight, texBottom);
                renderVertex(x + edgeOffset, y, texRight, midY);
                glEnd();

                renderNode(x + childOffset, y + childOffset, childEdgeLength, false);
                renderNode(x - childOffset, y - childOffset, childEdgeLength, false);
                return;
                break;

            case 0 :
                glBegin(GL_TRIANGLE_FAN);
                renderVertex(x, y, midX, midY);
                renderVertex(x - edgeOffset, y - edgeOffset, texLeft, texBottom);

                if (((iY - adjOffset) <= 0) || getQuadMatrixData(iX, iY - adjOffset) != 0)
                    renderVertex(x, y - edgeOffset, midX, texBottom);

                renderVertex(x + edgeOffset, y - edgeOffset, texRight, texBottom);

                if (((iX + adjOffset) >= mapSize) || getQuadMatrixData(iX + adjOffset, iY) != 0)
                    renderVertex(x + edgeOffset, y, texRight, midY);

                renderVertex(x + edgeOffset, y + edgeOffset, texRight, texTop);

                if (((iY + adjOffset) >= mapSize) || getQuadMatrixData(iX, iY + adjOffset) != 0)
                    renderVertex(x, y + edgeOffset, midX, texTop);

                renderVertex(x - edgeOffset, y + edgeOffset, texLeft, texTop);

                if (((iX - adjOffset) <= 0) ||getQuadMatrixData(iX - adjOffset, iY) != 0)
                    renderVertex(x - edgeOffset, y, texLeft, midY);

                renderVertex(x - edgeOffset, y - edgeOffset, texLeft, texBottom);
                glEnd();
                return;
                break;

            case 1: case 2: case 3 : case 4 : case 6 : case 7 :
            case 8: case 9: case 11: case 12: case 13: case 14:
                glBegin(GL_TRIANGLE_FAN);
                renderVertex(x, y, midX, midY);
                for (int suitePosition=suiteLength[code]; suitePosition>0; suitePosition--) {
                    switch (start) {
                    case 0 :
                        if (((iY - adjOffset) <= 0) ||
                                getQuadMatrixData(iX, iY - adjOffset) != 0 ||
                                suitePosition == suiteLength[code])
                            renderVertex(x, y - edgeOffset, midX, texBottom);

                        renderVertex(x + edgeOffset, y - edgeOffset, texRight, texBottom);

                        if (suitePosition == 1)
                            renderVertex(x + edgeOffset, y, texRight, midY );

                        break;

                    case 1 :
                        if (((x - adjOffset) <= 0) ||
                                getQuadMatrixData(iX - adjOffset, iY) != 0 ||
                                suitePosition == suiteLength[code])
                            renderVertex(x - edgeOffset, y, texLeft, midY );

                        renderVertex(x - edgeOffset, y - edgeOffset, texLeft, texBottom);

                        if (suitePosition == 1)
                            renderVertex(x, y - edgeOffset, midX, texBottom );
                        break;

                    case 2 :
                        if (((iY + adjOffset) >= sizeHeightMap) ||
                                getQuadMatrixData(iX, iY + adjOffset) != 0 ||
                                suitePosition == suiteLength[code])
                            renderVertex(x, y + edgeOffset, midX, texTop );

                        renderVertex(x - edgeOffset, y + edgeOffset, texLeft, texTop);

                        if (suitePosition == 1)
                            renderVertex(x - edgeOffset, y, texLeft, midY );
                        break;

                    case 3 :
                        if (((iX + adjOffset) >= sizeHeightMap) ||
                                getQuadMatrixData(iX + adjOffset, iY) != 0 ||
                                suitePosition == suiteLength[code])
                            renderVertex(x + edgeOffset, y, texRight, midY );

                        renderVertex(x + edgeOffset, y + edgeOffset, texRight, texTop);

                        if (suitePosition == 1)
                            renderVertex(x, y + edgeOffset, midX, texTop );
                        break;
                    }
                    start--;
                    start &= 3;
                }
                glEnd();

                for (int suitePosition=(4-suiteLength[code]); suitePosition>0; suitePosition--) {
                    switch (start) {
                    case 0 :
                        renderNode(x + childOffset, y - childOffset, childEdgeLength, false);
                        break;

                    case 1 :
                        renderNode(x - childOffset, y - childOffset, childEdgeLength, false);
                        break;

                    case 2 :
                        renderNode(x - childOffset, y + childOffset, childEdgeLength, false);
                        break;

                    case 3 :
                        renderNode(x + childOffset, y + childOffset, childEdgeLength, false);
                        break;
                    }
                    start--;
                    start &= 3;
                }
                return;
                break;

            default :
                std::cout << "Unexpect code vale"<< code << ", the center is ["<< iY <<"," << iY << "]." << std::endl;
                exit(0);
                break;
            }
        }
    }
}

void QuadTree::renderVertex(float x, float y, float u, float v) {
    glColor3ub(255, 255, 255);
//    glMultiTexCoord2f(GL_TEXTURE0, u, v);
    glTexCoord2f(u, v);
    glVertex3f((x / mapSize-0.5f)*mapScale,(y / mapSize-0.5f)*mapScale,
               getScaledHeightAtPoint((int)x, (int)y) / mapSize * mapScale);
}









