#include "BooleanOperation.hh"
#include <algorithm>
#include <cmath>
#include <vector>


using namespace BoolOp;

// b在向量pq左为正, 右为负
double cross(PolyLine::Point &p, PolyLine::Point &q, PolyLine::Point &b) {
    return (
        p[0] * q[1] - p[1] * q[0] + 
        q[0] * b[1] - q[1] * b[0] + 
        b[0] * p[1] - b[1] * p[0]
    );
}

// 忽略误差, b在向量pq左为正, 右为负
double to_left(PolyLine::Point &p, PolyLine::Point &q, PolyLine::Point &b) {
    double a = BoolOp::cross(p, q, b);
    if (std::abs(a) < ev) { a = 0.0; }// 小于误差，认为在向量上
    return a;
}


bool points_are_equal(PolyLine::Point &p, PolyLine::Point &q) {
    double x = p[0] - q[0];
    double y = p[1] - q[1];
    double l2 = x * x + y * y;
    if (l2 < ev_sq) { return true; }
    else { return false; }
}


bool segments_are_equal(Segment &seg1, Segment &seg2) {
    return (BoolOp::points_are_equal(seg1[0], seg2[0]) && BoolOp::points_are_equal(seg1[1], seg2[1])) 
        || (BoolOp::points_are_equal(seg1[1], seg2[0]) && BoolOp::points_are_equal(seg1[0], seg2[1]));
}


bool segments_overlap(Segment &seg1, Segment &seg2) {
    // seg2的顶点都在seg1上, 且其在两坐标轴的投影在seg1投影的内部 (非严格)
    return (
        BoolOp::to_left(seg1[0], seg1[1], seg2[0]) == 0 && 
        BoolOp::to_left(seg1[0], seg1[1], seg2[1]) == 0
    ) && (
        (seg1[0][0]-ev < seg2[0][0] && seg2[0][0] < seg1[1][0]+ev) || 
        (seg1[0][0]-ev < seg2[1][0] && seg2[1][0] < seg1[1][0]+ev)
    ) && (
        (seg1[0][1]-ev < seg2[0][1] && seg2[0][1] < seg1[1][1]+ev) || 
        (seg1[0][1]-ev < seg2[1][1] && seg2[1][1] < seg1[1][1]+ev)
    );
}


bool segments_intersect(Segment &seg1, Segment &seg2, bool on_seg = true, bool on_end = true) {
    if ( // 快速排斥实验
        (std::min(seg2[0][0], seg2[1][0]) - std::max(seg1[0][0], seg1[1][0]) > ev) ||
        (std::min(seg1[0][0], seg1[1][0]) - std::max(seg2[0][0], seg2[1][0]) > ev) ||
        (std::min(seg2[0][1], seg2[1][1]) - std::max(seg1[0][1], seg1[1][1]) > ev) ||
        (std::min(seg1[0][1], seg1[1][1]) - std::max(seg2[0][1], seg2[1][1]) > ev)
    ) { return false; }

    double tmp1 = BoolOp::to_left(seg1[0], seg1[1], seg2[0]) * BoolOp::to_left(seg1[0], seg1[1], seg2[1]);
    double tmp2 = BoolOp::to_left(seg2[0], seg2[1], seg1[0]) * BoolOp::to_left(seg2[0], seg2[1], seg1[1]);
    if ( tmp1 < 0.0 && tmp2 < 0.0 ) { return true; } // 严格跨立实验 (交点不能是端点, 也不能在线段内)

    bool intersect_on_end = false, intersect_on_seg = false;
    if ( // 在端点相交
        BoolOp::points_are_equal(seg1[0], seg2[0]) || BoolOp::points_are_equal(seg1[0], seg2[1]) ||
        BoolOp::points_are_equal(seg1[1], seg2[0]) || BoolOp::points_are_equal(seg1[1], seg2[1])
    ) { intersect_on_end = true; }
    else if ( // 不在端点相交, 但在线段上相交
        tmp1 <= 0.0 && tmp2 <= 0.0
    ) { intersect_on_seg = true; }
    else { return false; }

    if (intersect_on_end == on_end && intersect_on_seg == on_seg) { return true; }
    return false;
}


bool get_intersect_point_of_segments(Segment &seg1, Segment &seg2, PolyLine::Point& result) {
    double d1 = std::abs(BoolOp::cross(seg1[0], seg1[1], seg2[0]));
    double d2 = std::abs(BoolOp::cross(seg1[0], seg1[1], seg2[1]));
    double x = seg2[0][0] + (seg2[1][0] - seg2[0][0]) * d1 / (d1 + d2);
    double y = seg2[0][1] + (seg2[1][1] - seg2[0][1]) * d1 / (d1 + d2);
    result[0] = x, result[1] = y, result[2] = 0.0;
    return true;
}


bool get_intersect_point_of_lines(Segment &line1, Segment &line2, PolyLine::Point& result) {
    // 直线方程为 Ax+By=C, A=y2-y1, B=x1-x2, C=x1*y2-x2*y1
    double a1 = line1[1][1] - line1[0][1], b1 = line1[0][0] - line1[1][0];
    double c1 = line1[0][0] * line1[1][1] - line1[1][0] * line1[0][1];
    double a2 = line2[1][1] - line2[0][1], b2 = line2[0][0] - line2[1][0];
    double c2 = line2[0][0] * line2[1][1] - line2[1][0] * line2[0][1];
    // 克拉默法则解交点
    double c = a1 * b2 - b1 * a2;
    // 直线平行或重合
    if (std::abs(c) < ev) {
        result[0] = 0.0, result[1] = 0.0, result[2] = 0.0;
        return false;
    }
    else {
        result[0] = (c1 * b2 - b1 * c2) / c;
        result[1] = (a1 * c2 - c1 * a2) / c;
        result[2] = 0.0;
        return true;
    }
}


double polygon_directed_area(PolyLine* polygon) {
    const std::vector<PolyLine::Point>& vertices = polygon->points();
    size_t n_vertices = polygon->n_vertices();
    double area = 0.0;
    for (size_t i = 0; i < n_vertices; i++) {
        const PolyLine::Point& p1 = vertices[i];
        const PolyLine::Point& p2 = vertices[(i + 1) % n_vertices];
        area += p1[0] * p2[1] - p2[0] * p1[1];
    }
    return 0.5 * area;
}


void ensure_counter_clockwise_polygon(PolyLine* polygon) {
    if (BoolOp::polygon_directed_area(polygon) < 0.0) { // vertices order is clockwise
        std::vector<PolyLine::Point>& vertices = polygon->points();
        std::reverse(vertices.begin(), vertices.end()); // clockwise to counter-clockwise
    }
}