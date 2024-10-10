#include "widget.h"


std::vector<QPoint> calculate_vertices(const QPoint& _down_point, const QPoint& _up_point, const int& _base_length)
{
    //计算底边中点到顶点的向量
    int dx{std::abs(_up_point.x()- _down_point.x())};
    int dy{std::abs(_up_point.y()- _down_point.y())};
    //得到单位向量u
    float ux{dx / std::sqrt(dx * dx + dy * dy)};
    float uy{dy / std::sqrt(dx * dx + dy * dy)};
    //计算端点坐标
    QPoint left_point{}, right_point{};
    if((_down_point.x() > _up_point.x() && _down_point.y() < _up_point.y()) || (_down_point.x() < _up_point.x() && _down_point.y() > _up_point.y()))
    {
        left_point.setX(_down_point.x() + (_base_length * uy / 2));
        right_point.setX(_down_point.x() - (_base_length * uy / 2));
    }
    else
    {
        left_point.setX(_down_point.x() - (_base_length * uy / 2));
        right_point.setX(_down_point.x() + (_base_length * uy / 2));
    }
    left_point.setY(_down_point.y() + (_base_length * ux / 2));
    right_point.setY(_down_point.y() - (_base_length * ux / 2));
    //返回左右顶点
    std::vector<QPoint> vertex_v{};
    vertex_v.emplace_back(left_point);
    vertex_v.emplace_back(right_point);
    return vertex_v;
}

void Widget::save_points(QMouseEvent *event)
{
    int press_x{event->pos().x() - background_label->pos().x()};
    int press_y{event->pos().y() - background_label->pos().y()};
    if (press_x >= 0 && press_x < background_label->size().width() && press_y >= 0 && press_y < background_label->size().height())
    {
        if (event->button() == Qt::LeftButton)
        {
            point_info.point = QPoint(press_x, press_y);
            point_info_v.emplace_back(std::move(point_info));
        }
    }
    update();
}

void Widget::draw_image()
{
    QImage copy_image = image.scaled(background_label->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation).copy();
    // 创建 QPainter 对象并关联到qimage上
    QPainter painter{&copy_image};
    // 设置画笔的抗锯齿属性
    painter.setRenderHint(QPainter::Antialiasing, true);
    // 绘制点
    if(!point_info_v.empty())
    {
        for (const auto& p_info : point_info_v)
        {
            [[unlikely]] if(!movein_point_v.empty() && p_info.point == movein_point_v[0].point)
            {
                painter.setPen(QPen(Qt::red, Qt::NoPen, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)), painter.setBrush(Qt::red);
            }
            else
            {
                painter.setPen(QPen(Qt::yellow, Qt::NoPen, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)), painter.setBrush(Qt::yellow);
            }
            painter.drawEllipse(p_info.point, point_radius, point_radius);
        }
    }
    // 绘制线段
    if(point_info_v.size() > 1)
    {
        painter.setPen(QPen(Qt::yellow, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        for(std::size_t i{0}; i < point_info_v.size() - 1; ++i)
        {
            painter.drawLine(point_info_v[i].point, point_info_v[i + 1].point);
        }
        /// @param 绘制矩形
        painter.setPen(QPen(Qt::yellow, 1)), painter.setBrush(Qt::NoBrush);
        for (auto it = point_info_v.begin(); it != point_info_v.end(); ++it)
        {
            std::vector<QPoint> line_points_v{};
            if((it + 1) != point_info_v.end())
            {
                line_points_v.emplace_back(calculate_vertices(it->point, (it + 1)->point, it->point_size)[0]);
                line_points_v.emplace_back(calculate_vertices((it + 1)->point, it->point, (it + 1)->point_size)[0]);
                line_points_v.emplace_back(calculate_vertices((it + 1)->point, it->point, (it + 1)->point_size)[1]);
                line_points_v.emplace_back(calculate_vertices(it->point, (it + 1)->point, it->point_size)[1]);
                line_points_v.emplace_back(calculate_vertices(it->point, (it + 1)->point, it->point_size)[0]);
            }
            else if((it + 1) == point_info_v.end())
            {
                line_points_v.emplace_back(calculate_vertices(it->point , (it - 1)->point , it->point_size)[0]);
                line_points_v.emplace_back(calculate_vertices((it - 1)->point , it->point , (it - 1)->point_size)[0]);
                line_points_v.emplace_back(calculate_vertices((it - 1)->point , it->point , (it - 1)->point_size)[1]);
                line_points_v.emplace_back(calculate_vertices(it->point , (it - 1)->point , it->point_size)[1]);
                line_points_v.emplace_back(calculate_vertices(it->point , (it - 1)->point , it->point_size)[0]);
            }
            painter.drawPolygon(line_points_v.data(), line_points_v.size());
        }
    }
    // 总体绘制
    if(!LinesMap.empty())
    {
        for(const auto& [line_num, line_stu] : LinesMap)
        {
            if(line_stu.size() > 1)
            {
                /// @param 绘制线段序号
                painter.setPen(QPen(QColor("#00FFFF"), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)), painter.setFont(QFont("Arial", 20));
                painter.drawText(line_stu[1].point, QString::number(line_num));
                /// @param 绘制线段点
                for(const auto& p_info : line_stu)
                {
                    [[unlikely]] if(!movein_point_v.empty() && p_info.point == movein_point_v[0].point)
                    {
                        painter.setPen(QPen(Qt::red, Qt::NoPen, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)), painter.setBrush(Qt::red);
                    }
                    else
                    {
                        painter.setPen(QPen(Qt::yellow, Qt::NoPen, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)), painter.setBrush(Qt::yellow);
                    }
                    painter.drawEllipse(p_info.point, point_radius, point_radius);
                }
                /// @param 绘制线段
                painter.setPen(QPen(Qt::yellow, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                for(std::size_t i{0}; i < line_stu.size() - 1; ++i)
                {
                    painter.drawLine(line_stu[i].point, line_stu[i + 1].point);
                }
                /// @param 绘制矩形
                painter.setPen(QPen(Qt::yellow, 1)), painter.setBrush(Qt::NoBrush);
                for (auto it = line_stu.begin(); it != line_stu.end(); ++it)
                {
                    std::vector<QPoint> line_points_v{};
                    if((it + 1) != line_stu.end())
                    {
                        line_points_v.emplace_back(calculate_vertices(it->point, (it + 1)->point, it->point_size)[0]);
                        line_points_v.emplace_back(calculate_vertices((it + 1)->point, it->point, (it + 1)->point_size)[0]);
                        line_points_v.emplace_back(calculate_vertices((it + 1)->point, it->point, (it + 1)->point_size)[1]);
                        line_points_v.emplace_back(calculate_vertices(it->point, (it + 1)->point, it->point_size)[1]);
                        line_points_v.emplace_back(calculate_vertices(it->point, (it + 1)->point, it->point_size)[0]);
                    }
                    else if((it + 1) == line_stu.end())
                    {
                        line_points_v.emplace_back(calculate_vertices(it->point , (it - 1)->point , it->point_size)[0]);
                        line_points_v.emplace_back(calculate_vertices((it - 1)->point , it->point , (it - 1)->point_size)[0]);
                        line_points_v.emplace_back(calculate_vertices((it - 1)->point , it->point , (it - 1)->point_size)[1]);
                        line_points_v.emplace_back(calculate_vertices(it->point , (it - 1)->point , it->point_size)[1]);
                        line_points_v.emplace_back(calculate_vertices(it->point , (it - 1)->point , it->point_size)[0]);
                    }
                    painter.drawPolygon(line_points_v.data(), line_points_v.size());
                }
            }
        }
    }
    background_label->setPixmap(QPixmap::fromImage(copy_image));
}

void Widget::find_movein_point(QMouseEvent *event)
{
    movein_point_v.clear();
    int move_x{event->pos().x() - background_label->pos().x()};
    int move_y{event->pos().y() - background_label->pos().y()};
    for (const auto& p_info : point_info_v)
    {
        if (static_cast<int>(pow(move_x - p_info.point.x(), 2) + pow(move_y - p_info.point.y(), 2) - pow(point_radius, 2)) <= 0)
        {
            movein_point_v.emplace_back(std::move(p_info));
        }
    }
    for (const auto& [line_num, line_stu] : LinesMap)
    {
        for(const auto& p_info : line_stu)
        {
            if (static_cast<int>(pow(move_x - p_info.point.x(), 2) + pow(move_y - p_info.point.y(), 2) - pow(point_radius, 2)) <= 0)
            {
                movein_point_v.emplace_back(std::move(p_info));
            }
        }
    }
    update();
}

void Widget::save_line(QKeyEvent *event)
{
    int KeyValue = event->key();
    if(KeyValue >= Qt::Key_0 && KeyValue <= Qt::Key_6)
    {
        LinesMap[KeyValue - Qt::Key_0] = point_info_v;
        point_info_v.clear();
    }
    update();
}

void Widget::change_point_size(QWheelEvent *event)
{
    int scrollDelta{event->angleDelta().y()};
    [[likely]] if(scrollDelta > 0 && !movein_point_v.empty())
    {
        movein_point_v[0].point_size < 200 ? movein_point_v[0].point_size += 5 : movein_point_v[0].point_size = 200;
    }
    else if(scrollDelta < 0 && !movein_point_v.empty())
    {
        movein_point_v[0].point_size > 1 ? movein_point_v[0].point_size -= 5 : movein_point_v[0].point_size = 1;
    }
    else if(movein_point_v.empty())
    {
        return;
    }
    for(auto& p_info : point_info_v)
    {
        if(p_info.point == movein_point_v[0].point)
        {
            p_info.point_size = movein_point_v[0].point_size;
        }
    }
    for(auto& [line_num, line_stu] : LinesMap)
    {
        for(auto& p_info : line_stu)
        {
            if(p_info.point == movein_point_v[0].point)
            {
                p_info.point_size = movein_point_v[0].point_size;
            }
        }
    }
    update();
}

void Widget::revoke_drawed()
{
    if(!point_info_v.empty())
    {
        point_info_v.pop_back();
    }
    update();
}


