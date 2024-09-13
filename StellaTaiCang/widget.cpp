#include "widget.h"
#include "TaiCanyData.h"

void Widget::mousePressEvent(QMouseEvent *event)
{
    if(!image_directory_path.isEmpty())
    {
        static uint8_t convert_tag{};
        if(event->button() == Qt::LeftButton)
        {
            if(convert_tag == 2)
            {
                if(section_points_v.size() == 1)
                {
                    section_points_v.clear();
                }
                else if(section_points_v.size() > 1)
                {
                    section_lines_v.emplace_back(section_points_v);
                    section_points_v.clear();
                }
            }
            convert_tag = 1;
            load_points_v.emplace_back(event->pos());
        }
        else if(event->button() == Qt::RightButton)
        {
            if(convert_tag == 1)
            {
                if(load_points_v.size() == 1)
                {
                    load_points_v.clear();
                }
                else if(load_points_v.size() > 1)
                {
                    load_lines_v.emplace_back(load_points_v);
                    load_points_v.clear();
                }
            }
            convert_tag = 2;
            section_points_v.emplace_back(event->pos());
        }
        update();
        if (event->button() == Qt::LeftButton && event->pos() != edit_region->pos())
        {
            edit_region->clearFocus();
        }
        if (event->button() == Qt::LeftButton && event->pos() != edit_intersection->pos())
        {
            edit_intersection->clearFocus();
        }
        if (event->button() == Qt::LeftButton && event->pos() != edit_direction->pos())
        {
            edit_direction->clearFocus();
        }
    }
}

void Widget::paintEvent(QPaintEvent *event)
{
    if(!image_directory_path.isEmpty())
    {
        // 将绘制的内容更新到 QLabel
        q_image.load(image_directory_path);
        // 创建 QPainter 对象并关联到 q_image 上
        QPainter painter(&q_image);
        // 设置画笔的抗锯齿属性
        painter.setRenderHint(QPainter::Antialiasing, true);

        // 绘制点
        if(load_points_v.size() > 0 || section_points_v.size() > 0 )
        {
            painter.setPen(QPen(Qt::yellow, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            for (const QPoint& point : load_points_v)
            {
                painter.drawPoint(point);
            }
            for (const QPoint& point : section_points_v)
            {
                painter.drawPoint(point);
            }
        }
        // 绘制线段 or 绘制截面
        if (load_points_v.size() > 1 || section_points_v.size() > 1)
        {
            painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            for (size_t i = 1; i < load_points_v.size(); ++i)
            {
                painter.drawLine(load_points_v[i - 1], load_points_v[i]);
            }
            painter.setPen(QPen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            for (size_t i = 1; i < section_points_v.size(); ++i)
            {
                painter.drawLine(section_points_v[i - 1], section_points_v[i]);
            }
        }

        if(load_lines_v.size() > 0 || section_lines_v.size() > 0)
        {
            painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            for(const auto& lines_v : load_lines_v)
            {
                for (size_t i = 1; i < lines_v.size(); ++i)
                {
                    painter.drawLine(lines_v[i - 1], lines_v[i]);
                }
            }
            painter.setPen(QPen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            for(const auto& lines_v : section_lines_v)
            {
                for (size_t i = 1; i < lines_v.size(); ++i)
                {
                    painter.drawLine(lines_v[i - 1], lines_v[i]);
                }
            }
        }

        // 将绘制的内容更新到 QLabel
        label_image->setPixmap(QPixmap::fromImage(q_image));
    }
}



void Widget::revoke_draw()
{
    if(!image_directory_path.isEmpty())
    {
        if(load_points_v.size() > 0)
        {
            load_points_v.pop_back();
        }
        else if(section_points_v.size() > 0)
        {
            section_points_v.pop_back();
        }
        update();
    }
}


void Widget::save_line_data()
{
    if(load_points_v.size() > 1)
    {
        load_lines_v.emplace_back(load_points_v);
        load_points_v.clear();
    }
    else if(section_points_v.size() > 1)
    {
        section_lines_v.emplace_back(section_points_v);
        section_points_v.clear();
    }
    update();
}


constexpr void Widget::signals_connect_slots() noexcept
{
    connect(button_open_file, &QPushButton::clicked, this, &Widget::open_directory);
    connect(button_back_image, &QPushButton::clicked, this, [&] {switch_image(switch_iamge_enum::back_image);});
    connect(button_next_image, &QPushButton::clicked, this, [&] {switch_image(switch_iamge_enum::next_image);});
    connect(button_revoke, &QPushButton::clicked, this, Widget::revoke_draw);
    connect(button_info_complete, &QPushButton::clicked, this, &Widget::save_line_data);
    connect(combox_direction, &QComboBox::currentIndexChanged, this, [&]{
        int8_t combox_direction_index = combox_direction->currentIndex();
        data_txt.direction_number = combox_direction_index;
    });
    connect(combox_egress, &QComboBox::currentIndexChanged, this, [&]{
        int8_t combox_direction_index = combox_egress->currentIndex();
        data_txt.eqress_type = combox_direction_index;
    });
}

//-----------------------------------------------------------------------------------
Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    windows_init();
    windows_layout();
    signals_connect_slots();
}

Widget::~Widget() noexcept
{

}

constexpr void Widget::windows_init() noexcept
{
    this->setFixedSize(WINDOWS_WIDTH,WINDOWS_HEIGHT);
    this->setWindowTitle("StellaTaiCang");
    this->setWindowIcon(QIcon(":/image/Pictogrammers-Material-Road-variant.128.png"));
    this->setMouseTracking(true);
}

constexpr void Widget::windows_layout() noexcept
{
    label_image->setGeometry(5, 5, 720, 480);
    button_open_file->setGeometry(5, label_image->height() + 5 + 5, button_size, button_size);
    button_back_image->setGeometry(307, label_image->height() + 5 + 5, button_size, button_size);
    button_revoke->setGeometry(347, label_image->height() + 5 + 5, button_size, button_size);
    button_next_image->setGeometry(387, label_image->height() + 5 + 5, button_size, button_size);
    label_file_name->setGeometry(450, label_image->height() + 5 + 10, 270, 25);
    /// @value 路况信息组件
    select_win->setGeometry(label_image->width() + 10, 5, this->width() - (label_image->width()) - 15, this->height() - 10);
    label_direction->setGeometry(10, 30, 80, 30);
    combox_direction->setGeometry(90, 30, 100, 30);
    label_egress->setGeometry(10, 70, 80, 30);
    combox_egress->setGeometry(90, 70, 100, 30);
    label_region->setGeometry(10, 110, 80, 30);
    edit_region->setGeometry(92, 112, 95, 25);
    label_intersection->setGeometry(10, 150, 80, 30);
    edit_intersection->setGeometry(92, 152, 95, 25);
    label_direction_number->setGeometry(10, 190, 80, 30);
    edit_direction->setGeometry(92, 192, 95, 25);
    button_info_complete->setGeometry(50, 480, 100, 30);
    init_info_win->setGeometry(label_image->width() + 10, 5, this->width() - (label_image->width()) - 15, this->height() - 10);
    stella_logo->setGeometry(30, 200, 141, 71);
    /// @value 图标
    button_open_file->setIcon(QIcon(":/image/Microsoft-Fluentui-Emoji-Mono-Open-File-Folder.32.png"));
    button_back_image->setIcon(QIcon(":/image/Icons8-Ios7-Arrows-Left.32.png"));
    button_next_image->setIcon(QIcon(":/image/Icons8-Ios7-Arrows-Right.32.png"));
    button_revoke->setIcon(QIcon(":/image/Amitjakhu-Drip-Return.32.png"));
    button_info_complete->setIcon(QIcon(":/image/Icons8-Windows-8-Database-Accept.32.png"));
    stella_logo->setPixmap(QPixmap(":/image/c638f69d8df96a7859a5274115d84d5.png").scaled(stella_logo->size(), Qt::KeepAspectRatio));

    /// @value 按钮提示
    button_open_file->setToolTip("选择文件(O)");
    button_back_image->setToolTip("上一张(A)");
    button_next_image->setToolTip("下一张(D)");
    button_revoke->setToolTip("撤回(R)");
    button_info_complete->setToolTip("保存线段(Return)");
    /// @value 快捷键
    button_open_file->setShortcut(tr("o","O"));
    button_back_image->setShortcut(tr("a","A"));
    button_next_image->setShortcut(tr("d","D"));
    button_revoke->setShortcut(tr("r","R"));
    button_info_complete->setShortcut(Qt::Key_Return);
    /// @value 组件风格
    label_image->setStyleSheet("background-color:black;");
    label_file_name->setStyleSheet("font-family:'微软雅黑';font-size:10px;border:none;");
    select_win->setStyleSheet("font-family:'微软雅黑';");
    init_info_win->setStyleSheet("font-family:'微软雅黑';");
    // stella_logo->setStyleSheet("background-color:black;");
    edit_region->setStyleSheet("font-family:'微软雅黑';font-size:13px;border:none;");
    edit_intersection->setStyleSheet("font-family:'微软雅黑';font-size:13px;border:none;");
    edit_direction->setStyleSheet("font-family:'微软雅黑';font-size:13px;border:none;");
    /// @value 初始化文本框
    edit_region->setPlaceholderText("区域编号...");
    edit_intersection->setPlaceholderText("路口编号...");
    edit_direction->setPlaceholderText("区域方向...");
    /// @value 路口方向小组件
    label_direction->setText("路口方向：");
    combox_direction->addItem("          北");
    combox_direction->addItem("          东");
    combox_direction->addItem("          南");
    combox_direction->addItem("          西");
    /// @value 进出口类型小组件
    label_egress->setText("进出口类型：");
    combox_egress->addItem("        进口");
    combox_egress->addItem("        出口");
    /// @value 区域以及路口标号
    label_region->setText("区域编号：");
    label_intersection->setText("路口编号：");
    label_direction_number->setText("区域方向：");
    /// @code 隐藏
    select_win->hide();
    /// @code 事件监听
    this->installEventFilter(edit_region);
}

/// \name 监听窗口事件
/// \brief Widget::eventFilter
/// \param obj
/// \param event
/// \return bool event
bool Widget::eventFilter(QObject *obj, QEvent *event)
{
    return Widget::eventFilter(obj, event);
}

/// \name 打开图像所在目录
/// \brief Widget::open_directory
/// \return nullptr
void Widget::open_directory()
{
    //获取图片路径
    image_directory_path = file_dialog_win->getOpenFileName(nullptr,"选择文件夹", QDir::homePath(), "JPG Images (*.jpg);;PNG Images (*.png)");
    if(!image_directory_path.isEmpty())
    {
        //获取图片所在目录
        directory_name = QFileInfo(image_directory_path).dir().absolutePath();
        //创建图片名称合集
        QStringList images;
        if(QFileInfo(image_directory_path).suffix().toLower() == "jpg")
        {
            images << "*.jpg";
        }
        else
        {
            images << "*.png";
        }
        //提取图片索引位置
        images_list = QDir(directory_name).entryList(images, QDir::Files);
        select_image_index = images_list.indexOf(QFileInfo(image_directory_path).fileName());
        //创建数据存储文件夹
        QString DataDir = QDir(directory_name).filePath("..") + R"(/StellaTaiCany-Data/)";
        if(!QDir(DataDir).exists())
        {
            QDir().mkpath(DataDir);
        }
        read_files_data(image_directory_path);
        txt_to_struct();
        show_image(image_directory_path);
        QFileInfo file_info(image_directory_path);
        label_file_name->setText(file_info.fileName());
        init_info_win->hide();
        select_win->show();
        update();
    }
    else
    {
        image_directory_path = image_directory_path;
    }
}

void Widget::struct_to_txt()
{
    if(load_points_v.size() > 1)
    {
        load_lines_v.emplace_back(load_points_v);
        load_points_v.clear();
    }
    if(section_points_v.size() > 1)
    {
        section_lines_v.emplace_back(section_points_v);
        section_points_v.clear();
    }
    data_txt.direction = edit_direction->text().toStdString();
    data_txt.regin_number = edit_region->text().toStdString();
    data_txt.intersection_number = edit_intersection->text().toStdString();
    data_txt.section_number = section_lines_v.size();
}

void Widget::txt_to_struct()
{
    edit_direction->setText(QString::fromStdString(data_txt.direction));
    edit_region->setText(QString::fromStdString(data_txt.regin_number));
    edit_intersection->setText(QString::fromStdString(data_txt.intersection_number));
    combox_direction->setCurrentIndex(data_txt.direction_number);
    combox_egress->setCurrentIndex(data_txt.eqress_type);
}

/// \name 切换图像
/// \brief Widget::switch_image
/// \param image_handle 上/下一张图片
/// \return nullptr
void Widget::switch_image(switch_iamge_enum image_handle)
{
    if(!image_directory_path.isEmpty())
    {
        struct_to_txt();
        write_files_date(image_directory_path, data_txt);
        switch (image_handle)
        {
        case switch_iamge_enum::back_image:
        {
            if((select_image_index + 1) == images_list.size())
            {
                image_directory_path = QDir::toNativeSeparators(directory_name + "/" + images_list[0]);
                select_image_index = 0;
                break;
            }
            else
            {
                image_directory_path = QDir::toNativeSeparators(directory_name + "/" + images_list[select_image_index + 1]);
                select_image_index += 1;
                break;
            }
        }
        case switch_iamge_enum::next_image:
        {
            if(select_image_index == 0)
            {
                image_directory_path = QDir::toNativeSeparators(directory_name + "/" + images_list[images_list.size() - 1]);
                select_image_index = images_list.size() - 1;
                break;
            }
            else
            {
                image_directory_path = QDir::toNativeSeparators(directory_name + "/" + images_list[select_image_index - 1]);
                select_image_index -= 1;
                break;
            }
        }
        default:
            break;
        }
        show_image(image_directory_path);
        read_files_data(image_directory_path);
        txt_to_struct();
        QFileInfo file_info(image_directory_path);
        label_file_name->setText(file_info.fileName());
        update();
    }
    else
    {
        return;
    }
}

/// \name 展示/加载图像
/// \brief Widget::show_image
/// \param _file_path 图像所在路径
/// \return nullptr
void Widget::show_image(QString _file_path) noexcept
{
    q_image.load(_file_path);
    label_image->setPixmap(QPixmap::fromImage(q_image));
    label_image->setScaledContents(true);
}


