#pragma optimize("g", off)
// Voronoi
#include "VoronoiWindow.h"
#include "VoronoiDiagramGenerator.h"
#include "Utils.h"
#include "ui_UIVoronoi.h"


// Qt
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qtabwidget.h>
#include <qlineedit.h>

struct XYOrderFunctor
{
    bool operator()(const TePoint& p1, const TePoint& p2) const
    {
        if(p1.location().x_ < p2.location().x_)
            return true;
        else if(p1.location().x_ > p2.location().x_)
            return false;

        if(p1.location().y_ < p2.location().y_)
            return true;
        else if(p1.location().y_ > p2.location().y_)
            return false;

        return false;
    }
};



bool VoronoiWindow::MWDiagramAsTePolygonSet(MWVDiagram &diagram, TePolygonSet &ps) {
    MWVDiagram::iterator dIt;
    int polId=1;
	mwv_base base;
    for (MWVDiagram::iterator polIt=diagram.begin(); polIt!=diagram.end(); ++polIt) {
        std::list<Polygon_with_holes_2> res;
        std::list<Polygon_with_holes_2>::const_iterator it;
        cout << "Polygons: "<<polIt->number_of_polygons_with_holes() << endl;
        polIt->polygons_with_holes (std::back_inserter (res));
        TeMultiPolygon mpol;
        for (it=res.begin();it!=res.end();++it) {
            TePolygon pol;
            TeLinearRing ring;
            pol.clear();
            ring.clear();
            Polygon_2::Curve_const_iterator cIt;
            //cout << "Curves: "<<it->outer_boundary().number << endl;
            //reading outer boundary
            for(cIt=it->outer_boundary().curves_begin(); cIt!=it->outer_boundary().curves_end();++cIt) {

                vector<double>x,y;

                base.arcAsLinestring(*cIt,x,y);
                for(uint i=0; i<x.size()-1;i++) {
                    if( ! ((x[i]!=x[i]) || (y[i]!=y[i]) ) ) { //not nan
                        ring.add(TeCoord2D(x[i],y[i]));
                    }
                }
            }
            ring.add(ring[0]);
            pol.add(ring); //if (ring.IsValid())
            cout<<ring.size()<<endl;
			mwv_base base;
            //reading holes
            Polygon_with_holes_2::Hole_const_iterator hit;
            std::cout << "  " << it->number_of_holes() << " holes:" << std::endl;
            for (hit = it->holes_begin(); hit != it->holes_end(); ++hit) {
                TeLinearRing innerRing;
                for(cIt=hit->curves_begin(); cIt!=hit->curves_end();++cIt) {
                    vector<double>x,y;

                    base.arcAsLinestring(*cIt,x,y);
                    for(uint i=0; i<x.size()-1;i++) {
                        if( ! ((x[i]!=x[i]) || (y[i]!=y[i]) ) ) { //not nan
                            innerRing.add(TeCoord2D(x[i],y[i]));
                        }
                    }
                }
                if (it->number_of_holes()>0) {

                    innerRing.add(innerRing[0]);
                    //forcing orientation for holes.
                    if (TeOrientation(innerRing)==TeCLOCKWISE)
                        reverse(innerRing.begin(),innerRing.end());
                    pol.add(innerRing);
                }
            }
            //ring.getPoint(0,&p0);

            string objectId;
            stringstream ss;
            ss<<polId;
            ss>>objectId;
            pol.geomId(polId);
            pol.objectId(objectId);
            ps.add(pol);
        }
        polId++;

    }
	if (ps.size()>0) {
		return true;
	} else {
		return false;
	}
}

bool VoronoiWindow::MWDiagramAsTeMultiPolygons(MWVDiagram &diagram, std::vector<TeMultiPolygon> &mpols){
    MWVDiagram::iterator dIt;
    int polId=1;
    mwv_base base;
    for (MWVDiagram::iterator polIt=diagram.begin(); polIt!=diagram.end(); ++polIt) {
        std::list<Polygon_with_holes_2> res;
        std::list<Polygon_with_holes_2>::const_iterator it;
        cout << "Polygons: "<<polIt->number_of_polygons_with_holes() << endl;
        polIt->polygons_with_holes (std::back_inserter (res));
        TeMultiPolygon mpol;
        for (it=res.begin();it!=res.end();++it) {
            TePolygon pol;
            TeLinearRing ring;
            pol.clear();
            ring.clear();
            Polygon_2::Curve_const_iterator cIt;
            //cout << "Curves: "<<it->outer_boundary().number << endl;
            //reading outer boundary
            for(cIt=it->outer_boundary().curves_begin(); cIt!=it->outer_boundary().curves_end();++cIt) {

                vector<double>x,y;

                base.arcAsLinestring(*cIt,x,y);
                for(uint i=0; i<x.size()-1;i++) {
                    if( ! ((x[i]!=x[i]) || (y[i]!=y[i]) ) ) { //not nan
                        ring.add(TeCoord2D(x[i],y[i]));
                    }
                }
            }
            ring.add(ring[0]);
            pol.add(ring); //if (ring.IsValid())
            cout<<ring.size()<<endl;
            mwv_base base;
            //reading holes
            Polygon_with_holes_2::Hole_const_iterator hit;
            std::cout << "  " << it->number_of_holes() << " holes:" << std::endl;
            for (hit = it->holes_begin(); hit != it->holes_end(); ++hit) {
                TeLinearRing innerRing;
                for(cIt=hit->curves_begin(); cIt!=hit->curves_end();++cIt) {
                    vector<double>x,y;

                    base.arcAsLinestring(*cIt,x,y);
                    for(uint i=0; i<x.size()-1;i++) {
                        if( ! ((x[i]!=x[i]) || (y[i]!=y[i]) ) ) { //not nan
                            innerRing.add(TeCoord2D(x[i],y[i]));
                        }
                    }
                }
                if (it->number_of_holes()>0) {

                    innerRing.add(innerRing[0]);
                    //forcing orientation for holes.
                    if (TeOrientation(innerRing)==TeCLOCKWISE)
                        reverse(innerRing.begin(),innerRing.end());
                    pol.add(innerRing);
                }
            }
            mpol.add(pol);
            //ring.getPoint(0,&p0);
        }
        mpols.push_back(mpol);
        /*string objectId;
        stringstream ss;
        ss<<polId;
        ss>>objectId;
        pol.geomId(polId);
        pol.objectId(objectId);
        ps.add(pol);*/
        polId++;

    }
    if (mpols.size()>0) {
        return true;
    } else {
        return false;
    }
}

VoronoiWindow::VoronoiWindow(PluginParameters* pp, const enumDiagramType diagramType) : 
UIVoronoi((QWidget*)pp->qtmain_widget_ptr_),
plugin_params_(pp)
{
	/*! Selects what aditional parameters each diagram needs.*/
	ui = (new Ui::UIVoronoi);
	ui->setupUi(this);

    ui->weightGroupBox->hide();
    this->diagramType=diagramType;

   if(diagramType==Delaunay) {
        setCaption("Delaunay");
		ui->boxButtonGroup->hide();
    }
    if(diagramType==MWVoronoi) {
        setCaption("Multiplicatively Weighted Voronoi");
        ui->weightGroupBox->show();
    }
}

VoronoiWindow::~VoronoiWindow()
{}

void VoronoiWindow::generateLinesCheckBox_clicked()
{
    //ui->layerLinesButtonGroup->setEnabled(ui->generateLinesCheckBox->isChecked());
}

void VoronoiWindow::themeComboBox_activated(const QString& themeName)
{
    TeTheme* theme = getTheme(themeName.latin1());
    if(theme == 0)
        return;
    // Updates the combo box with delimiters...
    TeLayer* layer = theme->layer();
    ui->boxComboBox->setCurrentText(layer->name().c_str());

	// Lists attributes on the weightComboBox
    ui->weightComboBox->clear();
    //if (text=="") text=themeComboBox->currentText().latin1();
    TeTable *attrTable = &layer->attrTables()[0];
    TeAttributeList * attrList=&attrTable->attributeList();
    for (std::vector<TeAttribute>::iterator i=attrList->begin(); i!=attrList->end();++i) {
        cout<<i->rep_.name_<<endl;
        ui->weightComboBox->insertItem(i->rep_.name_.c_str());
    }
}

void VoronoiWindow::helpPushButton_clicked()
{
    Help* help = new Help(this);
	help->init(""); // TODO: Voronoi.html and Delaunay.html
	if(help->erro_ == false)
	{
		help->show();
		help->raise();
	}
	else
		delete help;
}

void VoronoiWindow::okPushButton_clicked()
{
    std::string layerName = (std::string)ui->layerNameLineEdit->text();	
	if(layerName.empty())
	{
		QMessageBox::information(this, tr("Information"), tr("Please, define a name to result Layer."));
        ui->voronoiTabWidget->setCurrentPage(0);
        ui->layerNameLineEdit->setFocus();
		return;
	}



    if(!isLayerNameValid(layerName))
    {
        ui->voronoiTabWidget->setCurrentPage(0);
        ui->layerNameLineEdit->setFocus();
        return;
    }


	TeDatabase* db = plugin_params_->getCurrentDatabasePtr();
    TeTheme* theme = getTheme(ui->themeComboBox->currentText().latin1());
	if(theme == 0)
	{
		QMessageBox::critical(this, tr("Error"), tr("Error getting the input Theme."));
		return;
	}
    
    TePrecision::instance().setPrecision(TeGetPrecision(theme->layer()->projection()));
    
    TeTheme *themeDelimiter = getTheme(ui->boxComboBox->currentText().latin1());
    if(themeDelimiter == 0)
    {
        QMessageBox::critical(this, tr("Error"), tr("Error getting the delimiter Layer."));
		return;
    }

    // Verifies is the box chosen is valid
    TeBox b = themeDelimiter->layer()->box();
    TeProjection* projFrom = themeDelimiter->layer()->projection();
    TeProjection* projTo = theme->layer()->projection();
    if(!((*projFrom) == (*projTo))) // need remap?
        b = TeRemapBox(b, projFrom, projTo);

    TeBox& inputBox = theme->layer()->box();
    if(!TeIntersects(b, inputBox))
    {
        QMessageBox::information(this, tr("Information"), tr("The box chosen do not intercepts the input Theme. Please, try another."));
        ui->voronoiTabWidget->setCurrentPage(1);
        ui->boxComboBox->setFocus();
        return;
    }
    //Transforms to CGAL's box:
    Bbox_2 extent = TeBox2Bbox_2(b);
    
    //if a breakline layer was chosen, set the breakline flag to true
    bool useBreakLines=false;
    std::string breakLinesLayerName=ui->breakLinesComboBox->currentText().ascii();

    //choose visibility concept
	cmwv_ps::VisibilityConcept visConcept;
	if (ui->visibilityConceptComboBox->currentText()=="Paulo, 2012") {
		visConcept=cmwv_ps::DePaulo;
	} else { //default
		visConcept=cmwv_ps::Wang;
	}
    

    obstacleVector obstacles;
    if (breakLinesLayerName.compare("None") ) {
		TeLayer* breakLinesLayer;
		breakLinesLayer=getLayer(breakLinesLayerName);
		if (breakLinesLayer->geomRep() & TeLINES) {
			TeLineSet breakLines;
			useBreakLines=true;
			getLayer(breakLinesLayerName)->getLines(breakLines);
			//QMessageBox::information(this, tr("Information"), tr("Breaklines found"));
			LinesetToObstacles(breakLines,obstacles);
			
		} else if (breakLinesLayer->geomRep() & TePOLYGONS) {
			TePolygonSet breakLines;
			useBreakLines=true;
			getLayer(breakLinesLayerName)->getPolygons(breakLines);
			//QMessageBox::information(this, tr("Information"), tr("Breaklines found"));
			PolygonSetToObstacles(breakLines,obstacles);
		}
        QMessageBox::information(this, tr("Information"), ((Te2String(obstacles.size()))+" break lines found.").c_str());
    } 
    
    
    //preparing to read the layer
    bool loadAllAttributes=false;
    if (diagramType==MWVoronoi) loadAllAttributes = true;
    bool loadGeometries = true;
    TeQuerierParams querierParams(loadGeometries, loadAllAttributes);
    querierParams.setParams(theme->layer());
    TeQuerier  querier(querierParams);
    querier.loadInstances();
    //finding weighting attribute
    TeAttributeList attrList = querier.getAttrList();
    int weightAttrN=0;
    if (diagramType==MWVoronoi) {
        for (std::vector<TeAttribute>::iterator i=attrList.begin(); i!=attrList.end();++i) {
            if (ui->weightComboBox->currentText().toStdString()==i->rep_.name_) {
                break;
            }
            weightAttrN++;
        }    
    }
    int n=querier.numElemInstances();

    if(n==0)
    {
        QMessageBox::critical(this, tr("Error"), tr("Error getting the points of input Theme."));
        return;
    }


    // Converts x,y to a float array in order to pass to VoronoiDiagramGenerator class
    float* x = new float[n];
	float* y = new float[n];
    //pointers for weighted voronoi
    float* w;
    int numPoints;
    w= new float[n];
    numPoints = 0;
    TeSTInstance sti;
    string peso;
    //TePoint pointBefore(0,0);
    while(querier.fetchInstance(sti)) { // for each point
        // Stores on float array
        if(sti.hasPoints())
            {
                TePointSet pointSet;
                //reading geometry
                sti.getGeometry(pointSet);
                x[numPoints] = pointSet[0].location().x();
                y[numPoints] = pointSet[0].location().y();
    
                //reading weight
                if (diagramType==MWVoronoi) {
                    stringstream ss;
                    sti.getPropertyValue(peso,weightAttrN);
                    ss<<peso;
                    ss>>w[numPoints];
                }                    
                numPoints++;
            }
        }
    
    // Generates the Voronoi Diagram
    TeLineSet ls;
    //mwVoronoiDiagramGenerator *mwvdg;
    
    //TePolygonSet diagram;
    vector<TeMultiPolygon> diagram;
    if(TeProgress::instance())
    {
        QString caption = tr("Import");
        TeProgress::instance()->setCaption(caption.latin1());
    }
    //progress bar initialization
    TeWaitCursor wait;
    //ti = clock();
    if(TeProgress::instance())  {
        QString msg = tr("Creating diagram. Please, wait!");
        TeProgress::instance()->setCaption("CMWVoronoi");
        TeProgress::instance()->setMessage(msg.latin1());
    }

    TeProgress::instance()->setTotalSteps(numPoints);
    TeProgress::instance()->setProgress(2 );

    vector<TeMultiPolygon> visibility;
    if(this->diagramType==MWVoronoi) {
        
        siteVector pointSet;
        weightVector weights;

        MWVDiagram mwdiagram;
        pointSet.resize(numPoints);
        weights.resize(numPoints);
        for (int i=0;i<numPoints;i++ ) {
            pointSet[i]=Point_2(x[i],y[i]);
            weights[i]=w[i];
        }
        
        if (useBreakLines) {
            cmwv_ps DiagramGenerator;
            //transform terralib's lineset into CGAL's lineset
            DiagramGenerator.getDiagram(pointSet, weights,obstacles,extent,mwdiagram, visConcept,1 );
            MWDiagramAsTeMultiPolygons(DiagramGenerator._visibleAreas,visibility);
            //mwvdg->generateVoronoi(x, y, w, numPoints, b.x1_, b.x2_, b.y1_, b.y2_,breakLines );
        } else {
            mwv DiagramGenerator;
            DiagramGenerator.getDiagram(pointSet, weights,extent, mwdiagram);
            //mwvdg->generateVoronoi(x, y, w, numPoints, b.x1_, b.x2_, b.y1_, b.y2_);
        }
        
        //mwvdg->writeCSV("/home/mauricio/Projetos/mwvd/saida_terraview.csv");
        //diagram=*mwvdg->domList;
        //MWDiagramAsTePolygonSet(mwdiagram,diagram);

        MWDiagramAsTeMultiPolygons(mwdiagram,diagram);

    }
   
    
    
    delete [] x;
    delete [] y;
    delete [] w;


    if (diagramType!=MWVoronoi){
        if(ls.empty())
        {
            QMessageBox::information(this, tr("Information"), tr("No line of Voronoi Diagram crosses the box chosen. Please, try another."));
            ui->voronoiTabWidget->setCurrentPage(1);
            ui->boxComboBox->setFocus();
            wait.resetWaitCursor();
            return;
        }
    }


    TeLayer * diagramLayer=createLayer(ui->layerNameLineEdit->text().latin1(), db, theme->layer()->projection(), diagram);
    TeLayer * visLayer=createLayer((ui->layerNameLineEdit->text()+"vis").latin1(), db, theme->layer()->projection(), visibility);

    //if (diagramType==MWVoronoi) delete mwvdg;
    
    if(!diagramLayer)
    {
        wait.resetWaitCursor();
        return;
    }

    //if the layers are not equal, I should to clip the diagram
    if (theme->name()!=themeDelimiter->name()) {
        clipLayer(diagramLayer,themeDelimiter,db);
    } 

    wait.resetWaitCursor();
    if(TeProgress::instance())
        TeProgress::instance()->reset();
    if(this->diagramType!=Delaunay)
	    QMessageBox::information(this, tr("Information"), tr("The Voronoi Diagram was generated successfully!"));
    else
        QMessageBox::information(this, tr("Information"), tr("The Delaunay Diagram was generated successfully!"));

    close();

    plugin_params_->updateTVInterface();
}

void VoronoiWindow::showWindow()
{
	if(plugin_params_->getCurrentDatabasePtr() == 0)
	{
		QMessageBox::critical(this, tr("Error"), tr("Connect to a database first."));
		return;
	}

	ui->themeComboBox->clear();

	TeThemeMap& themeMap = plugin_params_->getCurrentDatabasePtr()->themeMap();
	TeThemeMap::iterator themeIt;
    std::string delimiter;
	for(themeIt = themeMap.begin(); themeIt != themeMap.end(); ++themeIt)
	{
		TeTheme* t = dynamic_cast<TeTheme*>(themeIt->second);
		if(t == 0)
			continue;
		if(t->layer()->geomRep() & TePOINTS)
        {
            if(delimiter.empty())
                delimiter = t->layer()->name();
			ui->themeComboBox->insertItem(QString(t->name().c_str()));
        }
	}

    if(ui->themeComboBox->count() == 0)
    {
        QMessageBox::information(this, tr("Information"), tr("No Theme with point representation found."));
		return;
    }
    

    //Extent layer
    TeLayerMap& layerMap = plugin_params_->getCurrentDatabasePtr()->layerMap();
	TeLayerMap::iterator layerIt;
	for(layerIt = layerMap.begin(); layerIt != layerMap.end(); ++layerIt)
        ui->boxComboBox->insertItem(QString(layerIt->second->name().c_str()));
    
    ui->boxComboBox->setCurrentText(delimiter.c_str());

    //break lines layer
    ui->breakLinesComboBox->clear();
    ui->breakLinesComboBox->insertItem("None");

	TeLayerMap& layerMap2 = plugin_params_->getCurrentDatabasePtr()->layerMap();
    for(layerIt = layerMap2.begin(); layerIt != layerMap2.end(); ++layerIt) {
        if ((layerIt->second->geomRep() & TeLINES) || (layerIt->second->geomRep() & TePOLYGONS) ) 
            ui->breakLinesComboBox->insertItem(QString(layerIt->second->name().c_str()));
	}
    //filling attributes' box.

    this->themeComboBox_activated(ui->themeComboBox->currentText());
	exec();
}

TeLayer* VoronoiWindow::createLayer(const std::string& name, TeDatabase* db, TeProjection* proj, const TeGeomRep& rep)
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

bool VoronoiWindow::createLayer(const std::string& name, TeDatabase* db, TeProjection* proj, TeLineSet& ls)
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
    
TeLayer * VoronoiWindow::createLayer(const std::string& name, TeDatabase* db, TeProjection* proj, TePolygonSet& ps)
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

TeLayer * VoronoiWindow::createLayer(const std::string& name, TeDatabase* db, TeProjection* proj, vector<TeMultiPolygon>& ps)
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


TeTheme* VoronoiWindow::getTheme(const std::string& name)
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

TeLayer* VoronoiWindow::getLayer(const std::string& name)
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

bool VoronoiWindow::isLayerNameValid(const std::string& name)
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

TeTheme * VoronoiWindow::createTheme(string layername, TeDatabase *db, TeView *view) {
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

bool VoronoiWindow::clipLayer(TeLayer *diagramLayer, TeTheme *themeDelimiter, TeDatabase *db) {
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

bool VoronoiWindow::copyAttributes(TeLayer *diagramLayer, TeTheme *pointsTheme, TeDatabase *db) {
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

bool VoronoiWindow::LinesetToObstacles(TeLineSet &ls, obstacleVector &obsVector) {
    obsVector.reserve(ls.size());
    for (TeLineSet::iterator lineIt=ls.begin();lineIt!=ls.end();++lineIt) {
        vector<Point_2> obs;
        for (TeLine2D::iterator coordIt=lineIt->begin(); coordIt!=lineIt->end();++coordIt) {
            obs.push_back(Point_2(coordIt->x(),coordIt->y()));
        }
        obsVector.push_back(obs);
    }
	return (obsVector.size()>0);
}

bool VoronoiWindow::PolygonSetToObstacles(TePolygonSet &ps, obstacleVector &obsVector) {
    //obsVector.reserve(ps.size());
	for (TePolygonSet::iterator polIt=ps.begin();polIt!=ps.end();++polIt) {
		for (TePolygon::iterator ringIt=polIt->begin();ringIt!=polIt->end();++ringIt) {
			vector<Point_2> obs;
			for (TeLinearRing::iterator coordIt=ringIt->begin(); coordIt!=ringIt->end();++coordIt) {
				obs.push_back(Point_2(coordIt->x(),coordIt->y()));
			}
			obsVector.push_back(obs);
		}

	}
	return (obsVector.size()>0);
}

Bbox_2 VoronoiWindow::TeBox2Bbox_2(TeBox boxTe) {
    return Bbox_2(boxTe.x1(),boxTe.y1(),boxTe.x2(),boxTe.y2());
}
