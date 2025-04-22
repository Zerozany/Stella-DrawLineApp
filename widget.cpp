#include "widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    std::invoke(&Widget::init_ui, this);
    std::invoke(&Widget::signal_connect_slot, this);
    image_info->setText("<font color = 'red'>注意：该软件不支持目录或文件等包含中文路径！</font>");
}

Widget::~Widget() {}

void Widget::signal_connect_slot()
{
    connect(select_image_button, &QPushButton::clicked, this, &Widget::open_image);
    connect(back_image_button, &QPushButton::clicked, [this]{back2next_image(ImageHandle::back);});
    connect(next_image_button, &QPushButton::clicked, [this]{back2next_image(ImageHandle::next);});
    connect(revoke_button, &QPushButton::clicked, this, &Widget::revoke_drawed);
}


void Widget::resizeEvent(QResizeEvent *event)
{
    /// @brief 实时检测窗口分辨率变化
    if (!image.isNull())
    {
        background_label->setPixmap(QPixmap::fromImage(image.scaled(background_label->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    }
    QWidget::resizeEvent(event);
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if(!image.isNull())
    {
        std::invoke(&Widget::save_points, this, event);
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if(!image.isNull())
    {
        std::invoke(&Widget::find_movein_point, this, event);
    }
}

void Widget::paintEvent(QPaintEvent *event)
{
    if(!image.isNull())
    {
        std::invoke(&Widget::draw_image, this);
    }
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    if(!image.isNull())
    {
        std::invoke(&Widget::save_line, this, event);
    }
}

void Widget::wheelEvent(QWheelEvent *event)
{
    if(!image.isNull())
    {
        std::invoke(&Widget::change_point_size, this, event);
    }
}

void Widget::load_image(const QString &_iamge_path) noexcept
{
    image.load(_iamge_path);
    background_label->setPixmap(QPixmap::fromImage(image.scaled(background_label->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
}
