#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
//#include <GL/gl.h>
//#include <GL/glut.h>
#include <GL/freeglut.h>
#endif
#include <cmath>
#include <iostream>
#include <cstdlib>  // untuk rand() dan srand()
#include <ctime>    // untuk time()

#define BUILDING_WIDTH 40.0f
#define BUILDING_LENGTH 60.0f
#define WALL_HEIGHT 15.0f
#define ROOF_HEIGHT 12.0f
#define ROOF_OVERHANG 2.0f
#define FLOOR_COLOR_R 0.7f
#define FLOOR_COLOR_G 0.6f
#define FLOOR_COLOR_B 0.5f

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float rotateX = 0.0f, rotateY = 0.0f, translateZ = -50.0f;
int lastMouseX, lastMouseY;
bool mousePressed = false;

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    GLfloat global_ambient[] = { 0.1f, 0.1f, 0.1f, 0.8f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    GLfloat light_position[] = { 30.0f, 50.0f, 30.0f, 1.0f };
    GLfloat light_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glEnable(GL_NORMALIZE);

    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
}

void calculateNormal(float x1, float y1, float z1,
    float x2, float y2, float z2,
    float x3, float y3, float z3) {
    float v1x = x2 - x1;
    float v1y = y2 - y1;
    float v1z = z2 - z1;

    float v2x = x3 - x1;
    float v2y = y3 - y1;
    float v2z = z3 - z1;

    float nx = v1y * v2z - v1z * v2y;
    float ny = v1z * v2x - v1x * v2z;
    float nz = v1x * v2y - v1y * v2x;

    float length = sqrt(nx * nx + ny * ny + nz * nz);
    if (length > 0) {
        nx /= length;
        ny /= length;
        nz /= length;
    }

    glNormal3f(nx, ny, nz);
}

void drawQuad(float x1, float y1, float z1,
    float x2, float y2, float z2,
    float x3, float y3, float z3,
    float x4, float y4, float z4) {
    glBegin(GL_QUADS);
    calculateNormal(x1, y1, z1, x2, y2, z2, x3, y3, z3);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glVertex3f(x3, y3, z3);
    glVertex3f(x4, y4, z4);
    glEnd();
}

void drawDoors() {
    float doorWidth = 3.8f;
    float doorHeight = 10.0f;
    float doorGap = 0.1f;
    float wallThickness = 0.3f;
    float zOuter = BUILDING_LENGTH / 2 + 0.01f;
    float zInner = BUILDING_LENGTH / 2 - wallThickness;
    float horizontalOffset = 7.0f;
    float startX = -((4 * doorWidth) + (3 * doorGap)) / 2 + horizontalOffset;

    // Outer doors
    glColor3f(0.6f, 0.6f, 0.7f);
    for (int i = 0; i < 4; i++) {
        float xPos = startX + i * (doorWidth + doorGap);
        drawQuad(xPos, 0, zOuter,
            xPos + doorWidth, 0, zOuter,
            xPos + doorWidth, doorHeight, zOuter,
            xPos, doorHeight, zOuter);
    }

    // Inner doors (darker)
    glColor3f(0.5f, 0.5f, 0.6f);
    for (int i = 0; i < 4; i++) {
        float xPos = startX + i * (doorWidth + doorGap);
        drawQuad(xPos, 0, zInner,
            xPos + doorWidth, 0, zInner,
            xPos + doorWidth, doorHeight, zInner,
            xPos, doorHeight, zInner);
    }

    // Door handles
    glColor3f(0.3f, 0.3f, 0.3f);
    for (int i = 0; i < 4; i++) {
        float xPos = startX + i * (doorWidth + doorGap);
        // Outer handle
        drawQuad(xPos + doorWidth - 0.2f, doorHeight * 0.4f, zOuter + 0.02f,
            xPos + doorWidth - 0.1f, doorHeight * 0.4f, zOuter + 0.02f,
            xPos + doorWidth - 0.1f, doorHeight * 0.6f, zOuter + 0.02f,
            xPos + doorWidth - 0.2f, doorHeight * 0.6f, zOuter + 0.02f);
        // Inner handle
        drawQuad(xPos + 0.1f, doorHeight * 0.4f, zInner - 0.02f,
            xPos + 0.2f, doorHeight * 0.4f, zInner - 0.02f,
            xPos + 0.2f, doorHeight * 0.6f, zInner - 0.02f,
            xPos + 0.1f, doorHeight * 0.6f, zInner - 0.02f);
    }
}

void drawKetela(float x, float z) {
    float yBase = 0.01f;
    float batangTinggi = 1.2f;
    float batangRadius = 0.2f;

    // Batang (coklat)
    glColor3f(0.4f, 0.2f, 0.0f);
    glPushMatrix();
        glTranslatef(x, yBase, z);
        glRotatef(-90, 1, 0, 0); // agar silinder berdiri
        GLUquadric* quad = gluNewQuadric();
        gluCylinder(quad, batangRadius, batangRadius * 0.8, batangTinggi, 10, 10);
        gluDeleteQuadric(quad);
    glPopMatrix();

    // Daun (hijau)
    glColor3f(0.0f, 0.6f, 0.2f);
    glPushMatrix();
        glTranslatef(x, yBase + batangTinggi, z);
        glutSolidSphere(0.4f, 10, 10);
    glPopMatrix();
}

void colorWalls(){
    glColor3f(0.96f, 0.93f, 0.82f);
}

void drawWalls() {
    float halfW = BUILDING_WIDTH / 2;
    float halfL = BUILDING_LENGTH / 2;

    // Dinding depan
    colorWalls();
    drawQuad(-halfW, 0, halfL, halfW, 0, halfL, halfW, WALL_HEIGHT, halfL, -halfW, WALL_HEIGHT, halfL);

    // Dinding belakang
    colorWalls();
    drawQuad(-halfW, 0, -halfL, halfW, 0, -halfL, halfW, WALL_HEIGHT, -halfL, -halfW, WALL_HEIGHT, -halfL);

    // ==== JENDELA BELAKANG ====
    float jendelaSize = 5.0f;
    float gapX = 2.0f;
    float baseY = 5.0f;
    float zBack = -halfL - 0.3f;
    float tebalJendela = 1.0f;

    glColor3f(0.85f, 0.85f, 0.85f);
    float usableWidth = BUILDING_WIDTH - (2 * gapX);
    int numWindows = 4;
    float totalWidth = numWindows * jendelaSize + (numWindows - 1) * 1.0f;
    float startX = -totalWidth / 2.0f;

    for (int i = 0; i < numWindows; ++i) {
        float x1 = startX + i * (jendelaSize + 1.0f);
        float x2 = x1 + jendelaSize;
        float y1 = baseY;
        float y2 = baseY + jendelaSize;

        // Sisi luar
        drawQuad(x1, y1, zBack, x2, y1, zBack, x2, y2, zBack, x1, y2, zBack);
        // Sisi dalam
        drawQuad(x1, y1, zBack + tebalJendela, x2, y1, zBack + tebalJendela, x2, y2, zBack + tebalJendela, x1, y2, zBack + tebalJendela);
        // Sisi atas
        drawQuad(x1, y2, zBack, x2, y2, zBack, x2, y2, zBack + tebalJendela, x1, y2, zBack + tebalJendela);
        // Sisi bawah
        drawQuad(x1, y1, zBack, x2, y1, zBack, x2, y1, zBack + tebalJendela, x1, y1, zBack + tebalJendela);
        // Sisi kiri
        drawQuad(x1, y1, zBack, x1, y2, zBack, x1, y2, zBack + tebalJendela, x1, y1, zBack + tebalJendela);
        // Sisi kanan
        drawQuad(x2, y1, zBack, x2, y2, zBack, x2, y2, zBack + tebalJendela, x2, y1, zBack + tebalJendela);
    }

    // Dinding kiri
    colorWalls();
    drawQuad(-halfW, 0, -halfL, -halfW, 0, halfL, -halfW, WALL_HEIGHT, halfL, -halfW, WALL_HEIGHT, -halfL);

    // Dinding kanan
    colorWalls();
    drawQuad(halfW, 0, -halfL,  halfW, 0, halfL,  halfW, WALL_HEIGHT, halfL,  halfW, WALL_HEIGHT, -halfL);

    // ===== PINTU DI DINDING KANAN (3D Tebal) =====
    float pintuWidth = 4.0f;
    float pintuHeight = 7.0f;
    float pintuZ = 23.0f;
    float pintuY = 0.0f;
    float pintuThickness = 0.2f;

    float xLuar = halfW + 0.01f;
    float xDalam = halfW - 0.05f; // Lebih mundur lagi

    glColor3f(0.5f, 0.5f, 0.6f); // abu gelap

    // Sisi luar pintu (menghadap luar)
    drawQuad(
        xLuar, pintuY, pintuZ - pintuWidth / 2,
        xLuar, pintuY, pintuZ + pintuWidth / 2,
        xLuar, pintuY + pintuHeight, pintuZ + pintuWidth / 2,
        xLuar, pintuY + pintuHeight, pintuZ - pintuWidth / 2
    );

    // Sisi dalam pintu (menghadap dalam)
    drawQuad(
        xDalam, pintuY, pintuZ + pintuWidth / 2,
        xDalam, pintuY, pintuZ - pintuWidth / 2,
        xDalam, pintuY + pintuHeight, pintuZ - pintuWidth / 2,
        xDalam, pintuY + pintuHeight, pintuZ + pintuWidth / 2
    );

    // Sisi atas pintu
    drawQuad(
        xLuar, pintuY + pintuHeight, pintuZ - pintuWidth / 2,
        xDalam, pintuY + pintuHeight, pintuZ - pintuWidth / 2,
        xDalam, pintuY + pintuHeight, pintuZ + pintuWidth / 2,
        xLuar, pintuY + pintuHeight, pintuZ + pintuWidth / 2
    );

    // Sisi bawah pintu
    drawQuad(
        xLuar, pintuY, pintuZ - pintuWidth / 2,
        xDalam, pintuY, pintuZ - pintuWidth / 2,
        xDalam, pintuY, pintuZ + pintuWidth / 2,
        xLuar, pintuY, pintuZ + pintuWidth / 2
    );

    // Sisi kiri pintu
    drawQuad(
        xLuar, pintuY, pintuZ - pintuWidth / 2,
        xDalam, pintuY, pintuZ - pintuWidth / 2,
        xDalam, pintuY + pintuHeight, pintuZ - pintuWidth / 2,
        xLuar, pintuY + pintuHeight, pintuZ - pintuWidth / 2
    );

    // Sisi kanan pintu
    drawQuad(
        xLuar, pintuY, pintuZ + pintuWidth / 2,
        xDalam, pintuY, pintuZ + pintuWidth / 2,
        xDalam, pintuY + pintuHeight, pintuZ + pintuWidth / 2,
        xLuar, pintuY + pintuHeight, pintuZ + pintuWidth / 2
    );

        // ===== PINTU KEDUA DI UJUNG KANAN (bagian belakang) =====
    float pintuZ2 = -halfL + 4.0f; // posisi di ujung sisi kanan (misalnya 4.0f dari ujung belakang)

    // Sisi luar pintu (menghadap luar)
    drawQuad(
        xLuar, pintuY, pintuZ2 - pintuWidth / 2,
        xLuar, pintuY, pintuZ2 + pintuWidth / 2,
        xLuar, pintuY + pintuHeight, pintuZ2 + pintuWidth / 2,
        xLuar, pintuY + pintuHeight, pintuZ2 - pintuWidth / 2
    );

    // Sisi dalam pintu (menghadap dalam)
    drawQuad(
        xDalam, pintuY, pintuZ2 + pintuWidth / 2,
        xDalam, pintuY, pintuZ2 - pintuWidth / 2,
        xDalam, pintuY + pintuHeight, pintuZ2 - pintuWidth / 2,
        xDalam, pintuY + pintuHeight, pintuZ2 + pintuWidth / 2
    );

    // Sisi atas pintu
    drawQuad(
        xLuar, pintuY + pintuHeight, pintuZ2 - pintuWidth / 2,
        xDalam, pintuY + pintuHeight, pintuZ2 - pintuWidth / 2,
        xDalam, pintuY + pintuHeight, pintuZ2 + pintuWidth / 2,
        xLuar, pintuY + pintuHeight, pintuZ2 + pintuWidth / 2
    );

    // Sisi bawah pintu
    drawQuad(
        xLuar, pintuY, pintuZ2 - pintuWidth / 2,
        xDalam, pintuY, pintuZ2 - pintuWidth / 2,
        xDalam, pintuY, pintuZ2 + pintuWidth / 2,
        xLuar, pintuY, pintuZ2 + pintuWidth / 2
    );

    // Sisi kiri pintu
    drawQuad(
        xLuar, pintuY, pintuZ2 - pintuWidth / 2,
        xDalam, pintuY, pintuZ2 - pintuWidth / 2,
        xDalam, pintuY + pintuHeight, pintuZ2 - pintuWidth / 2,
        xLuar, pintuY + pintuHeight, pintuZ2 - pintuWidth / 2
    );

    // Sisi kanan pintu
    drawQuad(
        xLuar, pintuY, pintuZ2 + pintuWidth / 2,
        xDalam, pintuY, pintuZ2 + pintuWidth / 2,
        xDalam, pintuY + pintuHeight, pintuZ2 + pintuWidth / 2,
        xLuar, pintuY + pintuHeight, pintuZ2 + pintuWidth / 2
    );


    // Pintu depan
    drawDoors();
}


void drawLahanSampingKiri() {
    float halfW = BUILDING_WIDTH / 2.0f;
    float halfL = BUILDING_LENGTH / 2.0f;
    float offset = 2.0f;            // Jarak antar bangunan dan lahan
    float lahanWidth = 40.0f;       // Lebar lahan samping
    float y = 0.01f;                // Posisi Y sedikit di atas tanah

    float xKiri = -halfW - offset;  // Posisi awal lahan kiri
    float xLuar = xKiri - lahanWidth;

    float zDepan = halfL;
    float zBelakang = -halfL;
    float zTengah = 0.0f;           // Pembagi lahan depan & belakang

    // Lahan ketela (depan)
    glColor3f(0.55f, 0.27f, 0.07f); // Coklat tanah
    drawQuad(xLuar, y, zTengah, xKiri, y, zTengah, xKiri, y, zDepan, xLuar, y, zDepan);

    // Lahan rumah (belakang)
    glColor3f(0.8f, 0.8f, 0.6f);    // Krem terang, lahan rumah
    drawQuad(xLuar, y, zBelakang, xKiri, y, zBelakang, xKiri, y, zTengah, xLuar, y, zTengah);

    // Garis batas pemisah lahan
    glColor3f(0.0f, 0.0f, 0.0f);    // Hitam
    glLineWidth(3.0f);
    glBegin(GL_LINES);
        glVertex3f(xLuar, y + 0.01f, zTengah);
        glVertex3f(xKiri, y + 0.01f, zTengah);
    glEnd();

    for (float zx = zTengah + 2.0f; zx < zDepan - 2.0f; zx += 4.5f) {
        for (float xx = xLuar + 4.0f; xx < xKiri - 4.0f; xx += 4.5f) {
            drawKetela(xx, zx);
        }
    }
}


void drawRumahSampingKiri() {
    float halfW = BUILDING_WIDTH / 2.0f;
    float halfL = BUILDING_LENGTH / 2.0f;

    float offset = 5.0f;              // jarak dari bangunan utama
    float lahanWidth = 40.0f;
    float zTengah = 0.0f;
    float zBelakang = -halfL;

    float xKiri = -halfW - offset;
    float xLuar = xKiri - lahanWidth;

    float padding = 4.0f;  // agar rumah tidak nempel tepi lahan
    float x1 = xLuar + padding;
    float x2 = xKiri - padding;
    float z1 = zBelakang + padding;
    float z2 = zTengah - padding;

    float y1 = 0.01f;
    float y2 = y1 + 10.0f;   // tinggi rumah

    // Dinding rumah (warna krem)
    glColor3f(0.85f, 0.8f, 0.6f);
    // Depan
    drawQuad(x1, y1, z2, x2, y1, z2, x2, y2, z2, x1, y2, z2);
    // Belakang
    drawQuad(x1, y1, z1, x2, y1, z1, x2, y2, z1, x1, y2, z1);
    // Kiri
    drawQuad(x1, y1, z1, x1, y1, z2, x1, y2, z2, x1, y2, z1);
    // Kanan
    drawQuad(x2, y1, z1, x2, y1, z2, x2, y2, z2, x2, y2, z1);
    // Atas (langit-langit rumah)
    drawQuad(x1, y2, z1, x2, y2, z1, x2, y2, z2, x1, y2, z2);

    // Atap pelana
    float yAtap = y2 + 4.0f; // tinggi atap
    float xTengah = (x1 + x2) / 2.0f;

    glColor3f(0.6f, 0.2f, 0.2f);  // warna merah bata

    // Sisi kiri atap (miring)
    glBegin(GL_TRIANGLES);
        glVertex3f(x1, y2, z1);
        glVertex3f(x2, y2, z1);
        glVertex3f(xTengah, yAtap, z1);
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex3f(x1, y2, z2);
        glVertex3f(x2, y2, z2);
        glVertex3f(xTengah, yAtap, z2);
    glEnd();

    // Sisi segitiga kiri
    glBegin(GL_QUADS);
        glVertex3f(x1, y2, z1);
        glVertex3f(x1, y2, z2);
        glVertex3f(xTengah, yAtap, z2);
        glVertex3f(xTengah, yAtap, z1);
    glEnd();

    // Sisi segitiga kanan
    glBegin(GL_QUADS);
        glVertex3f(x2, y2, z1);
        glVertex3f(x2, y2, z2);
        glVertex3f(xTengah, yAtap, z2);
        glVertex3f(xTengah, yAtap, z1);
    glEnd();
}




// KANOPI BANGUNAN
void drawKanopi() {
    float halfW = BUILDING_WIDTH / 2;
    float zDepan = BUILDING_LENGTH / 2 + 0.5f; // sedikit keluar dari dinding depan
    float kanopiDepth = 15.5f; // menjorok ke depan
    float doorHeight = 25.0f;
    float kanopiY = doorHeight / 2.0f; // tengah antara bawah dan atas pintu

    glColor3f(0.6f, 0.6f, 0.7f); // warna abu seperti atap
    drawQuad(
        -halfW, kanopiY, zDepan,
         halfW, kanopiY, zDepan,
         halfW, kanopiY, zDepan + kanopiDepth,
        -halfW, kanopiY, zDepan + kanopiDepth
    );
}

void drawBangunanDepan() {

    float boxWidth = BUILDING_WIDTH / 3.0f;  // kecilin
    float boxLength = 16.0f;
    float boxHeight = 12.4f;
    float halfBoxW = boxWidth / 2;
    float halfBoxL = boxLength / 2;

    // Digeser lebih kiri dari tengah (pepet ke kiri bangunan besar)
    float offsetX = -halfBoxW - 6.5f;  
    float offsetZ = BUILDING_LENGTH / 2 + halfBoxL + 0.01f;

    float windowWidth = 5.0f;
    float windowHeight = 5.0f;
    float windowY = 4.0f;

    // ========= DINDING DEPAN =========
    glColor3f(0.96f, 0.93f, 0.82f); // Warna dinding depan
    drawQuad(
        offsetX - halfBoxW, 0, offsetZ + halfBoxL,
        offsetX + halfBoxW, 0, offsetZ + halfBoxL,
        offsetX + halfBoxW, boxHeight, offsetZ + halfBoxL,
        offsetX - halfBoxW, boxHeight, offsetZ + halfBoxL
    );

    // ====== JENDELA KIRI DEPAN ======
    glColor3f(0.85f, 0.85f, 0.85f);
    drawQuad(
        offsetX - windowWidth, windowY + windowHeight, offsetZ + halfBoxL + 0.01f,
        offsetX - 0.5f, windowY + windowHeight, offsetZ + halfBoxL + 0.01f,
        offsetX - 0.5f, windowY, offsetZ + halfBoxL + 0.01f,
        offsetX - windowWidth, windowY, offsetZ + halfBoxL + 0.01f
    );

    // ====== JENDELA KANAN DEPAN ======
    drawQuad(
        offsetX + 0.5f, windowY + windowHeight, offsetZ + halfBoxL + 0.01f,
        offsetX + windowWidth, windowY + windowHeight, offsetZ + halfBoxL + 0.01f,
        offsetX + windowWidth, windowY, offsetZ + halfBoxL + 0.01f,
        offsetX + 0.5f, windowY, offsetZ + halfBoxL + 0.01f
    );

    // ========= DINDING BELAKANG =========
    glColor3f(0.96f, 0.93f, 0.82f);
    drawQuad(offsetX - halfBoxW, 0, offsetZ - halfBoxL,
             offsetX + halfBoxW, 0, offsetZ - halfBoxL,
             offsetX + halfBoxW, boxHeight, offsetZ - halfBoxL,
             offsetX - halfBoxW, boxHeight, offsetZ - halfBoxL);

    // ========= DINDING KIRI =========
    drawQuad(offsetX - halfBoxW, 0, offsetZ - halfBoxL,
             offsetX - halfBoxW, 0, offsetZ + halfBoxL,
             offsetX - halfBoxW, boxHeight, offsetZ + halfBoxL,
             offsetX - halfBoxW, boxHeight, offsetZ - halfBoxL);

    // ========= DINDING KANAN =========
    drawQuad(offsetX + halfBoxW, 0, offsetZ - halfBoxL,
             offsetX + halfBoxW, 0, offsetZ + halfBoxL,
             offsetX + halfBoxW, boxHeight, offsetZ + halfBoxL,
             offsetX + halfBoxW, boxHeight, offsetZ - halfBoxL);

    // ===== PINTU DINDING KANAN (MENYENTUH TANAH) =====
    glColor3f(0.6f, 0.6f, 0.7f);
    float rightWallX = offsetX + halfBoxW + 0.01f;  // Sedikit maju biar ga nembus

    float jendelaKananAtasY = windowY + windowHeight;  // Tinggi jendela (misal: 4 + 5 = 9)
    float jendelaKananBawahY = 0.0f;                   // Menyentuh tanah

    drawQuad(
        rightWallX, jendelaKananAtasY, offsetZ - 3.0f,   // Kiri atas
        rightWallX, jendelaKananAtasY, offsetZ + 3.0f,   // Kanan atas
        rightWallX, jendelaKananBawahY, offsetZ + 3.0f,  // Kanan bawah (tanah)
        rightWallX, jendelaKananBawahY, offsetZ - 3.0f   // Kiri bawah (tanah)
    );


    // ========= ATAP =========
    glColor3f(0.6f, 0.6f, 0.7f);
    drawQuad(offsetX - halfBoxW, boxHeight, offsetZ - halfBoxL,
             offsetX + halfBoxW, boxHeight, offsetZ - halfBoxL,
             offsetX + halfBoxW, boxHeight, offsetZ + halfBoxL,
             offsetX - halfBoxW, boxHeight, offsetZ + halfBoxL);
}

void drawTree(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 0, z);

    // Batang
    glColor3f(0.55f, 0.27f, 0.07f);
    GLUquadric* trunk = gluNewQuadric();
    glPushMatrix();
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(trunk, 0.5f, 0.5f, 7.0f, 12, 12);
    glPopMatrix();
    gluDeleteQuadric(trunk);

    // Level 1 daun (besar)
    glColor3f(0.0f, 0.5f, 0.0f);
    GLUquadric* leaves1 = gluNewQuadric();
    glPushMatrix();
    glTranslatef(0.0f, 7.0f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(leaves1, 4.0f, 0.0f, 4.0f, 16, 16);
    glPopMatrix();
    gluDeleteQuadric(leaves1);

    // Level 2 daun (sedang)
    GLUquadric* leaves2 = gluNewQuadric();
    glPushMatrix();
    glTranslatef(0.0f, 11.0f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(leaves2, 3.2f, 0.0f, 3.5f, 16, 16);
    glPopMatrix();
    gluDeleteQuadric(leaves2);

    // Level 3 daun (kecil)
    GLUquadric* leaves3 = gluNewQuadric();
    glPushMatrix();
    glTranslatef(0.0f, 14.0f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(leaves3, 2.5f, 0.0f, 3.0f, 16, 16);
    glPopMatrix();
    gluDeleteQuadric(leaves3);

    // Level 4 daun (puncak)
    GLUquadric* leaves4 = gluNewQuadric();
    glPushMatrix();
    glTranslatef(0.0f, 17.0f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(leaves4, 1.8f, 0.0f, 2.5f, 16, 16);
    glPopMatrix();
    gluDeleteQuadric(leaves4);

    glPopMatrix();
}


void drawRandomTrees() {
    srand(4321); // Seed tetap agar acak tapi konsisten

    int treeCount = 500;  // Lebih banyak pohon
    int drawn = 0;
    int maxTry = 1000; // Dicukupkan agar nggak infinite loop

    // Area jalan
    float jalanMinX = -100.0f;
    float jalanMaxX = BUILDING_WIDTH - 2;
    float jalanMinZ = BUILDING_LENGTH / 2 + 15.0f;
    float jalanMaxZ = jalanMinZ + 20.0f;

    // Area gedung
    float gedungMinX = -50.0f, gedungMaxX = 50.0f;
    float gedungMinZ = -60.0f, gedungMaxZ = 60.0f;

    // Batas map
    float areaMinX = -100.0f, areaMaxX = 100.0f;
    float areaMinZ = -80.0f, areaMaxZ = 100.0f;

    float minDistGedung = 12.0f;

    while (drawn < treeCount && maxTry > 0) {
        float x = areaMinX + static_cast<float>(rand()) / RAND_MAX * (areaMaxX - areaMinX);
        float z = areaMinZ + static_cast<float>(rand()) / RAND_MAX * (areaMaxZ - areaMinZ);

        // Hindari gedung + jarak aman
        if (x > gedungMinX - minDistGedung && x < gedungMaxX + minDistGedung &&
            z > gedungMinZ - minDistGedung && z < gedungMaxZ + minDistGedung)
        { maxTry--; continue; }

        // Hindari jalan
        if (x > jalanMinX && x < jalanMaxX && z > jalanMinZ && z < jalanMaxZ)
        { maxTry--; continue; }

        drawTree(x, z);
        drawn++;
        maxTry--;
    }

    // Tetap gambar pohon rapi di sisi jalan
    float offsetSisi = 3.5f;
    int pohonSisi = 7;
    float stepX = (jalanMaxX - jalanMinX) / (pohonSisi - 1);
    for (int i = 0; i < pohonSisi; ++i) {
        float x = jalanMinX + i * stepX;
        if (x >= gedungMinX && x <= gedungMaxX)
            continue;

        float zKiri = jalanMinZ - offsetSisi;
        float zKanan = jalanMaxZ + offsetSisi;
        drawTree(x, zKiri);
        drawTree(x, zKanan);
    }
}


void drawTembokPendekDanTiangKanopi() {
    float halfW = BUILDING_WIDTH / 2;
    float zDepan = BUILDING_LENGTH / 2 + 0.5f;
    float kanopiDepth = 15.5f;
    float tembokHeight = WALL_HEIGHT / 4.0f;
    float tembokThickness = 0.5f;

    float zTembok = zDepan + kanopiDepth;

    float tiangWidth = 0.7f;
    float tiangDepth = 0.7f;
    float kanopiY = 25.0f / 2.0f; // tinggi kanopi

    // Posisi tiang
    float tiang1X = 0.0f; // tengah
    float tiang2X = halfW - tiangWidth/2; // kanan

    // Tembok kecil hanya dari tepi kanan tiang tengah ke tepi kiri tiang kanan
    float x1 = tiang1X + tiangWidth/2;
    float x2 = tiang2X - tiangWidth/2;

    glColor3f(0.96f, 0.93f, 0.82f);
    // Depan tembok
    drawQuad(
        x1, 0, zTembok,
        x2, 0, zTembok,
        x2, tembokHeight, zTembok,
        x1, tembokHeight, zTembok
    );
    // Belakang tembok
    drawQuad(
        x1, 0, zTembok + tembokThickness,
        x2, 0, zTembok + tembokThickness,
        x2, tembokHeight, zTembok + tembokThickness,
        x1, tembokHeight, zTembok + tembokThickness
    );
    // Sisi kiri tembok
    drawQuad(
        x1, 0, zTembok,
        x1, 0, zTembok + tembokThickness,
        x1, tembokHeight, zTembok + tembokThickness,
        x1, tembokHeight, zTembok
    );
    // Sisi kanan tembok
    drawQuad(
        x2, 0, zTembok,
        x2, 0, zTembok + tembokThickness,
        x2, tembokHeight, zTembok + tembokThickness,
        x2, tembokHeight, zTembok
    );
    // Sisi atas tembok
    drawQuad(
        x1, tembokHeight, zTembok,
        x2, tembokHeight, zTembok,
        x2, tembokHeight, zTembok + tembokThickness,
        x1, tembokHeight, zTembok + tembokThickness
    );

    // ================= TIANG KANOPI ===================
    glColor3f(0.96f, 0.93f, 0.82f);

    // Tiang tengah
    float tiang1Z = zTembok;
    drawQuad(
        tiang1X - tiangWidth/2, 0, tiang1Z - tiangDepth/2,
        tiang1X + tiangWidth/2, 0, tiang1Z - tiangDepth/2,
        tiang1X + tiangWidth/2, kanopiY, tiang1Z - tiangDepth/2,
        tiang1X - tiangWidth/2, kanopiY, tiang1Z - tiangDepth/2
    );
    drawQuad(
        tiang1X - tiangWidth/2, 0, tiang1Z + tiangDepth/2,
        tiang1X + tiangWidth/2, 0, tiang1Z + tiangDepth/2,
        tiang1X + tiangWidth/2, kanopiY, tiang1Z + tiangDepth/2,
        tiang1X - tiangWidth/2, kanopiY, tiang1Z + tiangDepth/2
    );
    // Sisi kiri & kanan tiang tengah
    drawQuad(
        tiang1X - tiangWidth/2, 0, tiang1Z - tiangDepth/2,
        tiang1X - tiangWidth/2, 0, tiang1Z + tiangDepth/2,
        tiang1X - tiangWidth/2, kanopiY, tiang1Z + tiangDepth/2,
        tiang1X - tiangWidth/2, kanopiY, tiang1Z - tiangDepth/2
    );
    drawQuad(
        tiang1X + tiangWidth/2, 0, tiang1Z - tiangDepth/2,
        tiang1X + tiangWidth/2, 0, tiang1Z + tiangDepth/2,
        tiang1X + tiangWidth/2, kanopiY, tiang1Z + tiangDepth/2,
        tiang1X + tiangWidth/2, kanopiY, tiang1Z - tiangDepth/2
    );
    // Sisi atas & bawah tiang tengah
    drawQuad(
        tiang1X - tiangWidth/2, kanopiY, tiang1Z - tiangDepth/2,
        tiang1X + tiangWidth/2, kanopiY, tiang1Z - tiangDepth/2,
        tiang1X + tiangWidth/2, kanopiY, tiang1Z + tiangDepth/2,
        tiang1X - tiangWidth/2, kanopiY, tiang1Z + tiangDepth/2
    );
    drawQuad(
        tiang1X - tiangWidth/2, 0, tiang1Z - tiangDepth/2,
        tiang1X + tiangWidth/2, 0, tiang1Z - tiangDepth/2,
        tiang1X + tiangWidth/2, 0, tiang1Z + tiangDepth/2,
        tiang1X - tiangWidth/2, 0, tiang1Z + tiangDepth/2
    );

    // Tiang kanan depan
    float tiang2Z = zTembok;
    drawQuad(
        tiang2X - tiangWidth/2, 0, tiang2Z - tiangDepth/2,
        tiang2X + tiangWidth/2, 0, tiang2Z - tiangDepth/2,
        tiang2X + tiangWidth/2, kanopiY, tiang2Z - tiangDepth/2,
        tiang2X - tiangWidth/2, kanopiY, tiang2Z - tiangDepth/2
    );
    drawQuad(
        tiang2X - tiangWidth/2, 0, tiang2Z + tiangDepth/2,
        tiang2X + tiangWidth/2, 0, tiang2Z + tiangDepth/2,
        tiang2X + tiangWidth/2, kanopiY, tiang2Z + tiangDepth/2,
        tiang2X - tiangWidth/2, kanopiY, tiang2Z + tiangDepth/2
    );
    // Sisi kiri & kanan tiang kanan
    drawQuad(
        tiang2X - tiangWidth/2, 0, tiang2Z - tiangDepth/2,
        tiang2X - tiangWidth/2, 0, tiang2Z + tiangDepth/2,
        tiang2X - tiangWidth/2, kanopiY, tiang2Z + tiangDepth/2,
        tiang2X - tiangWidth/2, kanopiY, tiang2Z - tiangDepth/2
    );
    drawQuad(
        tiang2X + tiangWidth/2, 0, tiang2Z - tiangDepth/2,
        tiang2X + tiangWidth/2, 0, tiang2Z + tiangDepth/2,
        tiang2X + tiangWidth/2, kanopiY, tiang2Z + tiangDepth/2,
        tiang2X + tiangWidth/2, kanopiY, tiang2Z - tiangDepth/2
    );
    // Sisi atas & bawah tiang kanan
    drawQuad(
        tiang2X - tiangWidth/2, kanopiY, tiang2Z - tiangDepth/2,
        tiang2X + tiangWidth/2, kanopiY, tiang2Z - tiangDepth/2,
        tiang2X + tiangWidth/2, kanopiY, tiang2Z + tiangDepth/2,
        tiang2X - tiangWidth/2, kanopiY, tiang2Z + tiangDepth/2
    );
    drawQuad(
        tiang2X - tiangWidth/2, 0, tiang2Z - tiangDepth/2,
        tiang2X + tiangWidth/2, 0, tiang2Z - tiangDepth/2,
        tiang2X + tiangWidth/2, 0, tiang2Z + tiangDepth/2,
        tiang2X - tiangWidth/2, 0, tiang2Z + tiangDepth/2
    );
}

void drawTembokSampingPendek() {
    float halfW = BUILDING_WIDTH / 2;
    float zDepan = BUILDING_LENGTH / 2 ;
    float kanopiDepth = 30.0f;
    float tembokHeight = WALL_HEIGHT / 4.0f;
    float tembokThickness = 0.5f;

    // Panjang tembok samping (misal hanya 1/3 dari panjang kanopi)
    float panjangTembok = kanopiDepth * 0.33f;

    // Posisi X menempel ke sisi kanan gedung utama
    float xTembok = halfW - tembokThickness;

    // Z awal tembok (menempel ke depan gedung utama)
    float zAwal = zDepan;
    // Z akhir tembok (lebih ke depan, tapi tidak sampai ke tiang)
    float zAkhir = zDepan + panjangTembok;

    glColor3f(0.96f, 0.93f, 0.82f);
    // Sisi luar tembok samping kanan
    drawQuad(
        xTembok, 0, zAwal,
        xTembok, 0, zAkhir,
        xTembok, tembokHeight, zAkhir,
        xTembok, tembokHeight, zAwal
    );
    // Sisi dalam tembok samping kanan
    drawQuad(
        xTembok + tembokThickness, 0, zAwal,
        xTembok + tembokThickness, 0, zAkhir,
        xTembok + tembokThickness, tembokHeight, zAkhir,
        xTembok + tembokThickness, tembokHeight, zAwal
    );
    // Sisi depan tembok
    drawQuad(
        xTembok, 0, zAkhir,
        xTembok + tembokThickness, 0, zAkhir,
        xTembok + tembokThickness, tembokHeight, zAkhir,
        xTembok, tembokHeight, zAkhir
    );
    // Sisi belakang tembok
    drawQuad(
        xTembok, 0, zAwal,
        xTembok + tembokThickness, 0, zAwal,
        xTembok + tembokThickness, tembokHeight, zAwal,
        xTembok, tembokHeight, zAwal
    );
    // Sisi atas tembok
    drawQuad(
        xTembok, tembokHeight, zAwal,
        xTembok, tembokHeight, zAkhir,
        xTembok + tembokThickness, tembokHeight, zAkhir,
        xTembok + tembokThickness, tembokHeight, zAwal
    );
}

void drawJendelaBangunanSamping(float offsetX, float offsetZ, float halfBoxL) {
    float jendelaW = 2.0f;
    float jendelaH = 6.0f;
    float gap = 0.5f;
    float jendelaY = 2.0f;
    float jendelaZ = offsetZ + halfBoxL +0.1f;

    float jendelaX_kiri = offsetX - (jendelaW + gap / 2);
    float jendelaX_kanan = offsetX + gap / 2;

    glColor3f(0.7f, 0.7f, 0.7f); // abu terang
    drawQuad(jendelaX_kiri, jendelaY, jendelaZ,
             jendelaX_kiri + jendelaW, jendelaY, jendelaZ,
             jendelaX_kiri + jendelaW, jendelaY + jendelaH, jendelaZ,
             jendelaX_kiri, jendelaY + jendelaH, jendelaZ);

    drawQuad(jendelaX_kanan, jendelaY, jendelaZ,
             jendelaX_kanan + jendelaW, jendelaY, jendelaZ,
             jendelaX_kanan + jendelaW, jendelaY + jendelaH, jendelaZ,
             jendelaX_kanan, jendelaY + jendelaH, jendelaZ);
}

void drawBangunanSamping() {
    // Ukuran bangunan kotak
    float boxWidth = 10.0f;
    float boxLength = 14.0f;
    float boxHeight = WALL_HEIGHT * 0.6f; // setengah lebih sedikit
    float halfBoxW = boxWidth / 2;
    float halfBoxL = boxLength / 2;
    float jarakAntarBangunan = 8.0f; 

    // Posisi: sebelah kiri pintu, agak mundur ke belakang
    float offsetX = BUILDING_WIDTH / 2 + halfBoxW + jarakAntarBangunan;

    float offsetZ = 8.0f; // agak masuk ke dalam

    drawJendelaBangunanSamping(offsetX, offsetZ,halfBoxL);
    // Dinding (warna sama)
    glColor3f(0.96f, 0.93f, 0.82f);
    // Depan
    drawQuad(offsetX - halfBoxW, 0, offsetZ + halfBoxL,
             offsetX + halfBoxW, 0, offsetZ + halfBoxL,
             offsetX + halfBoxW, boxHeight, offsetZ + halfBoxL,
             offsetX - halfBoxW, boxHeight, offsetZ + halfBoxL);
    // Belakang
    drawQuad(offsetX - halfBoxW, 0, offsetZ - halfBoxL,
             offsetX + halfBoxW, 0, offsetZ - halfBoxL,
             offsetX + halfBoxW, boxHeight, offsetZ - halfBoxL,
             offsetX - halfBoxW, boxHeight, offsetZ - halfBoxL);
    // Kiri
    drawQuad(offsetX - halfBoxW, 0, offsetZ - halfBoxL,
             offsetX - halfBoxW, 0, offsetZ + halfBoxL,
             offsetX - halfBoxW, boxHeight, offsetZ + halfBoxL,
             offsetX - halfBoxW, boxHeight, offsetZ - halfBoxL);
    // Kanan
    drawQuad(offsetX + halfBoxW, 0, offsetZ - halfBoxL,
             offsetX + halfBoxW, 0, offsetZ + halfBoxL,
             offsetX + halfBoxW, boxHeight, offsetZ + halfBoxL,
             offsetX + halfBoxW, boxHeight, offsetZ - halfBoxL);

    // Atap datar (warna abu seperti atap utama)
    glColor3f(0.6f, 0.6f, 0.7f);
    drawQuad(offsetX - halfBoxW, boxHeight, offsetZ - halfBoxL,
             offsetX + halfBoxW, boxHeight, offsetZ - halfBoxL,
             offsetX + halfBoxW, boxHeight, offsetZ + halfBoxL,
             offsetX - halfBoxW, boxHeight, offsetZ + halfBoxL);
}

// void drawJalan() {
//     glColor3f(0.55f, 0.55f, 0.6f); // Abu kebiruan, beda dengan warna teras
//     float halfW = BUILDING_WIDTH / 2;
//     float pavingY = -0.49f;
//     float zDepanTeras = BUILDING_LENGTH / 2 + 15.0f; // persis di depan paving teras
//     float pavingLebar = 20.0f; // lebar jalan (sumbu Z)
//     drawQuad(-100.0f, pavingY, zDepanTeras, 
//               BUILDING_WIDTH-2, pavingY, zDepanTeras,
//               BUILDING_WIDTH-2, pavingY, zDepanTeras + pavingLebar,
//              -100.0f, pavingY, zDepanTeras + pavingLebar);
// }

void drawJalan() {
    glColor3f(0.55f, 0.55f, 0.6f); // Abu kebiruan

    float pavingY = -0.49f;

    float offset = 18.0f; // Jarak agar tidak mepet dengan depan bangunan
    float zDepanTeras = BUILDING_LENGTH / 2 + offset;  // tambah offset di sini

    float pavingLebar = 20.0f; // panjang jalan (arah Z)
    float xKiri = -100.0f;
    float xKanan = BUILDING_WIDTH - 2.0f;

    drawQuad(
        xKiri,  pavingY, zDepanTeras,
        xKanan, pavingY, zDepanTeras,
        xKanan, pavingY, zDepanTeras + pavingLebar,
        xKiri,  pavingY, zDepanTeras + pavingLebar
    );
}


void drawAlas() {
    float pavingY = -0.48f;
    float backPavingY = pavingY + 1.0f; // Higher in the back
    float zDepan = BUILDING_LENGTH / 2;
    float zBelakang = -BUILDING_LENGTH / 2 - 8.5f;
    float halfW = BUILDING_WIDTH / 2;

    // Main floor (same color as badminton court)
    glColor3f(FLOOR_COLOR_R, FLOOR_COLOR_G, FLOOR_COLOR_B);
    drawQuad(-halfW, pavingY, zBelakang,
        halfW, pavingY, zBelakang,
        halfW, pavingY, zDepan + 15.0f,
        -halfW, pavingY, zDepan + 15.0f);

    // Back area (higher)
    // drawQuad(-halfW, backPavingY, zBelakang - 8.5f,
    //     halfW, backPavingY, zBelakang - 8.5f,
    //     halfW, backPavingY, zBelakang,
    //     -halfW, backPavingY, zBelakang);
}
void drawKanopiBelakang() {
    float halfW = BUILDING_WIDTH / 2;
    float zBelakang = -BUILDING_LENGTH / 2 - 0.5f; // sedikit keluar dari dinding belakang
    float kanopiDepth = 8.5f; // sama seperti depan
    float doorHeight = 25.0f;
    float kanopiY = doorHeight / 2.0f; // sejajar dengan kanopi depan

    glColor3f(0.6f, 0.6f, 0.7f); // warna abu seperti atap
    drawQuad(
        -halfW, kanopiY, zBelakang,
         halfW, kanopiY, zBelakang,
         halfW, kanopiY, zBelakang - kanopiDepth,
        -halfW, kanopiY, zBelakang - kanopiDepth
    );

    float tiangWidth = 0.7f;
    float tiangDepth = 0.7f;
    float tiangHeight = kanopiY;
    float zTiang = zBelakang - kanopiDepth; // posisi depan tiang (ujung kanopi)

    // Posisi X tiang kiri dan kanan (bisa diatur sesuai kebutuhan)
    float xTiangKiri = -halfW + tiangWidth;
    float xTiangKanan = halfW - tiangWidth;

    glColor3f(0.96f, 0.93f, 0.82f);

    // Tiang kiri
    drawQuad(
        xTiangKiri - tiangWidth/2, 0, zTiang - tiangDepth/2,
        xTiangKiri + tiangWidth/2, 0, zTiang - tiangDepth/2,
        xTiangKiri + tiangWidth/2, tiangHeight, zTiang - tiangDepth/2,
        xTiangKiri - tiangWidth/2, tiangHeight, zTiang - tiangDepth/2
    );
    drawQuad(
        xTiangKiri - tiangWidth/2, 0, zTiang + tiangDepth/2,
        xTiangKiri + tiangWidth/2, 0, zTiang + tiangDepth/2,
        xTiangKiri + tiangWidth/2, tiangHeight, zTiang + tiangDepth/2,
        xTiangKiri - tiangWidth/2, tiangHeight, zTiang + tiangDepth/2
    );
    // Sisi kiri & kanan tiang kiri
    drawQuad(
        xTiangKiri - tiangWidth/2, 0, zTiang - tiangDepth/2,
        xTiangKiri - tiangWidth/2, 0, zTiang + tiangDepth/2,
        xTiangKiri - tiangWidth/2, tiangHeight, zTiang + tiangDepth/2,
        xTiangKiri - tiangWidth/2, tiangHeight, zTiang - tiangDepth/2
    );
    drawQuad(
        xTiangKiri + tiangWidth/2, 0, zTiang - tiangDepth/2,
        xTiangKiri + tiangWidth/2, 0, zTiang + tiangDepth/2,
        xTiangKiri + tiangWidth/2, tiangHeight, zTiang + tiangDepth/2,
        xTiangKiri + tiangWidth/2, tiangHeight, zTiang - tiangDepth/2
    );
    // Sisi atas & bawah tiang kiri
    drawQuad(
        xTiangKiri - tiangWidth/2, tiangHeight, zTiang - tiangDepth/2,
        xTiangKiri + tiangWidth/2, tiangHeight, zTiang - tiangDepth/2,
        xTiangKiri + tiangWidth/2, tiangHeight, zTiang + tiangDepth/2,
        xTiangKiri - tiangWidth/2, tiangHeight, zTiang + tiangDepth/2
    );
    drawQuad(
        xTiangKiri - tiangWidth/2, 0, zTiang - tiangDepth/2,
        xTiangKiri + tiangWidth/2, 0, zTiang - tiangDepth/2,
        xTiangKiri + tiangWidth/2, 0, zTiang + tiangDepth/2,
        xTiangKiri - tiangWidth/2, 0, zTiang + tiangDepth/2
    );

    // Tiang kanan (copy dari kiri, ganti xTiangKanan)
    drawQuad(
        xTiangKanan - tiangWidth/2, 0, zTiang - tiangDepth/2,
        xTiangKanan + tiangWidth/2, 0, zTiang - tiangDepth/2,
        xTiangKanan + tiangWidth/2, tiangHeight, zTiang - tiangDepth/2,
        xTiangKanan - tiangWidth/2, tiangHeight, zTiang - tiangDepth/2
    );
    drawQuad(
        xTiangKanan - tiangWidth/2, 0, zTiang + tiangDepth/2,
        xTiangKanan + tiangWidth/2, 0, zTiang + tiangDepth/2,
        xTiangKanan + tiangWidth/2, tiangHeight, zTiang + tiangDepth/2,
        xTiangKanan - tiangWidth/2, tiangHeight, zTiang + tiangDepth/2
    );
    drawQuad(
        xTiangKanan - tiangWidth/2, 0, zTiang - tiangDepth/2,
        xTiangKanan - tiangWidth/2, 0, zTiang + tiangDepth/2,
        xTiangKanan - tiangWidth/2, tiangHeight, zTiang + tiangDepth/2,
        xTiangKanan - tiangWidth/2, tiangHeight, zTiang - tiangDepth/2
    );
    drawQuad(
        xTiangKanan + tiangWidth/2, 0, zTiang - tiangDepth/2,
        xTiangKanan + tiangWidth/2, 0, zTiang + tiangDepth/2,
        xTiangKanan + tiangWidth/2, tiangHeight, zTiang + tiangDepth/2,
        xTiangKanan + tiangWidth/2, tiangHeight, zTiang - tiangDepth/2
    );
    drawQuad(
        xTiangKanan - tiangWidth/2, tiangHeight, zTiang - tiangDepth/2,
        xTiangKanan + tiangWidth/2, tiangHeight, zTiang - tiangDepth/2,
        xTiangKanan + tiangWidth/2, tiangHeight, zTiang + tiangDepth/2,
        xTiangKanan - tiangWidth/2, tiangHeight, zTiang + tiangDepth/2
    );
    drawQuad(
        xTiangKanan - tiangWidth/2, 0, zTiang - tiangDepth/2,
        xTiangKanan + tiangWidth/2, 0, zTiang - tiangDepth/2,
        xTiangKanan + tiangWidth/2, 0, zTiang + tiangDepth/2,
        xTiangKanan - tiangWidth/2, 0, zTiang + tiangDepth/2
    );
    
}

//jalan
void drawJalanDepanKanopiBelakang(float panjangZ, float lebarX) {
    glColor3f(0.2f, 0.2f, 0.2f); // Abu gelap

    float zDepanKanopi = -BUILDING_LENGTH / 2 - 10.0f; // posisi awal jalan (di belakang bangunan)
    float zAkhir = zDepanKanopi - panjangZ;

    float xKiri = -lebarX / 2.0f;
    float xKanan = lebarX / 2.0f;

    drawQuad(
        xKiri, 0.01f, zDepanKanopi,
        xKanan, 0.01f, zDepanKanopi,
        xKanan, 0.01f, zAkhir,
        xKiri, 0.01f, zAkhir
    );
}


void drawJalanVertikalBawah(float panjangJalan, float lebarJalan) {
    glColor3f(0.2f, 0.2f, 0.2f); // Warna sama

    float zDepanKanopi = -BUILDING_LENGTH / 2 - 10.0f;
    float zAwal = zDepanKanopi - lebarJalan; // posisi dasar T
    float zAkhir = zAwal - panjangJalan;

    float x1 = -lebarJalan / 2;
    float x2 =  lebarJalan / 2;

    drawQuad(
        x1, 0.01f, zAwal,
        x2, 0.01f, zAwal,
        x2, 0.01f, zAkhir,
        x1, 0.01f, zAkhir
    );
}




void drawBox(float x, float y, float z, float width, float height, float depth) {
    float halfW = width / 2.0f;
    float halfH = height;
    float halfD = depth / 2.0f;

    float x1 = x - halfW, x2 = x + halfW;
    float y1 = y,        y2 = y + halfH;
    float z1 = z - halfD, z2 = z + halfD;

    // 6 sisi kotak
    glBegin(GL_QUADS);
    
    // Depan
    glVertex3f(x1, y1, z2);
    glVertex3f(x2, y1, z2);
    glVertex3f(x2, y2, z2);
    glVertex3f(x1, y2, z2);

    // Belakang
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y1, z1);
    glVertex3f(x2, y2, z1);
    glVertex3f(x1, y2, z1);

    // Kiri
    glVertex3f(x1, y1, z1);
    glVertex3f(x1, y1, z2);
    glVertex3f(x1, y2, z2);
    glVertex3f(x1, y2, z1);

    // Kanan
    glVertex3f(x2, y1, z1);
    glVertex3f(x2, y1, z2);
    glVertex3f(x2, y2, z2);
    glVertex3f(x2, y2, z1);

    // Atas
    glVertex3f(x1, y2, z1);
    glVertex3f(x2, y2, z1);
    glVertex3f(x2, y2, z2);
    glVertex3f(x1, y2, z2);

    // Bawah
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y1, z1);
    glVertex3f(x2, y1, z2);
    glVertex3f(x1, y1, z2);

    glEnd();
}


// rumah
void drawRumahSederhana(float x, float y, float z) {
    // Rumah kotak
    glColor3f(0.85f, 0.75f, 0.65f); // warna dinding
    drawBox(x, y, z, 10.0f, 6.0f, 8.0f);

    // Atap segitiga
    glColor3f(0.4f, 0.2f, 0.2f); // coklat tua
    float halfW = 5.0f;
    float halfL = 4.0f;
    float height = 4.0f;

    glBegin(GL_TRIANGLES);
    // sisi depan
    glVertex3f(x - halfW, y + 6.0f, z + halfL);
    glVertex3f(x + halfW, y + 6.0f, z + halfL);
    glVertex3f(x, y + 6.0f + height, z + halfL);
    // sisi belakang
    glVertex3f(x - halfW, y + 6.0f, z - halfL);
    glVertex3f(x + halfW, y + 6.0f, z - halfL);
    glVertex3f(x, y + 6.0f + height, z - halfL);
    glEnd();

    // sisi atas atap (dua sisi miring)
    glBegin(GL_QUADS);
    glVertex3f(x - halfW, y + 6.0f, z + halfL);
    glVertex3f(x - halfW, y + 6.0f, z - halfL);
    glVertex3f(x, y + 6.0f + height, z - halfL);
    glVertex3f(x, y + 6.0f + height, z + halfL);

    glVertex3f(x + halfW, y + 6.0f, z + halfL);
    glVertex3f(x + halfW, y + 6.0f, z - halfL);
    glVertex3f(x, y + 6.0f + height, z - halfL);
    glVertex3f(x, y + 6.0f + height, z + halfL);
    glEnd();
}

void drawRumahBerjejerBelakang() {
    float baseZ = -BUILDING_LENGTH / 2 - 30.5f; // lebih mundur dari kanopi
    drawRumahSederhana(-20.0f, 0.0f, baseZ);
    // drawRumahSederhana(  0.0f, 0.0f, baseZ);
    drawRumahSederhana( 20.0f, 0.0f, baseZ);
}


void drawRoof() {
    GLfloat roof_mat_ambient[] = {0.6f, 0.6f, 0.7f, 1.0f};
    GLfloat roof_mat_diffuse[] = {0.6f, 0.6f, 0.7f, 1.0f};
    GLfloat roof_mat_specular[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat roof_mat_shininess[] = {50.0f};
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, roof_mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, roof_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, roof_mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, roof_mat_shininess);
    glColor3f(0.6f, 0.6f, 0.7f);

    int segments = 50;
    float angleStep = M_PI / segments;
    float roofWidth = BUILDING_WIDTH + ROOF_OVERHANG * 2;
    float roofLength = BUILDING_LENGTH + ROOF_OVERHANG * 2;

    for (int i = 0; i < segments; i++) {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        float x1 = cos(angle1) * (roofWidth / 2);
        float y1 = sin(angle1) * ROOF_HEIGHT + WALL_HEIGHT;

        float x2 = cos(angle2) * (roofWidth / 2);
        float y2 = sin(angle2) * ROOF_HEIGHT + WALL_HEIGHT;

        drawQuad(x1, y1, -roofLength / 2,
                 x2, y2, -roofLength / 2,
                 x2, y2, roofLength / 2,
                 x1, y1, roofLength / 2);

        drawQuad(x1, y1, roofLength / 2,
                 x2, y2, roofLength / 2,
                 x2, y2 - 0.2f, roofLength / 2,
                 x1, y1 - 0.2f, roofLength / 2);

        drawQuad(x1, y1, -roofLength / 2,
                 x2, y2, -roofLength / 2,
                 x2, y2 - 0.2f, -roofLength / 2,
                 x1, y1 - 0.2f, -roofLength / 2);
    }

    int ovalSegments = 100;

    glBegin(GL_POLYGON);
    glColor3f(0.6f, 0.6f, 0.7f);
    for (int i = 0; i <= ovalSegments; i++) {
        float angle = i * M_PI / ovalSegments;
        float x = cos(angle) * (roofWidth / 2);
        float y = sin(angle) * ROOF_HEIGHT + WALL_HEIGHT;
        glVertex3f(x, y, roofLength / 2);
    }
    glVertex3f(roofWidth / 2, WALL_HEIGHT, roofLength / 2);
    glVertex3f(-roofWidth / 2, WALL_HEIGHT, roofLength / 2);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(0.6f, 0.6f, 0.7f);
    for (int i = 0; i <= ovalSegments; i++) {
        float angle = i * M_PI / ovalSegments;
        float x = cos(angle) * (roofWidth / 2);
        float y = sin(angle) * ROOF_HEIGHT + WALL_HEIGHT;
        glVertex3f(x, y, -roofLength / 2);
    }
    glVertex3f(roofWidth / 2, WALL_HEIGHT, -roofLength / 2);
    glVertex3f(-roofWidth / 2, WALL_HEIGHT, -roofLength / 2);
    glEnd();
}

void drawGround() {
    glDisable(GL_LIGHTING);  // Matikan lighting
    glColor3f(0.4f, 0.6f, 0.3f);  // Warna hijau
    drawQuad(-100, -0.5f, -100, 100, -0.5f, -100, 100, -0.5f, 100, -100, -0.5f, 100);
    glEnable(GL_LIGHTING);   // Nyalakan kembali lighting
}

void drawBadmintonCourt() {
    // Ukuran lapangan badminton standar (diperbesar 250%)
    float scale = 2.5f;
    float courtLength = 13.40f * scale;  // Panjang lapangan doubles
    float courtWidth = 6.10f * scale;    // Lebar lapangan doubles
    float singleWidth = 5.18f * scale;   // Lebar lapangan singles
    float lineWidth = 0.1f;              // Lebar garis

    // Posisi tengah lapangan
    float courtX = 0.0f;
    float courtZ = 0.0f;
    float courtY = -0.4f;

    // Lantai keramik cokelat muda
    glColor3f(0.7f, 0.6f, 0.5f);
    drawQuad(courtX - courtWidth / 2, courtY, courtZ - courtLength / 2,
        courtX + courtWidth / 2, courtY, courtZ - courtLength / 2,
        courtX + courtWidth / 2, courtY, courtZ + courtLength / 2,
        courtX - courtWidth / 2, courtY, courtZ + courtLength / 2);

    // Garis marka lapangan (warna putih)
    glColor3f(1.0f, 1.0f, 1.0f);

    // Garis samping untuk doubles
    drawQuad(courtX - courtWidth / 2, courtY + 0.01f, courtZ - courtLength / 2,
        courtX - courtWidth / 2 + lineWidth, courtY + 0.01f, courtZ - courtLength / 2,
        courtX - courtWidth / 2 + lineWidth, courtY + 0.01f, courtZ + courtLength / 2,
        courtX - courtWidth / 2, courtY + 0.01f, courtZ + courtLength / 2);

    drawQuad(courtX + courtWidth / 2 - lineWidth, courtY + 0.01f, courtZ - courtLength / 2,
        courtX + courtWidth / 2, courtY + 0.01f, courtZ - courtLength / 2,
        courtX + courtWidth / 2, courtY + 0.01f, courtZ + courtLength / 2,
        courtX + courtWidth / 2 - lineWidth, courtY + 0.01f, courtZ + courtLength / 2);

    // Garis belakang (untuk doubles)
    drawQuad(courtX - courtWidth / 2, courtY + 0.01f, courtZ - courtLength / 2,
        courtX + courtWidth / 2, courtY + 0.01f, courtZ - courtLength / 2,
        courtX + courtWidth / 2, courtY + 0.01f, courtZ - courtLength / 2 + lineWidth,
        courtX - courtWidth / 2, courtY + 0.01f, courtZ - courtLength / 2 + lineWidth);

    drawQuad(courtX - courtWidth / 2, courtY + 0.01f, courtZ + courtLength / 2 - lineWidth,
        courtX + courtWidth / 2, courtY + 0.01f, courtZ + courtLength / 2 - lineWidth,
        courtX + courtWidth / 2, courtY + 0.01f, courtZ + courtLength / 2,
        courtX - courtWidth / 2, courtY + 0.01f, courtZ + courtLength / 2);

    // Garis samping untuk singles
    float singleLineOuter = (courtWidth - singleWidth) / 2;
    drawQuad(courtX - courtWidth / 2 + singleLineOuter, courtY + 0.02f, courtZ - courtLength / 2,
        courtX - courtWidth / 2 + singleLineOuter + lineWidth, courtY + 0.02f, courtZ - courtLength / 2,
        courtX - courtWidth / 2 + singleLineOuter + lineWidth, courtY + 0.02f, courtZ + courtLength / 2,
        courtX - courtWidth / 2 + singleLineOuter, courtY + 0.02f, courtZ + courtLength / 2);

    drawQuad(courtX + courtWidth / 2 - singleLineOuter - lineWidth, courtY + 0.02f, courtZ - courtLength / 2,
        courtX + courtWidth / 2 - singleLineOuter, courtY + 0.02f, courtZ - courtLength / 2,
        courtX + courtWidth / 2 - singleLineOuter, courtY + 0.02f, courtZ + courtLength / 2,
        courtX + courtWidth / 2 - singleLineOuter - lineWidth, courtY + 0.02f, courtZ + courtLength / 2);

    // Garis servis depan (1.98m dari net)
    float serviceLineShort = 1.98f * scale;
    drawQuad(courtX - courtWidth / 2, courtY + 0.02f, courtZ - serviceLineShort,
        courtX + courtWidth / 2, courtY + 0.02f, courtZ - serviceLineShort,
        courtX + courtWidth / 2, courtY + 0.02f, courtZ - serviceLineShort + lineWidth,
        courtX - courtWidth / 2, courtY + 0.02f, courtZ - serviceLineShort + lineWidth);

    drawQuad(courtX - courtWidth / 2, courtY + 0.02f, courtZ + serviceLineShort - lineWidth,
        courtX + courtWidth / 2, courtY + 0.02f, courtZ + serviceLineShort - lineWidth,
        courtX + courtWidth / 2, courtY + 0.02f, courtZ + serviceLineShort,
        courtX - courtWidth / 2, courtY + 0.02f, courtZ + serviceLineShort);

    // Garis servis panjang untuk doubles (5.18m dari net)
    float serviceLineLong = 5.18f * scale;
    drawQuad(courtX - courtWidth / 2, courtY + 0.02f, courtZ - serviceLineLong,
        courtX + courtWidth / 2, courtY + 0.02f, courtZ - serviceLineLong,
        courtX + courtWidth / 2, courtY + 0.02f, courtZ - serviceLineLong + lineWidth,
        courtX - courtWidth / 2, courtY + 0.02f, courtZ - serviceLineLong + lineWidth);

    drawQuad(courtX - courtWidth / 2, courtY + 0.02f, courtZ + serviceLineLong - lineWidth,
        courtX + courtWidth / 2, courtY + 0.02f, courtZ + serviceLineLong - lineWidth,
        courtX + courtWidth / 2, courtY + 0.02f, courtZ + serviceLineLong,
        courtX - courtWidth / 2, courtY + 0.02f, courtZ + serviceLineLong);

    // Garis tengah (2.59m dari samping)
    float centerLinePos = 2.59f * scale;
    drawQuad(courtX - lineWidth / 2, courtY + 0.02f, courtZ - courtLength / 2,
        courtX + lineWidth / 2, courtY + 0.02f, courtZ - courtLength / 2,
        courtX + lineWidth / 2, courtY + 0.02f, courtZ + courtLength / 2,
        courtX - lineWidth / 2, courtY + 0.02f, courtZ + courtLength / 2);

    // Net badminton yang benar (tidak menyentuh lantai)
    float netHeight = 1.55f; // Tinggi net 1.55m
    float netGap = 0.2f;     // Jarak net dari lantai
    float netThickness = 0.02f;
    float poleHeight = netHeight + netGap + 2.1f; // Tinggi tiang
    float poleRadius = 0.04f; // Diameter tiang ~8cm (lebih kecil)

    // Tiang net (kiri dan kanan)
    glColor3f(0.8f, 0.8f, 0.8f); // Warna silver

    // Tiang kiri
    glPushMatrix();
    glTranslatef(-courtWidth / 2, courtY + netGap, courtZ);
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    GLUquadric* pole = gluNewQuadric();
    gluCylinder(pole, poleRadius, poleRadius, poleHeight, 8, 1); // 8 sisi saja
    glPopMatrix();

    // Tiang kanan
    glPushMatrix();
    glTranslatef(courtWidth / 2, courtY + netGap, courtZ);
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    gluCylinder(pole, poleRadius, poleRadius, poleHeight, 8, 1);
    glPopMatrix();
    gluDeleteQuadric(pole);

    // Jaring net badminton (tidak sampai lantai)
    glColor3f(0.95f, 0.95f, 0.95f); // Warna putih
    float netTop = courtY + netGap + netHeight;
    float netBottom = courtY + poleHeight + netGap;

    // Grid vertikal net (setiap 20cm)
    for (float x = -courtWidth / 2; x <= courtWidth / 2; x += 0.2f) {
        drawQuad(x, netTop, courtZ - netThickness / 2,
            x, netBottom, courtZ - netThickness / 2,
            x, netBottom, courtZ + netThickness / 2,
            x, netTop, courtZ + netThickness / 2);
    }

    // Grid horizontal net (setiap 15cm)
    for (float y = netBottom; y <= netTop; y += 0.15f) {
        drawQuad(-courtWidth / 2, y, courtZ - netThickness / 2,
            courtWidth / 2, y, courtZ - netThickness / 2,
            courtWidth / 2, y, courtZ + netThickness / 2,
            -courtWidth / 2, y, courtZ + netThickness / 2);
    }

    // Pita atas net (lebih tebal)
    glColor3f(0.9f, 0.9f, 0.7f); // Warna putih kekuningan
    drawQuad(-courtWidth / 2, netTop, courtZ - 0.05f,
        courtWidth / 2, netTop, courtZ - 0.05f,
        courtWidth / 2, netTop, courtZ + 0.05f,
        -courtWidth / 2, netTop, courtZ + 0.05f);
}

void drawLights() {
    // Nonaktifkan lighting sementara untuk menggambar visual lampu
    glDisable(GL_LIGHTING);
    glColor3f(0.9f, 0.9f, 0.8f); // Warna visual lampu

    // Gambar visual lampu kecil (hanya bentuknya, tanpa efek lighting)
    for (int i = 0; i < 4; i++) {
        float zPos = -BUILDING_LENGTH / 2 + (i + 1) * (BUILDING_LENGTH / 5);

        // Lampu kiri
        glPushMatrix();
        glTranslatef(-BUILDING_WIDTH / 2 + 3.0f, WALL_HEIGHT * 0.7f, zPos);
        glutSolidSphere(0.15f, 10, 10);
        glPopMatrix();

        // Lampu kanan
        glPushMatrix();
        glTranslatef(BUILDING_WIDTH / 2 - 3.0f, WALL_HEIGHT * 0.7f, zPos);
        glutSolidSphere(0.15f, 10, 10);
        glPopMatrix();
    }
    glEnable(GL_LIGHTING);

    // Parameter cahaya yang lebih natural
    GLfloat light_ambient[] = { 0.05f, 0.05f, 0.05f, 1.0f };
    GLfloat light_diffuse[] = { 0.4f, 0.4f, 0.35f, 1.0f };  // Lebih redup
    GLfloat light_specular[] = { 0.3f, 0.3f, 0.3f, 1.0f };  // Specular lebih redup

    // Attenuation untuk mengurangi intensitas
    float constant_atten = 1.0f;
    float linear_atten = 0.1f;    // Lebih besar untuk reduksi lebih cepat
    float quadratic_atten = 0.02f;

    // Lampu sisi kiri (4 buah) - intensitas dikurangi
    for (int i = 0; i < 4; i++) {
        GLenum light_id = GL_LIGHT1 + i;
        float zPos = -BUILDING_LENGTH / 2 + (i + 1) * (BUILDING_LENGTH / 5);
        GLfloat light_position[] = { -BUILDING_WIDTH / 2 + 3.0f, WALL_HEIGHT * 0.7f, zPos, 1.0f };

        glEnable(light_id);
        glLightfv(light_id, GL_POSITION, light_position);
        glLightfv(light_id, GL_AMBIENT, light_ambient);
        glLightfv(light_id, GL_DIFFUSE, light_diffuse);
        glLightfv(light_id, GL_SPECULAR, light_specular);
        glLightf(light_id, GL_CONSTANT_ATTENUATION, constant_atten);
        glLightf(light_id, GL_LINEAR_ATTENUATION, linear_atten);
        glLightf(light_id, GL_QUADRATIC_ATTENUATION, quadratic_atten);
    }

    // Lampu sisi kanan (4 buah) - intensitas dikurangi
    for (int i = 0; i < 4; i++) {
        GLenum light_id = GL_LIGHT5 + i;
        float zPos = -BUILDING_LENGTH / 2 + (i + 1) * (BUILDING_LENGTH / 5);
        GLfloat light_position[] = { BUILDING_WIDTH / 2 - 3.0f, WALL_HEIGHT * 0.7f, zPos, 1.0f };

        glEnable(light_id);
        glLightfv(light_id, GL_POSITION, light_position);
        glLightfv(light_id, GL_AMBIENT, light_ambient);
        glLightfv(light_id, GL_DIFFUSE, light_diffuse);
        glLightfv(light_id, GL_SPECULAR, light_specular);
        glLightf(light_id, GL_CONSTANT_ATTENUATION, constant_atten);
        glLightf(light_id, GL_LINEAR_ATTENUATION, linear_atten);
        glLightf(light_id, GL_QUADRATIC_ATTENUATION, quadratic_atten);
    }
}


// gambar rumput
void drawGrassPatch(float x, float z) {
    glPushMatrix();
    glTranslatef(x, -0.48f, z); // letakkan di permukaan tanah
    glColor3f(0.2f, 0.8f, 0.2f); // hijau terang

    for (int i = 0; i < 5; ++i) {
        float angle = i * 72.0f; // 5 helai, menyebar
        float height = 1.5f + 0.5f * (rand() % 100) / 100.0f; // tinggi 1.5 - 2.0

        glPushMatrix();
        glRotatef(angle, 0, 1, 0);
        glBegin(GL_TRIANGLES);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.1f, 0.0f, 0.0f);
        glVertex3f(0.05f, height, 0.0f);
        glEnd();
        glPopMatrix();
    }

    glPopMatrix();
}

// Fungsi untuk menggambar beberapa patch rumput di halaman depan
void drawGrassPatches() {
    // Area halaman luas (area pohon acak)
    float areaMinX = -100.0f, areaMaxX = 100.0f;
    float areaMinZ = -80.0f, areaMaxZ = 100.0f;

    // Area gedung dan jalan untuk dihindari
    float gedungMinX = -50.0f, gedungMaxX = 50.0f;
    float gedungMinZ = -60.0f, gedungMaxZ = 60.0f;
    float minDistGedung = 12.0f;

    float jalanMinX = -100.0f;
    float jalanMaxX = BUILDING_WIDTH - 2;
    float jalanMinZ = BUILDING_LENGTH / 2 + 15.0f;
    float jalanMaxZ = jalanMinZ + 20.0f;

    srand(5678); // Seed tetap agar posisi konsisten
    int count = 0, maxTry = 1000;
    int grassCount = 500; // jumlah patch rumput

    while (count < grassCount && maxTry > 0) {
        float x = areaMinX + static_cast<float>(rand()) / RAND_MAX * (areaMaxX - areaMinX);
        float z = areaMinZ + static_cast<float>(rand()) / RAND_MAX * (areaMaxZ - areaMinZ);

        // Hindari area gedung + jarak aman
        if (x > gedungMinX - minDistGedung && x < gedungMaxX + minDistGedung &&
            z > gedungMinZ - minDistGedung && z < gedungMaxZ + minDistGedung)
        { maxTry--; continue; }

        // Hindari area jalan
        if (x > jalanMinX && x < jalanMaxX && z > jalanMinZ && z < jalanMaxZ)
        { maxTry--; continue; }

        drawGrassPatch(x, z);
        count++;
        maxTry--;
    }
}

void drawPanggungIndoor() {
    float halfL = BUILDING_LENGTH / 2;
    float halfW = BUILDING_WIDTH / 2;

    // Parameter panggung INDOOR (revisi)
    float panggungPanjang = BUILDING_WIDTH;  // 100% lebar gedung 
    float panggungLebar = 6.0f;                    // Lebar cukup untuk panggung
    float panggungTinggi = 2.0f;                   // Tinggi 2 meter dari lantai (revisi)
    float panggungZ = -halfL + 10.0f;              // 10m dari dinding belakang

    // Warna panggung (kayu lapisan teak lebih gelap)
    glColor3f(0.5f, 0.4f, 0.25f);

    // Alas panggung (segiempat besar)
    drawQuad(-panggungPanjang / 2, 0, panggungZ,
        panggungPanjang / 2, 0, panggungZ,
        panggungPanjang / 2, 0, panggungZ - panggungLebar,
        -panggungPanjang / 2, 0, panggungZ - panggungLebar);

    // Sisi vertikal panggung (semua sisi)
    for (int i = 0; i < 4; i++) {
        float x1, z1, x2, z2;

        if (i == 0) { // Depan
            x1 = -panggungPanjang / 2; z1 = panggungZ;
            x2 = panggungPanjang / 2;  z2 = panggungZ;
        }
        else if (i == 1) { // Belakang
            x1 = -panggungPanjang / 2; z1 = panggungZ - panggungLebar;
            x2 = panggungPanjang / 2;  z2 = panggungZ - panggungLebar;
        }
        else if (i == 2) { // Kiri
            x1 = -panggungPanjang / 2; z1 = panggungZ;
            x2 = -panggungPanjang / 2; z2 = panggungZ - panggungLebar;
        }
        else { // Kanan
            x1 = panggungPanjang / 2; z1 = panggungZ;
            x2 = panggungPanjang / 2; z2 = panggungZ - panggungLebar;
        }

        drawQuad(x1, 0, z1,
            x2, 0, z2,
            x2, panggungTinggi, z2,
            x1, panggungTinggi, z1);
    }

    // Dek panggung (lantai atas)
    drawQuad(-panggungPanjang / 2, panggungTinggi, panggungZ,
        panggungPanjang / 2, panggungTinggi, panggungZ,
        panggungPanjang / 2, panggungTinggi, panggungZ - panggungLebar,
        -panggungPanjang / 2, panggungTinggi, panggungZ - panggungLebar);

    // Pilar penyangga struktur atap (revisi lebih besar)
    float pilarTinggi = 7.0f;       // Tinggi total 5m (2m panggung + 3m pilar)
    float pilarLebar = 0.5f;        // Lebar pilar diperbesar
    float jarakPilar = panggungPanjang * 0.6f; // Jarak disesuaikan

    // Warna pilar (beton ekspos lebih gelap)
    glColor3f(0.7f, 0.7f, 0.7f);

    // Pilar kiri dan kanan
    for (int i = 0; i < 2; i++) {
        float xPos = (i == 0) ? -jarakPilar / 2 : jarakPilar / 2;

        // Badan pilar
        drawQuad(xPos - pilarLebar / 2, panggungTinggi, panggungZ - panggungLebar / 2,
            xPos + pilarLebar / 2, panggungTinggi, panggungZ - panggungLebar / 2,
            xPos + pilarLebar / 2, panggungTinggi + pilarTinggi, panggungZ - panggungLebar / 2,
            xPos - pilarLebar / 2, panggungTinggi + pilarTinggi, panggungZ - panggungLebar / 2);

        // Penopang tambahan (cross beam)
        if (i == 0) { // Hanya gambar untuk pilar kiri sebagai contoh
            glColor3f(0.6f, 0.6f, 0.6f);
            drawQuad(xPos - pilarLebar / 2, panggungTinggi + 1.0f, panggungZ - panggungLebar / 2,
                xPos - pilarLebar / 2, panggungTinggi + 1.5f, panggungZ - panggungLebar / 2 - 1.0f,
                xPos - pilarLebar / 2, panggungTinggi + 1.5f, panggungZ - panggungLebar / 2 - 1.5f,
                xPos - pilarLebar / 2, panggungTinggi + 1.0f, panggungZ - panggungLebar / 2 - 0.5f);
        }
    }

    // Plat penyangga atap (revisi lebih besar)
    float platPanjang = panggungPanjang * 0.8f;
    float platLebar = 4.0f;
    float platTebal = 0.4f;

    glColor3f(0.65f, 0.65f, 0.7f); // Warna beton lebih gelap

    // Plat atas (horizontal)
    drawQuad(-platPanjang / 2, panggungTinggi + pilarTinggi, panggungZ - panggungLebar / 2,
        platPanjang / 2, panggungTinggi + pilarTinggi, panggungZ - panggungLebar / 2,
        platPanjang / 2, panggungTinggi + pilarTinggi, panggungZ - panggungLebar / 2 - platLebar,
        -platPanjang / 2, panggungTinggi + pilarTinggi, panggungZ - panggungLebar / 2 - platLebar);

    // Sisi-sisi plat
    for (int i = 0; i < 4; i++) {
        float x1, z1, x2, z2, y2;

        if (i == 0) { // Depan
            x1 = -platPanjang / 2; z1 = panggungZ - panggungLebar / 2;
            x2 = platPanjang / 2;  z2 = panggungZ - panggungLebar / 2;
            y2 = panggungTinggi + pilarTinggi - platTebal;
        }
        else if (i == 1) { // Belakang
            x1 = -platPanjang / 2; z1 = panggungZ - panggungLebar / 2 - platLebar;
            x2 = platPanjang / 2;  z2 = panggungZ - panggungLebar / 2 - platLebar;
            y2 = panggungTinggi + pilarTinggi - platTebal;
        }
        else if (i == 2) { // Kiri
            x1 = -platPanjang / 2; z1 = panggungZ - panggungLebar / 2;
            x2 = -platPanjang / 2; z2 = panggungZ - panggungLebar / 2 - platLebar;
            y2 = panggungTinggi + pilarTinggi - platTebal;
        }
        else { // Kanan
            x1 = platPanjang / 2; z1 = panggungZ - panggungLebar / 2;
            x2 = platPanjang / 2; z2 = panggungZ - panggungLebar / 2 - platLebar;
            y2 = panggungTinggi + pilarTinggi - platTebal;
        }

        drawQuad(x1, panggungTinggi + pilarTinggi, z1,
            x2, panggungTinggi + pilarTinggi, z2,
            x2, y2, z2,
            x1, y2, z1);
    }
}

void drawAwan(float x, float y, float z) {
    glColor3f(1.0f, 1.0f, 1.0f); // Putih

    glPushMatrix();
    glTranslatef(x, y, z);
    glutSolidSphere(2.5f, 20, 20);
    glTranslatef(2.0f, 0.5f, 0.5f);
    glutSolidSphere(2.0f, 20, 20);
    glTranslatef(-4.0f, 0.0f, 0.5f);
    glutSolidSphere(1.8f, 20, 20);
    glPopMatrix();
}

void panggilAwan(){
    drawAwan(-50.0f, 38.0f, -40.0f);
drawAwan(45.0f, 42.0f, 30.0f);
drawAwan(-35.0f, 36.5f, 25.0f);
drawAwan(30.0f, 39.0f, -20.0f);
drawAwan(-15.0f, 41.0f, 50.0f);

drawAwan(20.0f, 37.0f, -60.0f);
drawAwan(-30.0f, 36.5f, -10.0f);
drawAwan(5.0f, 38.0f, 10.0f);
drawAwan(-5.0f, 39.5f, 30.0f);
drawAwan(40.0f, 37.8f, -15.0f);

drawAwan(-45.0f, 35.0f, 20.0f);
drawAwan(10.0f, 40.0f, 40.0f);
drawAwan(-20.0f, 39.5f, -35.0f);
drawAwan(0.0f, 41.0f, 0.0f);
drawAwan(25.0f, 38.5f, -45.0f);

drawAwan(-10.0f, 36.5f, 15.0f);
drawAwan(15.0f, 37.8f, -25.0f);
drawAwan(-38.0f, 39.2f, 35.0f);
drawAwan(35.0f, 37.5f, 5.0f);
drawAwan(-22.0f, 38.0f, -5.0f);

drawAwan(28.0f, 39.3f, -32.0f);
drawAwan(-17.0f, 36.0f, 45.0f);
drawAwan(8.0f, 40.5f, 20.0f);
drawAwan(-12.0f, 38.5f, -50.0f);
drawAwan(0.0f, 36.8f, 60.0f);


}

//burung
void drawBurung(float x, float y, float z, float scale) {
    glColor3f(0.1f, 0.1f, 0.1f); // Hitam/siluet

    glBegin(GL_TRIANGLES);
    // Sayap kiri
    glVertex3f(x, y, z);
    glVertex3f(x - scale, y - scale / 2, z);
    glVertex3f(x - scale / 2, y, z);

    // Sayap kanan
    glVertex3f(x, y, z);
    glVertex3f(x + scale, y - scale / 2, z);
    glVertex3f(x + scale / 2, y, z);
    glEnd();
}

void panggilBurung(){
    drawBurung(-35.0f, 41.5f, -45.0f, 2.5f);
    drawBurung(25.0f, 43.0f, -30.0f, 2.0f);
    drawBurung(-20.0f, 42.5f, 35.0f, 3.0f);
    drawBurung(15.0f, 44.0f, 25.0f, 2.7f);
    drawBurung(0.0f, 45.5f, 0.0f, 3.0f);

    drawBurung(-28.0f, 43.8f, 15.0f, 2.2f);
    drawBurung(30.0f, 42.0f, 10.0f, 2.5f);
    drawBurung(-10.0f, 41.0f, -25.0f, 3.2f);
    drawBurung(5.0f, 43.3f, -15.0f, 2.8f);
    drawBurung(-15.0f, 44.5f, 5.0f, 2.0f);

}



void drawBuilding() {
    drawGround();
    drawGrassPatches();
    drawRandomTrees();  
    drawJalan();
    drawAlas();
    drawWalls();
    drawLahanSampingKiri();
    drawRumahSampingKiri();
    drawKanopi();
    drawKanopiBelakang();
    drawJalanDepanKanopiBelakang(15.0f,150.0f);
    drawJalanVertikalBawah(40.0f, 15.0f);
    // drawJalanVertikalMiring(60.0f, 15.0f, 15.0f);
    drawRumahBerjejerBelakang();
    drawTembokPendekDanTiangKanopi(); 
    drawTembokSampingPendek();
    drawRoof();
    drawBangunanSamping();  
    drawBangunanDepan();
    drawLights();
    drawBadmintonCourt();
    drawPanggungIndoor();
    // awan
    panggilAwan();
    //burung
    panggilBurung();

}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0, 0, translateZ);
    glRotatef(rotateX, 1, 0, 0);
    glRotatef(rotateY, 0, 1, 0);
    drawBuilding();
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / h, 1.0, 500.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int, int) {
    if (key == 27) exit(0);
    if (key == 'w') translateZ += 2.0f;
    if (key == 's') translateZ -= 2.0f;
    if (key == 'a') rotateY -= 5.0f;
    if (key == 'd') rotateY += 5.0f;
    if (key == 'q') rotateX -= 5.0f;
    if (key == 'e') rotateX += 5.0f;
    if (key == 'r') { rotateX = rotateY = 0; translateZ = -50; }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        mousePressed = (state == GLUT_DOWN);
        lastMouseX = x; lastMouseY = y;
    }
}

void mouseMotion(int x, int y) {
    if (mousePressed) {
        rotateY += (x - lastMouseX) * 0.5f;
        rotateX += (y - lastMouseY) * 0.5f;
        lastMouseX = x; lastMouseY = y;
        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("TUGAS RANCANG GRAFIKA KOMPUTER");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);

    glutMainLoop();
    return 0;
}
