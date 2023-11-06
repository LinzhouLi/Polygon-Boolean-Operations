#ifndef BOOLEANOPPLUGIN_HH
#define BOOLEANOPPLUGIN_HH

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <ObjectTypes/PolyLine/PolyLine.hh>

#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QSpinBox>

#include <string>

class BooleanOpPlugin : public QObject, BaseInterface, ToolboxInterface, LoggingInterface, LoadSaveInterface
{
    Q_OBJECT
    Q_INTERFACES(BaseInterface)
    Q_INTERFACES(ToolboxInterface)
    Q_INTERFACES(LoadSaveInterface)
    Q_INTERFACES(LoggingInterface)
    Q_PLUGIN_METADATA(IID "cn.edu.zju.linzhouli.BooleanOpPlugin")

signals:
    //BaseInterface
    void updateView();
    void updatedObject(int _identifier, const UpdateType& _type);
    //LoggingInterface
    void log(Logtype _type, QString _message);
    void log(QString _message);
    // ToolboxInterface
    void addToolbox( QString _name, QWidget* _widget );
    // LoadSaveInterface
    void addEmptyObject( DataType _type, int& _id);
    void deleteObject( int& _id );

public:
    BooleanOpPlugin();
    // BaseInterface
    QString name() { return (QString("Boolean Operator")); };
    QString description( ) { return (QString("Boolean operation of polygons")); };

private:
    int obj_id_1, obj_id_2, result_obj_id;
    std::vector<int> poly_seg_1, poly_seg_2;

    void clearSourceObjects();
    void clearResultObject();
    bool parseFile(const std::string &filePath, PolyLine* polygon, std::vector<int> &poly_seg);

private slots:
    // BaseInterface
    void initializePlugin();
    void pluginsInitialized();
    // boolean operator
    void calcIntersection();
    void calcUnion();
    void calcDifference();

    void loadPolygon1();
    void loadPolygon2();
    void clearObjects();

public slots:
    QString version() { return QString("1.0"); };
};

#endif //BOOLEANOPPLUGIN_HH