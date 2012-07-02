#ifndef __VORONOIDIAGRAM_WINDOW_H_
#define __VORONOIDIAGRAM_WINDOW_H_
  
#include "UIVoronoi.h"

// TerraLib
#include <PluginParameters.h>
#include <TeDataTypes.h>

// forward declarations
class TeDatabase;
class TeLayer;
class TeLineSet;
class TePolygonSet;
class TeProjection;
class TeTheme;

class VoronoiWindow : public UIVoronoi
{
    Q_OBJECT

public:
   
    VoronoiWindow(PluginParameters* pp, const bool& delaunay = false);
    
    ~VoronoiWindow();
    
public slots:

    void generateLinesCheckBox_clicked();
    void themeComboBox_activated(const QString& themeName);
	void okPushButton_clicked();
    void helpPushButton_clicked();

    void showWindow();

private:

    TeLayer* createLayer(const std::string& name, TeDatabase* db, TeProjection* proj, const TeGeomRep& rep);

    bool createLayer(const std::string& name, TeDatabase* db, TeProjection* proj, TeLineSet& ls);
    
    bool createLayer(const std::string& name, TeDatabase* db, TeProjection* proj, TePolygonSet& ps);

    TeTheme* getTheme(const std::string& name);
    TeLayer* getLayer(const std::string& name);
    
    bool isLayerNameValid(const std::string& name);
		
private:

    PluginParameters* plugin_params_;
    bool delaunay_;
};

#endif // __VORONOIDIAGRAM_WINDOW_H_


