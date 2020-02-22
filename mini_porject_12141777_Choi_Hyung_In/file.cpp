#define _CRT_SECURE_NO_WARNINGS
#include "file.h"
/*==============================================================================================*
function : get_file_list
desc     : sub_menu_list에 출력될 .lego파일 목록을 만듬
*==============================================================================================*/
void get_file_list(char* path_) {
	long h_file;
	char search_Path[500];
	FILE_SEARCH file_search;
	file_list.clear();
	sprintf_s(search_Path, "%s/*.*", path_);
	if ((h_file = _findfirst(search_Path, &file_search)) == -1L) {
		printf("No files in current directory!\n");
	}
	else {
		do {
			if (strstr(file_search.name, ".lego")) {
					file_list.push_back(file_search.name);
			}
		} while (_findnext(h_file, &file_search) == 0);
		_findclose(h_file);
	}
}
/*==============================================================================================*
function : read_form_file
desc     : 파일을 경로에서 읽어옴
*==============================================================================================*/
bool read_from_file(const char* filename, char* buffer, int len)
{
	char file_path[100];
	sprintf(file_path, "%s/%s", path, filename); //
	FILE* r = fopen(file_path, "rb");
	if (NULL == r)
		return false;
	size_t fileSize = fread(buffer, 1, len, r);
	fclose(r);
	return true;
}
/*==============================================================================================*
function : load_file
desc     : .lego 파일을 불러와 블록정보를 block_list에 저장함
*==============================================================================================*/
int load_file(const char* file_name)
{
	const int BufferLength = 65536;
	char readBuffer[BufferLength] = { 0, };
	if (false == read_from_file(file_name, readBuffer, BufferLength))
		return 0;

	string str_file = readBuffer;
	int start_idx, end_idx = 0;
	int index = str_file.find(chk_size);
	int list_size = 0;

	block temp;
	end_idx = str_file.find(";", index + 1);
	list_size = stoi(str_file.substr(index + chk_size.size(), end_idx - index - chk_size.size()));

	for (int i = 0; i < list_size; i++) {

		index = str_file.find(chk_type, index + 1);
		end_idx = str_file.find(";", index + 1);
		temp.type = stoi(str_file.substr(index + chk_type.size(), end_idx - index - chk_type.size()));

		index = str_file.find(chk_color, index + 1);
		end_idx = str_file.find(";", index + 1);
		temp.color = stoi(str_file.substr(index + chk_color.size(), end_idx - index - chk_color.size()));

		index = str_file.find(chk_blc_trans_x, index + 1);
		end_idx = str_file.find(";", index + 1);
		temp.blc_trans_x = stoi(str_file.substr(index + chk_blc_trans_x.size(), end_idx - index - chk_blc_trans_x.size()));

		index = str_file.find(chk_blc_trans_y, index + 1);
		end_idx = str_file.find(";", index + 1);
		temp.blc_trans_y = stoi(str_file.substr(index + chk_blc_trans_y.size(), end_idx - index - chk_blc_trans_y.size()));

		index = str_file.find(chk_blc_trans_z, index + 1);
		end_idx = str_file.find(";", index + 1);
		temp.blc_trans_z = stoi(str_file.substr(index + chk_blc_trans_z.size(), end_idx - index - chk_blc_trans_z.size()));

		index = str_file.find(chk_blc_rot_x, index + 1);
		end_idx = str_file.find(";", index + 1);
		temp.blc_rot_x = stoi(str_file.substr(index + chk_blc_rot_x.size(), end_idx - index - chk_blc_rot_x.size()));

		index = str_file.find(chk_blc_rot_y, index + 1);
		end_idx = str_file.find(";", index + 1);
		temp.blc_rot_y = stoi(str_file.substr(index + chk_blc_rot_y.size(), end_idx - index - chk_blc_rot_y.size()));

		index = str_file.find(chk_blc_rot_z, index + 1);
		end_idx = str_file.find(";", index + 1);
		temp.blc_rot_z = stoi(str_file.substr(index + chk_blc_rot_z.size(), end_idx - index - chk_blc_rot_z.size()));

		index = str_file.find(chk_blc_rot_angle, index + 1);
		end_idx = str_file.find(";", index + 1);
		temp.blc_rot_angle = stoi(str_file.substr(index + chk_blc_rot_angle.size(), end_idx - index - chk_blc_rot_angle.size()));

		read_block_list.push_back(temp);
	}

	block_list.clear();
	block_list.assign(read_block_list.begin(), read_block_list.end());
	read_block_list.clear();
	return 0;
}
/*==============================================================================================*
function : save_file
desc     : block_list의 정보를 양식에 맞추어 파일로 생성하여 저장함
*==============================================================================================*/
void save_file(const char* file_nm, vector <block> save_block_list) {
	int size = save_block_list.size();

	char file_path[100];
	sprintf(file_path, "%s/%s", path, file_nm); //

	ofstream myfile;
	myfile.open(file_path);
	myfile << "* block_list\n";
	myfile << "* size = "<< size <<";\n";
	for (int i = 0; i < size; i++) {
		myfile << "# num = " << i+1 << ";\n";
		myfile << "- type = "          << save_block_list.at(i).type << ";\n";
		myfile << "- color = "		   << save_block_list.at(i).color << ";\n";
		myfile << "- blc_trans_x = "   << save_block_list.at(i).blc_trans_x << ";\n";
		myfile << "- blc_trans_y = "   << save_block_list.at(i).blc_trans_y << ";\n";
		myfile << "- blc_trans_z = "   << save_block_list.at(i).blc_trans_z << ";\n";
		myfile << "- blc_rot_x = "	   << save_block_list.at(i).blc_rot_x << ";\n";
		myfile << "- blc_rot_y = "	   << save_block_list.at(i).blc_rot_y << ";\n";
		myfile << "- blc_rot_z = "	   << save_block_list.at(i).blc_rot_z << ";\n";
		myfile << "- blc_rot_angle = " << save_block_list.at(i).blc_rot_angle << ";\n";
	}
	myfile.close();
}