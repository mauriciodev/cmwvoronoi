#include "voronoiBaseWindow.h"

voronoiBaseWindow::voronoiBaseWindow(PluginParameters* pp): QDialog(0){ //
    this->plugin_params_=pp;
}

voronoiBaseWindow::~voronoiBaseWindow() {

}

TeLayer* voronoiBaseWindow::createLayer(const std::string& name, TeDatabase* db, TeProjection* proj, const TeGeomRep& rep)
{
    TeLayer* layer = new TeLayer(name, db, proj);

    TeAttributeList attList;
    TeAttribute at;
    at.rep_.type_ = TeSTRING;
    at.rep_.numChar_ = 255;
    at.rep_.name_ = "object_id";
    at.rep_.isPrimaryKey_ = true;
    attList.push_back(at);

    TeTable attTable(name, attList, "object_id", "object_id");
    if(!layer->createSfsTable(attTable, rep))
    {
        QMessageBox::critical(this, tr("Error"), tr("Error creating the new layer table."));
        db->deleteLayer(layer->id());
        return 0;
    }

    if(!layer->addGeometry(rep))
    {
        QMessageBox::critical(this, tr("Error"), tr("Error adding geometric representation to new layer."));
        db->deleteLayer(layer->id());
        return 0;
    }

    return layer;
}

bool voronoiBaseWindow::createLayer(const std::string& name, TeDatabase* db, TeProjection* proj, TeLineSet& ls)
{
    TeLayer* layer = createLayer(name, db, proj, TeLINES);
    if(layer == 0)
        return false;

    TeFeatureSet fs;
    TeTable& attrTable = layer->attrTables()[0];
    for(unsigned int i = 0; i < ls.size(); ++i)
    {
        std::string sid = Te2String(i);
        ls[i].objectId(sid);

        TeTableRow row;
        row.push_back(sid);

        TeMultiLine	geom(ls[i]);

        TeFeature feature(row, geom);
        fs.add(feature);
    }

    if(!layer->addFeatures(fs, false))
    {
        QMessageBox::critical(this, tr("Error"), tr("Error saving the new layer table."));
        db->deleteLayer(layer->id());
        return false;
    }

    return true;
}

TeLayer * voronoiBaseWindow::createLayer(const std::string& name, TeDatabase* db, TeProjection* proj, TePolygonSet& ps)
{
    TeLayer* layer = createLayer(name, db, proj, TeMULTIPOLYGONS);
    if(layer == 0)
        return false;

    TeFeatureSet fs;
    TeTable& attrTable = layer->attrTables()[0];
    std::string lastSid="";
    std::string sid;
    TeMultiPolygon lastGeom;
    TePolygonSet oldPoly;
    for(unsigned int i = 0; i < ps.size(); ++i)
    {
        if (ps[i].objectId()=="") {
            sid = Te2String(i);
            ps[i].objectId(sid);
        } else {
            sid=ps[i].objectId();
        }
        TeTableRow row;
        row.push_back(sid);





        if (lastSid!=sid) {
            oldPoly.clear();
            oldPoly.add(ps[i]);
            TeMultiPolygon geom(oldPoly);
            TeFeature feature(row, geom);
            fs.add(feature);

        } else {
            int lastId=fs.size()-1;
            oldPoly.add(ps[i]);
            //lastGeom.add(ps[i]);
            fs.remove(lastId);
            TeMultiPolygon geom(oldPoly);
            TeFeature feature(row, geom);
            fs.add(feature);
            //geom2=fs[lastId].getGeometry(0);
            //fs[lastId].addGeometry(ps[1]);
            //fs[lastId].removeGeometry(0);
            //fs[lastId].addGeometry(lastGeom);
        }
        lastSid=sid;

    }

    if(!layer->addFeatures(fs, false))
    {
        QMessageBox::critical(this, tr("Error"), tr("Error adding geometries to new layer."));
        db->deleteLayer(layer->id());
        return false;
    }

    return layer;
}

TeLayer * voronoiBaseWindow::createLayer(const std::string& name, TeDatabase* db, TeProjection* proj, vector<TeMultiPolygon>& ps)
{
    TeLayer* layer = createLayer(name, db, proj, TeMULTIPOLYGONS);
    if(layer == 0)
        return false;

    TeFeatureSet fs;
    TeTable& attrTable = layer->attrTables()[0];
    std::string lastSid="";
    std::string sid;
    TeMultiPolygon lastGeom;
    TePolygonSet oldPoly;
    for(unsigned int i = 0; i < ps.size(); ++i)
    {
        if (ps[i].objectId()=="") {
            sid = Te2String(i);
            ps[i].objectId(sid);
        } else {
            sid=ps[i].objectId();
        }
        TeTableRow row;
        row.push_back(sid);
        TeFeature feature(row, ps[i]);
        fs.add(feature);


    }

    if(!layer->addFeatures(fs, false))
    {
        QMessageBox::critical(this, tr("Error"), tr("Error adding geometries to new layer."));
        db->deleteLayer(layer->id());
        return false;
    }

    return layer;
}


TeTheme* voronoiBaseWindow::getTheme(const std::string& name)
{
    TeDatabase* db = plugin_params_->getCurrentDatabasePtr();
    TeThemeMap& themeMap = db->themeMap();
    TeTheme* theme = 0;
    TeThemeMap::iterator themeIt;
    for(themeIt = themeMap.begin(); themeIt != themeMap.end(); ++themeIt)
    {
        if(themeIt->second->name() == name)
        {
            theme = dynamic_cast<TeTheme*>(themeIt->second);
            break;
        }
    }
    return theme;
}

TeLayer* voronoiBaseWindow::getLayer(const std::string& name)
{
    TeDatabase* db = plugin_params_->getCurrentDatabasePtr();
    TeLayerMap& layerMap = db->layerMap();
    TeLayer* layer = 0;
    TeLayerMap::iterator layerIt;
    for(layerIt = layerMap.begin(); layerIt != layerMap.end(); ++layerIt)
    {
        if(layerIt->second->name() == name)
        {
            layer = layerIt->second;
            break;
        }
    }
    return layer;
}

bool voronoiBaseWindow::isLayerNameValid(const std::string& name)
{
    TeDatabase* db = plugin_params_->getCurrentDatabasePtr();
    if(db->layerExist(name))
    {
        QMessageBox::information(this, tr("Information"), tr("The Layer name already exist. Please, try another."));
        return false;
    }

    bool changed;
    std::string errorMessage;
    TeCheckName(name, changed, errorMessage);
    if(changed)
    {
        QString msg = tr("The Layer name is invalid: \n") + errorMessage.c_str();
        msg += "\n" + tr("Please, try another.");
        QMessageBox::information(this, tr("Information"), msg);
        return false;
    }

    return true;
}

TeTheme * voronoiBaseWindow::createTheme(string layername, TeDatabase *db, TeView *view) {
    TeLayer* layer = new TeLayer(layername,db);
    TeTable attTable2;
    layer->getAttrTablesByName(layername,attTable2);
    TeTheme* theme = new TeTheme(layername, layer);
    view->add(theme);
    theme ->addThemeTable(attTable2);
    theme->visibleRep(layer->geomRep());
    if (!theme->save() || !theme->buildCollection())
    {
        cout << "Error trying to create the theme"
               << db->errorMessage() << endl;
        return NULL;
    }
    cout << "Theme created ..\n";
    return theme;
}

bool voronoiBaseWindow::clipLayer(TeLayer *diagramLayer, TeTheme *themeDelimiter, TeDatabase *db) {
    TeView* view = new TeView("Voronoi");
    view->projection(diagramLayer->projection());
    db->insertView(view);
    TeTheme * diagramTheme=createTheme(diagramLayer->name(),db,view);

    TeBox bInvalid;
    TeLayer *clippedLayer=new TeLayer((diagramLayer->name()+"_clipped"), db, bInvalid, diagramLayer->projection());
    bool result=TeGeoOpOverlayIntersection(clippedLayer,diagramTheme,themeDelimiter,TeAll,TeAll,true);
    //delete clippedLayer;
    db->deleteTheme(diagramTheme->id());
    db->deleteView(view->id());
    return result;
}

bool voronoiBaseWindow::copyAttributes(TeLayer *diagramLayer, TeTheme *pointsTheme, TeDatabase *db) {
    TeView* view = new TeView("Voronoi");
    view->projection(diagramLayer->projection());
    db->insertView(view);
    TeTheme * diagramTheme=createTheme(diagramLayer->name(),db,view);

    //attribute mapper
    TeGroupingAttr measuresColl;
    //pointsTheme->loadThemeTables ();
    TeAttrTableVector attrTables;
    pointsTheme->getAttTables(attrTables);
    TeAttributeList attrList= attrTables[0].attributeList ();
    for (TeAttributeList::iterator i=attrList.begin(); i!=attrList.end();++i) {
        measuresColl.push_back(make_pair<TeAttributeRep, TeStatisticType>(i->rep_, TeMODE));
    }
    bool result=TeGeoOpAssignByLocationCollect(diagramTheme,pointsTheme,diagramLayer->name(),measuresColl   ,TeINTERSECTS);

    db->deleteTheme(diagramTheme->id());
    db->deleteView(view->id());
    return result;
}

bool voronoiBaseWindow::Union(TePolygonSet &psIn, TeMultiPolygon &mpol) {
    TePolygonSet polSetValidHoles;

    if(!TeOVERLAY::TeValidPolygonHoles(psIn, polSetValidHoles))
    {
        return false;
    }

    TePolygonSet psetOut;

    if(!TeOVERLAY::TeUnion(polSetValidHoles, psetOut)) {
        return false;
    }
    for (TePolygonSet::iterator pIt=polSetValidHoles.begin();pIt!=polSetValidHoles.end();++pIt) {
        mpol.add(*pIt);
    }
    return true;
}
