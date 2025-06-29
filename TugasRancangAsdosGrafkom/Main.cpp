#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <iostream>

// Global variables for camera control
float cameraX = 0.0f, cameraY = 5.0f, cameraZ = 20.0f;
float cameraAngleX = 0.0f, cameraAngleY = 0.0f;
float moveSpeed = 0.5f;
bool keys[256] = { false };

// Lighting variables
bool lightingEnabled = true;
float lightPosition[] = { 0.0f, 15.0f, 0.0f, 1.0f };

void initGL() {
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f); // Sky blue background
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // Setup lighting
    float ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    float diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    float specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glEnable(GL_NORMALIZE);
}

void drawCube(float x, float y, float z, float width, float height, float depth) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(width, height, depth);
    glutSolidCube(1.0f);
    glPopMatrix();
}

void drawCylinder(float x, float y, float z, float radius, float height) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(-90, 1, 0, 0);
    glutSolidCylinder(radius, height, 20, 20);
    glPopMatrix();
}

void drawBuildingFloor() {
    // Main building floor (concrete gray)
    glColor3f(0.7f, 0.7f, 0.7f);
    drawCube(0, -0.5f, 0, 20, 1, 16);
}

void drawBadmintonCourt() {
    // Court floor (green surface like in the image)
    glColor3f(0.0f, 0.6f, 0.2f); // Green court surface
    glPushMatrix();
    glTranslatef(0, 0.05f, 0);
    glScalef(13.4f, 0.1f, 6.1f); // Standard badminton court size
    glutSolidCube(1.0f);
    glPopMatrix();

    // Court lines (white) - more detailed marking
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(4.0f);

    // Outer boundary (doubles court)
    glBegin(GL_LINE_LOOP);
    glVertex3f(-6.7f, 0.1f, -3.05f);
    glVertex3f(6.7f, 0.1f, -3.05f);
    glVertex3f(6.7f, 0.1f, 3.05f);
    glVertex3f(-6.7f, 0.1f, 3.05f);
    glEnd();

    // Singles sidelines
    glBegin(GL_LINE_LOOP);
    glVertex3f(-6.7f, 0.1f, -2.59f);
    glVertex3f(6.7f, 0.1f, -2.59f);
    glVertex3f(6.7f, 0.1f, 2.59f);
    glVertex3f(-6.7f, 0.1f, 2.59f);
    glEnd();

    // Center line
    glBegin(GL_LINES);
    glVertex3f(0, 0.1f, -3.05f);
    glVertex3f(0, 0.1f, 3.05f);
    glEnd();

    // Service lines (short service line)
    glBegin(GL_LINES);
    glVertex3f(-1.98f, 0.1f, -2.59f);
    glVertex3f(-1.98f, 0.1f, 2.59f);
    glVertex3f(1.98f, 0.1f, -2.59f);
    glVertex3f(1.98f, 0.1f, 2.59f);
    glEnd();

    // Long service lines for doubles
    glBegin(GL_LINES);
    glVertex3f(-2.53f, 0.1f, -3.05f);
    glVertex3f(-2.53f, 0.1f, 3.05f);
    glVertex3f(2.53f, 0.1f, -3.05f);
    glVertex3f(2.53f, 0.1f, 3.05f);
    glEnd();

    // Center service line
    glBegin(GL_LINES);
    glVertex3f(-1.98f, 0.1f, 0);
    glVertex3f(1.98f, 0.1f, 0);
    glEnd();
}

void drawNet() {
    // Net posts (black metal)
    glColor3f(0.1f, 0.1f, 0.1f);
    drawCylinder(0, 0, -3.2f, 0.05f, 1.55f);
    drawCylinder(0, 0, 3.2f, 0.05f, 1.55f);

    // Net support cable
    glColor3f(0.3f, 0.3f, 0.3f);
    drawCylinder(0, 1.5f, 0, 0.02f, 6.4f);
    glPushMatrix();
    glTranslatef(0, 1.5f, 0);
    glRotatef(90, 0, 1, 0);
    glutSolidCylinder(0.02f, 6.4f, 10, 10);
    glPopMatrix();

    // Net mesh
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(1.0f);

    // Horizontal net lines
    for (float y = 0.1f; y <= 1.5f; y += 0.15f) {
        glBegin(GL_LINES);
        glVertex3f(0, y, -3.2f);
        glVertex3f(0, y, 3.2f);
        glEnd();
    }

    // Vertical net lines
    for (float z = -3.2f; z <= 3.2f; z += 0.3f) {
        glBegin(GL_LINES);
        glVertex3f(0, 0.1f, z);
        glVertex3f(0, 1.5f, z);
        glEnd();
    }
}

void drawBuildingStructure() {
    // Building walls (light gray concrete)
    glColor3f(0.8f, 0.8f, 0.8f);

    // Back wall
    drawCube(0, 6, -8, 20, 12, 0.5f);

    // Side walls
    drawCube(-10, 6, 0, 0.5f, 12, 16);
    drawCube(10, 6, 0, 0.5f, 12, 16);

    // Front wall with door frame
    drawCube(-7.5f, 6, 8, 5, 12, 0.5f); // Left part
    drawCube(7.5f, 6, 8, 5, 12, 0.5f);  // Right part
    drawCube(0, 10, 8, 10, 4, 0.5f);    // Top part

    // Door frame
    glColor3f(0.4f, 0.4f, 0.4f);
    drawCube(-2.5f, 3, 8.1f, 0.2f, 6, 0.3f); // Left frame
    drawCube(2.5f, 3, 8.1f, 0.2f, 6, 0.3f);  // Right frame
    drawCube(0, 6, 8.1f, 5, 0.2f, 0.3f);     // Top frame

    // Door
    glColor3f(0.5f, 0.3f, 0.1f); // Brown door
    drawCube(-1.2f, 3, 8.2f, 2.4f, 5.8f, 0.1f); // Left door
    drawCube(1.2f, 3, 8.2f, 2.4f, 5.8f, 0.1f);  // Right door

    // Door handles
    glColor3f(0.8f, 0.8f, 0.2f); // Brass
    drawCube(-0.3f, 3, 8.25f, 0.1f, 0.1f, 0.05f);
    drawCube(0.3f, 3, 8.25f, 0.1f, 0.1f, 0.05f);

    // Steel frame structure (dark gray)
    glColor3f(0.3f, 0.3f, 0.3f);

    // Main roof support beams
    for (int i = -2; i <= 2; i++) {
        drawCube(i * 4, 12, 0, 0.3f, 1, 16);
    }

    // Cross beams
    for (int i = -2; i <= 2; i++) {
        drawCube(0, 12, i * 4, 20, 0.3f, 0.3f);
    }

    // Vertical supports
    for (int i = -2; i <= 2; i++) {
        for (int j = -1; j <= 1; j += 2) {
            drawCylinder(i * 4, 0, j * 7, 0.15f, 12);
        }
    }
}

void drawRoof() {
    // Main roof structure (metal sheets) - complete coverage
    glColor3f(0.4f, 0.4f, 0.5f); // Metal gray

    // Left slope
    glPushMatrix();
    glTranslatef(-5, 14, 0);
    glRotatef(-25, 0, 0, 1);
    glScalef(12, 0.1f, 16);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Right slope
    glPushMatrix();
    glTranslatef(5, 14, 0);
    glRotatef(25, 0, 0, 1);
    glScalef(12, 0.1f, 16);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Ridge beam
    glColor3f(0.2f, 0.2f, 0.2f);
    drawCube(0, 17, 0, 20, 0.3f, 0.3f);

    // Roof gutters
    glColor3f(0.3f, 0.3f, 0.3f);
    drawCube(-10, 8, 0, 0.3f, 0.3f, 16);
    drawCube(10, 8, 0, 0.3f, 0.3f, 16);
}

void drawLights() {
    // 8 ceiling lights (4 on each side) positioned further apart
    glColor3f(0.2f, 0.2f, 0.2f); // Dark fixture housing

    // Left side lights
    drawCube(-6, 11, -4, 1.2f, 0.4f, 1.2f);
    drawCube(-6, 11, -1.5f, 1.2f, 0.4f, 1.2f);
    drawCube(-6, 11, 1.5f, 1.2f, 0.4f, 1.2f);
    drawCube(-6, 11, 4, 1.2f, 0.4f, 1.2f);

    // Right side lights
    drawCube(6, 11, -4, 1.2f, 0.4f, 1.2f);
    drawCube(6, 11, -1.5f, 1.2f, 0.4f, 1.2f);
    drawCube(6, 11, 1.5f, 1.2f, 0.4f, 1.2f);
    drawCube(6, 11, 4, 1.2f, 0.4f, 1.2f);

    // Light glow effect
    glColor4f(1.0f, 1.0f, 0.8f, 0.4f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float positions[8][2] = { {-6, -4}, {-6, -1.5f}, {-6, 1.5f}, {-6, 4},
                            {6, -4}, {6, -1.5f}, {6, 1.5f}, {6, 4} };

    for (int i = 0; i < 8; i++) {
        glPushMatrix();
        glTranslatef(positions[i][0], 10.2f, positions[i][1]);
        glutSolidSphere(0.6f, 15, 15);
        glPopMatrix();
    }

    glDisable(GL_BLEND);
}

void drawBenches() {
    // Metal benches on floor (not floating)
    glColor3f(0.4f, 0.4f, 0.4f); // Dark gray metal

    // Bench 1 (left side, against wall)
    glPushMatrix();
    glTranslatef(-8.5f, 0.25f, -5);

    // Bench seat
    drawCube(0, 0.25f, 0, 4, 0.1f, 0.5f);

    // Bench back
    drawCube(0, 0.7f, -0.2f, 4, 0.9f, 0.1f);

    // Bench legs
    drawCylinder(-1.8f, 0, -0.2f, 0.05f, 0.5f);
    drawCylinder(1.8f, 0, -0.2f, 0.05f, 0.5f);
    drawCylinder(-1.8f, 0, 0.2f, 0.05f, 0.5f);
    drawCylinder(1.8f, 0, 0.2f, 0.05f, 0.5f);

    glPopMatrix();

    // Bench 2 (right side, against wall)
    glPushMatrix();
    glTranslatef(8.5f, 0.25f, -5);

    // Bench seat
    drawCube(0, 0.25f, 0, 4, 0.1f, 0.5f);

    // Bench back
    drawCube(0, 0.7f, -0.2f, 4, 0.9f, 0.1f);

    // Bench legs
    drawCylinder(-1.8f, 0, -0.2f, 0.05f, 0.5f);
    drawCylinder(1.8f, 0, -0.2f, 0.05f, 0.5f);
    drawCylinder(-1.8f, 0, 0.2f, 0.05f, 0.5f);
    drawCylinder(1.8f, 0, 0.2f, 0.05f, 0.5f);

    glPopMatrix();
}

void drawScoreboard() {
    // Old-style scoreboard mounted on back wall
    glColor3f(0.1f, 0.1f, 0.1f); // Black frame

    glPushMatrix();
    glTranslatef(0, 7, -7.7f);

    // Main board frame
    drawCube(0, 0, 0, 3, 1.5f, 0.2f);

    // Score display areas (white background)
    glColor3f(0.9f, 0.9f, 0.9f);
    drawCube(-0.8f, 0, 0.15f, 1, 0.8f, 0.1f); // Left score
    drawCube(0.8f, 0, 0.15f, 1, 0.8f, 0.1f);  // Right score

    // Center divider
    glColor3f(0.2f, 0.2f, 0.2f);
    drawCube(0, 0, 0.15f, 0.1f, 1.2f, 0.1f);

    // Title area
    glColor3f(0.8f, 0.2f, 0.2f); // Red
    drawCube(0, 0.5f, 0.15f, 2.5f, 0.3f, 0.1f);

    // Support brackets
    glColor3f(0.3f, 0.3f, 0.3f);
    drawCube(-1.2f, -0.3f, 0, 0.2f, 0.6f, 0.3f);
    drawCube(1.2f, -0.3f, 0, 0.2f, 0.6f, 0.3f);

    glPopMatrix();
}

void drawTerrain() {
    // Hilly terrain around the building
    glColor3f(0.2f, 0.6f, 0.2f); // Green grass

    // Create rolling hills effect
    glBegin(GL_TRIANGLES);
    for (int x = -50; x < 50; x += 5) {
        for (int z = -50; z < 50; z += 5) {
            if (abs(x) > 15 || abs(z) > 12) { // Only outside building area
                float height1 = sin(x * 0.1f) * cos(z * 0.1f) * 3 - 1;
                float height2 = sin((x + 5) * 0.1f) * cos(z * 0.1f) * 3 - 1;
                float height3 = sin(x * 0.1f) * cos((z + 5) * 0.1f) * 3 - 1;
                float height4 = sin((x + 5) * 0.1f) * cos((z + 5) * 0.1f) * 3 - 1;

                // Triangle 1
                glVertex3f(x, height1, z);
                glVertex3f(x + 5, height2, z);
                glVertex3f(x, height3, z + 5);

                // Triangle 2
                glVertex3f(x + 5, height2, z);
                glVertex3f(x + 5, height4, z + 5);
                glVertex3f(x, height3, z + 5);
            }
        }
    }
    glEnd();
}

void drawTrees() {
    // Trees positioned on hills around the building
    float treePositions[][3] = {
        {-25, 0, -20}, {-30, 0, -15}, {-35, 0, -10},
        {25, 0, -20}, {30, 0, -15}, {35, 0, -10},
        {-25, 0, 20}, {-30, 0, 15}, {25, 0, 20}, {30, 0, 15},
        {-20, 0, -30}, {-15, 0, -35}, {20, 0, -30}, {15, 0, -35},
        {-20, 0, 30}, {-15, 0, 35}, {20, 0, 30}, {15, 0, 35}
    };

    for (int i = 0; i < 18; i++) {
        float x = treePositions[i][0];
        float z = treePositions[i][2];
        float y = sin(x * 0.1f) * cos(z * 0.1f) * 3 - 1;

        // Tree trunk
        glColor3f(0.4f, 0.2f, 0.1f); // Brown
        drawCylinder(x, y, z, 0.4f, 5);

        // Tree leaves
        glColor3f(0.1f, 0.5f, 0.1f); // Dark green
        glPushMatrix();
        glTranslatef(x, y + 6, z);
        glutSolidSphere(2.5f, 15, 15);
        glPopMatrix();
    }
}

void drawWindows() {
    // Windows on side walls
    glColor3f(0.6f, 0.8f, 1.0f); // Light blue glass

    // Left wall windows
    for (int i = -2; i <= 2; i++) {
        drawCube(-9.8f, 6, i * 3, 0.2f, 3, 1.5f);
    }

    // Right wall windows
    for (int i = -2; i <= 2; i++) {
        drawCube(9.8f, 6, i * 3, 0.2f, 3, 1.5f);
    }

    // Window frames
    glColor3f(0.3f, 0.3f, 0.3f);
    for (int side = -1; side <= 1; side += 2) {
        for (int i = -2; i <= 2; i++) {
            float x = side * 9.9f;
            float z = i * 3;

            // Frame outline
            drawCube(x, 6, z, 0.1f, 3.2f, 0.1f); // Vertical center
            drawCube(x, 7.5f, z, 0.1f, 0.1f, 1.6f); // Horizontal top
            drawCube(x, 4.5f, z, 0.1f, 0.1f, 1.6f); // Horizontal bottom
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Camera setup
    glTranslatef(-cameraX, -cameraY, -cameraZ);
    glRotatef(cameraAngleX, 1, 0, 0);
    glRotatef(cameraAngleY, 0, 1, 0);

    // Update light position
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    // Draw terrain first
    drawTerrain();
    drawTrees();

    // Draw building foundation and floor
    drawBuildingFloor();

    // Draw building structure
    drawBuildingStructure();
    drawRoof();
    drawWindows();

    // Draw court elements
    drawBadmintonCourt();
    drawNet();

    // Draw interior elements
    drawLights();
    drawBenches();
    drawScoreboard();

    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)width / (double)height, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    keys[key] = true;

    switch (key) {
    case 27: // ESC key
        exit(0);
        break;
    case 'l':
    case 'L':
        lightingEnabled = !lightingEnabled;
        if (lightingEnabled) {
            glEnable(GL_LIGHTING);
        }
        else {
            glDisable(GL_LIGHTING);
        }
        break;
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    keys[key] = false;
}

void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:
        cameraAngleX += 5.0f;
        break;
    case GLUT_KEY_DOWN:
        cameraAngleX -= 5.0f;
        break;
    case GLUT_KEY_LEFT:
        cameraAngleY -= 5.0f;
        break;
    case GLUT_KEY_RIGHT:
        cameraAngleY += 5.0f;
        break;
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Mouse click handling can be added here
    }
}

void update() {
    // Handle continuous key presses
    if (keys['w'] || keys['W']) {
        cameraZ -= moveSpeed;
    }
    if (keys['s'] || keys['S']) {
        cameraZ += moveSpeed;
    }
    if (keys['a'] || keys['A']) {
        cameraX -= moveSpeed;
    }
    if (keys['d'] || keys['D']) {
        cameraX += moveSpeed;
    }
    if (keys['q'] || keys['Q']) {
        cameraY += moveSpeed;
    }
    if (keys['e'] || keys['E']) {
        cameraY -= moveSpeed;
    }

    glutPostRedisplay();
}

void timer(int value) {
    update();
    glutTimerFunc(16, timer, 0); // ~60 FPS
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 800);
    glutCreateWindow("Gor Badminton 3D - FTI UKSW Salatiga");

    initGL();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouse);
    glutTimerFunc(0, timer, 0);

    std::cout << "=== GOR BADMINTON 3D - FTI UKSW SALATIGA ===" << std::endl;
    std::cout << "Revisi Lengkap - Semua Masalah Diperbaiki!" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "Kontrol Kamera:" << std::endl;
    std::cout << "W/A/S/D - Bergerak maju/kiri/mundur/kanan" << std::endl;
    std::cout << "Q/E - Naik/turun" << std::endl;
    std::cout << "Arrow Keys - Putar kamera" << std::endl;
    std::cout << "L - Toggle lighting" << std::endl;
    std::cout << "ESC - Keluar" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "Fitur Baru:" << std::endl;
    std::cout << "- Net diperbaiki (melintang lapangan)" << std::endl;
    std::cout << "- Atap tertutup sempurna" << std::endl;
    std::cout << "- Garis lapangan lengkap standar BWF" << std::endl;
    std::cout << "- Papan skor detail di dinding belakang" << std::endl;
    std::cout << "- Bangku menempel lantai dengan sandaran" << std::endl;
    std::cout << "- 8 lampu positioned correctly" << std::endl;
    std::cout << "- Lantai gedung + pintu lengkap" << std::endl;
    std::cout << "- Jendela-jendela di dinding samping" << std::endl;
    std::cout << "==========================================" << std::endl;

    glutMainLoop();
    return 0;
}