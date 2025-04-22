#include "widget.h"

void Widget::init_window() noexcept
{
    this->setLayoutDirection(Qt::LeftToRight);
    this->setWindowState(Qt::WindowNoState);
    this->setWindowTitle("昆山星际舟车道线标注系统");
    this->setWindowIcon(QIcon(":/icons/Pictogrammers-Material-Road-variant.128.png"));
    this->setMouseTracking(true);
    this->setFocusPolicy(Qt::StrongFocus);
    this->setStyleSheet("font-family:'微软雅黑';");
    this->setMinimumSize(WINDOW_WIDTH, WINDOW_HEIGHT);
}

void Widget::init_ui() noexcept
{
    std::invoke(&Widget::init_window, this);
    std::invoke(&Widget::init_picture, this);
    std::invoke(&Widget::init_control, this);
    ui_vlayout->addWidget(background_label, 12);
    ui_vlayout->addWidget(control_box, 1);
}

void Widget::init_picture() noexcept
{
    /// @brief 绘制窗口初始化
    background_label->setStyleSheet("background-color:black;");
    background_label->setAlignment(Qt::AlignCenter);
    background_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    background_label->installEventFilter(this);
    background_label->setMouseTracking(true);
}

void Widget::init_control() noexcept
{
    select_image_button->setIcon(QIcon(":/icons/Microsoft-Fluentui-Emoji-Mono-Open-File-Folder.32.png"));
    back_image_button->setIcon(QIcon(":/icons/Icons8-Ios7-Arrows-Left.32.png"));
    revoke_button->setIcon(QIcon(":/icons/Amitjakhu-Drip-Return.32.png"));
    next_image_button->setIcon(QIcon(":/icons/Icons8-Ios7-Arrows-Right.32.png"));

    select_image_button->setToolTip("选择文件");
    back_image_button->setToolTip("上一张");
    revoke_button->setToolTip("撤回");
    next_image_button->setToolTip("下一张");

    select_image_button->setShortcut(tr("o", "O"));
    back_image_button->setShortcut(tr("a", "A"));
    revoke_button->setShortcut(tr("r", "R"));
    next_image_button->setShortcut(tr("d", "D"));

    select_image_button->setFixedSize(BUTTON_SIZE, BUTTON_SIZE);
    back_image_button->setFixedSize(BUTTON_SIZE, BUTTON_SIZE);
    revoke_button->setFixedSize(BUTTON_SIZE, BUTTON_SIZE);
    next_image_button->setFixedSize(BUTTON_SIZE, BUTTON_SIZE);

    app_info->installEventFilter(this);
    app_info->setText("软件简介");
    app_info->setStyleSheet("font-family:'微软雅黑';font-size:12px;color:blue;text-decoration:underline;");
    image_info->setStyleSheet("font-family:'微软雅黑';font-size:12px;");

    QHBoxLayout* control_hlayout{new QHBoxLayout{control_box}};
    QHBoxLayout* select_image_hlayout{new QHBoxLayout{}};
    QHBoxLayout* control_image_hlayout{new QHBoxLayout{}};
    QHBoxLayout* app_info_hlayout{new QHBoxLayout{}};

    select_image_hlayout->addWidget(select_image_button, Qt::AlignCenter);
    select_image_hlayout->addWidget(image_info, Qt::AlignCenter);
    control_image_hlayout->addWidget(back_image_button, Qt::AlignCenter);
    control_image_hlayout->addWidget(revoke_button, Qt::AlignCenter);
    control_image_hlayout->addWidget(next_image_button, Qt::AlignCenter);
    app_info_hlayout->addWidget(app_info, Qt::AlignCenter);

    control_hlayout->addLayout(select_image_hlayout, 0);
    control_hlayout->addStretch();
    control_hlayout->addLayout(control_image_hlayout, 0);
    control_hlayout->addStretch();
    control_hlayout->addStretch();
    control_hlayout->addLayout(app_info_hlayout, 0);
}




























