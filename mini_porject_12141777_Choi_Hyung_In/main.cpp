#include <GL/glut.h>
#include <stdio.h>
#include <Windows.h>
#include <MMSystem.h>
#include "bmpfuncs.h"
#include "main_h.h"
#include "draw.h"
#include "file.h"
#include "Mmsystem.h"
#include "Digitalv.h"
#pragma comment(lib,"Winmm.lib")
#pragma warning(disable:4996)

//============== BGM 관련 변수 ==============//
MCI_OPEN_PARMS m_mciOpenParms;
MCI_PLAY_PARMS m_mciPlayParms;
DWORD m_dwDeviceID;
MCI_OPEN_PARMS mciOpen;
MCI_PLAY_PARMS mciPlay;
int dwID;
//============== 시점 관련 변수 ==============//
double radian = 0.1;
double th = 0.7;
double p = 0.7;
double r = 20;
double look_x = r * sin(th) * cos(p);
double look_y = r * sin(th) * sin(p);
double look_z = r * cos(th);
double pre_x, pre_y = 0;
int _width = 0;
int _height = 0;
//============== object 이동 관련 변수 ==============//
GLfloat trans_x, trans_y, trans_z = 0;
GLfloat pre_trans_x, pre_trans_y, pre_trans_z = 0;
GLfloat rot_x, rot_y, rot_z = 0;
GLfloat rot_angle = 0;

GLfloat rot_angle_x = 0;
GLfloat rot_angle_y = 0;
GLfloat rot_angle_z = 0;

float radar_r = 0;
float flight_speed = 0.08;
float flight_view_x = 25;
float flight_view_y = 20;
float flight_view_z = 0;
float flight_view_x_z = sqrt(pow(flight_view_x, 2) + pow(flight_view_z, 2));

GLfloat propeller_rot = 0;
int flight_expl = 0;
//============== mode 관련 변수 ==============//
int cur_type     = 1;
int cur_color    = 0;
bool simul_mode  = 0;
bool z_axis_mode = 0;
bool y_axis_mode = 1;
bool radar_mode  = 0;
//============== 관리할 리스트 ==============//
vector <block> block_list;
vector <block> read_block_list;
vector <string> file_list;
//============== file 관련 변수 ==============//
string cur_file_nm;
int warning_cnt = 0;
char path[] = "./save_file";
/*==============================================================================================*
function : init
desc     : 거리감(depth), 조명 및 재질 효과 설정
*==============================================================================================*/
void init(void) {
	glClearColor(0.0f, 0.0f, 0.6f, 0.0f);
	
	GLfloat ambientLight[]     = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuseLight[]     = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specularLight[]    = { 1.0f, 1.0f, 1.0f, 1.0f };
	
	GLfloat ambientMaterial[]  = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuseMaterial[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat specularMaterial[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT,  ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

	glMaterialfv(GL_FRONT, GL_AMBIENT, ambientMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specularMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, 60);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	make_target();
}
/*==============================================================================================*
function : idle
desc     : 프로펠러 회전각, 비행기 전진 및 레이더 관련 변수 일정 속도로 값을 더해줌
*==============================================================================================*/
void idle() {
	propeller_rot = propeller_rot + 3;
	if (propeller_rot > 360) propeller_rot -= 360;
	if (simul_mode == SIMUL_FLIGHT_MODE && trans_y>0) {
		trans_x -= flight_speed * cos(rot_angle_y  * 3.1416 / 180);
		trans_y += flight_speed * sin(-rot_angle_z * 3.1416 / 180);
		trans_z += flight_speed * sin(rot_angle_y  * 3.1416 / 180);
		if (trans_y < 20 && (int)trans_x % 3 == 0) {
			++warning_cnt;
			if (warning_cnt % 10 == 0) {
				sndPlaySoundA("./sound/warning_snd.wav", SND_ASYNC | SND_NODEFAULT);
			}
		}
	}
	else if (simul_mode == SIMUL_FLIGHT_MODE && trans_y <= 0) {
		if (flight_expl == 1) {
			sndPlaySoundA("./sound/bomb_snd.wav", SND_ASYNC | SND_NODEFAULT);
		}
		else if(flight_expl == 100) {
			trans_x = 0;
			trans_y = 35;
			trans_z = 0;
			th = 2.22;
			p = 0;
			r = 28.28;
			look_x = 20;
			look_y = 20;
			look_z = 0;
			flight_expl = 0;
		}
	}

	if (radar_mode) {
		if (radar_r < 500) {
			radar_r += 0.7;
		}
		else {
			radar_r = 0;
			for (int i = 0; i < target_list.size(); i++) {
				target_list.at(i).snd_chk = false;
			}
		}
	}
	glutPostRedisplay();
}
/*==============================================================================================*
function : resize
desc     : 윈도우 크기 변경 시 viewport size 재조정
*==============================================================================================*/
void resize(int width, int height) {
	_width = width;
	_height = height;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(100, (float)width / (float)height, 1, 1800);
	glMatrixMode(GL_MODELVIEW);
}
/*==============================================================================================*
function : draw_string
desc     : 비행모드 시 계기판 정보 출력
*==============================================================================================*/
void draw_string() {
	char msg[100];
	
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(-15, 15, -15, 15);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();

			glColor3f(0, 0, 0);
			glRasterPos3f(5, 13, 0);
			sprintf_s(msg, "== speed     : %3.2f\n", flight_speed*100);
			for (unsigned int i = 0; i < strlen(msg); i++)
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, msg[i]);

			glRasterPos3f(5, 12, 0);
			sprintf_s(msg, "== altitude   : %3.2f\n", trans_y);
			for (unsigned int i = 0; i < strlen(msg); i++)
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, msg[i]);

			glRasterPos3f(5, 11, 0);
			sprintf_s(msg, "== GPS_X      : %3.2f\n", trans_x);
			for (unsigned int i = 0; i < strlen(msg); i++)
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, msg[i]);

			glRasterPos3f(5, 10, 0);
			sprintf_s(msg, "== GPS_Y      : %3.2f\n", trans_z);
			for (unsigned int i = 0; i < strlen(msg); i++)
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, msg[i]);

			glRasterPos3f(5, 9, 0);
			sprintf_s(msg, "== pitch_angle: %3.2f\n", -rot_angle_z);
			for (unsigned int i = 0; i < strlen(msg); i++)
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, msg[i]);
			
			glBegin(GL_POLYGON);
				glColor3f(1, 1, 1);
				glVertex2f(14, 14);
				glVertex2f(14, 8);
				glVertex2f(5, 8);
				glVertex2f(5, 14);
			glEnd();
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopAttrib();
}
/*==============================================================================================*
function : coli_chk
desc     : 조립모드 시 블럭간 충돌여부 검사
*==============================================================================================*/
bool coli_chk() {
	int size = block_list.size();
	float blc_x=0, blc_y = 0, blc_z = 0, new_blc_x = 0, new_blc_y = 0, new_blc_z = 0, dis_x = 0, dis_y = 0, dis_z = 0;
	if (size == 0) {
		sndPlaySoundA("./sound/block_snd.wav", SND_ASYNC | SND_NODEFAULT);
		return true;
	}
	for (int i = 0; i < size; i++) {
		// 블록 종류별로 사이즈 정보가 부여됨
		if (block_list.at(i).type == BLOCK_THICK_2_4 ||
			block_list.at(i).type == BLOCK_HALF_2_4 || 
			block_list.at(i).type == BLOCK_MISSILE ) {
			if (block_list.at(i).blc_rot_angle == 90 || block_list.at(i).blc_rot_angle == 270) {
				blc_x = 2;
				blc_z = 4;
			}
			else {
				blc_x = 4;
				blc_z = 2;
			}
			blc_y = 1.5;
		}
		else if (block_list.at(i).type == BLOCK_THICK_2_2||
				 block_list.at(i).type == BLOCK_WHEEL    || 
				 block_list.at(i).type == BLOCK_PROPELLER|| 
				 block_list.at(i).type == BLOCK_JET_ENGIN ) {
			blc_x = 2;
			blc_z = 2;			
			blc_y = 1.5;
		}
		else if (block_list.at(i).type == BLOCK_FLAT_2_4) {
			if (rot_angle == 90 || rot_angle == 270) {
				blc_x = 2;
				blc_z = 4;
			}
			else {
				blc_x = 4;
				blc_z = 2;
			}
			blc_y = 0.5;
		}
		else if (block_list.at(i).type == BLOCK_FLAT_2_8) {
			if (rot_angle == 90 || rot_angle == 270) {
				blc_x = 2;
				blc_z = 8;
			}
			else {
				blc_x = 8;
				blc_z = 2;
			}
			blc_y = 0.5;
		}

		if (cur_type == BLOCK_THICK_2_4 ||
			cur_type == BLOCK_HALF_2_4  ||
			cur_type == BLOCK_MISSILE ) {
			if (rot_angle == 90 || rot_angle == 270) {
				new_blc_x = 2;
				new_blc_z = 4;
			}
			else {
				new_blc_x = 4;
				new_blc_z = 2;
			}
			new_blc_y = 1.5;
		}
		else if (cur_type == BLOCK_THICK_2_2 ||
			     cur_type == BLOCK_WHEEL     ||
			     cur_type == BLOCK_PROPELLER ||
			     cur_type == BLOCK_JET_ENGIN  ) {
			new_blc_x = 2;
			new_blc_z = 2;
			new_blc_y = 1.5;
		}
		else if (cur_type == BLOCK_FLAT_2_4) {
			if (rot_angle == 90 || rot_angle == 270) {
				new_blc_x = 2;
				new_blc_z = 4;
			}
			else {
				new_blc_x = 4;
				new_blc_z = 2;
			}
			new_blc_y = 0.5;
		}
		else if (cur_type == BLOCK_FLAT_2_8) {
			if (rot_angle == 90 || rot_angle == 270) {
				new_blc_x = 2;
				new_blc_z = 8;
			}
			else {
				new_blc_x = 8;
				new_blc_z = 2;
			}
			new_blc_y = 0.5;
		}
		
		dis_x = abs(trans_x - block_list.at(i).blc_trans_x) - (blc_x + new_blc_x);
		dis_y = abs(trans_y - block_list.at(i).blc_trans_y) - (blc_y + new_blc_y);
		dis_z = abs(trans_z - block_list.at(i).blc_trans_z) - (blc_z + new_blc_z);
		if (dis_x < 0 && dis_y < 0 && dis_z < 0) {
			printf("========NoNo========\n");
			sndPlaySoundA("./sound/wrong_snd.wav", SND_ASYNC | SND_NODEFAULT);
			return false;
		}
	}
	printf("OKOK\n");
	printf("=======================\n");
	sndPlaySoundA("./sound/block_snd.wav", SND_ASYNC | SND_NODEFAULT);
	return true;
	
}
/*==============================================================================================*
function : draw_complete
desc     : 조립 모드에서 F키 입력 시 해당 블록을 조립 확정하여 block_list에 push
*==============================================================================================*/
void draw_complete() {
	if (coli_chk()) {
		block cur_block;
		cur_block.type = cur_type;
		cur_block.color = cur_color;
		cur_block.blc_trans_x = trans_x;
		cur_block.blc_trans_y = trans_y;
		cur_block.blc_trans_z = trans_z;
		cur_block.blc_rot_angle = rot_angle;
		cur_block.blc_rot_x = rot_x;
		cur_block.blc_rot_y = rot_y;
		cur_block.blc_rot_z = rot_z;

		block_list.push_back(cur_block);
	}
}
/*==============================================================================================*
function : draw_preview_next
desc     : 조립모드에서 다음 블럭(R)을 보여줌
*==============================================================================================*/
void draw_preview_next(void) {
	glViewport(1.2 * _width / 2, 1.2 * _height / 2, _width / 2, _height / 2);
	glLoadIdentity();

	gluLookAt(5, 8, 10, 0, 0, 0, 0, 1, 0);
	glColor3f(0.0f, 0.0f, 0.0f);
	const char* str = "Next Block(T)";
	glRasterPos3i(0, 0, 8);
	for (int i = 0; i < strlen(str); i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
	}
	switch (cur_type+1) {
	case BLOCK_THICK_2_4:
		draw_block_1(cur_color);
		break;
	case BLOCK_THICK_2_2:
		draw_block_2(cur_color);
		break;
	case BLOCK_WHEEL:
		draw_block_3(cur_color);
		break;
	case BLOCK_FLAT_2_4:
		draw_block_4(cur_color);
		break;
	case BLOCK_FLAT_2_8:
		draw_block_5(cur_color);
		break;
	case BLOCK_PROPELLER:
		draw_block_6(cur_color, propeller_rot);
		break;
	case BLOCK_HALF_2_4:
		draw_block_7(cur_color);
		break;
	case BLOCK_MISSILE:
		draw_block_8(cur_color);
		break;
	case BLOCK_JET_ENGIN:
		draw_block_9(cur_color);
		break;
	default:
		draw_block_1(cur_color);
		break;
	}
	glFlush();
}
/*==============================================================================================*
function : draw_preview_prev
desc     : 조립모드에서 이전 블럭(R)을 보여줌
*==============================================================================================*/
void draw_preview_prev(void) {
	glViewport(- _width / 10, 1.2 * _height / 2, _width / 2, _height / 2);
	glLoadIdentity();

	gluLookAt(5, 8, 10, 0, 0, 0, 0, 1, 0);
	glColor3f(0.0f, 0.0f, 0.0f);
	const char* str = "Pre Block(R)";
	glRasterPos3i(0, 0, 8);
	for (int i = 0; i < strlen(str); i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
	}
	switch (cur_type - 1) {
	case BLOCK_THICK_2_4:
		draw_block_1(cur_color);
		break;
	case BLOCK_THICK_2_2:
		draw_block_2(cur_color);
		break;
	case BLOCK_WHEEL:
		draw_block_3(cur_color);
		break;
	case BLOCK_FLAT_2_4:
		draw_block_4(cur_color);
		break;
	case BLOCK_FLAT_2_8:
		draw_block_5(cur_color);
		break;
	case BLOCK_PROPELLER:
		draw_block_6(cur_color, propeller_rot);
		break;
	case BLOCK_HALF_2_4:
		draw_block_7(cur_color);
		break;
	case BLOCK_MISSILE:
		draw_block_8(cur_color);
		break;
	case BLOCK_JET_ENGIN:
		draw_block_9(cur_color);
		break;
	default:
		draw_block_9(cur_color);
		break;
	}
	glFlush();
}
/*==============================================================================================*
function : draw
desc     : 화면을 보여줌(조립모드, 비행모드에 따라 다르게 출력됨)
*==============================================================================================*/
void draw(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, _width, _height);
	glLoadIdentity();
	// 조립모드
	if (simul_mode == SIMUL_BLOCK_MODE) {
		if (th <= pi) {
			gluLookAt(look_x, look_z, look_y, 0, 0, 0, 0, 1, 0);
		}
		else if (th > pi) {
			gluLookAt(look_x, look_z, look_y, 0, 0, 0, 0, -1, 0);
		}
		//조명의 위치는 카메라 
		glPushMatrix();
			glTranslatef(look_x, look_z, look_y);
			GLfloat light_position_0[] = { 0.0f, 0.0f, 0.0f, 1.0f };
			glLightfv(GL_LIGHT0, GL_POSITION, light_position_0);
		glPopMatrix();

		draw_base();
		draw_axis();
		draw_block_list();

		glTranslatef(trans_x, trans_y, trans_z);
		glRotatef(rot_angle, rot_x, rot_y, rot_z);
		switch (cur_type) {
			case BLOCK_THICK_2_4:
				draw_block_1(cur_color);
				break;
			case BLOCK_THICK_2_2:
				draw_block_2(cur_color);
				break;
			case BLOCK_WHEEL:
				draw_block_3(cur_color);
				break;
			case BLOCK_FLAT_2_4:
				draw_block_4(cur_color);
				break;
			case BLOCK_FLAT_2_8:
				draw_block_5(cur_color);
				break;
			case BLOCK_PROPELLER:
				draw_block_6(cur_color, propeller_rot);
				break;
			case BLOCK_HALF_2_4:
				draw_block_7(cur_color);
				break;
			case BLOCK_MISSILE:
				draw_block_8(cur_color);
				break;
			case BLOCK_JET_ENGIN:
				draw_block_9(cur_color);
				break;
			default:
				draw_block_1(cur_color);
				break;
		}
		draw_preview_next();
		draw_preview_prev();
	}
	//비행모드
	else if (simul_mode == SIMUL_FLIGHT_MODE) {
		//상하좌우 회전각 설정
		float angle_y = rot_angle_y * 3.1416 / 180;
		float angle_z = rot_angle_z * 3.1416 / 180;
		flight_view_x_z = sqrt(pow(flight_view_x, 2) + pow(flight_view_z, 2));

		gluLookAt(trans_x + flight_view_x_z * cos(angle_y),
			trans_y + flight_view_y,
			trans_z + flight_view_x_z * sin(-angle_y),
			trans_x, trans_y, trans_z, 0, 1, 0);

		glPushMatrix();
			glTranslatef(trans_x + flight_view_x_z * cos(angle_y),
				trans_y + flight_view_y,
				trans_z + flight_view_x_z * sin(-angle_y));
			GLfloat light_position_0[] = { 0.0f, 0.0f, 0.0f, 1.0f };
			glLightfv(GL_LIGHT0, GL_POSITION, light_position_0);
		glPopMatrix();

		draw_skybox();
		draw_missile_trace();
		draw_target_list();
		//조준선
		glPushMatrix();
			glTranslatef(trans_x, trans_y, trans_z);
			glRotatef(rot_angle_y, 0, 1, 0);
			glRotatef(rot_angle_z, 0, 0, 1);
			glLineWidth(3);
			glBegin(GL_LINES);
			if (rot_angle_z > 0) {
				glColor3f(1, 0, 0);
				glVertex3f(-35, 0, 0);
				glVertex3f( - trans_y / tan(angle_z)+10, 0, 0);

				glColor3f(0, 1, 0);
				glVertex3f(-trans_y / tan(angle_z), 0, -trans_y / tan(angle_z) / 10);
				glVertex3f(-trans_y / tan(angle_z), 0, trans_y / tan(angle_z) / 10);
			}
			glEnd();
			glLineWidth(1);
			draw_block_list();
			//지면충돌효과
			if (trans_y <= 0) {
				draw_flight_explosion();
				++flight_expl;
			}
			//레이더
			if (radar_mode) {
				draw_radar_range(radar_r);
			}
		glPopMatrix();
		//레이더 탐지
		if (radar_mode) {
			draw_rader_target_d(radar_r, trans_x, trans_y, trans_z);
		}
		//계기판
		draw_string();
	}
	glFlush();
	glutSwapBuffers();
}
/*==============================================================================================*
function : specialkeyboard
desc     : 특수키 입력처리
*==============================================================================================*/
void specialkeyboard(int key, int x, int y) {
	if (simul_mode == SIMUL_BLOCK_MODE) {
		if (key == GLUT_KEY_LEFT) {
			p += radian;
			look_x = r * sin(th) * cos(p);
			look_y = r * sin(th) * sin(p);
			look_z = r * cos(th);
		}
		else if (key == GLUT_KEY_RIGHT) {
			p -= radian;
			look_x = r * sin(th) * cos(p);
			look_y = r * sin(th) * sin(p);
			look_z = r * cos(th);
		}
		else if (key == GLUT_KEY_UP) {
			th -= radian;
			if (th < 0) {
				th += 2 * pi;
			}
			look_x = r * sin(th) * cos(p);
			look_y = r * sin(th) * sin(p);
			look_z = r * cos(th);
		}
		else if (key == GLUT_KEY_DOWN) {
			th += radian;
			if (th > 2 * pi) {
				th -= 2 * pi;
			}
			look_x = r * sin(th) * cos(p);
			look_y = r * sin(th) * sin(p);
			look_z = r * cos(th);
		}
		glutPostRedisplay();
	}
}
/*==============================================================================================*
function : keyboard
desc     : 일반 키 입력 처리
*==============================================================================================*/
void keyboard(unsigned char key, int x, int y) {
	if (simul_mode == SIMUL_BLOCK_MODE) {
		switch (key)
		{
			//-----------------------블록 이동-----------------------//
		case 'w':
			trans_x -= 2;
			break;
		case 's':
			trans_x += 2;
			break;
		case 'a':
			trans_z += 2;
			break;
		case 'd':
			trans_z -= 2;
			break;
		case 'q':
			trans_y += 1;
			break;
		case 'e':
			trans_y -= 1;
			break;
			//-----------------------블록 회전-----------------------//
		case 'n':
			rot_x = 0;
			rot_y = 1;
			rot_z = 0;
			rot_angle += 90;
			break;
		case 'm':
			rot_x = 0;
			rot_y = 1;
			rot_z = 0;
			rot_angle -= 90;
			break;
		case 'h':
			rot_x = 1;
			rot_y = 0;
			rot_z = 0;
			rot_angle += 90;
			break;
		case 'j':
			rot_x = 1;
			rot_y = 0;
			rot_z = 0;
			rot_angle -= 90;
			break;
		case 'y':
			rot_x = 0;
			rot_y = 0;
			rot_z = 1;
			rot_angle += 90;
			break;
		case 'u':
			rot_x = 0;
			rot_y = 0;
			rot_z = 1;
			rot_angle -= 90;
			break;
			//-----------------------블록 조립 확정-----------------------//
		case 'f':
			draw_complete();
			break;
			//-----------------------실행 취소-----------------------//
		case 'z':
			undo_drawing();
			break;
			//-----------------------블록 변경-----------------------//
		case 'r':
			if (cur_type == BLOCK_THICK_2_4) {
				cur_type = BLOCK_JET_ENGIN;
			}
			else {
				cur_type--;
			}
			printf("cur_type : %d \n", cur_type);
			break;
		case 't':
			printf("cur_type : %d \n", cur_type);
			if (cur_type == BLOCK_JET_ENGIN) {
				cur_type = BLOCK_THICK_2_4;
			}
			else {
				cur_type++;
			}
			printf("cur_type : %d \n", cur_type);
			break;
			//-----------------------블록 색 변경-----------------------//
		case 'x':
			if (cur_color == 0) {
				cur_color = 7;
			}
			else {
				cur_color--;
			}
			break;
		case 'c':
			if (cur_color == 7) {
				cur_color = 0;
			}
			else {
				cur_color++;
			}
			break;
			//-----------------------모드 변경-----------------------//
		case 'p':
			if (simul_mode == SIMUL_BLOCK_MODE) simul_mode = SIMUL_FLIGHT_MODE;
			printf("simul_mode : %d====\n", simul_mode);
			if (simul_mode == SIMUL_FLIGHT_MODE) {
				trans_x = 0;
				trans_y = 35;
				trans_z = 0;
				th = 2.22;
				p = 0;
				r = 28.28;
				look_x = 20;
				look_y = 20;
				look_z = 0;
			}
			//BGM 변경
			mciSendCommandW(dwID, MCI_CLOSE, 0, NULL);        
			mciOpen.lpstrElementName = "./sound/air_plain_snd.wav";
			mciOpen.lpstrDeviceType = "mpegvideo";
			mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE,
				(DWORD)(LPVOID)& mciOpen);
			dwID = mciOpen.wDeviceID;
			mciSendCommand(dwID, MCI_PLAY, MCI_DGV_PLAY_REPEAT, (DWORD)(LPVOID)& m_mciPlayParms);	
			missile_trace_list.clear();
			break;
		}
	}
	else if (simul_mode == SIMUL_FLIGHT_MODE) {
		switch (key)
		{
			//-----------------------비행기 조종-----------------------//
		case 'w':
			y_axis_mode = false;
			z_axis_mode = true;
			rot_angle_z -= 1;
			break;
		case 's':
			y_axis_mode = false;
			z_axis_mode = true;
			rot_angle_z += 1;
			break;
		case 'a':
			y_axis_mode = true;
			z_axis_mode = false;
			rot_angle_y += 1;
			if (rot_angle_y > 360) rot_angle_y -= 360;
			else if (rot_angle_y < 0) rot_angle_y += 360;
			break;
		case 'd':
			y_axis_mode = true;
			z_axis_mode = false;
			rot_angle_y -= 1;
			if (rot_angle_y > 360) rot_angle_y -= 360;
			else if (rot_angle_y < 0) rot_angle_y += 360;
			break;
			//------------------------모드 변경------------------------//
		case 'p':
			if (simul_mode == SIMUL_FLIGHT_MODE) simul_mode = SIMUL_BLOCK_MODE;
			printf("simul_mode : %d====\n", simul_mode);
			trans_x = 0;
			trans_y = 0;
			trans_z = 0;
			th = 0.7;
			p = 0.7;
			r = 20;
			look_x = r * sin(th) * cos(p);
			look_y = r * sin(th) * sin(p);
			look_z = r * cos(th);
			//BGM 변경
			mciSendCommandW(dwID, MCI_CLOSE, 0, NULL);       
			mciOpen.lpstrElementName = "./sound/main.mp3"; 
			mciOpen.lpstrDeviceType = "mpegvideo";
			mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE,
				(DWORD)(LPVOID)& mciOpen);
			dwID = mciOpen.wDeviceID;
			mciSendCommand(dwID, MCI_PLAY, MCI_DGV_PLAY_REPEAT, (DWORD)(LPVOID)& m_mciPlayParms);
			break;
			//-----------------------미사일 발사-----------------------//
		case 'f':
			launch_missile(trans_x, trans_y, trans_z, rot_angle_y, rot_angle_z);
			sndPlaySoundA("./sound/missile_snd.wav", SND_ASYNC | SND_NODEFAULT);
			break;
			//-----------------------속도 조절-----------------------//
		case 'q':
			if (flight_speed < 0.4) {
				flight_speed += 0.02;
			}
			else {
				printf("최고 속도입니다.\n");
			}
			break;
		case 'e':
			if (flight_speed > 0.04) {
				flight_speed -= 0.02;
			}
			else {
				printf("최저 속도입니다.\n");
			}
			break;
			//-----------------------레이더 on/off-----------------------//
		case 'r':	
			printf("%== mode : %d\n", radar_mode);
			if (radar_mode) {
				radar_mode = 0;
			}
			else {
				radar_mode = 1;
			}
			radar_r = 0;
			break;
		}
	}
	glutPostRedisplay();
}
/*==============================================================================================*
function : mouse
desc     : 마우스 조작으로 시점 관련 변수들을 변경함
*==============================================================================================*/
void mouse(int button, int state, int x, int y) {
	if (simul_mode == SIMUL_BLOCK_MODE) {
		printf("Mouse button is clicked : (%d, %d, %d, %d)\n", button, state, x, y);
		if (button == 3)
		{
			r -= 0.7;
			look_x = r * sin(th) * cos(p);
			look_y = r * sin(th) * sin(p);
			look_z = r * cos(th);
		}
		else if (button == 4)
		{
			r += 0.7;
			look_x = r * sin(th) * cos(p);
			look_y = r * sin(th) * sin(p);
			look_z = r * cos(th);
		}
		else if (state == 1)
		{
			pre_x = 0;
			pre_y = 0;
		}
		glutPostRedisplay();
	}
	else if (simul_mode == SIMUL_FLIGHT_MODE) {
		printf("Mouse button is clicked : (%d, %d, %d, %d)\n", button, state, x, y);
		if (button == 4)
		{
			flight_view_x += 0.8;
			flight_view_y += 0.3;
		}
		else if (button == 3)
		{
			flight_view_x -= 0.8;
			flight_view_y -= 0.3;
		}
		glutPostRedisplay();
	}
}
void motion(int x, int y) {
	printf("Mouse is moving : (%d, %d)\n", x, y);
	if (pre_x != 0) {
		p -= (pre_x - x) / 300;
		th += (pre_y - y) / 300;
		look_x = r * sin(th) * cos(p);
		look_y = r * sin(th) * sin(p);
		look_z = r * cos(th);
		glutPostRedisplay();
	}
	pre_x = x;
	pre_y = y;
}
/*==============================================================================================*
function : sub_menu_function
desc     : 저장된 .lego 파일 리스트를 출력함
*==============================================================================================*/
void sub_menu_function(int option) {
	trans_x = 0;
	trans_y = 0;
	trans_z = 0;
	rot_angle = 0;
	rot_x = 0;
	rot_y = 0;
	rot_z = 0;
	printf("submenu %d has been selected\n", option);
	const char* file_nm;
	file_nm = file_list.at(option).c_str();
	load_file(file_nm);
	cur_file_nm = file_nm;
}
/*==============================================================================================*
function : main_menu_function
desc     : 
*==============================================================================================*/
void main_menu_function(int option) {
	printf("Main menu %d has been selected\n", option);
	if (option == 999) {
		exit(0);
	}
	else if (option == 111) {
	
		if (cur_file_nm.size()>0) {
			const char* file_nm = cur_file_nm.c_str();
			save_file(file_nm, block_list);
		}
		else {
			printf("== Enter file name : ");
			char file_nm[50];
			scanf_s("%s", file_nm, 50);
			save_file(file_nm, block_list);
			cur_file_nm = file_nm;
			
			get_file_list(path);
			const char* list_nm;
			int submenu1 = glutCreateMenu(sub_menu_function);
			for (int i = 0; i < file_list.size(); i++) {
				list_nm = file_list.at(i).c_str();
				glutAddMenuEntry(list_nm, i);
			}
			
			glutCreateMenu(main_menu_function);
			glutAddMenuEntry("Quit", 999);
			glutAddMenuEntry("save", 111);
			glutAddMenuEntry("clear", 222);
			glutAddSubMenu("file load", submenu1);
			glutAttachMenu(GLUT_RIGHT_BUTTON);
		}	
	}
	else if (option == 222) {
		block_list.clear();
	}
}

int main(int argc, char** argv) {
	int submenu1;
	get_file_list(path);
	//BGM ON
	mciOpen.lpstrElementName = "./sound/main.mp3";
	mciOpen.lpstrDeviceType = "mpegvideo";
	mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE,
		(DWORD)(LPVOID)& mciOpen);
	dwID= mciOpen.wDeviceID;
	mciSendCommand(dwID, MCI_PLAY, MCI_DGV_PLAY_REPEAT, (DWORD)(LPVOID)& m_mciPlayParms);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glClearDepth(1.0f);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(300, 300);
	glutCreateWindow("My First GL program");
	init();
	//.lego 파일리스트 불러오기
	const char* file_nm;
	submenu1 = glutCreateMenu(sub_menu_function);
	for (int i = 0; i < file_list.size(); i++) {
		file_nm = file_list.at(i).c_str();
		glutAddMenuEntry(file_nm, i);
	}
	
	glutCreateMenu(main_menu_function);
	glutAddMenuEntry("Quit", 999);
	glutAddMenuEntry("save", 111);
	glutAddMenuEntry("clear", 222);
	glutAddSubMenu("file load", submenu1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

    mapTextureMapping();

	glutDisplayFunc(draw);
	glutReshapeFunc(resize);
	glutSpecialFunc(specialkeyboard);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutIdleFunc(idle);
	glutMainLoop();

	return 0;
}	