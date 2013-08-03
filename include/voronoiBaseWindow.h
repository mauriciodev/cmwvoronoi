#ifndef __VORONOIBASEUTILS_H_
#define __VORONOIBASEUTILS_H_
// TerraLib
#include <TeDatabase.h>
#include <TeVectorRemap.h>
#include <TeWaitCursor.h>
#include <TeQuerier.h>
#include <TeQuerierParams.h>
#include <TeGeoProcessingFunctions.h>
#include <PluginParameters.h>
#include <TeDataTypes.h>

#include <QMessageBox>
#include <QtGui/QDialog>
#include "TeOverlay.h"
#include <stdlib.h>




class voronoiBaseWindow : public QDialog
{
    Q_OBJECT
public:
   
    voronoiBaseWindow(PluginParameters* pp); //
    
    ~voronoiBaseWindow();

    TeLayer* createLayer(const std::string& name, TeDatabase* db, TeProjection* proj, const TeGeomRep& rep);

    bool createLayer(const std::string& name, TeDatabase* db, TeProjection* proj, TeLineSet& ls);
    TeLayer * createLayer(const std::string& name, TeDatabase* db, TeProjection* proj, vector<TeMultiPolygon>& ps);
    
    TeLayer * createLayer(const std::string& name, TeDatabase* db, TeProjection* proj, TePolygonSet& ps);
    TeTheme * createTheme(std::string layername, TeDatabase *db, TeView *view);
    bool clipLayer(TeLayer *diagramLayer, TeTheme *themeDelimiter, TeDatabase *db);
    bool copyAttributes(TeLayer *diagramLayer, TeTheme *points, TeDatabase *db);
    TeTheme* getTheme(const std::string& name);
    TeLayer* getLayer(const std::string& name);
    std::vector<std::string> listAttributes(TeTheme* theme);
    bool isLayerNameValid(const std::string& name);
    bool Union(TePolygonSet &psIn, TeMultiPolygon &mpol);
private:
    PluginParameters* plugin_params_;

};



#endif // __VORONOIBASEUTILS_H_


 
