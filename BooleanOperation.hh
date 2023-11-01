#ifndef BOOLEANOPERATION_HH
#define BOOLEANOPERATION_HH

#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/PolyLine/PolyLine.hh>
#include <array>

#define ev 2.3140692632779263e-06
#define ev_sq 5.354916555247645e-12
#define cp 0.7561814590413005

namespace BoolOp {

typedef std::array<PolyLine::Point, 2> Segment;

double cross(PolyLine::Point &p, PolyLine::Point &q, PolyLine::Point &b);
double to_left(PolyLine::Point &p, PolyLine::Point &q, PolyLine::Point &b);

bool points_are_equal(PolyLine::Point &p, PolyLine::Point &q);
bool point_on_segment(PolyLine::Point &point, Segment &seg, bool on_end = true);
bool point_on_polygon_segment(PolyLine::Point &point, std::vector<Segment> &segs, Segment &which_seg);
bool point_inside_polygon_segments(PolyLine::Point &point, std::vector<Segment> &segs, bool on_seg = true);
bool point_outside_polygon_segments(PolyLine::Point &point, std::vector<Segment> &segs, bool on_seg = true);

bool segments_are_equal(Segment &seg1, Segment &seg2);
bool segments_in_same_dir(Segment &seg1, Segment &seg2);
bool segments_overlap(Segment &seg1, Segment &seg2);
bool segments_intersect(Segment &seg1, Segment &seg2, bool on_seg = true, bool on_end = true);
void interclip_segment(std::vector<Segment> &seg1s, std::vector<Segment> &seg2s, std::vector<Segment>& seg_clip);

bool get_intersect_point_of_segments(Segment &seg1, Segment &seg2, PolyLine::Point& result);
bool get_intersect_point_of_lines(Segment &line1, Segment &line2, PolyLine::Point& result);

void get_polygon_segments(PolyLine* polygon, std::vector<Segment> &segs);
double polygon_directed_area(PolyLine* polygon);
void ensure_clockwise_polygon(PolyLine* polygon);
void ensure_counter_clockwise_polygon(PolyLine* polygon);
bool connect_segment_set(std::vector<Segment> &segs, PolyLine* polygon);

bool polygon_intersection(PolyLine* polygon1, PolyLine* polygon2, PolyLine* result);
bool polygon_union(PolyLine* polygon1, PolyLine* polygon2, PolyLine* result);
bool polygon_difference(PolyLine* polygon1, PolyLine* polygon2, PolyLine* result);


}

#endif //BOOLEANOPERATION_HH