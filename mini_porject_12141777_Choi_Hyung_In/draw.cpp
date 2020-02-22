#include <Windows.h>
#include <MMSystem.h>
#include "draw.h"
#include "bmpfuncs.h"
#pragma warning(disable:4996)
#pragma comment(lib,"Winmm.lib")
vector <missile_trace> missile_trace_list;
vector <target> target_list;
GLuint* tex_map = new GLuint();
GLuint* tex_sample = new GLuint();
GLuint* tex_base = new GLuint();
GLuint* tex_view = new GLuint();
GLuint* tex_sky = new GLuint();
GLUquadricObj* qobj_ = gluNewQuadric();
GLuint g_nCubeTex;
/*==============================================================================================*
function : mapTextureMapping
desc     : 텍스처 매핑관련 설정
*==============================================================================================*/
void mapTextureMapping() {
	int imgW, imgH, ch;
	glGenTextures(1, tex_map);
	glBindTexture(GL_TEXTURE_2D, *tex_map);
	uchar* img = readImageData("./texture/map.bmp", &imgW, &imgH, &ch);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgW, imgH, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glGenTextures(1, tex_view);
	glBindTexture(GL_TEXTURE_2D, *tex_view);
	img = readImageData("./texture/tex_view_.bmp", &imgW, &imgH, &ch);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgW, imgH, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glGenTextures(1, tex_base);
	glBindTexture(GL_TEXTURE_2D, *tex_base);
	img = readImageData("./texture/base.bmp", &imgW, &imgH, &ch);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgW, imgH, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glGenTextures(1, tex_sky);
	glBindTexture(GL_TEXTURE_2D, *tex_sky);
	img = readImageData("./texture/tex_sky.bmp", &imgW, &imgH, &ch);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgW, imgH, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glGenTextures(1, tex_sample);
	glBindTexture(GL_TEXTURE_2D, *tex_sample);
	img = readImageData("./texture/sample.bmp", &imgW, &imgH, &ch);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgW, imgH, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_2D);

	gluQuadricTexture(qobj_, GL_TRUE);
	gluQuadricOrientation(qobj_, GLU_INSIDE);
}
/*==============================================================================================*
function : draw_base
desc     : 조립 모드에서 출력될 바닥면을 출력
*==============================================================================================*/
void draw_base(void) {
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
		glVertex3f(-400,-3, -400);
		glVertex3f(-400,-3, 400);
		glVertex3f(400, -3, 400);
		glVertex3f(400, -3, -400);
	glEnd();
}
/*==============================================================================================*
function : draw_skybox
desc     : 비행 모드에서 출력될 배경 텍스처를 출력
*==============================================================================================*/
void draw_skybox() {
	glColor3f(1.0, 1.0, 1.0);	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glDisable(GL_LIGHTING);
	glPushMatrix();
		glTranslatef(0, -400, 0);
		glColor3f(1.0, 1.0, 1.0);
		gluQuadricDrawStyle(qobj_, GLU_FILL);
		glRotatef(-90, 1, 0, 0);
		glBindTexture(GL_TEXTURE_2D, *tex_view);
		//cylinder로 출력
		gluCylinder(qobj_, 1000, 1000, 1000, 200, 200);
	glPopMatrix();
	
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, *tex_base);
	glBegin(GL_QUADS);
		glNormal3f(1, 0, 0);
		glTexCoord2f(0, 0); glVertex3f(-1000.0f, 0.0f,  1000.0f); // C
		glTexCoord2f(0, 1); glVertex3f( 1000.0f, 0.0f,  1000.0f); // B
		glTexCoord2f(1, 1); glVertex3f( 1000.0f, 0.0f, -1000.0f); // G
		glTexCoord2f(1, 0); glVertex3f(-1000.0f, 0.0f, -1000.0f); // H
	glEnd();

	glRotatef(180, 0, 1, 0);
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, *tex_sky);
	glBegin(GL_QUADS);
	glNormal3f(0, -1, 0);
	glTexCoord2f(0, 0); glVertex3f(-1000.0f, 600.0f, 1000.0f); // C
	glTexCoord2f(0, 1); glVertex3f(1000.0f,  600.0f, 1000.0f); // B
	glTexCoord2f(1, 1); glVertex3f(1000.0f,  600.0f, -1000.0f); // G
	glTexCoord2f(1, 0); glVertex3f(-1000.0f, 600.0f, -1000.0f); // H
	glEnd();
	glRotatef(-180, 0, 1, 0);

	glBindTexture(GL_TEXTURE_2D, *tex_sample);
	glPushMatrix();
		glTranslatef(100, 0, 100);
		glColor3f(0.9, 0, 0.9);
		glutSolidTeapot(0.5);
	glPopMatrix();

	glEnable(GL_LIGHTING);
}

void draw_map(void) {

	glBindTexture(GL_TEXTURE_2D, *tex_map);
	glBegin(GL_QUADS);
		glColor3f(1, 1, 1);
		glTexCoord2f(0, 0); glVertex3f(-400, 0, -400);
		glTexCoord2f(1, 0); glVertex3f(-400, 0, 400);
		glTexCoord2f(1, 1); glVertex3f(400, 0, 400);
		glTexCoord2f(0, 1); glVertex3f(400, 0, -400);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, *tex_sample);
	glPushMatrix();
		glTranslatef(100, 0, 100);
		glColor3f(0.9, 0, 0.9);
		glutSolidTeapot(0.5);
	glPopMatrix();
}

void draw_axis(void) {
	glLineWidth(3);
	glBegin(GL_LINES);
		glColor3f(1, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(7, 0, 0);

		glColor3f(0, 1, 0);
		glVertex3f(0, -7, 0);
		glVertex3f(0, 7, 0);

		glColor3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 7);
	glEnd();
	glLineWidth(1);
}
/*==============================================================================================*
function : check_rgb
desc     : color num을 받아 RGB 값을 반환해줌
*==============================================================================================*/
color_rgb check_rgb(int color) {
	color_rgb result;
	switch (color) {
	case 0: // Red
		result.color_r = 0.7f;
		result.color_g = 0.0f;
		result.color_b = 0.0f;
		return result;
		break;
	case 1: // Green
		result.color_r = 0.0f;
		result.color_g = 0.7f;
		result.color_b = 0.0f;
		return result;
		break;
	case 2: // Blue
		result.color_r = 0.0f;
		result.color_g = 0.0f;
		result.color_b = 0.7f;
		return result;
		break;
	case 3: // Cyan
		result.color_r = 0.0f;
		result.color_g = 0.7f;
		result.color_b = 0.7f;
		return result;
		break;
	case 4: // Yellow
		result.color_r = 0.7f;
		result.color_g = 0.7f;
		result.color_b = 0.0f;
		return result;
		break;
	case 5: // Magenta
		result.color_r = 0.7f;
		result.color_g = 0.0f;
		result.color_b = 0.7f;
		return result;
		break;
	case 6: // White
		result.color_r = 1.0f;
		result.color_g = 1.0f;
		result.color_b = 1.0f;
		return result;
		break;
	case 7: // Black
		result.color_r = 0.2f;
		result.color_g = 0.2f;
		result.color_b = 0.2f;
		return result;
		break;
	default:
		result.color_r = 0.6f;
		result.color_g = 0.6f;
		result.color_b = 0.0f;
		return result;
		break;
	}
}
/*==============================================================================================*
function : draw_block_1
desc     : BLOCK_THICK_2_4을 출력
*==============================================================================================*/
void draw_block_1(int color) {
	color_rgb temp_color;
	glPushMatrix();
		glTranslatef(0, 1.5, 0);
		temp_color = check_rgb(color);
		glColor3f(temp_color.color_r, temp_color.color_g, temp_color.color_b);
		glScalef(2.6666, 1, 1.3333);
		glutSolidCube(3.0f);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0, 1.5, 0);
		glColor3f(0.2f, 0.2f, 0.2f);
		glScalef(8, 3, 4);
		glutWireCube(1.0f);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(3, 3.3, 1);
		glRotatef(90, 1, 0, 0);
		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		gluQuadricDrawStyle(qobj, GLU_FILL);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);

		glTranslatef(0, -2, 0);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		; glRotatef(180, 1, 0, 0); gluDisk(qobj, 0, 0.7, 20, 20); glRotatef(180, 1, 0, 0);
		for (int i = 0; i < 3; i++)
		{
			glTranslatef(-2, 2, 0);
			gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
			
			glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);
			

			glTranslatef(0, -2, 0);
			gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
			
			glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);
		}
	glPopMatrix();
}
/*==============================================================================================*
function : draw_block_2
desc     : BLOCK_THICK_2_2을 출력
*==============================================================================================*/
void draw_block_2(int color) {
	color_rgb temp_color;
	glPushMatrix();
		glTranslatef(0, 1.5, 0);
		temp_color = check_rgb(color);
		glColor3f(temp_color.color_r, temp_color.color_g, temp_color.color_b);
		glScalef(1.3333, 1, 1.3333);
		glutSolidCube(3.0f);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0, 1.5, 0);
		glColor3f(0.2f, 0.2f, 0.2f);
		glScalef(4, 3, 4);
		glutWireCube(1.0f);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(1, 3.3, 1);
		glRotatef(90, 1, 0, 0);
		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		gluQuadricDrawStyle(qobj, GLU_FILL);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);

		glTranslatef(0, -2, 0);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);

		glTranslatef(-2, 2, 0);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);

		glTranslatef(0, -2, 0);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);
	glPopMatrix();
}
/*==============================================================================================*
function : draw_block_3
desc     : BLOCK_WHEEL을 출력
*==============================================================================================*/
void draw_block_3(int color) {
	color_rgb temp_color;
	glPushMatrix();
		glTranslatef(0, 2.75, 0);
		temp_color = check_rgb(color);
		glColor3f(temp_color.color_r, temp_color.color_g, temp_color.color_b);
		glScalef(8, 1, 8);
		glutSolidCube(0.5f);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0, 2.75, 0);
		glColor3f(0.2f, 0.2f, 0.2f);
		glScalef(4, 0.5, 4);
		glutWireCube(1.0f);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(1, 3.3, 1);
		glRotatef(90, 1, 0, 0);
		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		gluQuadricDrawStyle(qobj, GLU_FILL);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);
		glTranslatef(-2, 0, 0);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);

		glTranslatef(0, -2, 0);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);

		glTranslatef(2, 0, 0);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);

		glTranslatef(-2, 1, 0);

		glPushMatrix();
			glRotatef(90, 1, 0, 0);
			glTranslatef(1, 2, 0);
			glutSolidTorus(0.5, 0.8, 20, 20);

			glColor3f(0.9, 0.9, 0.9);
			glTranslatef(0, 0, 0.2);
			gluDisk(qobj, 0, 0.5, 20, 20);
			glTranslatef(0, 0, -0.4);
			gluDisk(qobj, 0, 0.5, 20, 20);
			glTranslatef(0, 0, -0.6);
			glColor3f(0.2, 0.2, 0.2);
			gluCylinder(qobj, 0.2, 0.2, 1.1, 20, 20);
			gluDisk(qobj, 0, 0.2, 20, 20);
			glTranslatef(0, 0, 1.1);
			gluDisk(qobj, 0, 0.2, 20, 20);
			glColor3f(0.4, 0.4, 0.4);
			
			glTranslatef(0, -0.5, -1);
			glPushMatrix();
				glScalef(5, 18, 1.2);
				glutSolidCube(0.1f);
			glPopMatrix();
			glRotatef(40, 0, 0, 1);
			glTranslatef(0.3, -0.7, 0);
			glPushMatrix();
				glScalef(3, 25, 1.1);
				glutSolidCube(0.1f);
			glPopMatrix();

		glPopMatrix();
	glPopMatrix();
}
/*==============================================================================================*
function : draw_block_4
desc     : BLOCK_FLAT_2_4을 출력
*==============================================================================================*/
void draw_block_4(int color) {
	color_rgb temp_color;
	glPushMatrix();
		glTranslatef(0, 1.5, 0);
		temp_color = check_rgb(color);
		glColor3f(temp_color.color_r, temp_color.color_g, temp_color.color_b);
		glScalef(8, 1, 4);
		glutSolidCube(1.0f);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0, 1.5, 0);
		glColor3f(0.2f, 0.2f, 0.2f);
		glScalef(8, 1, 4);
		glutWireCube(1.0f);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(3, 2.3, 1);
		glRotatef(90, 1, 0, 0);
		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		gluQuadricDrawStyle(qobj, GLU_FILL);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);

		glTranslatef(0, -2, 0);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);
		for (int i = 0; i < 3; i++)
		{
			glTranslatef(-2, 2, 0);
			gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
			glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);

			glTranslatef(0, -2, 0);
			gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
			glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);
		}
	glPopMatrix();
}
/*==============================================================================================*
function : draw_block_5
desc     : BLOCK_FLAT_2_8을 출력
*==============================================================================================*/
void draw_block_5(int color) {

	color_rgb temp_color;
	glPushMatrix();
	glTranslatef(0, 1.5, 0);
	temp_color = check_rgb(color);
	glColor3f(temp_color.color_r, temp_color.color_g, temp_color.color_b);
	glScalef(16, 1, 4);
	glutSolidCube(1.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 1.5, 0);
	glColor3f(0.2f, 0.2f, 0.2f);
	glScalef(16, 1, 4);
	glutWireCube(1.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(7, 2.3, 1);
	glRotatef(90, 1, 0, 0);
	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_FILL);
	gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
	glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);

	glTranslatef(0, -2, 0);
	gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
	glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);
	for (int i = 0; i < 7; i++)
	{
		glTranslatef(-2, 2, 0);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);

		glTranslatef(0, -2, 0);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);
	}
	glPopMatrix();
	
}
/*==============================================================================================*
function : draw_block_6
desc     : BLOCK_PROPELLER을 출력
*==============================================================================================*/
void draw_block_6(int color, GLfloat rot) {
	color_rgb temp_color;
	glPushMatrix();
		glTranslatef(0, 1.5, 0);
		temp_color = check_rgb(color);
		glColor3f(temp_color.color_r, temp_color.color_g, temp_color.color_b);
		glScalef(1.3333, 1, 1.3333);
		glutSolidCube(3.0f);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0, 1.5, 0);
		glColor3f(0.2f, 0.2f, 0.2f);
		glScalef(4, 3, 4);
		glutWireCube(1.0f);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(1, 3.3, 1);
		glRotatef(90, 1, 0, 0);
		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		gluQuadricDrawStyle(qobj, GLU_FILL);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);

		glTranslatef(0, -2, 0);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);

		glTranslatef(-2, 2, 0);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);

		glTranslatef(0, -2, 0);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);
	glPopMatrix();
	
	glPushMatrix();
		glLineWidth(1);
		glTranslatef(-2.5, 1.5, 0);
		glPushMatrix();
			glRotatef(90, 0, 1, 0);
			glTranslatef(0, 0, -0.5);
			gluCylinder(qobj, 0.4, 0.4, 1.5, 20, 20);
			gluDisk(qobj, 0, 0.4, 20, 20);
		glPopMatrix();
		glRotatef(90, 0, 0, 1);
		glRotatef(rot, 0, 1, 0);
		glColor3f(1, 0.5, 0);
		glScalef(10, 0.5, 1);
		
		gluCylinder(qobj, 0.5, 0.5, 0.3, 10, 10);
		gluDisk(qobj, 0, 0.5, 10, 10);
	glPopMatrix();
}
/*==============================================================================================*
function : draw_block_7
desc     : BLOCK_HALF_2_4을 출력
*==============================================================================================*/
void draw_block_7(int color) {
	color_rgb temp_color;
	glPushMatrix();
		glTranslatef(3, 0, 0);
		glPushMatrix();
			glTranslatef(0, 1.5, 0);
			temp_color = check_rgb(color);
			glColor3f(temp_color.color_r, temp_color.color_g, temp_color.color_b);
			glScalef(1, 1.5, 2);
			glutSolidCube(2.0f);
		glPopMatrix();

		glPushMatrix();
			glTranslatef(0, 1.5, 0);
			glColor3f(0.2f, 0.2f, 0.2f);
			glScalef(2, 3, 4);
			glutWireCube(1.0f);
		glPopMatrix();

		glTranslatef(0, 3.3 , 1);
		glRotatef(90, 1, 0, 0);
		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		gluQuadricDrawStyle(qobj, GLU_FILL);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);

		glTranslatef(0, -2, 0);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);
	glPopMatrix();

	glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glColor3f(1, 1, 1);
		//glTranslatef(0, 1, 0);
		glLineWidth(3);
		glBegin(GL_POLYGON);
			glVertex3f(2, 3, 2);
			glVertex3f(2, 3, -2);
			glVertex3f(-4, 1, -2);
			glVertex3f(-4, 1, 2);
		glEnd();
		glBegin(GL_POLYGON);
			glVertex3f(2,  3, 2);
			glVertex3f(2,  1, 2);
			glVertex3f(2,  0, 2);
			glVertex3f(-4, 0, 2);
			glVertex3f(-4, 1, 2);
		glEnd();
		glBegin(GL_POLYGON);
			glVertex3f(2,  3, -2);
			glVertex3f(-4, 1, -2);
			glVertex3f(-4, 0, -2);
			glVertex3f(2,  0, -2);
			glVertex3f(2,  1, -2);
		glEnd();
		glBegin(GL_POLYGON);
			glVertex3f(-4, 1, -2);
			glVertex3f(-4, 0, -2);
			glVertex3f(-4, 0, 2);
			glVertex3f(-4, 1, 2);
		glEnd();
	glDisable(GL_BLEND);
	
	glColor3f(0, 0, 0);
	glLineWidth(2);
	glBegin(GL_LINES);
		glVertex3f(2, 3, -2);
		glVertex3f(-4, 1, -2);
	glEnd();

	glBegin(GL_LINES);
		glVertex3f(2, 3, 2);
		glVertex3f(-4, 1, 2);
	glEnd();

	glBegin(GL_LINES);
		glVertex3f(-4, 1, -2);
		glVertex3f(-4, 1, 2);
	glEnd();

	glBegin(GL_LINES);
		glVertex3f(-4, 1, -2);
		glVertex3f(-4, 0, -2);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(-4, 1, 2);
	glVertex3f(-4, 0, 2);
	glEnd();
	glLineWidth(1);
}
/*==============================================================================================*
function : draw_block_8
desc     : BLOCK_MISSILE을 출력
*==============================================================================================*/
void draw_block_8(int color) {
	color_rgb temp_color;
	glPushMatrix();
		glTranslatef(0, 2.5, 0);
		temp_color = check_rgb(color);
		glColor3f(temp_color.color_r, temp_color.color_g, temp_color.color_b);
		glScalef(4, 1, 4);
		glutSolidCube(1.0f);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0, 2.5, 0);
		glColor3f(0.2f, 0.2f, 0.2f);
		glScalef(4, 1, 4);
		glutWireCube(1.0f);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(1, 3.3, 1);
		glRotatef(90, 1, 0, 0);
		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		gluQuadricDrawStyle(qobj, GLU_FILL);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);
		glTranslatef(-2, 0, 0);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);
		glTranslatef(0, -2, 0);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);
		glTranslatef(2, 0, 0);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);

		glPushMatrix();
			glRotatef(90, 0, 1, 0);
			glTranslatef(-1.7, 1, -4);
			gluCylinder(qobj, 0.7, 0.7, 7, 20, 20);
			glRotatef(180, 1, 0, 0);
			glColor3f(0.9, 0.9, 0.9);
			glutSolidCone(0.7, 1.5, 20, 20);
			glTranslatef(0, 0, -2.3);
			gluCylinder(qobj, 0.8, 0.8, 0.25, 20, 20);
			gluDisk(qobj, 0, 0.8, 20, 20);
			glTranslatef(0, 0, 0.25);
			gluDisk(qobj, 0, 0.8, 20, 20);
	
			glTranslatef(0, 0, -3);
			gluCylinder(qobj, 0.8, 0.8, 0.25, 20, 20);
			gluDisk(qobj, 0, 0.8, 20, 20);
			glTranslatef(0, 0, 0.25);
			gluDisk(qobj, 0, 0.8, 20, 20);

			glTranslatef(0, 0, -2);
			glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);
			glPushMatrix();
				glTranslatef(0.8, 0, 0);
				glColor3f(temp_color.color_r, temp_color.color_g, temp_color.color_b);
				glScalef(5, 1, 10);
				glutSolidCube(0.1f);
			glPopMatrix();

			glRotatef(90, 0, 0, 1);
			glPushMatrix();
				glTranslatef(0.8, 0, 0);
				glScalef(5, 1, 10);
				glutSolidCube(0.1f);
			glPopMatrix();

			glRotatef(90, 0, 0, 1);
			glPushMatrix();
				glTranslatef(0.8, 0, 0);
				glScalef(5, 1, 10);
				glutSolidCube(0.1f);
			glPopMatrix();

			glRotatef(90, 0, 0, 1);
			glPushMatrix();
				glTranslatef(0.8, 0, 0);
				glScalef(5, 1, 10);
				glutSolidCube(0.1f);
			glPopMatrix();

		glPopMatrix();
	glPopMatrix();
}
/*==============================================================================================*
function : draw_block_9
desc     : BLOCK_JET_ENGIN을 출력
*==============================================================================================*/
void draw_block_9(int color) {
	color_rgb temp_color;
	glPushMatrix();
		glTranslatef(0, 1.5, 0);
		temp_color = check_rgb(color);
		glColor3f(temp_color.color_r, temp_color.color_g, temp_color.color_b);
		glScalef(1.3333, 1, 1.3333);
		glutSolidCube(3.0f);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0, 1.5, 0);
		glColor3f(0.2f, 0.2f, 0.2f);
		glScalef(4, 3, 4);
		glutWireCube(1.0f);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(1, 3.3, 1);
		glRotatef(90, 1, 0, 0);
		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		gluQuadricDrawStyle(qobj, GLU_FILL);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);

		glTranslatef(0, -2, 0);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);

		glTranslatef(-2, 2, 0);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);

		glTranslatef(0, -2, 0);
		gluCylinder(qobj, 0.7, 0.7, 0.3, 20, 20);
		glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);

		glPushMatrix();
			glRotatef(90, 0, 1, 0);
			glTranslatef(-1.7, 1, 3);
			glScalef(1, 1, 0.6);
			gluCylinder(qobj, 1.5, 1, 3, 8, 8);
			glScalef(1, 1, 1.16666);
			glRotatef(180, 1, 0, 0);
			glRotatef(180, 0, 1, 0);
			glutSolidCone(1, 2.4, 20, 20);
			glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				glTranslatef(0, 0, 1.5);
				glColor3f(0.9, 0, 0);
				glScalef(1, 1, 5);
				gluCylinder(qobj, 1, 0.5, 0.3, 20, 20);
				glScalef(1, 1, 0.2);
				glTranslatef(0, 0, 0.8);
				glColor3f(0.9, 0.7, 0);
				glutSolidCone(0.9, 0.7, 20, 20);
			glDisable(GL_BLEND);
		glPopMatrix();
	glPopMatrix();
}
/*==============================================================================================*
function : draw_block
desc     : 입력받은 블록을 그림
*==============================================================================================*/
void draw_block(int type, int color, GLfloat blc_trans_x, GLfloat blc_trans_y, GLfloat blc_trans_z, GLfloat blc_rot_angle,
	GLfloat blc_rot_x, GLfloat blc_rot_y, GLfloat blc_rot_z, GLfloat propeller_rot) {
	glPushMatrix();
	glTranslatef(blc_trans_x, blc_trans_y, blc_trans_z);
	glRotatef(blc_rot_angle, blc_rot_x, blc_rot_y, blc_rot_z);
	switch (type) {
		case BLOCK_THICK_2_4:
			draw_block_1(color);
			break;
		case BLOCK_THICK_2_2:
			draw_block_2(color);
			break;
		case BLOCK_WHEEL:
			draw_block_3(color);
			break;
		case BLOCK_FLAT_2_4:
			draw_block_4(color);
			break;
		case BLOCK_FLAT_2_8:
			draw_block_5(color);
			break;
		case BLOCK_PROPELLER:
			draw_block_6(color, propeller_rot);
			break;
		case BLOCK_HALF_2_4:
			draw_block_7(color);
			break;
		case BLOCK_MISSILE:
			draw_block_8(color);
			break;
		case BLOCK_JET_ENGIN:
			draw_block_9(color);
			break;
		default:
			draw_block_1(color);
			break;
	}
	glPopMatrix();
}
/*==============================================================================================*
function : launch_missile
desc     : 비행 모드에서 미사일 발사 시 발사 정보를 misile_trace_list에 push
*==============================================================================================*/
void launch_missile(GLfloat trans_x, GLfloat trans_y, GLfloat trans_z, GLfloat rot_angle_y, GLfloat rot_angle_z) {
	int size = block_list.size();
	missile_trace temp;
	float angle_y = rot_angle_y * 3.1416 / 180;
	float angle_z = rot_angle_z * 3.1416 / 180;
	float missile_x_z = 0;
	printf("angle_y : %f, angle_z : %f\n", rot_angle_y, rot_angle_z);

	for (int i = 0; i < size; i++) {
		if (block_list.at(i).type == BLOCK_MISSILE) {

			missile_x_z = sqrt(pow(block_list.at(i).blc_trans_x, 2) + pow(block_list.at(i).blc_trans_z+1, 2));
			temp.launch_x = trans_x + missile_x_z * sin(angle_y) * abs(block_list.at(i).blc_trans_z) / block_list.at(i).blc_trans_z;
			temp.launch_y = trans_y + block_list.at(i).blc_trans_y;
			temp.launch_z = trans_z + missile_x_z * cos(angle_y) * abs(block_list.at(i).blc_trans_z) / block_list.at(i).blc_trans_z;
			
			temp.base_x = trans_x + trans_y / tan(-angle_z) * cos(angle_y) + (block_list.at(i).blc_trans_z + 1) * sin(angle_y);
			temp.base_y = 0;
			temp.base_z = trans_z + (trans_y / tan(-angle_z)) * sin(-angle_y) + (block_list.at(i).blc_trans_z + 1) * cos(angle_y);

			temp.angle_y = rot_angle_y;
			temp.angle_z = rot_angle_z;

			temp.missile_x	=  temp.launch_x;
			temp.missile_y	=  temp.launch_y;
			temp.missile_z	=  temp.launch_z;

			missile_trace_list.push_back(temp);
		}														
	}
}
/*==============================================================================================*
function : draw_missle_trace
desc     : 발사된 misile_trace_list의 정보를 화면에 출력함
*==============================================================================================*/
void draw_missile_trace() {
	int missile_size = missile_trace_list.size();
	
	for (int i = 0; i < missile_size; i++) {
		glLineWidth(3);
		glBegin(GL_LINES);
			glColor3f(1, 0, 0);
			glVertex3f(missile_trace_list.at(i).launch_x,
					   missile_trace_list.at(i).launch_y,
					   missile_trace_list.at(i).launch_z);
			glVertex3f(missile_trace_list.at(i).missile_x,
			           missile_trace_list.at(i).missile_y,
			           missile_trace_list.at(i).missile_z);
		glEnd();

		if (missile_trace_list.at(i).missile_y > 0) {
			missile_trace_list.at(i).missile_x -= 0.02 * ((missile_trace_list.at(i).launch_x - missile_trace_list.at(i).base_x));
			missile_trace_list.at(i).missile_y -= 0.02 * ((missile_trace_list.at(i).launch_y - missile_trace_list.at(i).base_y));
			missile_trace_list.at(i).missile_z -= 0.02 * ((missile_trace_list.at(i).launch_z - missile_trace_list.at(i).base_z));
		}
		
		if (missile_trace_list.at(i).missile_y > 0 && 
			missile_trace_list.at(i).missile_y - 0.02 * ((missile_trace_list.at(i).launch_y - missile_trace_list.at(i).base_y)) < 0) {
			printf("bomb!!!\n");
			sndPlaySoundA("./sound/bomb_snd.wav", SND_ASYNC | SND_NODEFAULT);
		}

		glPushMatrix();
			glTranslatef(missile_trace_list.at(i).missile_x,
						 missile_trace_list.at(i).missile_y,
						 missile_trace_list.at(i).missile_z);

			if (missile_trace_list.at(i).missile_y <= 0) {
				draw_explosion();
			}
			glRotatef(missile_trace_list.at(i).angle_y, 0, 1, 0);
			glRotatef(missile_trace_list.at(i).angle_z, 0, 0, 1);

			glTranslatef(0, 1.5, 0);
			glRotatef(90, 1, 0, 0);

			GLUquadricObj* qobj;
			qobj = gluNewQuadric();
			gluQuadricDrawStyle(qobj, GLU_FILL);

			glPushMatrix();
				glRotatef(90, 0, 1, 0);
				glTranslatef(-1.7, 0, -2.5);
				gluCylinder(qobj, 0.7, 0.7, 7, 20, 20);
				glRotatef(180, 1, 0, 0);
				glColor3f(0.9, 0.9, 0.9);
				glutSolidCone(0.7, 1.5, 20, 20);
				glTranslatef(0, 0, -2.3);
				gluCylinder(qobj, 0.8, 0.8, 0.25, 20, 20);
				gluDisk(qobj, 0, 0.8, 20, 20);
				glTranslatef(0, 0, 0.25);
				gluDisk(qobj, 0, 0.8, 20, 20);

				glTranslatef(0, 0, -3);
				gluCylinder(qobj, 0.8, 0.8, 0.25, 20, 20);
				gluDisk(qobj, 0, 0.8, 20, 20);
				glTranslatef(0, 0, 0.25);
				gluDisk(qobj, 0, 0.8, 20, 20);

				glTranslatef(0, 0, -2);
				glRotatef(180, 1, 0, 0);gluDisk(qobj, 0, 0.7, 20, 20);glRotatef(180, 1, 0, 0);
				glPushMatrix();
					glTranslatef(0.8, 0, 0);
					glColor3f(0.9, 0.9, 0.9);
					glScalef(0.5, 0.1, 1);
					glutSolidCube(1.0f);
				glPopMatrix();

				glRotatef(90, 0, 0, 1);
				glPushMatrix();
					glTranslatef(0.8, 0, 0);
					glScalef(0.5, 0.1, 1);
					glutSolidCube(1.0f);
				glPopMatrix();

				glRotatef(90, 0, 0, 1);
				glPushMatrix();
					glTranslatef(0.8, 0, 0);
					glScalef(0.5, 0.1, 1);
					glutSolidCube(1.0f);
				glPopMatrix();

				glRotatef(90, 0, 0, 1);
				glPushMatrix();
					glTranslatef(0.8, 0, 0);
					glScalef(0.5, 0.1, 1);
					glutSolidCube(1.0f);
				glPopMatrix();

				glPopMatrix();
		glPopMatrix();
	}
}
/*==============================================================================================*
function : draw_rader_range
desc     : 레이더 효과 (초록색 반투명 구)를 출력함
*==============================================================================================*/
void draw_radar_range(float r) {
	glPushMatrix();
		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		glEnable(GL_BLEND);
			gluQuadricDrawStyle(qobj, GLU_FILL);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glColor3f(0.1, 0.2, 0.1);
			GLfloat specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
			glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
			gluSphere(qobj, r, 20, 20);
			specular[0] = { 1.0f };
			specular[1] = { 1.0f };
			specular[2] = { 1.0f };
			glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
		glDisable(GL_BLEND);
	glPopMatrix();
}
/*==============================================================================================*
function : draw_explosion
desc     : 미사일 폭발효과를 출력함
*==============================================================================================*/
void draw_explosion() {
	glPushMatrix();
	glBegin(GL_POLYGON);
		glColor3f(0.9, 0.9, 0.0);
		glVertex3f(0, 0, 4);
		glVertex3f(0, 6, 8);
		glVertex3f(0, 4, 0);
		glVertex3f(0, 10, -5);
		glVertex3f(0, 0, -4);
	glEnd();
	glTranslated(-0.5, 0, 0);
	glBegin(GL_POLYGON);
		glColor3f(0.9, 0, 0);
		glVertex3f(0, 0, 6);
		glVertex3f(0, 8, 10);
		glVertex3f(0, 6, 0);
		glVertex3f(0, 12, -7);
		glVertex3f(0, 0, -6);
	glEnd();
	glTranslated(-0.5, 0, 0);
	glBegin(GL_POLYGON);
		glColor3f(0.9, 0.9, 0.0);
		glVertex3f(0, 0, 4);
		glVertex3f(0, 6, 8);
		glVertex3f(0, 4, 0);
		glVertex3f(0, 10, -5);
		glVertex3f(0, 0, -4);
	glEnd();

	glRotated(60, 0, 1, 0);
	glBegin(GL_POLYGON);
		glColor3f(0.9, 0, 0);
		glVertex3f(0, 0, 4);
		glVertex3f(0, 6, 8);
		glVertex3f(0, 4, 0);
		glVertex3f(0, 10, -5);
		glVertex3f(0, 0, -4);
	glEnd();
	glTranslated(-0.5, 0, 0);
	glBegin(GL_POLYGON);
		glColor3f(0.9, 0.9, 0);
		glVertex3f(0, 0, 6);
		glVertex3f(0, 8, 10);
		glVertex3f(0, 6, 0);
		glVertex3f(0, 12, -7);
		glVertex3f(0, 0, -6);
	glEnd();
	glTranslated(-0.5, 0, 0);
	glBegin(GL_POLYGON);
		glColor3f(0.9, 0, 0);
		glVertex3f(0, 0, 4);
		glVertex3f(0, 6, 8);
		glVertex3f(0, 4, 0);
		glVertex3f(0, 10, -5);
		glVertex3f(0, 0, -4);
	glEnd();
	glRotated(60, 0, 1, 0);
		glBegin(GL_POLYGON);
		glColor3f(0.9, 0, 0);
		glVertex3f(0, 0, 4);
		glVertex3f(0, 6, 8);
		glVertex3f(0, 4, 0);
		glVertex3f(0, 10, -5);
		glVertex3f(0, 0, -4);
	glEnd();
	glTranslated(-0.5, 0, 0);
	glBegin(GL_POLYGON);
		glColor3f(0.9, 0.9, 0);
		glVertex3f(0, 0, 6);
		glVertex3f(0, 8, 10);
		glVertex3f(0, 6, 0);
		glVertex3f(0, 12, -7);
		glVertex3f(0, 0, -6);
	glEnd();
	glTranslated(-0.5, 0, 0);
	glBegin(GL_POLYGON);
		glColor3f(0.9, 0, 0);
		glVertex3f(0, 0, 4);
		glVertex3f(0, 6, 8);
		glVertex3f(0, 4, 0);
		glVertex3f(0, 10, -5);
		glVertex3f(0, 0, -4);
	glEnd();
	glPopMatrix();
}
/*==============================================================================================*
function : draw_explosion
desc     : 비행기 폭발효과를 출력함
*==============================================================================================*/
void draw_flight_explosion() {
	glPushMatrix();
	
	glScalef(2, 2, 2);
	glBegin(GL_POLYGON);
		glColor3f(0.9, 0.9, 0.0);
		glVertex3f(0, 0, 4);
		glVertex3f(0, 6, 8);
		glVertex3f(0, 4, 0);
		glVertex3f(0, 10, -5);
		glVertex3f(0, 0, -4);
	glEnd();
	glTranslated(-0.5, 0, 0);
	glBegin(GL_POLYGON);
		glColor3f(0.9, 0, 0);
		glVertex3f(0, 0, 6);
		glVertex3f(0, 8, 10);
		glVertex3f(0, 6, 0);
		glVertex3f(0, 12, -7);
		glVertex3f(0, 0, -6);
	glEnd();
	glTranslated(-0.5, 0, 0);
	glBegin(GL_POLYGON);
		glColor3f(0.9, 0.9, 0.0);
		glVertex3f(0, 0, 4);
		glVertex3f(0, 6, 8);
		glVertex3f(0, 4, 0);
		glVertex3f(0, 10, -5);
		glVertex3f(0, 0, -4);
	glEnd();

	glRotated(60, 0, 1, 0);
	glBegin(GL_POLYGON);
		glColor3f(0.9, 0, 0);
		glVertex3f(0, 0, 4);
		glVertex3f(0, 6, 8);
		glVertex3f(0, 4, 0);
		glVertex3f(0, 10, -5);
		glVertex3f(0, 0, -4);
	glEnd();
	glTranslated(-0.5, 0, 0);
	glBegin(GL_POLYGON);
		glColor3f(0.9, 0.9, 0);
		glVertex3f(0, 0, 6);
		glVertex3f(0, 8, 10);
		glVertex3f(0, 6, 0);
		glVertex3f(0, 12, -7);
		glVertex3f(0, 0, -6);
	glEnd();
	glTranslated(-0.5, 0, 0);
	glBegin(GL_POLYGON);
		glColor3f(0.9, 0, 0);
		glVertex3f(0, 0, 4);
		glVertex3f(0, 6, 8);
		glVertex3f(0, 4, 0);
		glVertex3f(0, 10, -5);
		glVertex3f(0, 0, -4);
	glEnd();
	glRotated(60, 0, 1, 0);
		glBegin(GL_POLYGON);
		glColor3f(0.9, 0, 0);
		glVertex3f(0, 0, 4);
		glVertex3f(0, 6, 8);
		glVertex3f(0, 4, 0);
		glVertex3f(0, 10, -5);
		glVertex3f(0, 0, -4);
	glEnd();
	glTranslated(-0.5, 0, 0);
	glBegin(GL_POLYGON);
		glColor3f(0.9, 0.9, 0);
		glVertex3f(0, 0, 6);
		glVertex3f(0, 8, 10);
		glVertex3f(0, 6, 0);
		glVertex3f(0, 12, -7);
		glVertex3f(0, 0, -6);
	glEnd();
	glTranslated(-0.5, 0, 0);
	glBegin(GL_POLYGON);
		glColor3f(0.9, 0, 0);
		glVertex3f(0, 0, 4);
		glVertex3f(0, 6, 8);
		glVertex3f(0, 4, 0);
		glVertex3f(0, 10, -5);
		glVertex3f(0, 0, -4);
	glEnd();
	glPopMatrix();

}
/*==============================================================================================*
function : draw_block_list
desc     : 작업된 block_list를 출력함
*==============================================================================================*/
void draw_block_list() {
	block temp;
	for (int i = 0; i < block_list.size(); i++) {
		temp = block_list.at(i);
		draw_block(temp.type, temp.color, temp.blc_trans_x, temp.blc_trans_y, temp.blc_trans_z,
			temp.blc_rot_angle, temp.blc_rot_x, temp.blc_rot_y, temp.blc_rot_z, propeller_rot);
	}
}
/*==============================================================================================*
function : undo_drawing
desc     : 조립 모드에서 F키로 조립 확정한것을 실행 취소함
*==============================================================================================*/
void undo_drawing() {
	block_list.pop_back();
}
/*==============================================================================================*
function : make_target
desc     : 비행 모드시 타겟(주전자)를 5개 생성함
*==============================================================================================*/
void make_target() {
	target temp;
	temp.traget_x = -50;
	temp.traget_y = 3;
	temp.traget_z = 50;
	temp.snd_chk = false;
	target_list.push_back(temp);

	for (int i = 0; i < 5; i++) {
		temp.traget_x -= i*50;
		target_list.push_back(temp);
	}
}
/*==============================================================================================*
function : draw_target_list
desc     : 생성한 타겟을 출력함
*==============================================================================================*/
void draw_target_list() {
	int size = target_list.size();
	for (int i = 0; i < size; i++) {
		glPushMatrix();
		glTranslatef(target_list.at(i).traget_x, target_list.at(i).traget_y, target_list.at(i).traget_z);
			glColor3f(0.9, 0, 0.9);
			glutSolidTeapot(5);
		glPopMatrix();
	}
}
/*==============================================================================================*
function : draw_rader_target
desc     : 레이더 on시 레이더 범위 안에서 타겟 탐지시 초록색 대시선으로 표시
*==============================================================================================*/
void draw_rader_target_d(float radar_r, GLfloat trans_x, GLfloat trans_y, GLfloat trans_z){
	int size = target_list.size();
	float distance = 0;
	for (int i = 0; i < size; i++) {
		distance = sqrt(pow(target_list.at(i).traget_x - trans_x, 2)
			+ pow(target_list.at(i).traget_y - trans_y, 2)
			+ pow(target_list.at(i).traget_z - trans_z, 2));
		if (radar_r > distance) {
			glLineWidth(5);
			glEnable(GL_LINE_STIPPLE);
			glLineStipple(1, 0x1C47);  /*  dotted  */
			glBegin(GL_LINES);
				glColor3f(0, 1, 0);
				glVertex3f(target_list.at(i).traget_x, target_list.at(i).traget_y, target_list.at(i).traget_z);
				glVertex3f(trans_x, trans_y, trans_z);		
			glEnd();
			glDisable(GL_LINE_STIPPLE);
			glLineWidth(1);

			if (!target_list.at(i).snd_chk) {
				sndPlaySoundA("./sound/radar_snd.wav", SND_ASYNC | SND_NODEFAULT);
				target_list.at(i).snd_chk = true;
			}
		}
	}
}