_Pragma("once");
#include <QPainter>
#include <map>
#include <QPoint>
#include <vector>

/// @param 圆点半径
constexpr int point_radius{7};

/// @param 线段结构体
struct PointInfo
{
    QPoint point{};                    //线段点
    std::size_t point_size{1};         //线段粗细度
};

/// @param 临时结构体缓存区
inline PointInfo point_info{};

/// @param 临时数据缓存区
inline std::vector<PointInfo> point_info_v{};

/// @param 临时移动点缓存区
inline std::vector<PointInfo> movein_point_v{};

/// @param 数据总缓存区
inline std::map<std::int32_t, std::vector<PointInfo>> LinesMap{};
