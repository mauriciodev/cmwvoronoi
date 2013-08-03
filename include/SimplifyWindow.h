#ifndef UISIMPLIFY_H
#define UISIMPLIFY_H
#include "ui_simplify.h"
//#include "VoronoiWindow.h"
#include <QMessageBox>


//#include <ui/qt4_help.h>
#include <TeDatabase.h>
#include <TeVectorRemap.h>
#include <TeWaitCursor.h>
#include <TeQuerier.h>
#include <TeQuerierParams.h>
#include <TeGeoProcessingFunctions.h>
#include <PluginParameters.h>
#include <TeDataTypes.h>
#include "voronoiBaseWindow.h"

// forward declarations
class TeDatabase;
class TeLayer;
class TeLineSet;
class TePolygonSet;
class TeProjection;
class TeTheme;

#include <stdlib.h>

class SimplifyWindow: public voronoiBaseWindow, private Ui::UISimplify   {
    Q_OBJECT

public:
        SimplifyWindow(PluginParameters* pp) ;
        ~SimplifyWindow(){};
        void showWindow();

public slots:
    void pushButton_clicked();
    void loadThemes();


private:
     PluginParameters* plugin_params_;

        
};

#endif // UIVORONOI_H


