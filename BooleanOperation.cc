#include "BooleanOperation.hh"
#include <algorithm>
#include <cmath>
#include <vector>
#include <iostream>


namespace BoolOp {

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
    double a = cross(p, q, b);
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


bool point_on_segment(PolyLine::Point &point, Segment &seg, bool on_end) {
    if (points_are_equal(point, seg[0]) || points_are_equal(point, seg[1])) {
        if (on_end) { return true; }
        else { return false; }
    }
    if (
        to_left(point, seg[0], seg[1]) == 0.0 &&
        (std::min(seg[0][0], seg[1][0])-ev < point[0] && point[0] < std::max(seg[0][0], seg[1][0])+ev) &&
        (std::min(seg[0][1], seg[1][1])-ev < point[1] && point[1] < std::max(seg[0][1], seg[1][1])+ev)
    ) { return true; }
    return false;
}


bool point_on_polygon_segment(PolyLine::Point &point, std::vector<Segment> &segs, Segment &which_seg) {
    for (auto seg : segs) {
        if (point_on_segment(point, seg)) {
            which_seg = seg;
            return true;
        }
    }
    return false;
}


bool point_inside_polygon_segments(PolyLine::Point &point, std::vector<Segment> &segs, bool on_seg) {
    double x_min = point[0];
    for (auto seg : segs) { if (seg[0][0] < x_min) { x_min = seg[0][0]; } }
    x_min -= 10.0;
    PolyLine::Point tmp_p(x_min, point[1] + cp, 0.0);
    Segment radial_seg{tmp_p, point}; // 扫描线

    int n_intersect = 0;
    for (auto seg : segs) {
        if (point_on_segment(point, seg)) { 
            if (on_seg) { return true; }
            else { return false; }
        }
        if (segments_intersect(radial_seg, seg, false, false)) {
            n_intersect++;
        }
    }

    if (n_intersect % 2 == 1) { return true; }
    else { return false; }
}


bool point_outside_polygon_segments(PolyLine::Point &point, std::vector<Segment> &segs, bool on_seg) {
    return !point_inside_polygon_segments(point, segs, !on_seg);
}


bool segments_are_equal(Segment &seg1, Segment &seg2) {
    return (points_are_equal(seg1[0], seg2[0]) && points_are_equal(seg1[1], seg2[1])) 
        || (points_are_equal(seg1[1], seg2[0]) && points_are_equal(seg1[0], seg2[1]));
}


bool segments_in_same_dir(Segment &seg1, Segment &seg2) {
    double seg1_vec_x = seg1[1][0] - seg1[0][0];
    double seg1_vec_y = seg1[1][1] - seg1[0][1];
    double seg2_vec_x = seg2[1][0] - seg2[0][0];
    double seg2_vec_y = seg2[1][1] - seg2[0][1];
    double dot_val = (seg1_vec_x * seg2_vec_x) + (seg1_vec_y * seg2_vec_y);
    if (dot_val > 0.0) { return true; }
    else { return false; }
}


bool segments_overlap(Segment &seg1, Segment &seg2) {
    // seg2的顶点都在seg1上, 且其在两坐标轴的投影在seg1投影的内部 (非严格)
    if ( // seg1 与 seg2 不共线
        to_left(seg1[0], seg1[1], seg2[0]) != 0 ||
        to_left(seg1[0], seg1[1], seg2[1]) != 0
    ) { return false; }
    if (
        point_on_segment(seg2[0], seg1) || point_on_segment(seg2[1], seg1) ||
        point_on_segment(seg1[0], seg2) || point_on_segment(seg1[1], seg2)
    ) { return true; }
    return false;
}


bool segments_intersect(Segment &seg1, Segment &seg2, bool on_seg, bool on_end) {
    if ( // 快速排斥实验
        (std::min(seg2[0][0], seg2[1][0]) - std::max(seg1[0][0], seg1[1][0]) > ev) ||
        (std::min(seg1[0][0], seg1[1][0]) - std::max(seg2[0][0], seg2[1][0]) > ev) ||
        (std::min(seg2[0][1], seg2[1][1]) - std::max(seg1[0][1], seg1[1][1]) > ev) ||
        (std::min(seg1[0][1], seg1[1][1]) - std::max(seg2[0][1], seg2[1][1]) > ev)
    ) { return false; }

    double tmp1 = to_left(seg1[0], seg1[1], seg2[0]) * to_left(seg1[0], seg1[1], seg2[1]);
    double tmp2 = to_left(seg2[0], seg2[1], seg1[0]) * to_left(seg2[0], seg2[1], seg1[1]);
    if ( tmp1 < 0.0 && tmp2 < 0.0 ) { return true; } // 严格跨立实验 (交点不能是端点, 也不能在线段内)

    bool intersect_on_end = false, intersect_on_seg = false;
    if ( // 在端点相交
        points_are_equal(seg1[0], seg2[0]) || points_are_equal(seg1[0], seg2[1]) ||
        points_are_equal(seg1[1], seg2[0]) || points_are_equal(seg1[1], seg2[1])
    ) { intersect_on_end = true; }
    else if ( // 不在端点相交, 但在线段上相交
        tmp1 <= 0.0 && tmp2 <= 0.0
    ) { intersect_on_seg = true; }
    else { return false; }

    if (intersect_on_end && on_end && !(intersect_on_seg && !on_seg)) { return true; }
    if (intersect_on_seg && on_seg && !(intersect_on_end && !on_end)) { return true; }
    return false;
}


bool get_intersect_point_of_segments(Segment &seg1, Segment &seg2, PolyLine::Point& result) {
    double d1 = std::abs(cross(seg1[0], seg1[1], seg2[0]));
    double d2 = std::abs(cross(seg1[0], seg1[1], seg2[1]));
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


void interclip_segment(std::vector<Segment> &seg1s, std::vector<Segment> &seg2s, std::vector<Segment>& seg_clip) {
    for (auto seg1 : seg1s) {
        std::vector<PolyLine::Point> intersecters{seg1[0]};
        for (auto seg2: seg2s) {
            if (segments_overlap(seg1, seg2)) { // 线段重合
                if (point_on_segment(seg2[0], seg1, false)) {
                    intersecters.push_back(seg2[0]);
                }
                if (point_on_segment(seg2[1], seg1, false)) {
                    intersecters.push_back(seg2[1]);
                }
            }
            else if (segments_intersect(seg1, seg2, true, false)) { // 线段相交
                PolyLine::Point isp;
                get_intersect_point_of_segments(seg1, seg2, isp); // 若线段重合则无法求交点
                if (
                    (!points_are_equal(isp, seg1[0])) &&
                    (!points_are_equal(isp, seg1[1]))
                ) { intersecters.push_back(isp); }
            }
        }
        intersecters.push_back(seg1[1]);
        if (seg1[0][0] < seg1[1][0]) {
            std::sort(
                intersecters.begin(), intersecters.end(),
                [](PolyLine::Point x, PolyLine::Point y) { return x[0] < y[0]; }
            );
        }
        else if (seg1[0][0] > seg1[1][0]) {
            std::sort(
                intersecters.begin(), intersecters.end(),
                [](PolyLine::Point x, PolyLine::Point y) { return x[0] > y[0]; }
            );
        }
        else {
            if (seg1[0][1] < seg1[1][1]) {
                std::sort(
                    intersecters.begin(), intersecters.end(),
                    [](PolyLine::Point x, PolyLine::Point y) { return x[1] < y[1]; }
                );
            }
            else if (seg1[0][1] > seg1[1][1]) {
                std::sort(
                    intersecters.begin(), intersecters.end(),
                    [](PolyLine::Point x, PolyLine::Point y) { return x[1] > y[1]; }
                );
            }
        }
        for (size_t i = 0; i < intersecters.size() - 1; i++) {
            if (!points_are_equal(intersecters[i], intersecters[i+1])) {
                Segment seg{intersecters[i], intersecters[i+1]};
                seg_clip.push_back(seg);
            }
        }
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


void ensure_clockwise_polygon(PolyLine* polygon) {
    if (polygon_directed_area(polygon) > 0.0) { // vertices order is clockwise
        std::vector<PolyLine::Point>& vertices = polygon->points();
        std::reverse(vertices.begin(), vertices.end()); // clockwise to counter-clockwise
    }
}


void ensure_counter_clockwise_polygon(PolyLine* polygon) {
    if (polygon_directed_area(polygon) < 0.0) { // vertices order is clockwise
        std::vector<PolyLine::Point>& vertices = polygon->points();
        std::reverse(vertices.begin(), vertices.end()); // clockwise to counter-clockwise
    }
}


void get_polygon_segments(PolyLine* polygon, std::vector<int> &poly_seg, std::vector<Segment> &segs, bool reverse) {
    std::vector<PolyLine::Point>& vertices = polygon->points();
    size_t n_vertices = polygon->n_vertices();

    for (size_t i = 0; i < n_vertices - 1; i++) {
        if (i == poly_seg[0]) {
            poly_seg.erase(poly_seg.begin());
            continue;
        }
        if (reverse) {
            Segment seg{vertices[i], vertices[i + 1]};
            segs.push_back(seg);
        } else {
            Segment seg{vertices[i + 1], vertices[i]};
            segs.push_back(seg);
        }
    }
    return;
}


bool connect_segment_set(std::vector<Segment> &segs, PolyLine* polygon) {
    if (segs.size() == 0) { return true; }
    PolyLine::Point first_point = segs[0][0];
    PolyLine::Point last_point = segs[0][1];
    polygon->add_point(first_point);
    segs.erase(segs.begin());

    int max_iter = 1000, iter = 0;
    while(segs.size() > 0) {
        if (points_are_equal(first_point, last_point)) { // emmit a closed sub polygon
            polygon->add_point(first_point);
            first_point = segs[0][0];
            last_point = segs[0][1];
            polygon->add_point(first_point);
            segs.erase(segs.begin());
        }
        polygon->add_point(last_point);

        bool found = false;
        for(size_t i = 0; i < segs.size(); i++) {
            if (points_are_equal(segs[i][0], last_point)) { // find next segment
                last_point = segs[i][1];
                segs.erase(segs.begin() + i);
                found = true;
                break;
            }
        }
        iter++;
        if (iter > max_iter) { return false; } // iteration too much
        if (!found) { return false; } // can not find next segment
    }
    polygon->add_point(first_point);
    return true;
}


bool polygon_intersection(PolyLine* polygon1, PolyLine* polygon2, std::vector<int> &poly_seg_1, std::vector<int> &poly_seg_2, PolyLine* result) {
    // ensure_counter_clockwise_polygon(polygon1);
    // ensure_counter_clockwise_polygon(polygon2);

    std::vector<Segment> seg1s, seg2s;
    get_polygon_segments(polygon1, poly_seg_1, seg1s);
    get_polygon_segments(polygon2, poly_seg_2, seg2s);

    std::vector<Segment> seg1clips, seg2clips;
    interclip_segment(seg1s, seg2s, seg1clips);
    interclip_segment(seg2s, seg1s, seg2clips);

    std::vector<Segment> intersection_set;
    for (auto seg : seg1clips) {
        PolyLine::Point tmp_p(
            seg[0][0] + cp * (seg[1][0] - seg[0][0]),
            seg[0][1] + cp * (seg[1][1] - seg[0][1]),
            0.0
        );
        Segment on_seg2;
        if (point_on_polygon_segment(tmp_p, seg2s, on_seg2)) {
            if(segments_in_same_dir(seg, on_seg2)) { intersection_set.push_back(seg); }
        }
        else if (point_inside_polygon_segments(tmp_p, seg2s, false)) { intersection_set.push_back(seg); }
    }
    for (auto seg : seg2clips) {
        PolyLine::Point tmp_p(
            seg[0][0] + cp * (seg[1][0] - seg[0][0]),
            seg[0][1] + cp * (seg[1][1] - seg[0][1]),
            0.0
        );
        if (point_inside_polygon_segments(tmp_p, seg1s, false)) { intersection_set.push_back(seg); }
    }

    return connect_segment_set(intersection_set, result);
}


bool polygon_union(PolyLine* polygon1, PolyLine* polygon2, std::vector<int> &poly_seg_1, std::vector<int> &poly_seg_2, PolyLine* result) {
    // ensure_counter_clockwise_polygon(polygon1);
    // ensure_counter_clockwise_polygon(polygon2);

    std::vector<Segment> seg1s, seg2s;
    get_polygon_segments(polygon1, poly_seg_1, seg1s);
    get_polygon_segments(polygon2, poly_seg_2, seg2s);

    std::vector<Segment> seg1clips, seg2clips;
    interclip_segment(seg1s, seg2s, seg1clips);
    interclip_segment(seg2s, seg1s, seg2clips);

    std::vector<Segment> union_set;
    for (auto seg : seg1clips) {
        PolyLine::Point tmp_p(
            seg[0][0] + cp * (seg[1][0] - seg[0][0]),
            seg[0][1] + cp * (seg[1][1] - seg[0][1]),
            0.0
        );
        Segment on_seg2;
        if (point_on_polygon_segment(tmp_p, seg2s, on_seg2)) {
            if(segments_in_same_dir(seg, on_seg2)) { union_set.push_back(seg); }
        }
        else if (point_outside_polygon_segments(tmp_p, seg2s, false)) { union_set.push_back(seg); }
    }
    for (auto seg : seg2clips) {
        PolyLine::Point tmp_p(
            seg[0][0] + cp * (seg[1][0] - seg[0][0]),
            seg[0][1] + cp * (seg[1][1] - seg[0][1]),
            0.0
        );
        if (point_outside_polygon_segments(tmp_p, seg1s, false)) { union_set.push_back(seg); }
    }

    return connect_segment_set(union_set, result);
}


bool polygon_difference(PolyLine* polygon1, PolyLine* polygon2, std::vector<int> &poly_seg_1, std::vector<int> &poly_seg_2, PolyLine* result) {
    // ensure_counter_clockwise_polygon(polygon1);
    // ensure_clockwise_polygon(polygon2);

    std::vector<Segment> seg1s, seg2s;
    get_polygon_segments(polygon1, poly_seg_1, seg1s);
    get_polygon_segments(polygon2, poly_seg_2, seg2s, true); // clockwise

    std::vector<Segment> seg1clips, seg2clips;
    interclip_segment(seg1s, seg2s, seg1clips);
    interclip_segment(seg2s, seg1s, seg2clips);

    std::vector<Segment> difference_set;
    for (auto seg : seg1clips) {
        PolyLine::Point tmp_p(
            seg[0][0] + cp * (seg[1][0] - seg[0][0]),
            seg[0][1] + cp * (seg[1][1] - seg[0][1]),
            0.0
        );
        Segment on_seg2;
        if (point_on_polygon_segment(tmp_p, seg2s, on_seg2)) {
            if(segments_in_same_dir(seg, on_seg2)) { difference_set.push_back(seg); }
        }
        else if (point_outside_polygon_segments(tmp_p, seg2s, false)) { difference_set.push_back(seg); }
    }
    for (auto seg : seg2clips) {
        PolyLine::Point tmp_p(
            seg[0][0] + cp * (seg[1][0] - seg[0][0]),
            seg[0][1] + cp * (seg[1][1] - seg[0][1]),
            0.0
        );
        if (point_inside_polygon_segments(tmp_p, seg1s, false)) { difference_set.push_back(seg); }
    }

    return connect_segment_set(difference_set, result);
}


}


    // std::cout << "\n seg1s" << std::endl;
    // for (auto seg : seg1s) {
    //     std::cout << seg[0][0] << " " << seg[0][1] << "  " << seg[1][0] << " " << seg[1][1] << std::endl;
    // }
    // std::cout << "\n seg2s" << std::endl;
    // for (auto seg : seg2s) {
    //     std::cout << seg[0][0] << " " << seg[0][1] << "  " << seg[1][0] << " " << seg[1][1] << std::endl;
    // }
    // std::cout << "\n seg1clips" << std::endl;
    // for (auto seg : seg1clips) {
    //     std::cout << seg[0][0] << " " << seg[0][1] << "  " << seg[1][0] << " " << seg[1][1] << std::endl;
    // }
    // std::cout << "\n seg2clips" << std::endl;
    // for (auto seg : seg2clips) {
    //     std::cout << seg[0][0] << " " << seg[0][1] << "  " << seg[1][0] << " " << seg[1][1] << std::endl;
    // }
    // std::cout << "\n intersection_set" << std::endl;
    // for (auto seg : intersection_set) {
    //     std::cout << seg[0][0] << " " << seg[0][1] << "  " << seg[1][0] << " " << seg[1][1] << std::endl;
    // }