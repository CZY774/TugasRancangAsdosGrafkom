#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <cmath>
#include <iostream>

#define BUILDING_WIDTH 40.0f
#define BUILDING_LENGTH 60.0f
#define WALL_HEIGHT 15.0f
#define ROOF_HEIGHT 12.0f
#define ROOF_OVERHANG 2.0f

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

void drawQuad(float x1, float y1, float z1, float x2, float y2, float z2, 
              float x3, float y3, float z3, float x4, float y4, float z4) {
    glBegin(GL_QUADS);
    glVertex3f(x1, y1, z1); glVertex3f(x2, y2, z2);
    glVertex3f(x3, y3, z3); glVertex3f(x4, y4, z4);
    glEnd();
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
    colorWalls();
    float halfW = BUILDING_WIDTH / 2, halfL = BUILDING_LENGTH / 2;
    
    drawQuad(-halfW, 0, halfL,  halfW, 0, halfL,  halfW, WALL_HEIGHT, halfL, -halfW, WALL_HEIGHT, halfL);
    drawDoors();
    colorWalls();
    drawQuad(-halfW, 0, -halfL, halfW, 0, -halfL, halfW, WALL_HEIGHT, -halfL, -halfW, WALL_HEIGHT, -halfL);
    drawQuad(-halfW, 0, -halfL, -halfW, 0, halfL, -halfW, WALL_HEIGHT, halfL, -halfW, WALL_HEIGHT, -halfL);
    drawQuad(halfW, 0, -halfL,  halfW, 0, halfL,  halfW, WALL_HEIGHT, halfL,  halfW, WALL_HEIGHT, -halfL);
}

void drawRoof() {
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
    glColor3f(0.4f, 0.6f, 0.3f);
    drawQuad(-100, -0.5f, -100, 100, -0.5f, -100, 100, -0.5f, 100, -100, -0.5f, 100);
}

void drawBuilding() {
    drawGround();
    drawWalls();
    drawRoof();
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
    glutCreateWindow("Gedung dengan Atap Arc");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);

    glutMainLoop();
    return 0;
}
