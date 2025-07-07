// #include <GL/freeglut.h>
// #include <GL/gl.h>
// #include <GL/glu.h>
// #include <cmath>
// #include <iostream>

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <cmath>
#include <iostream>
// #include <GL/glut.h>
#include <math.h>
#include <stdio.h>

// Variabel untuk rotasi kamera
float rotateX = 0.0f, rotateY = 0.0f;
float translateZ = -50.0f;
int lastMouseX, lastMouseY;
bool mousePressed = false;

// Konstanta untuk dimensi bangunan
#define BUILDING_WIDTH 40.0f
#define BUILDING_LENGTH 60.0f
#define WALL_HEIGHT 15.0f
#define ROOF_HEIGHT 8.0f

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    
    // Pengaturan cahaya
    GLfloat light_position[] = {20.0f, 30.0f, 20.0f, 1.0f};
    GLfloat light_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat light_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f); // Warna langit biru
}

void drawQuad(float x1, float y1, float z1, float x2, float y2, float z2, 
              float x3, float y3, float z3, float x4, float y4, float z4) {
    glBegin(GL_QUADS);
        glVertex3f(x1, y1, z1);
        glVertex3f(x2, y2, z2);
        glVertex3f(x3, y3, z3);
        glVertex3f(x4, y4, z4);
    glEnd();
}

void drawRect(float x, float y, float z, float width, float height) {
    glBegin(GL_QUADS);
        glVertex3f(x, y, z);
        glVertex3f(x + width, y, z);
        glVertex3f(x + width, y + height, z);
        glVertex3f(x, y + height, z);
    glEnd();
}

void drawWindow(float x, float y, float z, float width, float height) {
    // Frame jendela (putih)
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
        glVertex3f(x, y, z + 0.1f);
        glVertex3f(x + width, y, z + 0.1f);
        glVertex3f(x + width, y + height, z + 0.1f);
        glVertex3f(x, y + height, z + 0.1f);
    glEnd();
    
    // Kaca jendela (biru muda transparan)
    glColor3f(0.7f, 0.9f, 1.0f);
    glBegin(GL_QUADS);
        glVertex3f(x + 0.5f, y + 0.5f, z + 0.2f);
        glVertex3f(x + width - 0.5f, y + 0.5f, z + 0.2f);
        glVertex3f(x + width - 0.5f, y + height - 0.5f, z + 0.2f);
        glVertex3f(x + 0.5f, y + height - 0.5f, z + 0.2f);
    glEnd();
}

void drawRoof() {
    // Atap melengkung dengan material seng
    glColor3f(0.6f, 0.6f, 0.7f); // Warna seng
    
    int segments = 20;
    float angleStep = M_PI / segments;
    
    for (int i = 0; i < segments; i++) {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;
        
        float x1 = cos(angle1) * ROOF_HEIGHT;
        float y1 = sin(angle1) * ROOF_HEIGHT + WALL_HEIGHT;
        float x2 = cos(angle2) * ROOF_HEIGHT;
        float y2 = sin(angle2) * ROOF_HEIGHT + WALL_HEIGHT;
        
        // Segmen atap
        glBegin(GL_QUADS);
            glVertex3f(x1, y1, -BUILDING_LENGTH/2);
            glVertex3f(x2, y2, -BUILDING_LENGTH/2);
            glVertex3f(x2, y2, BUILDING_LENGTH/2);
            glVertex3f(x1, y1, BUILDING_LENGTH/2);
        glEnd();
        
        // Garis-garis seng
        glColor3f(0.5f, 0.5f, 0.6f);
        glBegin(GL_LINES);
            glVertex3f(x1, y1, -BUILDING_LENGTH/2);
            glVertex3f(x1, y1, BUILDING_LENGTH/2);
        glEnd();
        glColor3f(0.6f, 0.6f, 0.7f);
    }
}

void drawFloor() {
    // Lantai kayu parket
    glColor3f(0.8f, 0.6f, 0.4f);
    glBegin(GL_QUADS);
        glVertex3f(-BUILDING_WIDTH/2, 0, -BUILDING_LENGTH/2);
        glVertex3f(BUILDING_WIDTH/2, 0, -BUILDING_LENGTH/2);
        glVertex3f(BUILDING_WIDTH/2, 0, BUILDING_LENGTH/2);
        glVertex3f(-BUILDING_WIDTH/2, 0, BUILDING_LENGTH/2);
    glEnd();
    
    // Garis lapangan badminton
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    
    // Garis tengah
    glBegin(GL_LINES);
        glVertex3f(0, 0.1f, -BUILDING_LENGTH/2 + 5);
        glVertex3f(0, 0.1f, BUILDING_LENGTH/2 - 5);
    glEnd();
    
    // Garis lapangan
    float courtWidth = 20.0f;
    float courtLength = 40.0f;
    
    glBegin(GL_LINE_LOOP);
        glVertex3f(-courtWidth/2, 0.1f, -courtLength/2);
        glVertex3f(courtWidth/2, 0.1f, -courtLength/2);
        glVertex3f(courtWidth/2, 0.1f, courtLength/2);
        glVertex3f(-courtWidth/2, 0.1f, courtLength/2);
    glEnd();
    
    // Garis servis
    glBegin(GL_LINES);
        glVertex3f(-courtWidth/2, 0.1f, -courtLength/4);
        glVertex3f(courtWidth/2, 0.1f, -courtLength/4);
        glVertex3f(-courtWidth/2, 0.1f, courtLength/4);
        glVertex3f(courtWidth/2, 0.1f, courtLength/4);
    glEnd();
}

void drawWalls() {
    // Dinding depan
    glColor3f(0.9f, 0.9f, 0.9f); // Putih
    drawQuad(-BUILDING_WIDTH/2, 0, BUILDING_LENGTH/2,
             BUILDING_WIDTH/2, 0, BUILDING_LENGTH/2,
             BUILDING_WIDTH/2, WALL_HEIGHT/2, BUILDING_LENGTH/2,
             -BUILDING_WIDTH/2, WALL_HEIGHT/2, BUILDING_LENGTH/2);
    
    // Bagian atas dinding depan (merah)
    glColor3f(0.8f, 0.4f, 0.3f);
    drawQuad(-BUILDING_WIDTH/2, WALL_HEIGHT/2, BUILDING_LENGTH/2,
             BUILDING_WIDTH/2, WALL_HEIGHT/2, BUILDING_LENGTH/2,
             BUILDING_WIDTH/2, WALL_HEIGHT, BUILDING_LENGTH/2,
             -BUILDING_WIDTH/2, WALL_HEIGHT, BUILDING_LENGTH/2);
    
    // Jendela-jendela di dinding depan
    drawWindow(-15, 8, BUILDING_LENGTH/2, 8, 4);
    drawWindow(-5, 8, BUILDING_LENGTH/2, 8, 4);
    drawWindow(5, 8, BUILDING_LENGTH/2, 8, 4);
    
    // Dinding belakang
    glColor3f(0.9f, 0.9f, 0.9f);
    drawQuad(-BUILDING_WIDTH/2, 0, -BUILDING_LENGTH/2,
             -BUILDING_WIDTH/2, WALL_HEIGHT/2, -BUILDING_LENGTH/2,
             BUILDING_WIDTH/2, WALL_HEIGHT/2, -BUILDING_LENGTH/2,
             BUILDING_WIDTH/2, 0, -BUILDING_LENGTH/2);
    
    glColor3f(0.8f, 0.4f, 0.3f);
    drawQuad(-BUILDING_WIDTH/2, WALL_HEIGHT/2, -BUILDING_LENGTH/2,
             -BUILDING_WIDTH/2, WALL_HEIGHT, -BUILDING_LENGTH/2,
             BUILDING_WIDTH/2, WALL_HEIGHT, -BUILDING_LENGTH/2,
             BUILDING_WIDTH/2, WALL_HEIGHT/2, -BUILDING_LENGTH/2);
    
    // Dinding samping kiri
    glColor3f(0.9f, 0.9f, 0.9f);
    drawQuad(-BUILDING_WIDTH/2, 0, -BUILDING_LENGTH/2,
             -BUILDING_WIDTH/2, WALL_HEIGHT/2, -BUILDING_LENGTH/2,
             -BUILDING_WIDTH/2, WALL_HEIGHT/2, BUILDING_LENGTH/2,
             -BUILDING_WIDTH/2, 0, BUILDING_LENGTH/2);
    
    glColor3f(0.8f, 0.4f, 0.3f);
    drawQuad(-BUILDING_WIDTH/2, WALL_HEIGHT/2, -BUILDING_LENGTH/2,
             -BUILDING_WIDTH/2, WALL_HEIGHT, -BUILDING_LENGTH/2,
             -BUILDING_WIDTH/2, WALL_HEIGHT, BUILDING_LENGTH/2,
             -BUILDING_WIDTH/2, WALL_HEIGHT/2, BUILDING_LENGTH/2);
    
    // Dinding samping kanan
    glColor3f(0.9f, 0.9f, 0.9f);
    drawQuad(BUILDING_WIDTH/2, 0, -BUILDING_LENGTH/2,
             BUILDING_WIDTH/2, 0, BUILDING_LENGTH/2,
             BUILDING_WIDTH/2, WALL_HEIGHT/2, BUILDING_LENGTH/2,
             BUILDING_WIDTH/2, WALL_HEIGHT/2, -BUILDING_LENGTH/2);
    
    glColor3f(0.8f, 0.4f, 0.3f);
    drawQuad(BUILDING_WIDTH/2, WALL_HEIGHT/2, -BUILDING_LENGTH/2,
             BUILDING_WIDTH/2, WALL_HEIGHT/2, BUILDING_LENGTH/2,
             BUILDING_WIDTH/2, WALL_HEIGHT, BUILDING_LENGTH/2,
             BUILDING_WIDTH/2, WALL_HEIGHT, -BUILDING_LENGTH/2);
    
    // Jendela di dinding samping
    drawWindow(BUILDING_WIDTH/2, 2, -10, 0, 6);
    drawWindow(BUILDING_WIDTH/2, 2, 0, 0, 6);
    drawWindow(BUILDING_WIDTH/2, 2, 10, 0, 6);
}

void drawCanopy() {
    // Kanopi di depan pintu masuk
    glColor3f(0.8f, 0.8f, 0.8f);
    
    // Atap kanopi
    glBegin(GL_QUADS);
        glVertex3f(-10, 12, BUILDING_LENGTH/2 + 5);
        glVertex3f(10, 12, BUILDING_LENGTH/2 + 5);
        glVertex3f(10, 12, BUILDING_LENGTH/2 + 1);
        glVertex3f(-10, 12, BUILDING_LENGTH/2 + 1);
    glEnd();
    
    // Tiang penyangga
    glColor3f(0.7f, 0.7f, 0.7f);
    
    // Tiang kiri
    glBegin(GL_QUADS);
        glVertex3f(-8, 0, BUILDING_LENGTH/2 + 4);
        glVertex3f(-7, 0, BUILDING_LENGTH/2 + 4);
        glVertex3f(-7, 12, BUILDING_LENGTH/2 + 4);
        glVertex3f(-8, 12, BUILDING_LENGTH/2 + 4);
    glEnd();
    
    // Tiang kanan
    glBegin(GL_QUADS);
        glVertex3f(7, 0, BUILDING_LENGTH/2 + 4);
        glVertex3f(8, 0, BUILDING_LENGTH/2 + 4);
        glVertex3f(8, 12, BUILDING_LENGTH/2 + 4);
        glVertex3f(7, 12, BUILDING_LENGTH/2 + 4);
    glEnd();
}

void drawDoor() {
    // Pintu masuk
    glColor3f(0.6f, 0.4f, 0.2f); // Cokelat kayu
    glBegin(GL_QUADS);
        glVertex3f(-4, 0, BUILDING_LENGTH/2 + 0.1f);
        glVertex3f(4, 0, BUILDING_LENGTH/2 + 0.1f);
        glVertex3f(4, 8, BUILDING_LENGTH/2 + 0.1f);
        glVertex3f(-4, 8, BUILDING_LENGTH/2 + 0.1f);
    glEnd();
    
    // Panel pintu
    glColor3f(0.5f, 0.3f, 0.1f);
    glBegin(GL_QUADS);
        glVertex3f(-3, 1, BUILDING_LENGTH/2 + 0.2f);
        glVertex3f(3, 1, BUILDING_LENGTH/2 + 0.2f);
        glVertex3f(3, 7, BUILDING_LENGTH/2 + 0.2f);
        glVertex3f(-3, 7, BUILDING_LENGTH/2 + 0.2f);
    glEnd();
}

void drawGround() {
    // Tanah di sekitar bangunan
    glColor3f(0.4f, 0.3f, 0.2f);
    glBegin(GL_QUADS);
        glVertex3f(-100, -0.5f, -100);
        glVertex3f(100, -0.5f, -100);
        glVertex3f(100, -0.5f, 100);
        glVertex3f(-100, -0.5f, 100);
    glEnd();
    
    // Jalur masuk
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
        glVertex3f(-5, -0.4f, BUILDING_LENGTH/2 + 5);
        glVertex3f(5, -0.4f, BUILDING_LENGTH/2 + 5);
        glVertex3f(5, -0.4f, BUILDING_LENGTH/2 + 20);
        glVertex3f(-5, -0.4f, BUILDING_LENGTH/2 + 20);
    glEnd();
}

void drawBuilding() {
    drawGround();
    drawFloor();
    drawWalls();
    drawRoof();
    drawCanopy();
    drawDoor();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Pengaturan kamera
    glTranslatef(0.0f, 0.0f, translateZ);
    glRotatef(rotateX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
    
    drawBuilding();
    
    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)width / (double)height, 1.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'w': case 'W':
            translateZ += 2.0f;
            break;
        case 's': case 'S':
            translateZ -= 2.0f;
            break;
        case 'a': case 'A':
            rotateY -= 5.0f;
            break;
        case 'd': case 'D':
            rotateY += 5.0f;
            break;
        case 'q': case 'Q':
            rotateX -= 5.0f;
            break;
        case 'e': case 'E':
            rotateX += 5.0f;
            break;
        case 'r': case 'R':
            rotateX = 0.0f;
            rotateY = 0.0f;
            translateZ = -50.0f;
            break;
        case 27: // ESC
            exit(0);
            break;
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mousePressed = true;
            lastMouseX = x;
            lastMouseY = y;
        } else {
            mousePressed = false;
        }
    }
}

void mouseMotion(int x, int y) {
    if (mousePressed) {
        rotateY += (x - lastMouseX) * 0.5f;
        rotateX += (y - lastMouseY) * 0.5f;
        lastMouseX = x;
        lastMouseY = y;
        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Model 3D Gedung Olahraga");
    
    init();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    
    printf("Kontrol:\n");
    printf("W/S: Zoom in/out\n");
    printf("A/D: Putar kiri/kanan\n");
    printf("Q/E: Putar atas/bawah\n");
    printf("R: Reset kamera\n");
    printf("Mouse: Drag untuk memutar\n");
    printf("ESC: Keluar\n");
    
    glutMainLoop();
    return 0;
}