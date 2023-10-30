#ifndef BOOLEANOPERATION_HH
#define BOOLEANOPERATION_HH

#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/PolyLine/PolyLine.hh>

double polygon_directed_area(PolyLine* polygon);
void ensure_counter_clockwise_polygon(PolyLine* polygon);

#endif //BOOLEANOPERATION_HH