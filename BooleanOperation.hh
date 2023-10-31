#ifndef BOOLEANOPERATION_HH
#define BOOLEANOPERATION_HH

#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/PolyLine/PolyLine.hh>
#include <vector>

namespace BoolOp {

#define ev 2.3140692632779263e-06
#define ev_sq 5.354916555247645e-12
typedef std::vector<PolyLine::Point> Segment;

double cross(PolyLine::Point &p, PolyLine::Point &q, PolyLine::Point &b);
double to_left(PolyLine::Point &p, PolyLine::Point &q, PolyLine::Point &b);
bool point_is_equal(PolyLine::Point &p, PolyLine::Point &q);

bool get_intersect_point_of_segments(Segment &seg1, Segment &seg2, PolyLine::Point& result);
bool get_intersect_point_of_lines(Segment &line1, Segment &line2, PolyLine::Point& result);

double polygon_directed_area(PolyLine* polygon);
void ensure_counter_clockwise_polygon(PolyLine* polygon);

}

#endif //BOOLEANOPERATION_HH