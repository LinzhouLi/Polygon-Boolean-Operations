#include "BooleanOpPlugin.hh"
#include "BooleanOperation.hh"
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

BooleanOpPlugin::BooleanOpPlugin() { }


void BooleanOpPlugin::initializePlugin() {
  // Create the Toolbox Widget
  QWidget* toolBox = new QWidget();
  QPushButton* intersectionButton = new QPushButton("&Intersection", toolBox);
  QPushButton* unionButton = new QPushButton("&Union", toolBox);
  QPushButton* differenceButton = new QPushButton("&Difference", toolBox);
  QPushButton* clearButton = new QPushButton("&Clear", toolBox);

  QGridLayout* layout = new QGridLayout(toolBox);
  layout->addWidget(intersectionButton, 0, 0);
  layout->addWidget(unionButton,        0, 1);
  layout->addWidget(differenceButton,   0, 2);
  layout->addWidget(clearButton,        1, 1);
  layout->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Expanding), 2, 0, 1, 2);

  connect(intersectionButton, SIGNAL(clicked()), this, SLOT(calcIntersection()));
  connect(unionButton,        SIGNAL(clicked()), this, SLOT(calcUnion()));
  connect(differenceButton,   SIGNAL(clicked()), this, SLOT(calcDifference()));
  emit addToolbox(tr("Boolean Op"), toolBox);
}


void BooleanOpPlugin::pluginsInitialized() {
    // Variable which will store the id of the newly created object.
    obj_id_1 = -1, obj_id_2 = -1, result_obj_id = -1;
    PolyLineObject* obj_1(0), *obj_2(0);

    // Emit the signal, that we want to create a new object of the specified type plane
    emit addEmptyObject( DATA_POLY_LINE, obj_id_1 );
    emit addEmptyObject( DATA_POLY_LINE, obj_id_2 );

    // Get the newly created object
    PluginFunctions::getObject(obj_id_1, obj_1);
    PluginFunctions::getObject(obj_id_2, obj_2);

    if(obj_1) {
        obj_1->line()->set_closed(true);
        PolyLine::Point p1(-1.0, -1.0, 0.0);
        PolyLine::Point p2(-0.5,  1.0, 0.0);
        PolyLine::Point p3( 0.5,  1.0, 0.0);
        PolyLine::Point p4( 1.0, -1.0, 0.0);
        obj_1->line()->add_point(p1);
        obj_1->line()->add_point(p2);
        obj_1->line()->add_point(p3);
        obj_1->line()->add_point(p4);
        // BoolOp::ensure_counter_clockwise_polygon(obj_1->line());
        log("Create Polygon 1!");
    } else {
        log("Fail to create Polygon 1!!");
    }

    if(obj_2) {
        obj_2->line()->set_closed(true);
        PolyLine::Point p1( 1.0,  1.0, 0.0);
        PolyLine::Point p2( 0.0, -1.0, 0.0);
        PolyLine::Point p3(-1.0,  1.0, 0.0);
        obj_2->line()->add_point(p1);
        obj_2->line()->add_point(p2);
        obj_2->line()->add_point(p3);
        // BoolOp::ensure_counter_clockwise_polygon(obj_2->line());
        log("Create Polygon 2!");

        ///////////////
        // double cross_val = BoolOp::cross(p1, p2, p3);
        // log("Cross of Polygon 2: " + QString("%1").arg(cross_val));
    } else {
        log("Fail to create Polygon 2!!");
    }

    ///////////////
    // double area1 = BoolOp::polygon_directed_area(obj_1->line());
    // double area2 = BoolOp::polygon_directed_area(obj_2->line());
    // log("Area of Polygon 1: " + QString("%1").arg(area1));
    // log("Area of Polygon 2: " + QString("%1").arg(area2));
    PolyLine::Point p1(1.0, 0.0, 0.0), p2(0.0, 1.0, 0.0);
    PolyLine::Point p3(1.0, 0.0, 0.0), p4(0.5, 0.5, 0.0);
    BoolOp::Segment seg1{p1, p2}, seg2{p3, p4};
    bool intersect_result = BoolOp::segments_intersect(seg1, seg2, true, false);
    bool overlap_result = BoolOp::segments_overlap(seg2, seg1);
    // bool overlap_result = (
    //     (seg1[0][0]-ev < seg2[0][0] && seg2[0][0] < seg1[1][0]+ev) || 
    //     (seg1[0][0]-ev < seg2[1][0] && seg2[1][0] < seg1[1][0]+ev)
    // );
    log("segments_intersect: " + QString("%1").arg(intersect_result));
    log("segments_overlap: " + QString("%1").arg(overlap_result));
}


void BooleanOpPlugin::calcIntersection() {
    log("Calculate intersection!");
}


void BooleanOpPlugin::calcUnion() {
    log("Calculate union!");
}


void BooleanOpPlugin::calcDifference() {
    log("Calculate difference!");
}