// TP OpenGL: Joerg Liebelt, Serigne Sow
#include "sky.h"
#include <QGLWidget>

bool SKY::loadTexture(int side, const std::string filename) {
    textures[side] = cv::imread(filename);
    if (textures[side].empty()) {
        qWarning("Sky texture loading failed for side %d", side);
        return false;
    }
    return true;
}

void SKY::render() {
    glDisable(GL_LIGHTING);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    if (paintTextures) {
        for (int side=0; side<SKY_SIDES; ++side) {
            glEnable(GL_TEXTURE_2D);
            int type, _type;
            if (textures[side].type()==CV_8UC3) {
                type  = GL_RGB;
                _type = GL_BGR_EXT;
            } else if (textures[side].type()==CV_8UC4) {
                type  = GL_RGBA;
                _type = GL_BGRA_EXT;
            }
            else {
                std::cout << "Do not support extra sky texture formats." << std::endl;
                exit(0);
            }

            glGenTextures(1, &(texIDs[side]));
            glBindTexture(GL_TEXTURE_2D, (texIDs[side]));
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, type, textures[side].cols,
                         textures[side].rows, 0, _type, GL_UNSIGNED_BYTE,
                         textures[side].data);
        }
    }
    else glDisable(GL_TEXTURE_2D);

    glPushMatrix();
    //std::cout << texIDs[SKY_FRONT] << std::endl;
    glBindTexture(GL_TEXTURE_2D, texIDs[SKY_FRONT]);
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(vecMax[0], vecMin[1], vecMin[2]);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(vecMax[0], vecMin[1], vecMax[2]);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(vecMin[0], vecMin[1], vecMax[2]);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(vecMin[0], vecMin[1], vecMin[2]);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texIDs[SKY_BACK]);
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(vecMin[0], vecMax[1], vecMin[2]);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(vecMin[0], vecMax[1], vecMax[2]);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(vecMax[0], vecMax[1], vecMax[2]);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(vecMax[0], vecMax[1], vecMin[2]);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texIDs[SKY_RIGHT]);
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(vecMax[0], vecMax[1], vecMin[2]);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(vecMax[0], vecMax[1], vecMax[2]);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(vecMax[0], vecMin[1], vecMax[2]);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(vecMax[0], vecMin[1], vecMin[2]);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texIDs[SKY_LEFT]);
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(vecMin[0], vecMin[1], vecMin[2]);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(vecMin[0], vecMin[1], vecMax[2]);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(vecMin[0], vecMax[1], vecMax[2]);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(vecMin[0], vecMax[1], vecMin[2]);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texIDs[SKY_TOP]);
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(vecMin[0], vecMax[1], vecMax[2]);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(vecMin[0], vecMin[1], vecMax[2]);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(vecMax[0], vecMin[1], vecMax[2]);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(vecMax[0], vecMax[1], vecMax[2]);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texIDs[SKY_BOTTOM]);
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(vecMin[0], vecMin[1], vecMin[2]);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(vecMin[0], vecMax[1], vecMin[2]);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(vecMax[0], vecMax[1], vecMin[2]);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(vecMax[0], vecMin[1], vecMin[2]);
    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}
