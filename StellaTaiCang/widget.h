#ifndef WIDGET_H
#define WIDGET_H
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QImage>
#include <QMouseEvent>
#include <QFileDialog>
#include <QGroupBox>
#include <QComboBox>


constexpr uint32_t WINDOWS_WIDTH{930};
constexpr uint32_t WINDOWS_HEIGHT{530};
constexpr uint16_t button_size{35};

enum struct switch_iamge_enum
{
    back_image = 0,
    next_image = 1
};


class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = nullptr);
    ~Widget() noexcept;
    constexpr void windows_init() noexcept;
    constexpr void windows_layout() noexcept;
    constexpr void signals_connect_slots() noexcept;
    void show_image(QString _file_path) noexcept;
    void struct_to_txt();
    void txt_to_struct();
public:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
public slots:
    void open_directory();
    void switch_image(switch_iamge_enum image_handle);
    void revoke_draw();
    void save_line_data();
private:
    QLabel *label_image{new QLabel(this)};                                            //图片窗口背景（参与opencv窗口绑定）
    QPushButton *button_open_file{new QPushButton(this)};                             //打开文件按钮按钮
    QPushButton *button_back_image{new QPushButton(this)};                            //上一张图片按钮
    QPushButton *button_next_image{new QPushButton(this)};                            //下一张图片按钮
    QPushButton *button_revoke{new QPushButton(this)};                                //撤回按钮
    QLabel *label_file_name{new QLabel(this)};
    /// @param select_GroupBox_windows
    /// \brief select_win
    /// \value child control
    QGroupBox *select_win{new QGroupBox(tr("路况信息"),this)};
    QLabel *label_direction{new QLabel(select_win)};
    QComboBox *combox_direction{new QComboBox(select_win)};
    QLabel *label_egress{new QLabel(select_win)};
    QComboBox *combox_egress{new QComboBox(select_win)};
	
    QLabel *label_region{new QLabel(select_win)};
    QLineEdit *edit_region{new QLineEdit(select_win)};
    QLabel *label_intersection{new QLabel(select_win)};
    QLineEdit *edit_intersection{new QLineEdit(select_win)};
    QPushButton *button_info_complete{new QPushButton(select_win)};
    QLabel *label_direction_number{new QLabel(select_win)};
    QLineEdit *edit_direction{new QLineEdit(select_win)};

    QGroupBox *init_info_win{new QGroupBox(tr("Stella"),this)};
    QLabel *stella_logo{new QLabel(init_info_win)};

    QFileDialog *file_dialog_win{new QFileDialog(this)};
    QString image_directory_path{};
    QStringList images_list{};
    QString directory_name{};
    uint32_t select_image_index{};
    QImage q_image{};
};
#endif // WIDGET_H
