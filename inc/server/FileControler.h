#ifndef __FILE_CONTROLER__
#define __FILE_CONTROLER__


#include<filesystem>
#include<string>
#include<iostream>
#include<map>

namespace fs = std::filesystem;
class FileControl
{
public:
    std::string file_path;
    std::string travers_res;
    unsigned short file_count;
    std::map<int,std::pair<std::string,std::string>> file_group;

public:
    FileControl():file_count(0){}
    std::string Traverse(const fs::path& dir);  //遍历文件夹并返回遍历结果


};











#endif