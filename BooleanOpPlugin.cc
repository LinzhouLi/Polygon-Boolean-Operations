#include "BooleanOpPlugin.hh"
#include "BooleanOperation.hh"
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include <iostream>
#include <cassert>
#include <fstream>


BooleanOpPlugin::BooleanOpPlugin() { }


void BooleanOpPlugin::initializePlugin() {
  // Create the Toolbox Widget
  QWidget* toolBox = new QWidget();
  QPushButton* intersectionButton = new QPushButton("&Intersection", toolBox);
  QPushButton* unionButton = new QPushButton("&Union", toolBox);
  QPushButton* differenceButton = new QPushButton("&Difference", toolBox);
  QPushButton* loadPoly1Button = new QPushButton("&LoadPolygon1", toolBox);
  QPushButton* loadPoly2Button = new QPushButton("&LoadPolygon2", toolBox);
  QPushButton* clearButton = new QPushButton("&Clear", toolBox);

  QGridLayout* layout = new QGridLayout(toolBox);
  layout->addWidget(intersectionButton, 0, 0);
  layout->addWidget(unionButton,        0, 1);
  layout->addWidget(differenceButton,   0, 2);
  layout->addWidget(loadPoly1Button,    1, 0);
  layout->addWidget(loadPoly2Button,    1, 1);
  layout->addWidget(clearButton,        1, 2);
  layout->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Expanding), 2, 0, 1, 2);

  connect(intersectionButton, SIGNAL(clicked()), this, SLOT(calcIntersection()));
  connect(unionButton,        SIGNAL(clicked()), this, SLOT(calcUnion()));
  connect(differenceButton,   SIGNAL(clicked()), this, SLOT(calcDifference()));
  connect(loadPoly1Button,    SIGNAL(clicked()), this, SLOT(loadPolygon1()));
  connect(loadPoly2Button,    SIGNAL(clicked()), this, SLOT(loadPolygon2()));
  connect(clearButton,        SIGNAL(clicked()), this, SLOT(clearObjects()));
  emit addToolbox(tr("Boolean Op"), toolBox);
}


void BooleanOpPlugin::pluginsInitialized() {
    // Variable which will store the id of the newly created object.
    obj_id_1 = -1, obj_id_2 = -1, result_obj_id = -1;
    PolyLineObject* obj_1(0), *obj_2(0), *result_obj(0);

    // Emit the signal, that we want to create a new object of the specified type plane
    emit addEmptyObject( DATA_POLY_LINE, obj_id_1 );
    emit addEmptyObject( DATA_POLY_LINE, obj_id_2 );
    emit addEmptyObject( DATA_POLY_LINE, result_obj_id );

    // Get the newly created object
    PluginFunctions::getObject(obj_id_1, obj_1);
    PluginFunctions::getObject(obj_id_2, obj_2);
    PluginFunctions::getObject(result_obj_id, result_obj);

    if(obj_1) { log("Create polygon 1!"); } 
    else { log("Fail to create polygon 1!!"); }

    if(obj_2) { log("Create polygon 2!"); } 
    else { log("Fail to create polygon 2!!"); }

    if(result_obj) { log("Create result polygon!"); } 
    else { log("Fail to result polygon!!"); }
}


void BooleanOpPlugin::clearSourceObjects() {
    PolyLineObject* obj_1(0), *obj_2(0);
    PluginFunctions::getObject(obj_id_1, obj_1);
    PluginFunctions::getObject(obj_id_2, obj_2);
    obj_1->line()->clear();
    obj_2->line()->clear();
    emit updatedObject(obj_id_1, UPDATE_GEOMETRY);
    emit updatedObject(obj_id_2, UPDATE_GEOMETRY);
    log("Clear source objects.");
}


void BooleanOpPlugin::clearResultObject() {
    PolyLineObject* result_obj(0);
    PluginFunctions::getObject(result_obj_id, result_obj);
    result_obj->line()->clear();
    emit updatedObject(result_obj_id, UPDATE_GEOMETRY);
    log("Clear result object.");
}


void BooleanOpPlugin::calcIntersection() {
    log("Calculate intersection...");
    PolyLineObject* obj_1(0), *obj_2(0), *result_obj(0);
    PluginFunctions::getObject(obj_id_1, obj_1);
    PluginFunctions::getObject(obj_id_2, obj_2);
    PluginFunctions::getObject(result_obj_id, result_obj);

    if (obj_1->line()->n_vertices() < 3 || obj_1->line()->n_vertices() < 3) {
        log("Invalid source objects!");
        return;
    }
    if (result_obj->line()->n_vertices() > 0) {
        log("Result object is not empty!");
        return;
    }

    if ( BoolOp::polygon_intersection(obj_1->line(), obj_2->line(), result_obj->line()) ) {
        log("Success to calculate intersection!");
        emit updatedObject(result_obj_id, UPDATE_GEOMETRY);
        clearSourceObjects();
    } else {
        log("Fail to calculate intersection!");
    }
}


void BooleanOpPlugin::calcUnion() {
    log("Calculate union...");
    PolyLineObject* obj_1(0), *obj_2(0), *result_obj(0);
    PluginFunctions::getObject(obj_id_1, obj_1);
    PluginFunctions::getObject(obj_id_2, obj_2);
    PluginFunctions::getObject(result_obj_id, result_obj);

    if (obj_1->line()->n_vertices() < 3 || obj_1->line()->n_vertices() < 3) {
        log("Invalid source objects!");
        return;
    }
    if (result_obj->line()->n_vertices() > 0) {
        log("Result object is not empty!");
        return;
    }

    if ( BoolOp::polygon_union(obj_1->line(), obj_2->line(), result_obj->line()) ) {
        log("Success to calculate union!");
        emit updatedObject(result_obj_id, UPDATE_GEOMETRY);
        clearSourceObjects();
    } else {
        log("Fail to calculate union!");
    }
}


void BooleanOpPlugin::calcDifference() {
    log("Calculate difference...");
    PolyLineObject* obj_1(0), *obj_2(0), *result_obj(0);
    PluginFunctions::getObject(obj_id_1, obj_1);
    PluginFunctions::getObject(obj_id_2, obj_2);
    PluginFunctions::getObject(result_obj_id, result_obj);

    if (obj_1->line()->n_vertices() < 3 || obj_1->line()->n_vertices() < 3) {
        log("Invalid source objects!");
        return;
    }
    if (result_obj->line()->n_vertices() > 0) {
        log("Result object is not empty!");
        return;
    }

    if ( BoolOp::polygon_difference(obj_1->line(), obj_2->line(), result_obj->line()) ) {
        log("Success to calculate difference!");
        emit updatedObject(result_obj_id, UPDATE_GEOMETRY);
        clearSourceObjects();
    } else {
        log("Fail to calculate difference!");
    }
}


bool BooleanOpPlugin::parseFile(const std::string &filePath, PolyLine* polygon) {
    using namespace std;
    auto f = fstream(filePath, ios::in);
    if (!f.is_open()) { return false; }

    string line;
    istringstream linestream;
    while (getline(f, line)) {
        linestream = istringstream(line);
        double x, y;
        linestream >> x >> y;
        PolyLine::Point p(x, y, 0.0);
        polygon->add_point(p);
    }

    f.close();
    return true;
}


void BooleanOpPlugin::loadPolygon1() {
    PolyLineObject* obj_1(0);
    PluginFunctions::getObject(obj_id_1, obj_1);
    if (obj_1->line()->n_vertices() > 0) {
        log("Object 1 is not empty");
        return;
    }

    auto filePath = QFileDialog::getOpenFileName(
        nullptr, tr("Choose one file"),
        QString(), u8"Plain Text (*.txt *.ppp)"
    );

    if (filePath.isEmpty() || filePath.isNull()) { return; }
    if (!parseFile(filePath.toStdString(), obj_1->line())) {
        log("Fail to parse file: " + QString(filePath));
        return;
    }

    obj_1->line()->set_closed(true);
    emit updatedObject(obj_id_1, UPDATE_GEOMETRY);
    log("Load polygon 1!");
}


void BooleanOpPlugin::loadPolygon2() {
    PolyLineObject* obj_2(0);
    PluginFunctions::getObject(obj_id_2, obj_2);
    if (obj_2->line()->n_vertices() > 0) {
        log("Object 2 is not empty");
        return;
    }

    auto filePath = QFileDialog::getOpenFileName(
        nullptr, tr("Choose one file"),
        QString(), u8"Plain Text (*.txt *.ppp)"
    );
    
    if (filePath.isEmpty() || filePath.isNull()) { return; }
    if (!parseFile(filePath.toStdString(), obj_2->line())) {
        log("Fail to parse file: " + QString(filePath));
        return;
    }

    obj_2->line()->set_closed(true);
    emit updatedObject(obj_id_2, UPDATE_GEOMETRY);
    log("Load polygon 2!");
}


void BooleanOpPlugin::clearObjects() {
    clearSourceObjects();
    clearResultObject();
}