#include "GL/glut.h"
#include <gl/freeglut.h>
#include <cmath>
#include <iostream>
#include <windows.h>
#include <vector>
/*
 * scene.cpp
 *
 *  Created on: Dec 27, 2014
 *      Author: Ioana
 */

using namespace std;

#define WIDTH   800
#define HEIGHT  600


GLdouble floorYLength = 60.0, floorXLength = 60.0;
GLdouble frustum_left = -floorXLength / 2.0;
GLdouble frustum_right = floorXLength / 2.0;
GLdouble frustum_bottom = -floorYLength / 2.0;
GLdouble frustum_top = floorYLength / 2.0;
GLdouble frustum_dnear = 80.0;
GLdouble frustum_dfar = 500.0;
GLdouble eyeX = floorXLength / 2, eyeY = floorYLength/2, eyeZ = 100.0;
GLdouble centerX = floorXLength / 2, centerY = floorYLength / 2, centerZ = 0.0f;
GLdouble mouseX = 0, mouseY = 0;
GLdouble yaw = 45.0, pitch = 45.0;
GLfloat xpos = 0, zpos = -50;

GLfloat light_position[] = {0, 28, 0, 1.0 };

GLfloat wood_brown[] = { 94.0/255, 51.0/255, 6.0/255};
GLfloat wood_beige[] = {240.0/255, 229.0/255, 211.0/255};
GLfloat wall_blue[] = {253.0/255, 255.0/255, 194.0/255};

GLfloat zoom_factor = 0.2;
GLfloat zoom = 1;
GLfloat aspect = WIDTH/HEIGHT;
GLint fogMode;


bool left_wall = false, right_wall = false, back_wall = false, front_wall = false, ceiling = false;
bool light_enabled = true;
bool fog_enabled = false;
bool shadows = true, for_shadow;
struct Axis {
	GLdouble x, y, z;
};

struct Point {
	GLdouble x, y, z;
};

struct Camera {
	Axis Oz; //direction
	Axis Ox; //right
	Axis Oy; //up
	Point origin;
};

Camera camera;

GLvoid normalize(GLdouble &x, GLdouble &y, GLdouble &z) {
	GLdouble norm = sqrt(x*x + y*y + z*z);
	x = x / norm;
	y = y / norm;
	z = z / norm;
}

Axis crossedProduct(Axis u, Axis v) {
	Axis r;
	r.x = u.y * v.z - u.z * v.y;
	r.y = u.z * v.x - u.x * v.z;
	r.z = u.x * v.y - u.y * v.x;
	return r;
}

GLvoid computeCameraSystem() {
	camera.Oz.x = centerX - camera.origin.x;
	camera.Oz.y = centerY - camera.origin.y;
	camera.Oz.z = centerZ - camera.origin.z;

	camera.Oy.x = 0.0f;
	camera.Oy.y = 1.0f;
	camera.Oy.z = 0.0f;

	camera.Ox = crossedProduct(camera.Oz, camera.Oy);
	camera.Oy = crossedProduct(camera.Ox, camera.Oz);

	normalize(camera.Ox.x, camera.Ox.y, camera.Ox.z);
	normalize(camera.Oy.x, camera.Oy.y, camera.Oy.z);
}

void drawParallelepiped(GLfloat sizeX, GLfloat sizeY, GLfloat sizeZ, GLenum type) {
	GLfloat n[6][3] = {
			{-1.0, 0.0, 0.0},
			{0.0, 1.0, 0.0},
			{1.0, 0.0, 0.0},
			{0.0, -1.0, 0.0},
			{0.0, 0.0, 1.0},
			{0.0, 0.0, -1.0}
	};

	GLint faces[6][4] ={
			{0, 1, 2, 3},
			{3, 2, 6, 7},
			{7, 6, 5, 4},
			{4, 5, 1, 0},
			{5, 6, 2, 1},
			{7, 4, 0, 3}
	};

	GLfloat v[8][3];
	GLint i;

	v[0][0] = v[1][0] = v[2][0] = v[3][0] = -sizeX / 2;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = sizeX / 2;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -sizeY / 2;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = sizeY / 2;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = -sizeZ / 2;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = sizeZ / 2;

	for (i = 5; i >= 0; i--) {
		glBegin(type);
		glNormal3fv(&n[i][0]);
		glVertex3fv(&v[faces[i][0]][0]);
		glVertex3fv(&v[faces[i][1]][0]);
		glVertex3fv(&v[faces[i][2]][0]);
		glVertex3fv(&v[faces[i][3]][0]);
		glEnd();
	}
}




GLvoid init() {
	glClearColor(196.0/255, 236.0/255, 255.0/255, 0.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel (GL_SMOOTH);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glCullFace(GL_BACK);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
	glEnable(GL_FOG);{GLfloat fogColor[4] = {0.5, 0.5, 0.5, 1.0};
	fogMode = GL_LINEAR;
	glFogi (GL_FOG_MODE, fogMode);
	glFogfv (GL_FOG_COLOR, fogColor);
	glFogf (GL_FOG_DENSITY, 0.35);
	glHint (GL_FOG_HINT, GL_DONT_CARE);
	glFogf (GL_FOG_START, 0);
	glFogf (GL_FOG_END, 100);
	glDisable(GL_FOG);}
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//	gluLookAt(floorXLength/2, -1.0f, 100.0f, centerX, centerY, centerZ, 0.0f, 1.0f, 0.0f);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	glFrustum(-floorXLength/2, floorXLength / 2, -floorYLength / 2, floorYLength / 2, 50.0f, 120.0f);

	//initialize camera
	camera.origin.x = eyeX;
	camera.origin.y = eyeY;
	camera.origin.z = eyeZ;
	camera.Oy.x = 0;
	camera.Oy.y = 1;
	camera.Oy.z = 0;

	normalize(camera.Oz.x, camera.Oz.y, camera.Oz.z);
}

GLvoid rotate(GLdouble angle, Axis axis, Point &p) {
	normalize(axis.x, axis.y, axis.z);
	GLdouble cosine = cos(angle);
	GLdouble sine = sin(angle);
	GLdouble tx = (cosine + axis.x*axis.x*(1-cosine))*p.x + (axis.x*axis.y*(1-cosine) - axis.z*sine)*p.y + (axis.x*axis.z*(1-cosine) + axis.y*sine) * p.z;
	GLdouble ty = (axis.y*axis.x*(1-cosine) + axis.z*sine)*p.x + (cosine + axis.y*axis.y*(1-cosine))*p.y + (axis.y*axis.z*(1-cosine) - axis.x*sine) * p.z;
	GLdouble tz = (axis.z*axis.x*(1-cosine) - axis.y*sine)*p.x + (axis.z*axis.y*(1-cosine) + axis.x*sine)*p.y + (cosine+axis.z*axis.z*(1-cosine)) * p.z;
	p.x = tx;
	p.y = ty;
	p.z = tz;
}


GLvoid show_axes() {
	glLineWidth(2);
	glBegin(GL_LINES);
	glColor4f(1.0f, 0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(200.0f, 0.0f, 0.0f);
	glEnd();
	glBegin(GL_LINES);
	glColor4f(0.0f, 1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 200.0f, 0.0f);
	glEnd();
	glBegin(GL_LINES);
	glColor4f(0.0f, 0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 200.0f);
	glEnd();
}

GLvoid draw_right_wall() {
	//right wall
	if (!right_wall) return;
	glNormal3f(1.0, 0.0, 0.0);
	glColor3fv(wall_blue);
	glBegin(GL_QUADS);
		glVertex3f(-30, 0, -30);
    	glVertex3f(-30, 0,  30);
    	glVertex3f(-30, 30,  30);
    	glVertex3f(-30, 30, -30);
    glEnd();
}

GLvoid draw_left_wall() {
    //left wall
	if (!left_wall) return;
	glColor3fv(wall_blue);
    glNormal3f(-1.0, 0.0, 0.0);
    glBegin(GL_QUADS);
    		glVertex3f(30, 0, 30);
        	glVertex3f(30, 30,  30);
        	glVertex3f(30, 30,  -30);
        	glVertex3f(30, 0, -30);
        glEnd();
}

GLvoid draw_back_wall() {
    //back wall
	if (!back_wall) return;
	glColor3fv(wall_blue);
    glNormal3f(0.0, 0.0, -1.0);
    glBegin(GL_QUADS);
    	glVertex3f(30, 0, 30);
        glVertex3f(-30, 0,  30);
        glVertex3f(-30, 30,  30);
        glVertex3f(30, 30, 30);
    glEnd();

}

GLvoid draw_front_wall () {
	//front wall
	if (!front_wall) return;
	glColor3fv(wall_blue);
	glNormal3f(0.0, 0.0, 1.0);
	glBegin(GL_QUADS);
	glVertex3f(30, 0, -30);
	glVertex3f(30, 30, -30);
 	glVertex3f(-30, 30,  -30);
    glVertex3f(-30, 0, -30);
	glEnd();
}

GLvoid draw_ceiling() {
	//ceiling
	if (!ceiling) return;
	glColor3fv(wall_blue);
	glNormal3f(0.0, -1.0, 0.0);
	glBegin(GL_QUADS);
	glVertex3f(-30, 30, 30);
	glVertex3f(-30, 30, -30);
	glVertex3f(30, 30,  -30);
	glVertex3f(30, 30, 30);
	glEnd();
}

GLvoid show_floor() {
	GLfloat mat_ambient[] = { 64.0/255, 86.0/255, 122.0/255, 1.0 };
	//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_ambient);

	glColor3fv(mat_ambient);
	glNormal3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    		glVertex3f(-30, 0, -30);
        	glVertex3f(-30, 0,  30);
        	glVertex3f(30, 0,  30);
        	glVertex3f( 30, 0, -30);
        glEnd();
}

GLvoid show_desk() {
	//GLfloat mat_shininess[] = { 50.0 };
	//GLfloat mat_emission[] = { 0.0 };
	//glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	//glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	if (!for_shadow)
		glColor3fv(wood_brown);
	glPushMatrix();
	glTranslated(-10, 2.5, 25.5);
	drawParallelepiped(0.5, 5, 4, GL_POLYGON);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-26, 2.5, 25.5);
	drawParallelepiped(0.5, 5, 4, GL_POLYGON);
	glPopMatrix();
	glPushMatrix();
	if (!for_shadow)
		glColor3fv(wood_beige);
	glTranslated(-18, 5.25, 25.5);
	drawParallelepiped(17, 0.5, 7, GL_POLYGON);
	glPopMatrix();
	if (!for_shadow)
		glColor3f(44.0/255, 191.0/255, 181.0/255);
	glPushMatrix();
	glTranslated(-15, 6.2, 27);
	glutSolidTeapot(1);
	glPopMatrix();
}

GLvoid show_garbage_bin() {
	if (!for_shadow)
		glColor3f(1,0,0);
	glPushMatrix();
	glTranslated(-7, 0, 26);
	glRotated(-90.0f, 1, 0, 0);

	GLUquadricObj *quadObj = gluNewQuadric();
	gluQuadricOrientation(quadObj, GLU_OUTSIDE);
	gluCylinder(quadObj, 1, 2, 3, 10, 10);
	glTranslated(0, 0, 0.1);
	gluDisk(gluNewQuadric(), 0, 0.98, 32, 32);
	glPopMatrix();
}

GLvoid show_bed() {

	GLfloat mat_shininess[] = { 100 };
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	//GLfloat mat_specular[] = { 1, 1, 1, 1.0 };
//	GLfloat mat_shininess[] = { 0.0 };
	//GLfloat mat_emission[] = { 0.0 };
	//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);
//	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
//	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	if (!for_shadow)
		glColor3fv(wood_brown);
	//legs
	glPushMatrix();
	glTranslated(20, 0.5, 0);
	drawParallelepiped(0.5, 1, 0.5, GL_POLYGON);
	glPopMatrix();
	glPushMatrix();
	glTranslated(27, 0.5, 0);
	drawParallelepiped(0.5, 1, 0.5, GL_POLYGON);
	glPopMatrix();
	glPushMatrix();
	glTranslated(27, 0.5, 27);
	drawParallelepiped(0.5, 1, 0.5, GL_POLYGON);
	glPopMatrix();
	glPushMatrix();
	glTranslated(20, 0.5, 27);
	drawParallelepiped(0.5, 1, 0.5, GL_POLYGON);
	glPopMatrix();

	if (!for_shadow)
		glColor3fv(wood_beige);
	glPushMatrix();
	glTranslated(23.5, 1.25, 13.5);
	drawParallelepiped(8, 1, 28, GL_POLYGON);
	glPopMatrix();

	//pillow
	if (!for_shadow)
		glColor3f(0.8, 0.2, 0);
	GLfloat specular[] = {1,1,1,1};
	glMaterialfv(GL_FRONT, GL_SPECULAR,  specular);
	glPushMatrix();
	glTranslated(21.5, 1.75, 24.5);
	glRotated(90.0, 0, 1, 0);
	GLUquadricObj *quadObj = gluNewQuadric();
	gluCylinder(quadObj, 1.3, 1.3, 4, 10, 10);
	glPopMatrix();
}


GLvoid show_chair() {
	if (!for_shadow)
		glColor3fv(wood_brown);
	//bottom
	glPushMatrix();
	glTranslated(-21.5, 2.75, 16.5);
	drawParallelepiped(3.5, 0.5, 3.5, GL_POLYGON);
	glPopMatrix();



	//back
	glPushMatrix();
	glTranslated(-20, 5, 15);
	drawParallelepiped(0.25, 4, 0.25, GL_POLYGON);
	glPopMatrix();

	if (!for_shadow)
		glColor3fv(wood_beige);

	glPushMatrix();
	glTranslated(-21, 5, 15);

	drawParallelepiped(0.25, 4, 0.25, GL_POLYGON);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-22, 5, 15);
	drawParallelepiped(0.25, 4, 0.25, GL_POLYGON);
	glPopMatrix();



	//legs
		glPushMatrix();
		glTranslated(-20, 1.25, 15);
		drawParallelepiped(0.5, 2.5, 0.5, GL_POLYGON);
		glPopMatrix();
		glPushMatrix();
		glTranslated(-23, 1.25, 15);
		drawParallelepiped(0.5, 2.5, 0.5, GL_POLYGON);
		glPopMatrix();
		glPushMatrix();
		glTranslated(-20, 1.25, 18);
		drawParallelepiped(0.5, 2.5, 0.5, GL_POLYGON);
		glPopMatrix();
		glPushMatrix();
		glTranslated(-23, 1.25, 18);
		drawParallelepiped(0.5, 2.5, 0.5, GL_POLYGON);
		glPopMatrix();


	//brown_back
	glPushMatrix();
	glTranslated(-23, 5, 15);
	if (!for_shadow)
		glColor3fv(wood_brown);
	drawParallelepiped(0.25, 4, 0.25, GL_POLYGON);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-21.5, 7, 15);
	drawParallelepiped(3.25, 0.25, 0.25, GL_POLYGON);
	glPopMatrix();

}

GLvoid show_bookshelf() {
	//back
	glPushMatrix();
	glTranslated(-29, 7.5, -5);
	drawParallelepiped(0.5, 15, 9, GL_POLYGON);
	glPopMatrix();

	//left
	glPushMatrix();
	glTranslated(-27.25, 7.5, -0.75);
	drawParallelepiped(3, 15, 0.5, GL_POLYGON);
	glPopMatrix();

	//right
	glPushMatrix();
	glTranslated(-27.25, 7.5, -9.25);
	drawParallelepiped(3, 15, 0.5, GL_POLYGON);
	glPopMatrix();

	//bottom
	glPushMatrix();
	glTranslated(-27.5, 0.25, -5);
	drawParallelepiped(3.5, 0.5, 9, GL_POLYGON);
	glPopMatrix();

	//top
	glPushMatrix();
	glTranslated(-27.5, 15.25, -5);
	drawParallelepiped(3.5, 0.5, 9, GL_POLYGON);
	glPopMatrix();

	if (!for_shadow)
		glColor3fv(wood_beige);
	//second from bottom
	glPushMatrix();
	glTranslated(-27.5, 3.25, -5);
	drawParallelepiped(3.45, 0.5, 8.7, GL_POLYGON);
	glPopMatrix();

	//second from bottom
	glPushMatrix();
	glTranslated(-27.5, 6.25, -5);
	drawParallelepiped(3.45, 0.5, 8.7, GL_POLYGON);
	glPopMatrix();

	//second from top
	glPushMatrix();
	glTranslated(-27.5, 9.25, -5);
	drawParallelepiped(3.45, 0.5, 8.7, GL_POLYGON);
	glPopMatrix();

	//first from top
	glPushMatrix();
	glTranslated(-27.5, 12.25, -5);
	drawParallelepiped(3.45, 0.5, 8.7, GL_POLYGON);
	glPopMatrix();

//	GLfloat specular[] = {1,1,1,1};
	//glMaterialfv(GL_FRONT, GL_SPECULAR,  specular);
	GLfloat book_blue[] = {46.0/255, 65.0/255, 114.0/255};
	//show the books

	if (!for_shadow)
		glColor3fv(book_blue);
	glPushMatrix();
	glTranslated(-27.5, 13.5, -1.2);
	drawParallelepiped(2, 2, 0.5, GL_POLYGON);
	glTranslated(1, 1, 0);
	glRotated(90.0, 1, 0, 0);
	GLUquadricObj *quadObj = gluNewQuadric();
	gluCylinder(quadObj, 0.25, 0.25, 2, 10, 10);
	glPopMatrix();



	GLfloat book_red[] = {170.0/255, 14.0/255, 14.0/255};
	if (!for_shadow)
		glColor3fv(book_red);
	for(float i = 0.5; i <= 2; i += 0.5) {
		glPushMatrix();
		glTranslated(-27.5, 13.5, -1.2 - i);
		drawParallelepiped(2, 2, 0.5, GL_POLYGON);
		glTranslated(1, 1, 0);
		glRotated(90.0, 1, 0, 0);
		gluCylinder(quadObj, 0.25, 0.25, 2, 10, 10);
		glPopMatrix();
	}

	if (!for_shadow)
		glColor3fv(book_blue);
	for(float i = 0.5; i <= 1.5; i += 0.5) {
		glPushMatrix();
		glTranslated(-27.5, 13.5, -3.2 - i);
		drawParallelepiped(2, 2, 0.5, GL_POLYGON);
		glTranslated(1, 1, 0);
		glRotated(90.0, 1, 0, 0);
		gluCylinder(quadObj, 0.25, 0.25, 2, 10, 10);
		glPopMatrix();
	}

	GLfloat book_green[] = {38.0/255, 85.0/255, 36.0/255};
	if (!for_shadow)
		glColor3fv(book_green);
	for(float i = 0.5; i <= 1; i += 0.5) {
		glPushMatrix();
		glTranslated(-27.5, 13.5, -4.7 - i);
		drawParallelepiped(2, 2, 0.5, GL_POLYGON);
		glTranslated(1, 1, 0);
		glRotated(90.0, 1, 0, 0);
		gluCylinder(quadObj, 0.25, 0.25, 2, 10, 10);
		glPopMatrix();
	}

	if (!for_shadow)
		glColor3fv(book_red);
	glPushMatrix();
	glTranslated(-27.5, 13.5, -6.2);
	drawParallelepiped(2, 2, 0.5, GL_POLYGON);
	glTranslated(1, 1, 0);
	glRotated(90.0, 1, 0, 0);
	gluCylinder(quadObj, 0.25, 0.25, 2, 10, 10);
	glPopMatrix();

	GLfloat teapot_gold[] = {0.8, 0.7, 0.3};
	if (!for_shadow)
		glColor3fv(teapot_gold);
	glPushMatrix();
	glTranslated(-27.5, 13.1, -7.8);
	glRotated(-90.0, 0, 1, 0);
	glutSolidTeapot(0.8);
	glPopMatrix();

	GLfloat book_gb[]={33.0/255, 95.0/255, 95.0/255};
	vector<GLfloat*> v;
	v.push_back(book_red);
	v.push_back(book_green);
	v.push_back(book_green);
	v.push_back(book_blue);
	v.push_back(book_green);
	v.push_back(book_red);
	v.push_back(book_green);
	v.push_back(book_green);
	v.push_back(book_red);
	v.push_back(book_gb);
	v.push_back(book_gb);

	float i = 9;
	for (int j = 0; j <= 10; i-=0.5, j++) {
		if (!for_shadow)
			glColor3fv(v[j]);
		glPushMatrix();
		glTranslated(-27.5, 10.5, -i);
		drawParallelepiped(2, 2, 0.5, GL_POLYGON);
		glTranslated(1, 1, 0);
		glRotated(90.0, 1, 0, 0);
		gluCylinder(quadObj, 0.25, 0.25, 2, 10, 10);
		glPopMatrix();
	}

	GLfloat book_yellow[] = {1.0, 164.0/255, 0};
	if (!for_shadow)
		glColor3fv(book_red);
	glPushMatrix();
	glTranslated(-27.5, 10.5, -1.2);
	drawParallelepiped(2, 2, 0.5, GL_POLYGON);
	glTranslated(1, 1, 0);
	glRotated(90.0, 1, 0, 0);
	gluCylinder(quadObj, 0.25, 0.25, 2, 10, 10);
	glPopMatrix();

	if (!for_shadow)
		glColor3fv(book_yellow);
	glPushMatrix();
	glTranslated(-27.5, 10.5, -1.7);
	drawParallelepiped(2, 2, 0.5, GL_POLYGON);
	glTranslated(1, 1, 0);
	glRotated(90.0, 1, 0, 0);
	gluCylinder(quadObj, 0.25, 0.25, 2, 10, 10);
	glPopMatrix();

	if (!for_shadow)
		glColor3fv(book_blue);
	glPushMatrix();
	glTranslated(-27.5, 10.3, -2.7);
	glRotated(45.0, 1, 0,0);
	drawParallelepiped(2, 2, 0.5, GL_POLYGON);
	glTranslated(1, 1, 0);
	glRotated(90.0, 1, 0, 0);
	gluCylinder(quadObj, 0.25, 0.25, 2, 10, 10);
	glPopMatrix();



}

GLfloat leftwallshadow[4][4];
GLfloat floorshadow[4][4];
GLfloat backwallshadow[4][4];
GLfloat rightwallshadow[4][4];
GLfloat frontwallshadow[4][4];

enum {X, Y, Z, W};

void shadowmatrix(GLfloat shadowMat[4][4],  GLfloat groundplane[4],  GLfloat lightpos[4])
{
  GLfloat dot;

  /* find dot product between light position vector and ground plane normal */
  dot = groundplane[X] * lightpos[X] +
    groundplane[Y] * lightpos[Y] +
    groundplane[Z] * lightpos[Z] +
    groundplane[W] * lightpos[W];

  shadowMat[0][0] = dot - lightpos[X] * groundplane[X];
  shadowMat[1][0] = 0.f - lightpos[X] * groundplane[Y];
  shadowMat[2][0] = 0.f - lightpos[X] * groundplane[Z];
  shadowMat[3][0] = 0.f - lightpos[X] * groundplane[W];

  shadowMat[X][1] = 0.f - lightpos[Y] * groundplane[X];
  shadowMat[1][1] = dot - lightpos[Y] * groundplane[Y];
  shadowMat[2][1] = 0.f - lightpos[Y] * groundplane[Z];
  shadowMat[3][1] = 0.f - lightpos[Y] * groundplane[W];

  shadowMat[X][2] = 0.f - lightpos[Z] * groundplane[X];
  shadowMat[1][2] = 0.f - lightpos[Z] * groundplane[Y];
  shadowMat[2][2] = dot - lightpos[Z] * groundplane[Z];
  shadowMat[3][2] = 0.f - lightpos[Z] * groundplane[W];

  shadowMat[X][3] = 0.f - lightpos[W] * groundplane[X];
  shadowMat[1][3] = 0.f - lightpos[W] * groundplane[Y];
  shadowMat[2][3] = 0.f - lightpos[W] * groundplane[Z];
  shadowMat[3][3] = dot - lightpos[W] * groundplane[W];

}

enum {A, B, C, D};

/* find the plane equation given 3 points */
void findplane(GLfloat plane[4], GLfloat v0[3], GLfloat v1[3], GLfloat v2[3])
{
  GLfloat vec0[3], vec1[3];

  /* need 2 vectors to find cross product */
  vec0[X] = v1[X] - v0[X];
  vec0[Y] = v1[Y] - v0[Y];
  vec0[Z] = v1[Z] - v0[Z];

  vec1[X] = v2[X] - v0[X];
  vec1[Y] = v2[Y] - v0[Y];
  vec1[Z] = v2[Z] - v0[Z];

  /* find cross product to get A, B, and C of plane equation */
  plane[A] = vec0[Y] * vec1[Z] - vec0[Z] * vec1[Y];
  plane[B] = -(vec0[X] * vec1[Z] - vec0[Z] * vec1[X]);
  plane[C] = vec0[X] * vec1[Y] - vec0[Y] * vec1[X];

  plane[D] = -(plane[A] * v0[X] + plane[B] * v0[Y] + plane[C] * v0[Z]);
}



GLvoid compute_shadows() {
	  //shadow for the floor
	  GLfloat v0[3], v1[3], v2[3];
	  v0[X] = 30.0f;
	  v0[Y] = 0.0f;
	  v0[Z] = 30.0f;
	  v1[X] = 30.0f;
	  v1[Y] = 0.0f;
	  v1[Z] = -30.0f;
	  v2[X] = -30.0f;
	  v2[Y] = 0.0f;
	  v2[Z] = -30.0f;
	  GLfloat plane[4];

	  findplane(plane, v0, v1, v2);
	  shadowmatrix(floorshadow, plane, light_position);

	  //shadow for the left wall
	  v0[X] = 30.0f;
	  v0[Y] = 0.0f;
	  v0[Z] = 30.0f;
	  v1[X] = 30.0f;
	  v1[Y] = 30.0f;
	  v1[Z] = 30.0f;
	  v2[X] = 30.0f;
	  v2[Y] = 30.0f;
	  v2[Z] = -30.0f;

	  findplane(plane, v0, v1, v2);
	  shadowmatrix(leftwallshadow, plane, light_position);

	  //shadow for the right wall
	  v0[X] = -30.0f;
	  v0[Y] = 0.0f;
	  v0[Z] = 30.0f;
	  v1[X] = -30.0f;
	  v1[Y] = 0.0f;
	  v1[Z] = -30.0f;
	  v2[X] = -30.0f;
	  v2[Y] = 30.0f;
	  v2[Z] = -30.0f;

	  findplane(plane, v0, v1, v2);
	  shadowmatrix(rightwallshadow, plane, light_position);

	  //shadow for the back wall
	  v0[X] = 30.0f;
	  v0[Y] = 0.0f;
	  v0[Z] = 30.0f;
	  v1[X] = -30.0f;
	  v1[Y] = 0.0f;
	  v1[Z] = 30.0f;
	  v2[X] = -30.0f;
	  v2[Y] = 30.0f;
	  v2[Z] = 30.0f;

	  findplane(plane, v0, v1, v2);
	  shadowmatrix(backwallshadow, plane, light_position);

	  //shadow for the front wall
	  v0[X] = 30.0f;
	  v0[Y] = 0.0f;
	  v0[Z] = -30.0f;
	  v1[X] = 30.0f;
	  v1[Y] = 30.0f;
	  v1[Z] = -30.0f;
	  v2[X] = -30.0f;
	  v2[Y] = 30.0f;
	  v2[Z] = -30.0f;

	  findplane(plane, v0, v1, v2);
	  shadowmatrix(frontwallshadow, plane, light_position);

}

GLvoid display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	compute_shadows();
	//Using glFrustum
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//	gluLookAt(camera.origin.x, camera.origin.y, camera.origin.z, centerX, centerY, centerZ, camera.Oy.x, camera.Oy.y, camera.Oy.z);
//
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//
//
//	glFrustum(frustum_left, frustum_right, frustum_bottom, frustum_top, frustum_dnear, frustum_dfar);
//	//gluPerspective(100, 10, 40,50);

	//Not using glFrustum
	glLoadIdentity();

	glTranslated(xpos , 0, zpos);
	glRotated(pitch, 1, 0, 0);
	glRotated(yaw, 0, 1, 0);
	// Create light components
	GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuseLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat specularLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };

	// Assign created components to GL_LIGHT0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	show_floor();
	show_axes();
	draw_right_wall();
	draw_left_wall();
	draw_front_wall();
	draw_back_wall();
	draw_ceiling();
	//shadows
	if (shadows) {
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		//Updating the stencil buffer
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xffffffff);
		show_floor();
		//Reenable colors
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glColor4f(0.f, 0.f, 0.0f, 0.8f);
		glStencilFunc(GL_EQUAL, 1, 0xffffffff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glPushMatrix();
		glMultMatrixf((GLfloat *) floorshadow);
		for_shadow = true;
		 glEnable (GL_BLEND);
		 glDepthMask (GL_FALSE);
		 glBlendFunc (GL_SRC_ALPHA,  GL_ONE_MINUS_SRC_ALPHA);
		show_bed();
		show_desk();
		show_garbage_bin();
		show_chair();
		show_bookshelf();
		for_shadow = false;
		glPopMatrix();
		glDisable(GL_STENCIL_TEST);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glDepthMask (GL_TRUE);
		glDisable (GL_BLEND);
	}


	 if (shadows && left_wall) {
		 glClear(GL_STENCIL_BUFFER_BIT);
		 glDisable(GL_DEPTH_TEST);
		 glDisable(GL_LIGHTING);
		 glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		 //Updating the stencil buffer
		 glEnable(GL_STENCIL_TEST);
		 glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		 glStencilFunc(GL_ALWAYS, 1, 0xffffffff);
		 draw_left_wall();
		 //Reenable colors
		 glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		 glColor4f(0.f, 0.f, 0.0f, 0.8f);
		 glStencilFunc(GL_EQUAL, 1, 0xffffffff);
		 glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		 glPushMatrix();
		 glMultMatrixf((GLfloat *) leftwallshadow);
		 for_shadow = true;
		 glEnable (GL_BLEND);
		 glDepthMask (GL_FALSE);
		 glBlendFunc (GL_SRC_ALPHA,  GL_ONE_MINUS_SRC_ALPHA);
		 show_bed();
		 show_desk();
		 show_garbage_bin();
		 show_chair();
		 show_bookshelf();
		 for_shadow = false;
		 glPopMatrix();
		 glDisable(GL_STENCIL_TEST);
		 glEnable(GL_DEPTH_TEST);
		 glEnable(GL_LIGHTING);
		 glDepthMask (GL_TRUE);
         glDisable (GL_BLEND);
	  }

	 if (shadows && right_wall) {
		 glClear(GL_STENCIL_BUFFER_BIT);
	 	 glDisable(GL_DEPTH_TEST);
	 	 glDisable(GL_LIGHTING);
	 	 glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	 	 //Updating the stencil buffer
	 	 glEnable(GL_STENCIL_TEST);
	 	 glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	 	 glStencilFunc(GL_ALWAYS, 1, 0xffffffff);
	 	 draw_right_wall();
	 	 //Reenable colors
	 	 glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	 	 glColor4f(0.f, 0.f, 0.0f, 0.8f);
	 	 glStencilFunc(GL_EQUAL, 1, 0xffffffff);
	 	 glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	 	 glPushMatrix();
	 	 glMultMatrixf((GLfloat *) rightwallshadow);
	 	 for_shadow = true;
	 	 glEnable (GL_BLEND);
	 	 glDepthMask (GL_FALSE);
	 	 glBlendFunc (GL_SRC_ALPHA,  GL_ONE_MINUS_SRC_ALPHA);
	 	 show_bed();
	 	 show_desk();
	 	 show_garbage_bin();
	 	 show_chair();
	 	 show_bookshelf();
	 	 for_shadow = false;
	 	 glPopMatrix();
	 	 glDisable(GL_STENCIL_TEST);
	 	 glEnable(GL_DEPTH_TEST);
	 	 glEnable(GL_LIGHTING);
	 	 glDepthMask (GL_TRUE);
	     glDisable (GL_BLEND);
	  }

	 if (shadows && back_wall) {
		 glClear(GL_STENCIL_BUFFER_BIT);
	 	 glDisable(GL_DEPTH_TEST);
	 	 glDisable(GL_LIGHTING);
	 	 glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	 	 //Updating the stencil buffer
	 	 glEnable(GL_STENCIL_TEST);
	 	 glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	 	 glStencilFunc(GL_ALWAYS, 1, 0xffffffff);
	 	 draw_back_wall();
	 	 //Reenable colors
	 	 glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	 	 glColor4f(0.f, 0.f, 0.0f, 0.8f);
	 	 glStencilFunc(GL_EQUAL, 1, 0xffffffff);
	 	 glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	 	 glPushMatrix();
	 	 glMultMatrixf((GLfloat *) backwallshadow);
	 	 for_shadow = true;
	 	 glEnable (GL_BLEND);
	 	 glDepthMask (GL_FALSE);
	 	 glBlendFunc (GL_SRC_ALPHA,  GL_ONE_MINUS_SRC_ALPHA);
	 	 show_bed();
	 	 show_desk();
	 	 show_garbage_bin();
	 	 show_chair();
	 	 show_bookshelf();
	 	 for_shadow = false;
	 	 glPopMatrix();
	 	 glDisable(GL_STENCIL_TEST);
	 	 glEnable(GL_DEPTH_TEST);
	 	 glEnable(GL_LIGHTING);
	 	 glDepthMask (GL_TRUE);
	 	 glDisable (GL_BLEND);
	  }

	 if (shadows && front_wall) {
		glClear(GL_STENCIL_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		//Updating the stencil buffer
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xffffffff);
		draw_front_wall();
		//Reenable colors
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glColor4f(0.f, 0.f, 0.0f, 0.8f);
		glStencilFunc(GL_EQUAL, 1, 0xffffffff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glPushMatrix();
		glMultMatrixf((GLfloat *) frontwallshadow);
		for_shadow = true;
		glEnable (GL_BLEND);
		glDepthMask (GL_FALSE);
		glBlendFunc (GL_SRC_ALPHA,  GL_ONE_MINUS_SRC_ALPHA);
		show_bed();
		show_desk();
		show_garbage_bin();
		show_chair();
		show_bookshelf();
		for_shadow = false;
		glPopMatrix();
		glDisable(GL_STENCIL_TEST);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glDepthMask (GL_TRUE);
	    glDisable (GL_BLEND);
	 }

	 show_axes();
	 		show_bed();
	 		show_desk();
	 		show_garbage_bin();
	 		show_chair();
	 		show_bookshelf();

	GLfloat mat_specular[] = { 1, 1, 1, 1.0 };
	GLfloat mat_shininess[] = { 100 };
		//GLfloat mat_emission[] = { 0.0 };
		//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);
//		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
//		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	//sphere for light test
	glPushMatrix();
	glTranslated(0, 3, 0);
	gluSphere(gluNewQuadric(), 3, 32, 32);
	glPopMatrix();
	//glFlush();
	glutSwapBuffers();

}


GLvoid callback_mouse(GLint button, GLint state, GLint x, GLint y) {
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
		mouseX = x;
		mouseY = y;
	}
}

GLvoid translate(Point &a, Point source, Point dest) {
	a.x = a.x + (dest.x - source.x);
	a.y = a.y + (dest.y - source.y);
	a.z = a.z + (dest.z - source.z);
}

void update(int value) {
	Point centerFocus;
	computeCameraSystem();
	centerFocus.x = camera.origin.x;
	centerFocus.y = camera.origin.y;
	centerFocus.z = camera.origin.z;
	Point center;
	center.x = centerX;
	center.y = centerY;
	center.z = centerZ;
	translate(centerFocus, center, camera.origin);
	rotate(yaw, camera.Oy, centerFocus);
	rotate(pitch, camera.Ox, centerFocus);
	translate(centerFocus, camera.origin, center);
	Point origin;
	origin.x = 0.0f;
	origin.y = 0.0f;
	origin.z = 0.0f;
	camera.origin = centerFocus;
	glutPostRedisplay();
	//glutTimerFunc(500, update,0);
}


GLvoid callback_motion(GLint x, GLint y) {
	pitch += (y - mouseY );///10000;
	yaw += (x - mouseX);///10000;
	mouseX = x;
	mouseY = y;
	glutPostRedisplay();
	//update(0);
}


GLvoid zoom_image() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f *zoom,aspect, 0.1f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
}

GLvoid callback_reshape(GLsizei width, GLsizei height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	aspect = (GLfloat)width/(GLfloat)height;
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height, 0.1f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
}

GLvoid callback_keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case '1': {

			if (left_wall)
				left_wall = false;
			else left_wall = true;
			glutPostRedisplay();
			break;
		}
		case '2': {
			if (back_wall)
				back_wall = false;
			else back_wall = true;
			glutPostRedisplay();
			break;
		}
		case '3': {
			if (right_wall)
				right_wall = false;
			else right_wall = true;
			glutPostRedisplay();
			break;
		}
		case '4': {
			if (front_wall)
				front_wall = false;
			else front_wall = true;
			glutPostRedisplay();
			break;
		}
		case '5': {
			if (ceiling)
				ceiling = false;
			else ceiling = true;
			glutPostRedisplay();
			break;
		}
		case 'o': {
			if (light_enabled) {
				light_enabled = false;
				glDisable(GL_LIGHT0);
 			} else {
 				light_enabled = true;
 				glEnable(GL_LIGHT0);
 			}
			glutPostRedisplay();
			break;
		}
		case 'f': {
			if (fog_enabled) {
				fog_enabled = false;
				glDisable(GL_FOG);
			} else {
				fog_enabled = true;
				glEnable(GL_FOG); {

				}
			}
			glutPostRedisplay();
			break;
		}
		case '-': {
			zoom = zoom + zoom_factor;
			zoom_image();
			glutPostRedisplay();
			break;
		}
		case '+': {
			zoom = zoom - zoom_factor;
			zoom_image();
			glutPostRedisplay();
			break;
		}
		case 'w':{
			//moves the light source towards positive Oz
			light_position[2] ++;
			glutPostRedisplay();
			break;
		}
		case 'a': {
			//moves the light source towards positive Ox
			light_position[0] ++;
			glutPostRedisplay();
			break;
		}
		case 's': {
			//moves the light source towards negative Oz
			light_position[2]--;
			glutPostRedisplay();
			break;
		}
		case 'd': {
			//moves the light source towards negative Ox
			light_position[0]--;
			glutPostRedisplay();
			break;
		}
		case 'u': {
			//activates or deactivates shadows
			shadows = !shadows;
			glutPostRedisplay();
			break;
		}

	}
}

int main(int argc, char *argv[])
{
	//glut initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH |  GLUT_STENCIL );
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Room");

	init();
	glutDisplayFunc(display);
	glutReshapeFunc(callback_reshape);
//	glutTimerFunc(500, update,0);

	glutMouseFunc(callback_mouse);
	glutKeyboardFunc(callback_keyboard);
	glutMotionFunc(callback_motion);
	glutMainLoop();

	return 0;
}
