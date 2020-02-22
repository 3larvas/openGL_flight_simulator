#pragma once
#include "main_h.h"
using namespace std;
typedef struct _finddata_t FILE_SEARCH;

const string chk_size = "* size = ";
const string chk_num = "# num = ";
const string chk_type = "- type = ";
const string chk_color = "- color = ";
const string chk_blc_trans_x = "- blc_trans_x = ";
const string chk_blc_trans_y = "- blc_trans_y = ";
const string chk_blc_trans_z = "- blc_trans_z = ";
const string chk_blc_rot_x = "- blc_rot_x = ";
const string chk_blc_rot_y = "- blc_rot_y = ";
const string chk_blc_rot_z = "- blc_rot_z = ";
const string chk_blc_rot_angle = "- blc_rot_angle = ";

void get_file_list(char* path);
bool read_from_file(const char* filename, char* buffer, int len);
int load_file(const char* file_name);
void save_file(const char* file_nm, vector <block> save_block_list);