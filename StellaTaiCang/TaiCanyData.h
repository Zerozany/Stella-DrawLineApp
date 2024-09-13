#ifndef TAICANYDATA_H
#define TAICANYDATA_H
#include <map>
#include <vector>
#include <QPainter>
#include <fstream>
#include <QFileDialog>
#include <string.h>
#include <iostream>

std::vector<std::string> direction_text{"北", "东", "南", "西"};
std::vector<std::string> eqress_text{"进口", "出口"};

typedef struct
{
    std::string direction{};
    std::string regin_number{};
    std::string intersection_number{};
    int direction_number{0};
    int eqress_type{0};
    std::size_t section_number{};
}DataTxt;

using points_v = std::vector<QPoint>;
using lines_v = std::vector<points_v>;

points_v load_points_v{};
points_v section_points_v{};
lines_v load_lines_v{};
lines_v section_lines_v{};
DataTxt data_txt{};

void clean_vector(DataTxt& _data_txt)
{
    load_points_v.clear();
    section_points_v.clear();
    load_lines_v.clear();
    section_lines_v.clear();
    _data_txt = {};
}

void write_files_date(const QString& _file_path, DataTxt& _data_txt)
{

    QString _directory_name = QFileInfo(_file_path).dir().absolutePath();
    QString image_file_name = QDir(_directory_name).filePath("..") + R"(/StellaTaiCany-Data/)" + QFileInfo(_file_path).baseName() + ".txt";
    std::ofstream image_ofs;    //写文件
    image_ofs.open(image_file_name.toStdString(), std::ios::out | std::ios::trunc);
    if (!image_ofs.is_open())
    {
        return;
    }
    else
    {
        image_ofs << "Direction: " << _data_txt.direction << '\n';
        if(!load_lines_v.empty())
        {
            for(const auto& l : load_lines_v)
            {
                image_ofs << "Line: ";
                for(const auto& p : l)
                {
                    image_ofs << p.x() << " " << p.y() << " ";
                }
                image_ofs << '\n';
            }
        }
        image_ofs << "Region_Number: " << _data_txt.regin_number << '\n';
        image_ofs << "Intersection_Number: " << _data_txt.intersection_number << '\n';
        image_ofs << "Direction_Number: " << _data_txt.direction_number << '\n';
        image_ofs << "Eqress_Type: " << _data_txt.eqress_type << '\n';
        image_ofs << "Section_Number: " << _data_txt.section_number << '\n';
        if(!section_lines_v.empty())
        {
            for(const auto& l : section_lines_v)
            {
                image_ofs << "Section: ";
                for(const auto& p : l)
                {
                    image_ofs << p.x() << " " << p.y() << " ";;
                }
                image_ofs << '\n';
            }
        }
        image_ofs.close();
        clean_vector(_data_txt);
    }
}


void read_files_data(const QString& _file_path)
{
    QString _directory_name = QFileInfo(_file_path).dir().absolutePath();
    QString image_file_name = QDir(_directory_name).filePath("..") + R"(/StellaTaiCany-Data/)" + QFileInfo(_file_path).baseName() + ".txt";
    std::ifstream image_ifs;    // 读文件
    image_ifs.open(image_file_name.toStdString(), std::ios::in);
    if (!image_ifs.is_open())
    {
        return;
    }
    // 逐行读取文件内容
    std::string line;
    while (std::getline(image_ifs, line))
    {
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        if (token == "Direction:")
        {
            iss >> data_txt.direction;
        }
        else if (token == "Line:")
        {
            int x, y;
            while (iss >> x >> y)
            {
                load_points_v.emplace_back(QPoint(x, y));
            }
            load_lines_v.emplace_back(load_points_v);
            load_points_v.clear();
        }
        else if (token == "Region_Number:")
        {
            iss >> data_txt.regin_number;
        }
        else if (token == "Intersection_Number:")
        {

            iss >> data_txt.intersection_number;
        }
        else if (token == "Direction_Number:")
        {
            iss >> data_txt.direction_number;
        }
        else if (token == "Eqress_Type:")
        {
            iss >> data_txt.eqress_type;
        }
        else if (token == "Section_Number:")
        {
            iss >> data_txt.section_number;
        }
        else if (token == "Section:")
        {
            int x, y;
            while (iss >> x >> y)
            {
                section_points_v.emplace_back(QPoint(x, y));
            }
            section_lines_v.emplace_back(section_points_v);
            section_points_v.clear();
        }
    }
    // 关闭文件
    image_ifs.close();
}



#endif // TAICANYDATA_H
