//  ========================================================================
//  FILE NAME: Gallery.cpp
//  Gallery for COSC363 Assignment 1
//  ========================================================================

#include <iostream>
#include <cmath>
#include <GL/freeglut.h>
#include "loadBMP.h"
using namespace std;

GLuint txId[2];
GLUquadricObj* q;

// GLOBAL VARIABLES
bool can_rotate = true;

int counter = 0;
int fast_counter = 0;
int viewX = 0;
int viewY = 0;
int viewZ = 0;

int eyeX = 0;
int eyeY = 0;
int eyeZ = 70;

float* x, * y, * z;					//vertex coordinates
int* nv, * t1, * t2, * t3, * t4;		//number of vertices and vertex indices of each face
int nvert, nface;					//total number of vertices and faces


//--------------------------------------------------------------------------------
void loadTexture()
{
	glGenTextures(2, txId);   //Get 2 texture IDs 
	glBindTexture(GL_TEXTURE_2D, txId[0]);  //Use this texture name for the following OpenGL texture
	loadBMP("scanimation_cat.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, txId[1]);  //Use this texture name for the following OpenGL texture
	loadBMP("checkerboard_floor.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

}


//-- Loads mesh data in OFF format    -------------------------------------
void loadMeshFile(const char* fname)
{
	ifstream fp_in;
	int ne;

	fp_in.open(fname, ios::in);
	if (!fp_in.is_open())
	{
		cout << "Error opening mesh file" << endl;
		exit(1);
	}

	fp_in.ignore(INT_MAX, '\n');				//ignore first line
	fp_in >> nvert >> nface >> ne;			    // read number of vertices, polygons, edges (not used)

	x = new float[nvert];                        //create arrays
	y = new float[nvert];
	z = new float[nvert];

	nv = new int[nface];
	t1 = new int[nface];
	t2 = new int[nface];
	t3 = new int[nface];
	t4 = new int[nface];

	for (int i = 0; i < nvert; i++)                         //read vertex list 
		fp_in >> x[i] >> y[i] >> z[i];

	for (int i = 0; i < nface; i++)                         //read polygon list 
	{
		fp_in >> nv[i] >> t1[i] >> t2[i] >> t3[i];
		if (nv[i] == 4)
			fp_in >> t4[i];
		else
			t4[i] = -1;
	}

	fp_in.close();
	cout << " File successfully read." << endl;
}

void initialise()
{
	loadMeshFile("Ames_Window.off");			// Load Ames Window OFF file
	loadTexture();

	glClearColor(0.4f, 0.4f, 0.8f, 1.0f); 	 //Background, light blue
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	float white[] = { 1, 1, 1, 0.5 }; // Enable lights 1 & 2
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 90);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 0);

	glEnable(GL_LIGHT2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, white);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 90);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 128);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);  //Default
	glEnable(GL_COLOR_MATERIAL);

	q = gluNewQuadric();
	gluQuadricDrawStyle(q, GLU_FILL);
	gluQuadricNormals(q, GLU_SMOOTH);
	glEnable(GL_TEXTURE_2D);
	gluQuadricTexture(q, GL_TRUE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, 2.0, 10., 180.);
}

//---- Function to compute the normal vector using three vertices ----
void normal(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{
	float nx, ny, nz;
	nx = y1 * (z2 - z3) + y2 * (z3 - z1) + y3 * (z1 - z2);
	ny = z1 * (x2 - x3) + z2 * (x3 - x1) + z3 * (x1 - x2);
	nz = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);
	glNormal3f(nx, ny, nz);
}

//--Function to compute the normal vector of a triangle with index indx ----------
void normal(int indx)
{
	float x1 = x[t1[indx]], x2 = x[t2[indx]], x3 = x[t3[indx]];
	float y1 = y[t1[indx]], y2 = y[t2[indx]], y3 = y[t3[indx]];
	float z1 = z[t1[indx]], z2 = z[t2[indx]], z3 = z[t3[indx]];
	float nx, ny, nz;
	nx = y1 * (z2 - z3) + y2 * (z3 - z1) + y3 * (z1 - z2);
	ny = z1 * (x2 - x3) + z2 * (x3 - x1) + z3 * (x1 - x2);
	nz = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);
	glNormal3f(nx, ny, nz);
}

// SCENE COMPONENET DRAW FUNCTIONS

void draw_gallery_floor()
{
	int size = 64;
	int offset = size / 2;

	glColor3f(1, 1, 1);		//Colour
	glNormal3f(0, 0, 1);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, txId[1]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


	glBegin(GL_QUADS);			//A single quad
		glTexCoord2f(0, 10);
		glVertex3f(-offset, 1, offset);
		glTexCoord2f(10, 10);
		glVertex3f(offset, 1, offset);
		glTexCoord2f(10, 0);
		glVertex3f(offset, 1, -offset);
		glTexCoord2f(0, 0);
		glVertex3f(-offset, 1, -offset);
		glEnd();
	glDisable(GL_TEXTURE_2D);
}

void draw_scanimation_back()
{
	int size = 256;
	int offset = size / 2;

	glColor3f(1, 1, 1);		// White
	glNormal3f(0, 0, 1);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, txId[0]); // Map scanimation texture
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


	glBegin(GL_QUADS);			//A single quad
	glTexCoord2f(0, 1);
	glVertex3f(-offset, offset, 1);
	glTexCoord2f(1, 1);
	glVertex3f(offset, offset, 1);
	glTexCoord2f(1, 0);
	glVertex3f(offset, -offset, 1);
	glTexCoord2f(0, 0);
	glVertex3f(-offset, -offset, 1);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void draw_scanimation_slider(int translation)
{
	int size = 256;
	int offset = size / 2;

	int lineWidth = 5;
	int lineGap = 1;
	int startX = -(size + offset);
	int lineNum = size / (lineGap + lineWidth);

	int leftX;
	int rightX;


	glNormal3f(0, 0, 1);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glTranslatef(translation, 0.0, 0.0);	 //Translate Slider along x-axis by X units	
	for (int i = 0; i <= lineNum; i++) { // Loop to draw scanimation lines

		leftX = startX + (i * (lineGap + lineWidth));
		rightX = leftX + lineWidth;
		if (-(leftX+offset) < translation && -(leftX-offset) > translation) { // Only show lines when they are over the scanimation image
			glColor4f(0, 0, 0, 1);		//Colour
			glBegin(GL_QUADS);
			glVertex3f(leftX, offset, 2);
			glVertex3f(rightX, offset, 2);
			glVertex3f(rightX, -offset, 2);
			glVertex3f(leftX, -offset, 2);
			glEnd();
		}
		
	}

}

void draw_scanimation() 
{

	//Background Plane
	draw_scanimation_back();

	//Slider
	draw_scanimation_slider((counter%512)); // Move slider depending on counter


}

void draw_ames_window() 
{
	for (int indx = 0; indx < nface; indx++)		//draw each face
	{
		normal(indx); // Colour ames window faces depending on location
		if (indx > 8)		glColor3f(1., 1., 1.); // White
		else                glColor3f(.5, .5, .5); // Grey
		if (nv[indx] == 3)	glBegin(GL_TRIANGLES);
		else				glBegin(GL_QUADS);
		glVertex3d(x[t1[indx]], y[t1[indx]], z[t1[indx]]);
		glVertex3d(x[t2[indx]], y[t2[indx]], z[t2[indx]]);
		glVertex3d(x[t3[indx]], y[t3[indx]], z[t3[indx]]);
		if (nv[indx] == 4)
			glVertex3d(x[t4[indx]], y[t4[indx]], z[t4[indx]]);
		glEnd();
	}
}

void draw_aao3() 
{
	int ballTimer = fast_counter%360;

	glColor3f(0.7, .7, 0.7);        //Base Plane colour
	glBegin(GL_QUADS); // Draw Base Plane
		normal(-32, 0, 32, 32, 0, 32, 32, 0, -32);
		glVertex3f(-16, -8, 32);
		glVertex3f(16, -8, 32);
		glVertex3f(16, -8, -32);
		glVertex3f(-16, -8, -32);
	glEnd();


	glPushMatrix(); // Column cylander 3
		glTranslatef(0, -8, -21);
		glRotatef(-90, 1.0, 0.0, 0.0);
		glColor3f(1.0, 0.5, 0.0);
		gluCylinder(q, 4, 4, 10, 36, 17);
	glPopMatrix();

	glPushMatrix(); // Column cylander 2
		glTranslatef(0, -8, 21);
		glRotatef(-90, 1.0, 0.0, 0.0);
		glColor3f(1.0, 0.5, 0.0);
		gluCylinder(q, 4, 4, 10, 36, 17);
	glPopMatrix();

	glPushMatrix(); // Column cylander Middle
		glRotatef(-90, 1.0, 0.0, 0.0);
		glColor3f(1.0, 1.0, 1.0);
		gluCylinder(q, 3, 3, 5, 36, 17);
	glPopMatrix();

	glPushMatrix(); // Column Cylander Middle 2
		glTranslatef(0, -8, 0);
		glRotatef(-90, 1.0, 0.0, 0.0);
		glColor3f(1.0, 0.5, 0.0);
		gluCylinder(q, 5, 3, 10, 36, 17);
	glPopMatrix();

	if (ballTimer < 210 || ballTimer > 345) glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // Hide balls when not visible
	else                                    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	glPushMatrix(); // Ball 4
		glColor3f(1.0, 1.0, 0.5); // Yellow
		glTranslatef(-1, 5, -10);
		glRotatef(((ballTimer + 0) % 360), 1.0, 0.0, 0);
		glTranslatef(0, 0, -11);
		gluSphere(q, 1.5, 36, 17);
	glPopMatrix();

	glPushMatrix(); // Ball 3
		glColor3f(0.5, 1.0, 1.0); // Cyan
		glRotatef(-((ballTimer) % 360), 1.0, 0.0, 0);
		glTranslatef(-1, 5, 21);
		gluSphere(q, 1.5, 36, 17);
	glPopMatrix();

	glPushMatrix(); // Ball 2
		glColor3f(1.0, 0.5, 1.0); // Magenta
		glTranslatef(-1, 5, 11);
		glRotatef((ballTimer + 180) % 360, 1.0, 0.0, 0);
		glTranslatef(0, 0, 11);
		gluSphere(q, 1.5, 36, 17);
	glPopMatrix();


	// Rotation of arm and rings
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glRotatef(ballTimer, 0.0, 1.0, 0.0);

	glPushMatrix(); // Ring Torus 2
		glColor3f(0.5, 0.5, 1.0);
		glTranslatef(0, 5, -21);
		glRotatef(-90, 1.0, 0.0, 0.0);
		glutSolidTorus(1, 4, 36, 17);
	glPopMatrix();

	glPushMatrix(); // Ring Torus 1
		glColor3f(0.5, 0.5, 1.0);
		glTranslatef(0, 5, 21);
		glRotatef(-90, 1.0, 0.0, 0.0);
	glutSolidTorus(1, 4, 36, 17);
	glPopMatrix();

	glPushMatrix(); // Arm Cylander 2
		glTranslatef(0, 5, -3.5);
		glRotatef(180, 0.0, .0, 0.0);
		glColor3f(1.0, 0.8, 0.8);
		gluCylinder(q, .8, .8, 14, 36, 17);
	glPopMatrix();

	glPushMatrix(); // Arm Cylander 1
		glTranslatef(0, 5, 3.5);
		glColor3f(1.0, 0.8, 0.8);
		gluCylinder(q, .8, .8, 14, 36, 17);
	glPopMatrix();


	glPushMatrix(); // Tube Torus
		glColor3f(1.0, 0.5, 0.5);
		glTranslatef(0, 5, 0);
		glRotatef(-90, 1.0, 0.0, 0.0);
		glutSolidTorus(1, 3.8, 36, 17);
	glPopMatrix();

}

//============================================================
void display()
{
	int angleCounter = counter % 360;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(eyeX, eyeY, eyeZ, viewX, viewY, viewZ, 0., 1., 0.);

	glPushMatrix(); // AAO 3
		glTranslatef(60, -7, 25);
		glScalef(0.8, 0.8, 0.8);

		draw_aao3();
	glPopMatrix();

	glPushMatrix(); // Scanimation
		glScalef(0.1,0.1,0.1);
		draw_scanimation();
	glPopMatrix();

	glPushMatrix(); // Ames Window		
		glTranslatef(-60, 0, 25);
		glScalef(1.2, 1.2, 1.2);

		glRotatef(angleCounter, 0, 1, 0);
		glTranslatef(-11.5, 0, 0); // Move point of rotation to  centre
		draw_ames_window();
	glPopMatrix();

	glPushMatrix(); // Ames window lighting
		float light2_pos[4] = { -10, 0, 60, 1 };
		float light2_dir[3] = { -1, 0.0, -1 };
		glLightfv(GL_LIGHT2, GL_POSITION, light2_pos);
		glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, light2_dir);
		glTranslatef(-10, 0, 60);
	glPopMatrix();

	glPushMatrix();
		float light1_pos[4] = { -120, 0, -5, 1 };
		float light1_dir[3] = { 1, 0.0, -1 };
		glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1_dir);
		glTranslatef(-120, 0, -5);
	glPopMatrix();

	glPushMatrix(); // Gallery Floor
		glTranslatef(0, -10, 60);
		draw_gallery_floor();
	glPopMatrix();

	

	glutSwapBuffers();
}

//----------------------------------------------------------------
void timer(int value)
{
	counter++;
	fast_counter += 3;
	glutTimerFunc(50, timer, value);
	glutPostRedisplay();
}

//------------ Special key event callback ---------------------------------
// To enable the use of left and right arrow keys to rotate the camera
void special(int key, int x, int y)
{
	if (can_rotate) 
	{
		if (key == GLUT_KEY_LEFT) viewX--;
		else if (key == GLUT_KEY_RIGHT) viewX++;
		else if (key == GLUT_KEY_UP) viewY++;
		else if (key == GLUT_KEY_DOWN) viewY--;
	}
	
	if (viewX > 75) viewX = 75;
	if (viewX < -75) viewX = -75;

	if (viewY > 50) viewY = 50;
	if (viewY < -50) viewY = -50;

	viewZ = abs(viewX/2);
	printf("%d %d %d\n", viewX, viewY, viewZ);
	glutPostRedisplay();
}
//------------Keyboard key event callback ---------------------------------
// To enable the use of the num keys to switch views
void keyboard(unsigned char key, int x, int y) 
{
	switch (key) 
	{
	case '1':  eyeX = -10; eyeY = 0; eyeZ = 40; viewX = -55; viewY = 0; viewZ = 27.5; can_rotate = false;  break; // Ames Window View
	case '2':  eyeX = 0; eyeY = 0; eyeZ = 40; viewX = 0; viewY = 0; viewZ = 0; can_rotate = false;  break; // Scanimation View
	case '3':  eyeX = 10; eyeY = 10; eyeZ = 25; viewX = 51; viewY = 2; viewZ = 25; can_rotate = false;  break; // AAO 3 View
	case '0':  eyeX = 0; eyeY = 0; eyeZ = 70; viewX = 0; viewY = 0; viewZ = 0; can_rotate = true; break; // Gallery View
	}

	glutPostRedisplay();
}

//----------------------------------------------------------------
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 400);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	initialise();
	glutDisplayFunc(display);
	glutSpecialFunc(special);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(50, timer, 0);
	glutMainLoop();
	return 0;
}