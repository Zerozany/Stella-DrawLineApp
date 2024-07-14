#include "widget.h"
#include "DataStructure.h"

QLabel* Widget::VehicleLineBackgroundImage{};                   //图片窗口背景（参与opencv窗口绑定）
QString Widget::ImageName;                                      //动态图片路径
bool ImageSettingflags = true;                                  //Qt与opencv窗口融合开关
std::string Widget::VehicleWindowName = "VehicleDrawSystem";    //opencv图片窗口名称（涉及绘制函数）
cv::Mat InitImage;                                              //原始图片
cv::Mat VehiclePix;                                             //绘制图片原图（需要被类静态函数调用）
cv::Mat TryVehiclePix;                                          //绘制图片绘制面板（需要被类静态函数调用）
int TemporaryPointSize;                                         //临时线段点尺寸（需要被类静态函数调用）
bool ModifyPointSize_Tag = false;                               //线段点尺寸调整开关（需要被类静态函数调用）
void ReadAndDrawVector();                                       //遍历并绘制图像（需要被类静态函数调用）
void PaintFillPoly();                                           //绘制四边形函数（需要维护，故与“ReadAndDrawVector”函数暂时分开）
QLabel* Widget::PointCoordinate{};                              //坐标实时显示标签（可忽略）
std::vector<cv::Point> CountPointNate(const cv::Point down_point, const cv::Point up_point, const int base_length); //计算底边顶点坐标函数
int g_VehicleLineBackgroundImage_width{};
int g_VehicleLineBackgroundImage_height{};


void Widget::open_picture()
{
    //获取图片路径
    ImageName = ImageFile->getOpenFileName(nullptr,"选择文件夹", QDir::homePath(), "JPG Images (*.jpg);;PNG Images (*.png)");
    if(!ImageName.isEmpty())
    {
        //获取图片所在目录
        ImageFileDir = QFileInfo(ImageName).dir().absolutePath();
        //创建图片名称合集
        QStringList nameFilters;
        if(QFileInfo(ImageName).suffix().toLower() == "jpg")
        {
            nameFilters << "*.jpg";
        }
        else
        {
            nameFilters << "*.png";
        }
        //提取图片索引位置
        FilesListName = QDir(ImageFileDir).entryList(nameFilters, QDir::Files);
        IndexOfSelectedFile = FilesListName.indexOf(QFileInfo(ImageName).fileName());
        //打印图片路径、文件序号
        SerialNumber = QDir::toNativeSeparators(QString::number(IndexOfSelectedFile + 1) + "/" + QString::number(FilesListName.size()));
        ImageDirsName->setText("目录：" + ImageName + "&nbsp;&nbsp;&nbsp;<font color='blue'>" + SerialNumber + "</font>");
        //图像与窗口融合（生命周期只有一次）
        if(ImageSettingflags)
        {
            //调用cv窗口与qt窗口融合函数
            image_setting();
            //启用opencv鼠标事件回调函数
            mouse_callback();
        }
        ImageSettingflags = false;
        //创建或搜索数据文件
        //创建数据存储文件夹
        QString DataDir = QDir(ImageFileDir).filePath("..") + R"(/LaneAnnotations/)";
        if(!QDir(DataDir).exists())
        {
            QDir().mkpath(DataDir);
        }
        //加载图片
        pix_read_show_clone(ImageName);
        //加载图片数据
        filePathBackups = ImageName;
        TryVehiclePix.copyTo(VehiclePix);
        //读取txt文件数据加载至缓存
        ReadFilesData(filePathBackups);
        //绘制图像
        ReadAndDrawVector();
        cv::imshow(VehicleWindowName,VehiclePix);
    }
    else
    {
        ImageDirsName->setText("当前目录失效，请重新选择文件目录及相应图像！");
    }
}

void Widget::back_and_next_picture(int _BackOrNext)
{
    if(!ImageName.isEmpty())
    {
        //写入上一张图片操作后的总表数据
        WriteFilesDate(filePathBackups);
        switch (_BackOrNext)
        {
        case _NEXT_PICTURE:
        {
            if((IndexOfSelectedFile + 1) == FilesListName.size())
            {
                QString FirstPath = QDir::toNativeSeparators(ImageFileDir + "/" + FilesListName[0]);
                pix_read_show_clone(FirstPath);
                IndexOfSelectedFile = 0;
                SerialNumber = QDir::toNativeSeparators(QString::number(IndexOfSelectedFile + 1) + "/" + QString::number(FilesListName.size()));
                ImageDirsName->setText("目录：" + FirstPath + "&nbsp;&nbsp;&nbsp;<font color='blue'>" + SerialNumber + "</font>");
                filePathBackups = FirstPath;
                break;
            }
            else
            {
                QString NextPath = QDir::toNativeSeparators(ImageFileDir + "/" + FilesListName[IndexOfSelectedFile + 1]);
                pix_read_show_clone(NextPath);
                IndexOfSelectedFile += 1;
                SerialNumber = QDir::toNativeSeparators(QString::number(IndexOfSelectedFile + 1) + "/" + QString::number(FilesListName.size()));
                ImageDirsName->setText("目录：" + NextPath + "&nbsp;&nbsp;&nbsp;<font color='blue'>" + SerialNumber + "</font>");
                filePathBackups = NextPath;
                break;
            }
        }
        case _BACK_PICTURE:
        {
            if(IndexOfSelectedFile == 0)
            {
                QString EndPath = QDir::toNativeSeparators(ImageFileDir + "/" + FilesListName[FilesListName.size() - 1]);
                pix_read_show_clone(EndPath);
                IndexOfSelectedFile = FilesListName.size() - 1;
                SerialNumber = QDir::toNativeSeparators(QString::number(IndexOfSelectedFile + 1) + "/" + QString::number(FilesListName.size()));
                ImageDirsName->setText("目录：" + EndPath + "&nbsp;&nbsp;&nbsp;<font color='blue'>" + SerialNumber + "</font>");
                filePathBackups = EndPath;
                break;
            }
            else
            {
                QString BackPath = QDir::toNativeSeparators(ImageFileDir + "/" + FilesListName[IndexOfSelectedFile - 1]);
                pix_read_show_clone(BackPath);
                IndexOfSelectedFile -= 1;
                SerialNumber = QDir::toNativeSeparators(QString::number(IndexOfSelectedFile + 1) + "/" + QString::number(FilesListName.size()));
                ImageDirsName->setText("目录：" + BackPath + "&nbsp;&nbsp;&nbsp;<font color='blue'>" + SerialNumber + "</font>");
                filePathBackups = BackPath;
                break;
            }
        }
        default:
            break;
        }
        TryVehiclePix.copyTo(VehiclePix);
        //读取下一张图片数据，绘制图像
        ReadFilesData(filePathBackups);
        ReadAndDrawVector();
        cv::imshow(VehicleWindowName,VehiclePix);
    }
    else
    {
        ImageDirsName->setText("请加载相应图像目录！");
    }
}

void Widget::draw_line(int event, int x, int y, int flags, void *userdate)
{
    if(!ImageName.isEmpty())
    {
        cv::Point start_point;          //创建起点临时变量
        cv::Point end_point;            //创建终点临时变量
        DrawLineInfo StellaStruct{};    //创建单次操作线段结构体
        //当按下鼠标左键
        if(event == CV_EVENT_LBUTTONDOWN && flags & CV_EVENT_FLAG_LBUTTON)
        {
            start_point.x = x;
            start_point.y = y;
            StellaStruct._point = start_point;
            //写入线段起点数据
            SingleLine.push_back(StellaStruct);
            ReadAndDrawVector();
        }
        //当鼠标按下时移动鼠标
        else if(event == CV_EVENT_MOUSEMOVE && x <= g_VehicleLineBackgroundImage_width && y <= g_VehicleLineBackgroundImage_height && flags & CV_EVENT_FLAG_LBUTTON)
        {
            x > 0 ? end_point.x = x : 0;
            y > 0 ? end_point.y = y : 0;
            TryVehiclePix.copyTo(VehiclePix);
            StellaStruct._point = end_point;
            //实时显示绘制操作且不改变总表数据
            SingleLine.push_back(StellaStruct);
            ReadAndDrawVector();
            SingleLine.pop_back();
        }
        //当鼠标左键抬起
        else if(event == CV_EVENT_LBUTTONUP)
        {
            //写入线段终点数据
            StellaStruct._point.x = x;
            StellaStruct._point.y = y;
            if(StellaStruct._point != (SingleLine.end() - 1)->_point)
            {
                SingleLine.push_back(StellaStruct);
            }
            ReadAndDrawVector();
        }
        //当鼠标悬停至线段点
        else if(event == CV_EVENT_MOUSEMOVE && x <= g_VehicleLineBackgroundImage_width && y <= g_VehicleLineBackgroundImage_height)
        {
            if(SingleLine.size() != 0)
            {
                for(auto it = SingleLine.begin(); it != SingleLine.end(); it++)
                {
                    if ((pow((x - it->_point.x),2) + pow((y - it->_point.y),2) - pow(CIRCLE_SIZE,2)) <= 0)
                    {
                        cv::circle(VehiclePix, it->_point, CIRCLE_SIZE, cv::Scalar(0,115,178), -1);
                        if(MovepointVector.size() == 0)
                        {
                            MovepointVector.push_back(it->_point);
                        }
                        TemporaryPointSize = it->_pointsize;
                    }
                    else
                    {
                        cv::circle(VehiclePix, it->_point, CIRCLE_SIZE, cv::Scalar(0,100,255), -1);
                    }
                }
            }
            if(AllLine.size() != 0)
            {
                for(auto& pair:AllLine)
                {
                    for(auto it = pair.second.begin(); it != pair.second.end(); it ++)
                    {
                        if ((pow((x - it->_point.x),2) + pow((y - it->_point.y),2) - pow(CIRCLE_SIZE,2)) <= 0)
                        {
                            cv::circle(VehiclePix, it->_point, CIRCLE_SIZE, cv::Scalar(0,115,178), -1);
                            if(MovepointVector.size() == 0)
                            {
                                MovepointVector.push_back(it->_point);
                            }
                            TemporaryPointSize = it->_pointsize;
                        }
                        else
                        {
                            cv::circle(VehiclePix, it->_point, CIRCLE_SIZE, cv::Scalar(0,100,255), -1);
                        }
                    }
                }
            }
            if(MovepointVector.size() != 0)
            {
                MovepointVector.pop_back();
                //滚轮开关开启
                ModifyPointSize_Tag = true;
            }
            else
            {
                //滚轮开关关闭
                ModifyPointSize_Tag = false;
            }
        }
        cv::imshow(VehicleWindowName,VehiclePix);
        //标签实时显示鼠标坐标
        std::string Point_C = "(" + std::to_string(x > 0 ? x : 0) + "," + std::to_string(y > 0 ? y : 0) + ")";
        PointCoordinate->setText(QString::fromStdString(Point_C));
    }
}


void Widget::wheelEvent(QWheelEvent *event)
{
    if(!ImageName.isEmpty())
    {
        if(ModifyPointSize_Tag)
        {
            int scrollDelta = event->angleDelta().y();
            if(scrollDelta > 0)
            {
                if(TemporaryPointSize < 200)
                {
                    TemporaryPointSize += 5;
                }
                else
                {
                    TemporaryPointSize = 200;
                }
            }
            else
            {
                if(TemporaryPointSize > 1)
                {
                    TemporaryPointSize --;
                }
                else
                {
                    TemporaryPointSize = 1;
                }
            }
            for(auto it = SingleLine.begin(); it != SingleLine.end(); it++)
            {
                if(it->_point == MovepointVector[0])
                {
                    it->_pointsize = TemporaryPointSize;
                }
            }

            if(AllLine.size() != 0)
            {
                for(auto& pair:AllLine)
                {
                    for(auto it = pair.second.begin(); it != pair.second.end(); it ++)
                    {
                        if(it->_point == MovepointVector[0])
                        {
                            it->_pointsize = TemporaryPointSize;
                        }
                    }
                }
            }
            TryVehiclePix.copyTo(VehiclePix);
            ReadAndDrawVector();
            cv::imshow(VehicleWindowName,VehiclePix);
        }
    }
}

void Widget::WriteFilesDate(QString _FilePath)
{
    QString image_file_name = QDir(ImageFileDir).filePath("..") + R"(/LaneAnnotations/)" + QFileInfo(_FilePath).baseName() + ".txt";
    std::ofstream image_ofs;    //写文件
    image_ofs.open(image_file_name.toStdString(),std::ios::out | std::ios::trunc);
    if(!image_ofs.is_open())
    {
        return;
    }
    else
    {
        if(AllLine.size() != 0)
        {
            for(auto& pair:AllLine)
            {
                if(pair.second.size() != 0)
                {
                    image_ofs << pair.first << ":" << " ";
                    for(auto it = pair.second.begin(); it != pair.second.end(); it ++)
                    {
                        it->_point.x = static_cast<int>(0.5f + it->_point.x * (InitImage.cols / static_cast<float>(g_VehicleLineBackgroundImage_width)));
                        it->_point.y = static_cast<int>(0.5f + it->_point.y * (InitImage.rows / static_cast<float>(g_VehicleLineBackgroundImage_height)));
                        image_ofs << it->_point.x << "," << it->_point.y << " " << it->_pointsize << " ";
                    }
                    image_ofs << "\n";
                }
            }
        }
        image_ofs.close();
    }
    SingleLine.clear();
    AllLine.clear();
}

void Widget::ReadFilesData(QString _FilePath)
{
    QString image_file_name = QDir(ImageFileDir).filePath("..") + R"(/LaneAnnotations/)" + QFileInfo(_FilePath).baseName() + ".txt";
    std::ifstream image_ifs;    //读文件
    image_ifs.open(image_file_name.toStdString(),std::ios::in);
    if(!image_ifs.is_open())
    {
        return;
    }
    try
    {
        std::string LineDatas;
        while (std::getline(image_ifs, LineDatas))
        {
            std::istringstream iss(LineDatas);
            std::string LineKey;
            if (!(iss >> LineKey))
            {
                // 数据不完整，抛出异常
                throw std::invalid_argument(" ");
            }
            char Space;
            DrawLineInfo PointsFile{};
            std::vector<DrawLineInfo> SingleLine;
            while (iss >> PointsFile._point.x >> Space >> PointsFile._point.y >> PointsFile._pointsize)
            {
                PointsFile._point.x = static_cast<int>(0.5f + PointsFile._point.x * (g_VehicleLineBackgroundImage_width / static_cast<float>(InitImage.cols)));
                PointsFile._point.y = static_cast<int>(0.5f + PointsFile._point.y * (g_VehicleLineBackgroundImage_height / static_cast<float>(InitImage.rows)));
                SingleLine.push_back(PointsFile);
            }
            if (SingleLine.empty())
            {
                // 数据不完整，抛出异常
                throw std::invalid_argument(" ");
            }
            int Key = std::stoi(LineKey);
            AllLine[Key] = SingleLine;
        }
        image_ifs.close();
    }
    catch(const std::invalid_argument& e)
    {
        // 处理数据不完整的情况
//        std::cerr << "Invalid argument: " << e.what() << '\n';
    }
    catch(const std::ios_base::failure& e)
    {
        // 处理文件读取失败的情况
//        std::cerr << "IO Exception: " << e.what() << '\n';
    }
}

void ReadAndDrawVector()
{
    PaintFillPoly();
    if(SingleLine.size() != 0)
    {
        for(auto it = SingleLine.begin(); it != SingleLine.end(); it++)
        {
            if((it + 1) != SingleLine.end())
            {
                DrawPixMethod(VehiclePix,it->_point,(it + 1)->_point);
            }
        }
    }
    if(AllLine.size() != 0)
    {
        cv::Point text_point;
        for(const auto& pair:AllLine)
        {
            for(auto it = pair.second.begin(); it != pair.second.end(); it ++)
            {
                if((it + 1) != pair.second.end())
                {
                    DrawPixMethod(VehiclePix,it->_point,(it + 1)->_point);
                    if(pair.second.size() > 1)
                    {
                        if((CountPointNate(pair.second[1]._point , pair.second[0]._point , pair.second[1]._pointsize)[1].x +  30) > g_VehicleLineBackgroundImage_width)
                        {
                            text_point.x = pair.second[1]._point.x - (pair.second[1]._pointsize / 2) - 30;
                        }
                        else
                        {
                            text_point.x = pair.second[1]._point.x + (pair.second[1]._pointsize / 2);
                        }
                        if(pair.second[1]._point.y + 30 > g_VehicleLineBackgroundImage_height)
                        {
                            text_point.y = pair.second[1]._point.y - 10;
                        }
                        else if(pair.second[1]._point.y - 30 < 0)
                        {
                            text_point.y = pair.second[1]._point.y + 30;
                        }
                        else
                        {
                            text_point.y = pair.second[1]._point.y;
                        }
                        cv::putText(VehiclePix,std::to_string(pair.first),text_point, cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 0, 255), 2);
                    }
                }
            }
        }
    }
}

void PaintFillPoly()
{
    if(SingleLine.size() > 1)
    {
        for(auto it = SingleLine.begin(); it != SingleLine.end(); it++)
        {
            std::vector<cv::Point> l_r_pointvector;
            if((it + 1) != SingleLine.end())
            {
                l_r_pointvector.push_back(CountPointNate(it->_point , (it + 1)->_point , it->_pointsize)[0]);
                l_r_pointvector.push_back(CountPointNate((it + 1)->_point , it->_point , (it + 1)->_pointsize)[0]);
                l_r_pointvector.push_back(CountPointNate((it + 1)->_point , it->_point , (it + 1)->_pointsize)[1]);
                l_r_pointvector.push_back(CountPointNate(it->_point , (it + 1)->_point , it->_pointsize)[1]);
                l_r_pointvector.push_back(CountPointNate(it->_point , (it + 1)->_point , it->_pointsize)[0]);
            }
            else if((it + 1) == SingleLine.end())
            {
                l_r_pointvector.push_back(CountPointNate(it->_point , (it - 1)->_point , it->_pointsize)[0]);
                l_r_pointvector.push_back(CountPointNate((it - 1)->_point , it->_point , (it - 1)->_pointsize)[0]);
                l_r_pointvector.push_back(CountPointNate((it - 1)->_point , it->_point , (it - 1)->_pointsize)[1]);
                l_r_pointvector.push_back(CountPointNate(it->_point , (it - 1)->_point , it->_pointsize)[1]);
                l_r_pointvector.push_back(CountPointNate(it->_point , (it - 1)->_point , it->_pointsize)[0]);
            }
            cv::polylines(VehiclePix, std::vector<std::vector<cv::Point>>(1, l_r_pointvector), true, cv::Scalar(0, 255, 255), 1, cv::LINE_AA);
            l_r_pointvector.clear();
        }
    }
    if(AllLine.size() > 0)
    {
        for(const auto& pair:AllLine)
        {
            std::vector<cv::Point> l_r_pointvector;
            for(auto it = pair.second.begin(); it != pair.second.end(); it++)
            {
                if((it + 1) != pair.second.end())
                {
                    l_r_pointvector.push_back(CountPointNate(it->_point , (it + 1)->_point , it->_pointsize)[0]);
                    l_r_pointvector.push_back(CountPointNate((it + 1)->_point , it->_point , (it + 1)->_pointsize)[0]);
                    l_r_pointvector.push_back(CountPointNate((it + 1)->_point , it->_point , (it + 1)->_pointsize)[1]);
                    l_r_pointvector.push_back(CountPointNate(it->_point , (it + 1)->_point , it->_pointsize)[1]);
                    l_r_pointvector.push_back(CountPointNate(it->_point , (it + 1)->_point , it->_pointsize)[0]);
                }
                else if((it + 1) == pair.second.end())
                {
                    l_r_pointvector.push_back(CountPointNate(it->_point , (it - 1)->_point , it->_pointsize)[0]);
                    l_r_pointvector.push_back(CountPointNate((it - 1)->_point , it->_point , (it - 1)->_pointsize)[0]);
                    l_r_pointvector.push_back(CountPointNate((it - 1)->_point , it->_point , (it - 1)->_pointsize)[1]);
                    l_r_pointvector.push_back(CountPointNate(it->_point , (it - 1)->_point , it->_pointsize)[1]);
                    l_r_pointvector.push_back(CountPointNate(it->_point , (it - 1)->_point , it->_pointsize)[0]);
                }
                cv::polylines(VehiclePix, std::vector<std::vector<cv::Point>>(1, l_r_pointvector), true, cv::Scalar(0, 255, 255), 1, cv::LINE_AA);
                l_r_pointvector.clear();
            }
        }
    }
}

std::vector<cv::Point> CountPointNate(const cv::Point down_point, const cv::Point up_point, const int base_length)
{
    //计算底边中点到顶点的向量
    int dx = std::abs(up_point.x - down_point.x);
    int dy = std::abs(up_point.y - down_point.y);
    //得到单位向量u
    float ux = dx / std::sqrt(dx * dx + dy * dy);
    float uy = dy / std::sqrt(dx * dx + dy * dy);
    //计算端点坐标
    cv::Point _leftpoint , _rightpoint;
    if((down_point.x > up_point.x && down_point.y < up_point.y) || (down_point.x < up_point.x && down_point.y > up_point.y))
    {
            _leftpoint.x = down_point.x + (base_length * uy / 2);
            _rightpoint.x = down_point.x - (base_length * uy / 2);
    }
    else
    {
            _leftpoint.x = down_point.x - (base_length * uy / 2);
            _rightpoint.x = down_point.x + (base_length * uy / 2);
    }
    _leftpoint.y = down_point.y + (base_length * ux / 2);
    _rightpoint.y = down_point.y - (base_length * ux / 2);
    //返回左右顶点
    std::vector<cv::Point> left_right_pointvector;
    left_right_pointvector.push_back(_leftpoint);
    left_right_pointvector.push_back(_rightpoint);
    return left_right_pointvector;
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    if(!ImageName.isEmpty())
    {
        TryVehiclePix.copyTo(VehiclePix);
        int KeyValue = event->key();
        if(KeyValue >= Qt::Key_0 && KeyValue <= Qt::Key_6)
        {
        std::string LineNum = std::to_string(KeyValue - Qt::Key_0);
        if(SingleLine.size() > 1)
        {
            AllLine[KeyValue - Qt::Key_0] = SingleLine;
        }
        else if(SingleLine.size() == 0)
        {
            AllLine[KeyValue - Qt::Key_0] = SingleLine;
        }
        SingleLine.clear();
        }
        ReadAndDrawVector();
        cv::imshow(VehicleWindowName,VehiclePix);
    }
}

void Widget::revoke_image_line()
{
    if(!ImageName.isEmpty())
    {
        TryVehiclePix.copyTo(VehiclePix);
        if(SingleLine.size() != 0)
        {
            SingleLine.pop_back();
        }
        ReadAndDrawVector();
        cv::imshow(VehicleWindowName,VehiclePix);
    }
}


/////////////////////////////////////////////////////////////////////////
Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    self_windows();
    set_part();
    connect_signal_slots();
    ImageDirsName->setText("<font color = 'red'>注意：该软件不支持目录或文件等包含中文路径！</font>");
}

Widget::~Widget()
{

}

void Widget::self_windows()
{
    this->setMinimumSize(_WINDOWS_SIZE);
    this->setWindowTitle("昆山星际舟车道线标注系统");
    this->setWindowIcon(QIcon(":/Stella.ico"));
    this->setMouseTracking(true);
}

void Widget::set_part()
{
    VehicleLineBackgroundImage = new QLabel(this);
    SelectImage = new QPushButton(this);
    BackImage = new QPushButton(this);
    NextImage = new QPushButton(this);
    Revoke = new QPushButton(this);
    ImageFile = new QFileDialog;
    ImageDirsName = new QLabel(this);
    APP_INFO = new QLabel(this);
    PointCoordinate = new QLabel(this);
    VehicleLineBackgroundImage->installEventFilter(this);
    APP_INFO->installEventFilter(this);
    ImageFile->setFileMode(QFileDialog::ExistingFile);
    SelectImage->setIcon(QIcon(":/image/Microsoft-Fluentui-Emoji-Mono-Open-File-Folder.32.png"));
    BackImage->setIcon(QIcon(":/image/Icons8-Ios7-Arrows-Left.32.png"));
    NextImage->setIcon(QIcon(":/image/Icons8-Ios7-Arrows-Right.32.png"));
    Revoke->setIcon(QIcon(":/image/Amitjakhu-Drip-Return.32.png"));
    APP_INFO->setText("软件简介");
    SelectImage->setToolTip("选择文件");
    BackImage->setToolTip("上一张");
    NextImage->setToolTip("下一张");
    Revoke->setToolTip("撤回");
    SelectImage->setShortcut(tr("o"));
    SelectImage->setShortcut(tr("O"));
    BackImage->setShortcut(tr("a"));
    BackImage->setShortcut(tr("A"));
    NextImage->setShortcut(tr("d"));
    NextImage->setShortcut(tr("D"));
    Revoke->setShortcut(tr("r"));
    Revoke->setShortcut(tr("R"));
    ImageFile->setNameFilter("PNG Images (*.png)");
    ImageFile->setDefaultSuffix("png");
    ImageDirsName->setStyleSheet("font-family:'微软雅黑';font-size:13px;");
    PointCoordinate->setStyleSheet("font-family:'微软雅黑';font-size:13px;color:red;");
    APP_INFO->setStyleSheet("font-family:'微软雅黑';font-size:13px;color:blue;text-decoration:underline;");
    VehicleLineBackgroundImage->setStyleSheet("background-color:black;");
}

void Widget::resizeEvent(QResizeEvent *event)
{
    SingleLine.clear();
    AllLine.clear();
    int width = this->width();
    int height = this->height();
    VehicleLineBackgroundImage->setGeometry(5, 5, width - 5 - 5, height - 5 - 5 - _BUTTON_SIZE - 5 );
    SelectImage->setGeometry(5, 5 + VehicleLineBackgroundImage->height() + 5, _BUTTON_SIZE, _BUTTON_SIZE);
    ImageDirsName->setGeometry(5 + _BUTTON_SIZE + 5, SelectImage->pos().y() + 2, 660, 35);
    BackImage->setGeometry(width / 2 - 20 - 5 - _BUTTON_SIZE, SelectImage->pos().y(), _BUTTON_SIZE, _BUTTON_SIZE);
    Revoke->setGeometry(BackImage->pos().x() + _BUTTON_SIZE + 5, BackImage->pos().y(), _BUTTON_SIZE, _BUTTON_SIZE);
    NextImage->setGeometry(Revoke->pos().x() + _BUTTON_SIZE + 5, BackImage->pos().y(), _BUTTON_SIZE, _BUTTON_SIZE);
    PointCoordinate->setGeometry(NextImage->pos().x() + _BUTTON_SIZE + 20, BackImage->pos().y(), 100, _BUTTON_SIZE);
    APP_INFO->setGeometry(width - 5 - 150, BackImage->pos().y(), 55, _BUTTON_SIZE );
    g_VehicleLineBackgroundImage_width = VehicleLineBackgroundImage->width();
    g_VehicleLineBackgroundImage_height = VehicleLineBackgroundImage->height();
    //reszie图片大小
    if(!ImageName.isEmpty())
    {
        pix_read_show_clone(filePathBackups);
        cv::resize(InitImage, VehiclePix, cv::Size(g_VehicleLineBackgroundImage_width, g_VehicleLineBackgroundImage_height));
        cv::resizeWindow(VehicleWindowName,g_VehicleLineBackgroundImage_width, g_VehicleLineBackgroundImage_height);
        ReadFilesData(filePathBackups);
        ReadAndDrawVector();
        cv::imshow(VehicleWindowName,VehiclePix);
    }
}

bool Widget::eventFilter(QObject *object, QEvent *event)
{
    if (object == VehicleLineBackgroundImage && event->type() == QEvent::Enter)
    {
        this->setCursor(Qt::CrossCursor);
        return true;
    }
    else if (object == VehicleLineBackgroundImage && event->type() == QEvent::Leave)
    {
        this->setCursor(Qt::ArrowCursor);
        return true;
    }
    else if (object == APP_INFO && event->type() == QEvent::Enter)
    {
        this->setCursor(Qt::PointingHandCursor);
        return true;
    }
    else if (object == APP_INFO && event->type() == QEvent::Leave)
    {
        this->setCursor(Qt::ArrowCursor);
        return true;
    }
    else if (object == APP_INFO && event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton)
        {
            QDialog InfoDialog(this);
            InfoDialog.setFixedSize(540, 340);
            InfoDialog.setWindowTitle("车道线绘制软件使用说明");
            // 添加 QLabel 控件来显示图片
            QLabel *infoLabel = new QLabel(&InfoDialog);
            infoLabel->setGeometry(20, 5, 500, 320); // 设置标签的位置和大小
            infoLabel->setPixmap(QPixmap(":/image/Snipaste_2024-02-05_10-50-41.png"));

            InfoDialog.exec();
        }
        return true;
    }
    return QWidget::eventFilter(object, event);
}

void Widget::connect_signal_slots()
{
    connect(SelectImage,&QPushButton::clicked,this,&Widget::open_picture);
    connect(BackImage,&QPushButton::clicked,this,[=]{back_and_next_picture(_BACK_PICTURE);});
    connect(NextImage,&QPushButton::clicked,this,[=]{back_and_next_picture(_NEXT_PICTURE);});
    connect(Revoke,&QPushButton::clicked,this,&Widget::revoke_image_line);
}

void Widget::pix_read_show_clone(QString _filepath)
{
    //读取图片路径
    InitImage = cv::imread(_filepath.toStdString(), cv::IMREAD_COLOR);
    //reszie图片大小
    cv::resize(InitImage, VehiclePix, cv::Size(g_VehicleLineBackgroundImage_width, g_VehicleLineBackgroundImage_height));
    //展示图片
    cv::imshow(VehicleWindowName,VehiclePix);
    //克隆绘制面板
    TryVehiclePix = VehiclePix.clone();
}

void Widget::image_setting()
{
    cv::namedWindow(VehicleWindowName,cv::WINDOW_NORMAL);
    HWND VehiclePix_Handle = (HWND)cvGetWindowHandle(VehicleWindowName.c_str());    //融合opencv窗口句柄
    HWND Parent_Handle = GetParent(VehiclePix_Handle);                              //创建父句柄
    SetParent(VehiclePix_Handle,(HWND)VehicleLineBackgroundImage->winId());
    ShowWindow(Parent_Handle,SW_HIDE);
    cv::resizeWindow(VehicleWindowName,g_VehicleLineBackgroundImage_width, g_VehicleLineBackgroundImage_height);
    cv::moveWindow(VehicleWindowName,5,5);
}

void Widget::mouse_callback()
{
    cv::setMouseCallback(VehicleWindowName,draw_line,(void*)(&VehiclePix));         //绘制回调函数
}