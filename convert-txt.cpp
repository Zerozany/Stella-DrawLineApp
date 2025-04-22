#include "widget.h"

void Widget::read_txt(const std::string& _image_path, std::map<std::int32_t, std::vector<PointInfo>>& _lines_map)
{
    std::ifstream image_ifs{};
    image_ifs.open(_image_path,std::ios::in);
    if(!image_ifs.is_open())
    {
        return;
    }
    std::string line_params{};
    while (std::getline(image_ifs, line_params))
    {
        std::istringstream iss(line_params);
        std::string line_key{};
        if (!(iss >> line_key))
        {
            return;
        }
        char space{};
        PointInfo point_info{};
        std::vector<PointInfo> single_line{};
        int temp_x{}, temp_y{};
        while (iss >> temp_x >> space >> temp_y >> point_info.point_size)
        {
            point_info.point.setX(static_cast<int>(0.5f + temp_x * (background_label->width() / static_cast<float>(image.width()))));
            point_info.point.setY(static_cast<int>(0.5f + temp_y * (background_label->height() / static_cast<float>(image.height()))));
            single_line.emplace_back(std::move(point_info));
        }
        if (single_line.empty())
        {
            return;
        }
        std::int32_t key{std::stoi(line_key)};
        _lines_map[key] = single_line;
    }
    image_ifs.close();
}

void Widget::write_txt(const std::map<int32_t, std::vector<PointInfo>>& _lines_map, const std::string& _image_path)
{
    std::ofstream image_ofs{};
    image_ofs.open(_image_path,std::ios::out | std::ios::trunc);
    if(!image_ofs.is_open())
    {
        return;
    }
    if(!_lines_map.empty())
    {
        for(const auto& [key, value] : _lines_map)
        {
            if(!value.empty())
            {
                image_ofs << key << ":" << " ";
                for(auto it = value.begin(); it != value.end(); ++it)
                {
                    int fpoint_x{static_cast<int>(0.5f + it->point.x() * (image.width() / static_cast<float>(background_label->width())))};
                    int fpoint_y{ static_cast<int>(0.5f + it->point.y() * (image.height() / static_cast<float>(background_label->height())))};
                    image_ofs << fpoint_x << "," << fpoint_y << " " << it->point_size << " ";
                }
                image_ofs << "\n";
            }
        }
    }
    image_ofs.close();
}
