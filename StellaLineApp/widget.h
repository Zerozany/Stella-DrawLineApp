#ifndef WIDGET_H
#define WIDGET_H

#define _WINDOWS_SIZE 1545,930
#define _BUTTON_SIZE 40
#define _BACK_PICTURE 0
#define _NEXT_PICTURE 1

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QFileDialog>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QDialog>

#include <QMouseEvent>
#include <QKeyEvent>
#include <QEvent>

#include <cmath>
#include <fstream>


class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
private:
    void self_windows();                                //APP窗口头信息
    void set_part();                                    //软件元素及布局
    void connect_signal_slots();                        //连接信号与槽总接口
    void pix_read_show_clone(QString _filepath);        //读取展示克隆原图
    void image_setting();                               //qt || opencv 窗口结合
    void mouse_callback();                              //绘图回调函数
    void revoke_image_line();                           //撤回函数
    void WriteFilesDate(QString _FilePath);             //写入txt点位数据函数
    void ReadFilesData(QString _FilePath);              //读取txt点位数据函数
    static void draw_line(int event,int x,int y,int flags,void *userdate);  //绘制线条函数
private:
    bool eventFilter(QObject *object, QEvent *event);   //鼠标状态跟踪库函数
    void keyPressEvent(QKeyEvent *event);               //鼠标按键库函数
    void wheelEvent(QWheelEvent *event);                //鼠标滑轮事件
    void resizeEvent(QResizeEvent *event);              //全局UI伸缩变换
public slots:
    void open_picture();                                //打开绘制文件
    void back_and_next_picture(int _BackOrNext);        //切换图片
private:
    static QLabel *VehicleLineBackgroundImage;          //图片窗口背景（参与opencv窗口绑定）
    QPushButton *SelectImage;                           //打开文件按钮按钮
    QPushButton *BackImage;                             //上一张图片按钮
    QPushButton *NextImage;                             //下一张图片按钮
    QPushButton *Revoke;                                //撤回按钮
    QFileDialog *ImageFile;                             //文件对话框
    QLabel *ImageDirsName;                              //图片名称动态标签
    QLabel *ImageNumbers;                               //图片序号动态标签
    QLabel *APP_INFO;                                   //软件著作信息
    QStringList FilesListName;                          //图片名称汇总集合
    QString ImageFileDir;                               //图片所在目录名称
    static QLabel *PointCoordinate;                     //坐标实时显示
    static QString ImageName;                           //动态图片路径
    int IndexOfSelectedFile;                            //图片索引
    QString SerialNumber;                               //动态图片名称
    static std::string VehicleWindowName;               //opencv图片窗口名称（涉及绘制函数）
    QString filePathBackups;                            //图片路径备份
};
#endif // WIDGET_H