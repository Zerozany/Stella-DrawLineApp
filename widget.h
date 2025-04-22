_Pragma("once");
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QImage>
#include <QMouseEvent>
#include <fstream>
#include "param-structure.h"


constexpr int BUTTON_SIZE{40};
constexpr int WINDOW_WIDTH{1260};
constexpr int WINDOW_HEIGHT{800};

enum struct [[maybe_unused]] ImageHandle: std::int32_t
{
    back = 0,
    next = 1
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);

    ~Widget();

private:
    /// @brief 初始化win
    void init_window() noexcept;

    /// @brief 初始化ui
    void init_ui() noexcept;

    /// @brief 画布布局
    void init_picture() noexcept;

    /// @brief 控件布局
    void init_control() noexcept;

    /// @brief 加载图片
    void load_image(const QString& _iamge_path) noexcept;

    /// @brief 绑定信号与槽
    void signal_connect_slot();

    /// @brief 保存绘制点
    void save_points(QMouseEvent *event);

    /// @brief 绘制图像点
    void draw_image();

    /// @brief 搜寻坐标点
    void find_movein_point(QMouseEvent *event);

    /// @brief 保存线段
    void save_line(QKeyEvent *event);

    /// @brief 调整线段粗细
    void change_point_size(QWheelEvent *event);


    void read_txt(const std::string& _image_path, std::map<std::int32_t, std::vector<PointInfo>>& _lines_map);

    void write_txt(const std::map<std::int32_t, std::vector<PointInfo>>& _lines_map, const std::string& _image_path);

private:
    /// @brief 窗口分辨率监听
    void resizeEvent(QResizeEvent *event) override;

    /// @brief 鼠标点击事件
    void mousePressEvent(QMouseEvent *event) override;

    /// @brief 鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event) override;

    /// @brief 绘制时间
    void paintEvent(QPaintEvent *event) override;

    /// @brief 按键事件
    void keyPressEvent(QKeyEvent *event) override;

    /// @brief 滚轮事件
    void wheelEvent(QWheelEvent *event) override;

private slots:
    /// @brief 选择图片
    void open_image() noexcept;

    /// @brief 切换图片
    void back2next_image(const ImageHandle& _handle) noexcept;

    /// @brief 撤回
    void revoke_drawed();

private:
    /// @brief 加载图片背景
    QLabel* background_label{new QLabel{this}};

    /// @brief 打开文件夹按钮
    QPushButton* select_image_button{new QPushButton{this}};

    /// @brief 图片信息
    QLabel* image_info{new QLabel{this}};

    /// @brief 上一张图片
    QPushButton* back_image_button{new QPushButton{this}};

    /// @brief 撤回
    QPushButton* revoke_button{new QPushButton{this}};

    /// @brief 下一张图片
    QPushButton* next_image_button{new QPushButton{this}};

    /// @brief 软件信息标签
    QLabel* app_info{new QLabel{this}};

    /// @brief 控件总布局容器
    QGroupBox* control_box{new QGroupBox(this)};

    /// @brief ui总布局
    QVBoxLayout* ui_vlayout{new QVBoxLayout{this}};

    /// @brief 文件对话框
    QFileDialog* file_dialog{new QFileDialog{this}};

    /// @brief 绘制图片图层
    QImage image{};
};

