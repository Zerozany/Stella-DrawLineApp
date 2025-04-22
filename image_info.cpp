#include "widget.h"

/// @brief 图片路径
static QString image_path{};
static QString image_dir_path{};
static QStringList images_index{};
static ssize_t image_index{};
static QString image_serial{};

void Widget::open_image() noexcept
{
    image_path = file_dialog->getOpenFileName(nullptr,"选择文件", QDir::homePath(), "JPG Images (*.jpg);;PNG Images (*.png)");
    if(!image_path.isEmpty())
    {
        //获取图片所在目录
        image_dir_path = QFileInfo(image_path).dir().absolutePath();
        read_txt((QFileInfo(image_path).dir().absolutePath() + "/" + QFileInfo(image_path).completeBaseName() + ".txt").toStdString(), LinesMap);
        //创建图片名称合集
        QStringList images_list{};
        QFileInfo(image_path).suffix().toLower() == "jpg" ? images_list << "*.jpg" : images_list << "*.png";
        //提取图片索引位置
        images_index = QDir(image_dir_path).entryList(images_list, QDir::Files);
        image_index = images_index.indexOf(QFileInfo(image_path).fileName());
        //打印图片路径、文件序号
        image_serial = QDir::toNativeSeparators(QString::number(image_index + 1) + "/" + QString::number(images_list.size()));
        image_info->setText("目录：" + image_path + "&nbsp;&nbsp;&nbsp;<font color='blue'>" + image_serial + "</font>");
        //加载图片
        load_image(image_path);
        read_txt((QFileInfo(image_path).dir().absolutePath() + "/" + QFileInfo(image_path).completeBaseName() + ".txt").toStdString(), LinesMap);
    }
    else
    {
        image_info->setText("当前目录失效，请重新选择文件目录及相应图像！");
    }
}

void Widget::back2next_image(const ImageHandle& _handle) noexcept
{
    if(!image.isNull())
    {
        write_txt(LinesMap, (QFileInfo(image_path).dir().absolutePath() + "/" + QFileInfo(image_path).completeBaseName() + ".txt").toStdString());
        point_info_v.clear();
        LinesMap.clear();
        switch (_handle)
        {
        case ImageHandle::next:
        {
            if((image_index + 1) == images_index.size())
            {
                image_path = QDir::toNativeSeparators(image_dir_path + "/" + images_index[0]);
                image_index = 0;
                break;
            }
            else
            {
                image_path = QDir::toNativeSeparators(image_dir_path + "/" + images_index[image_index + 1]);
                image_index += 1;
                break;
            }
        }
        case ImageHandle::back:
        {
            if(image_index == 0)
            {
                image_path = QDir::toNativeSeparators(image_dir_path + "/" + images_index[images_index.size() - 1]);
                image_index = images_index.size() - 1;
                break;
            }
            else
            {
                image_path = QDir::toNativeSeparators(image_dir_path + "/" + images_index[image_index - 1]);
                image_index -= 1;
                break;
            }
        }
        default:
            break;
        }
        read_txt((QFileInfo(image_path).dir().absolutePath() + "/" + QFileInfo(image_path).completeBaseName() + ".txt").toStdString(), LinesMap);
        load_image(image_path);
        image_serial = QDir::toNativeSeparators(QString::number(image_index + 1) + "/" + QString::number(images_index.size()));
        image_info->setText("目录：" + image_path + "&nbsp;&nbsp;&nbsp;<font color='blue'>" + image_serial + "</font>");
    }
    else
    {
        image_info->setText("请加载相应图像目录！");
    }
}






