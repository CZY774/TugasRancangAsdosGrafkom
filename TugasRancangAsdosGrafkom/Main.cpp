#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <cmath>
#include <iostream>
#include <cstdlib>  // untuk rand() dan srand()
#include <ctime>    // untuk time()

#define BUILDING_WIDTH 40.0f
#define BUILDING_LENGTH 60.0f
#define WALL_HEIGHT 15.0f
#define ROOF_HEIGHT 12.0f
#define ROOF_OVERHANG 2.0f

// Window constants
#define WINDOW_WIDTH 4.0f
#define WINDOW_HEIGHT 6.0f
#define WINDOW_FRAME 0.2f

float rotateX = 0.0f, rotateY = 0.0f, translateZ = -50.0f;
int lastMouseX, lastMouseY;
bool mousePressed = false;

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    GLfloat light_position[] = {20.0f, 30.0f, 20.0f, 1.0f};
    GLfloat light_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat light_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
}

void drawQuad(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float x4, float y4, float z4) {
    glBegin(GL_QUADS);
    glVertex3f(x1, y1, z1); glVertex3f(x2, y2, z2);
    glVertex3f(x3, y3, z3); glVertex3f(x4, y4, z4);
    glEnd();
}

void drawWindow(float centerX, float centerY, float z, bool vertical = true) {
    float halfWidth = WINDOW_WIDTH/2;
    float halfHeight = WINDOW_HEIGHT/2;
    
    // Window glass (light blue)
    glColor3f(0.7f, 0.8f, 0.9f);
    drawQuad(centerX - halfWidth + WINDOW_FRAME, centerY - halfHeight + WINDOW_FRAME, z,
             centerX + halfWidth - WINDOW_FRAME, centerY - halfHeight + WINDOW_FRAME, z,
             centerX + halfWidth - WINDOW_FRAME, centerY + halfHeight - WINDOW_FRAME, z,
             centerX - halfWidth + WINDOW_FRAME, centerY + halfHeight - WINDOW_FRAME, z);
    
    // Window frame (dark gray)
    glColor3f(0.3f, 0.3f, 0.3f);
    
    // Outer frame
    drawQuad(centerX - halfWidth, centerY - halfHeight, z,
             centerX + halfWidth, centerY - halfHeight, z,
             centerX + halfWidth, centerY + halfHeight, z,
             centerX - halfWidth, centerY + halfHeight, z);
    
    // Inner frame (cutout)
    glColor3f(0.96f, 0.93f, 0.82f); // wall color
    drawQuad(centerX - halfWidth + WINDOW_FRAME, centerY - halfHeight + WINDOW_FRAME, z,
             centerX + halfWidth - WINDOW_FRAME, centerY - halfHeight + WINDOW_FRAME, z,
             centerX + halfWidth - WINDOW_FRAME, centerY + halfHeight - WINDOW_FRAME, z,
             centerX - halfWidth + WINDOW_FRAME, centerY + halfHeight - WINDOW_FRAME, z);
    
    // Cross bars (vertical window)
    if (vertical) {
        // Vertical divider
        glColor3f(0.3f, 0.3f, 0.3f);
        drawQuad(centerX - 0.1f, centerY - halfHeight + WINDOW_FRAME, z,
                 centerX + 0.1f, centerY - halfHeight + WINDOW_FRAME, z,
                 centerX + 0.1f, centerY + halfHeight - WINDOW_FRAME, z,
                 centerX - 0.1f, centerY + halfHeight - WINDOW_FRAME, z);
        
        // Horizontal dividers (2 bars)
        for (int i = -1; i <= 1; i += 2) {
            float y = centerY + i * (halfHeight - WINDOW_FRAME)/2;
            drawQuad(centerX - halfWidth + WINDOW_FRAME, y - 0.1f, z,
                     centerX + halfWidth - WINDOW_FRAME, y - 0.1f, z,
                     centerX + halfWidth - WINDOW_FRAME, y + 0.1f, z,
                     centerX - halfWidth + WINDOW_FRAME, y + 0.1f, z);
        }
    } else {
        // Horizontal window dividers would go here
    }
}

void drawDoors() {
    float doorWidth = 4.0f;
    float doorHeight = 10.0f;
    float doorGap = 0.15f; // celah di tengah antar daun pintu
    float halfW = BUILDING_WIDTH / 2;
    float z = BUILDING_LENGTH / 2 + 0.01f; // sedikit keluar agar tidak z-fighting

    glColor3f(0.6f, 0.6f, 0.7f); // warna abu seperti atap
    float offsetX = 7.0f; // geser kanan
    // Pintu kiri
    drawQuad(
        -doorGap/2 + offsetX, 0, z,
        -doorGap/2 - doorWidth + offsetX, 0, z,
        -doorGap/2 - doorWidth + offsetX, doorHeight, z,
        -doorGap/2 + offsetX, doorHeight, z
    );
    // Pintu kanan
    drawQuad(
        doorGap/2 + offsetX, 0, z,
        doorGap/2 + doorWidth + offsetX, 0, z,
        doorGap/2 + doorWidth + offsetX, doorHeight, z,
        doorGap/2 + offsetX, doorHeight, z
    );
}

void colorWalls(){
    glColor3f(0.96f, 0.93f, 0.82f);
}

void drawWalls() {
    float halfW = BUILDING_WIDTH / 2;
    float halfL = BUILDING_LENGTH / 2;

    // Dinding depan
    colorWalls();
    drawQuad(-halfW, 0, halfL,  halfW, 0, halfL,  halfW, WALL_HEIGHT, halfL, -halfW, WALL_HEIGHT, halfL);

    // Dinding belakang
    colorWalls();
    drawQuad(-halfW, 0, -halfL, halfW, 0, -halfL, halfW, WALL_HEIGHT, -halfL, -halfW, WALL_HEIGHT, -halfL);
    
    // ==== JENDELA BELAKANG ====
    float windowSpacing = 1.5f;
    float windowBottom = 3.0f;
    float zBack = -halfL - 0.01f;
    
    // Calculate how many windows fit
    int windowsPerRow = 4;
    float totalWidth = windowsPerRow * WINDOW_WIDTH + (windowsPerRow - 1) * windowSpacing;
    float startX = -totalWidth / 2.0f;
    
    for (int i = 0; i < windowsPerRow; i++) {
        float x = startX + i * (WINDOW_WIDTH + windowSpacing) + WINDOW_WIDTH/2;
        drawWindow(x, windowBottom + WINDOW_HEIGHT/2, zBack);
    }

    // Dinding kiri
    colorWalls();
    drawQuad(-halfW, 0, -halfL, -halfW, 0, halfL, -halfW, WALL_HEIGHT, halfL, -halfW, WALL_HEIGHT, -halfL);

    // Dinding kanan
    colorWalls();
    drawQuad(halfW, 0, -halfL,  halfW, 0, halfL,  halfW, WALL_HEIGHT, halfL,  halfW, WALL_HEIGHT, -halfL);

    // Pintu (di depan)
    drawDoors();
}

// [Rest of the code remains the same until drawBangunanDepan]

void drawBangunanDepan() {
    float boxWidth = BUILDING_WIDTH / 3.0f;
    float boxLength = 16.0f;
    float boxHeight = 12.4f;
    float halfBoxW = boxWidth / 2;
    float halfBoxL = boxLength / 2;

    // Digeser lebih kiri dari tengah (pepet ke kiri bangunan besar)
    float offsetX = -halfBoxW - 6.5f;
    float offsetZ = BUILDING_LENGTH / 2 + halfBoxL + 0.01f;

    // Warna dinding
    glColor3f(0.96f, 0.93f, 0.82f);
    
    // Dinding depan dengan jendela
    float windowY = 3.0f + WINDOW_HEIGHT/2; // center of window
    
    // Gambar dinding depan kiri
    drawQuad(offsetX - halfBoxW, 0, offsetZ + halfBoxL,
             offsetX - 1.5f, 0, offsetZ + halfBoxL,
             offsetX - 1.5f, boxHeight, offsetZ + halfBoxL,
             offsetX - halfBoxW, boxHeight, offsetZ + halfBoxL);
    
    // Gambar dinding depan kanan
    drawQuad(offsetX + 1.5f, 0, offsetZ + halfBoxL,
             offsetX + halfBoxW, 0, offsetZ + halfBoxL,
             offsetX + halfBoxW, boxHeight, offsetZ + halfBoxL,
             offsetX + 1.5f, boxHeight, offsetZ + halfBoxL);
    
    // Gambar dinding depan atas (di atas jendela)
    drawQuad(offsetX - 1.5f, windowY + WINDOW_HEIGHT/2, offsetZ + halfBoxL,
             offsetX + 1.5f, windowY + WINDOW_HEIGHT/2, offsetZ + halfBoxL,
             offsetX + 1.5f, boxHeight, offsetZ + halfBoxL,
             offsetX - 1.5f, boxHeight, offsetZ + halfBoxL);
    
    // Gambar dinding depan bawah (di bawah jendela)
    drawQuad(offsetX - 1.5f, 0, offsetZ + halfBoxL,
             offsetX + 1.5f, 0, offsetZ + halfBoxL,
             offsetX + 1.5f, windowY - WINDOW_HEIGHT/2, offsetZ + halfBoxL,
             offsetX - 1.5f, windowY - WINDOW_HEIGHT/2, offsetZ + halfBoxL);

    // Gambar jendela utama (lebih besar, di tengah)
    drawWindow(offsetX, windowY, offsetZ + halfBoxL + 0.01f);

    // Kembali ke warna dinding untuk sisi lainnya
    glColor3f(0.96f, 0.93f, 0.82f);
    
    // Dinding belakang
    drawQuad(offsetX - halfBoxW, 0, offsetZ - halfBoxL,
             offsetX + halfBoxW, 0, offsetZ - halfBoxL,
             offsetX + halfBoxW, boxHeight, offsetZ - halfBoxL,
             offsetX - halfBoxW, boxHeight, offsetZ - halfBoxL);

    // Dinding kiri
    drawQuad(offsetX - halfBoxW, 0, offsetZ - halfBoxL,
             offsetX - halfBoxW, 0, offsetZ + halfBoxL,
             offsetX - halfBoxW, boxHeight, offsetZ + halfBoxL,
             offsetX - halfBoxW, boxHeight, offsetZ - halfBoxL);

    // Dinding kanan
    drawQuad(offsetX + halfBoxW, 0, offsetZ - halfBoxL,
             offsetX + halfBoxW, 0, offsetZ + halfBoxL,
             offsetX + halfBoxW, boxHeight, offsetZ + halfBoxL,
             offsetX + halfBoxW, boxHeight, offsetZ - halfBoxL);

    // Atap
    glColor3f(0.6f, 0.6f, 0.7f);
    drawQuad(offsetX - halfBoxW, boxHeight, offsetZ - halfBoxL,
             offsetX + halfBoxW, boxHeight, offsetZ - halfBoxL,
             offsetX + halfBoxW, boxHeight, offsetZ + halfBoxL,
             offsetX - halfBoxW, boxHeight, offsetZ + halfBoxL);
}

void drawBangunanSamping() {
    // Ukuran bangunan kotak
    float boxWidth = 10.0f;
    float boxLength = 14.0f;
    float boxHeight = WALL_HEIGHT * 0.6f;
    float halfBoxW = boxWidth / 2;
    float halfBoxL = boxLength / 2;
    float jarakAntarBangunan = 8.0f; 

    // Posisi: sebelah kiri pintu, agak mundur ke belakang
    float offsetX = BUILDING_WIDTH / 2 + halfBoxW + jarakAntarBangunan;
    float offsetZ = 8.0f; // agak masuk ke dalam

    // Draw windows on front side
    float windowY = 3.0f + WINDOW_HEIGHT/2;
    drawWindow(offsetX - 2.5f, windowY, offsetZ + halfBoxL + 0.01f);
    drawWindow(offsetX + 2.5f, windowY, offsetZ + halfBoxL + 0.01f);

    // Dinding (warna sama)
    glColor3f(0.96f, 0.93f, 0.82f);
    // [Rest of the wall drawing code remains the same]
}

// [Rest of the original code remains unchanged]