#pragma once
#include "main_h.h"

struct color_rgb {
	GLclampf color_r, color_g, color_b;
};

void mapTextureMapping();
void draw_base(void);
void draw_skybox();
void draw_map(void);
void draw_axis(void);
color_rgb check_rgb(int color);
void draw_block_1(int color);
void draw_block_2(int color);
void draw_block_3(int color);
void draw_block_4(int color);
void draw_block_5(int color);
void draw_block_6(int color, GLfloat rot);
void draw_block_7(int color);
void draw_block_8(int color);
void draw_block_9(int color);
void draw_block(int type, int color, GLfloat blc_trans_x, GLfloat blc_trans_y, GLfloat blc_trans_z, GLfloat blc_rot_angle,
	GLfloat blc_rot_x, GLfloat blc_rot_y, GLfloat blc_rot_z, GLfloat propeller_rot);
void launch_missile(GLfloat trans_x, GLfloat trans_y, GLfloat trans_z, GLfloat rot_angle_y, GLfloat rot_angle_z);
void draw_missile_trace();
void draw_explosion();
void draw_flight_explosion();
void draw_radar_range(float r);
void draw_block_list();
void undo_drawing();
void make_target();
void draw_target_list();
void draw_rader_target_d(float r, GLfloat trans_x, GLfloat trans_y, GLfloat trans_z);

