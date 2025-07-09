//#include <iostream>
//#include <GL/freeglut.h>
//
//void display() {
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glNormal3f(0.0f, 0.0f, 1.0f); // Normal pointing out of the screen (towards viewer)
//
//	GLfloat light0_position[] = { 0.1f, 0.1f, 1.0f, 0.0f }; // directional light pointing toward Z
//	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
//
//	glBegin(GL_QUADS);
//	glVertex2f(-0.5f, 0.5f); //A
//	glVertex2f(-0.5f, -0.5f); //B
//	glVertex2f(0.5f, -0.5f); //C
//	glVertex2f(0.5f, 0.5f); //D
//	glEnd();
//
//	glutSwapBuffers();
//}
//
//void myinit() {
//	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	gluOrtho2D(-1.0f, 1.0f, -1.0f, 1.0f);
//
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//
//	glEnable(GL_LIGHTING);
//	glEnable(GL_LIGHT0);
//
//	// Define the ambient color of Light 0
//	GLfloat light0_ambient[] = { 1.0f, 0.0f, 0.0f, 1.0f }; 
//	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
//
//	GLfloat light0_diffuse[] = { 1.0f, 0.0f, 0.0f, 1.0f };
//	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
//
//	GLfloat light0_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // White specular light for shiny highlights
//	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
//
//	// Set the global ambient light for the scene
//	// This is light that comes from everywhere and contributes to every surface
//	GLfloat global_ambient[] = { 1.0f, 0.0f, 0.0f, 1.0f };
//	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
//
//	// Define the material properties of the object (the quad)
//	// Here, we only set the ambient reflection property
//	GLfloat material_ambient[] = { 1.0f, 0.0f, 0.0f, 1.0f };
//	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
//
//	GLfloat material_diffuse[] = { 1.0f, 0.0f, 0.0f, 1.0f };
//	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
//
//	GLfloat material_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // White specular reflection
//	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
//
//	GLfloat material_shininess[] = { 100.0f };
//	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
//
//	// If you enable GL_COLOR_MATERIAL, then glColor3f calls will affect the ambient, diffuse,
//	// and specular properties of the material directly.
//	// For specific control, it's better to use glMaterialfv as shown above.
//	//glEnable(GL_COLOR_MATERIAL);
//	//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE); // This would make glColor3f affect ambient and diffuse
//}
//
//int main(int argc, char** argv) {
//	glutInit(&argc, argv);
//	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
//	glutInitWindowSize(500, 500);
//	glutInitWindowPosition(0, 0);
//	glutCreateWindow("Lighting OpenGL");
//	glutDisplayFunc(display);
//
//	myinit();
//
//	glutMainLoop();
//
//	return 0;
//}