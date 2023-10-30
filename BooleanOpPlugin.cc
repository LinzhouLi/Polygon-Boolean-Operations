#include "BooleanOpPlugin.hh"
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
        PolyLine::Point p2( 0.0,  1.0, 0.0);
        PolyLine::Point p3( 1.0, -1.0, 0.0);
        obj_1->line()->add_point(p1);
        obj_1->line()->add_point(p2);
        obj_1->line()->add_point(p3);
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
        log("Create Polygon 2!");
    } else {
        log("Fail to create Polygon 2!!");
    }
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