#include"..\..\inc\server\FileControler.h"

std::string FileControl::Traverse(const fs::path& dir) 
{
    std::string result;
    for (const auto& entry : fs::directory_iterator(dir)) 
    {
        if (entry.is_directory()) 
        {
            result += Traverse(entry.path()); //递归子目录             //result不追加这一步会出现返回值为空的情况
        } else {
            result.append(std::to_string(file_count));
            result.append(":  ");
            result += entry.path().u8string() + "\n";

            file_group.insert(std::make_pair(file_count,std::make_pair(entry.path().u8string(),entry.path().filename().u8string())));  //将遍历到的文件的索引，路径、名称存入容器中
            file_count ++;
        }
    }

    return result;
}