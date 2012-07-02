// Voronoi
#include "VoronoiWindow.h"
#include "VoronoiDiagramGenerator.h"
#include "Utils.h"
#include "ui_UIVoronoi.h"

// TerraLib
#include <ui/qt4_help.h>
#include <TeDatabase.h>
#include <TeVectorRemap.h>
#include <TeWaitCursor.h>

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

VoronoiWindow::VoronoiWindow(PluginParameters* pp, const bool& delaunay) : 
UIVoronoi((QWidget*)pp->qtmain_widget_ptr_),
plugin_params_(pp),
delaunay_(delaunay)
{
	ui = (new Ui::UIVoronoi);
	ui->setupUi(this);

    if(delaunay_ == false)
        return;
    setCaption("Delaunay");
    ui->boxButtonGroup->hide();
}

VoronoiWindow::~VoronoiWindow()
{}

void VoronoiWindow::generateLinesCheckBox_clicked()
{
    ui->layerLinesButtonGroup->setEnabled(ui->generateLinesCheckBox->isChecked());
}

void VoronoiWindow::themeComboBox_activated(const QString& themeName)
{
    TeTheme* theme = getTheme(themeName.latin1());
    if(theme == 0)
        return;
    // Updates the combo box with delimiters...
    TeLayer* layer = theme->layer();
    ui->boxComboBox->setCurrentText(layer->name().c_str());
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

    std::string layerLinesName;
    if(ui->generateLinesCheckBox->isChecked())
    {
		layerLinesName = ui->layerLinesLineEdit->text().latin1();
        if(layerLinesName.empty())
	    {
		    QMessageBox::information(this, tr("Information"), tr("Please, define a name to Layer of Lines."));
            ui->voronoiTabWidget->setCurrentPage(1);
            ui->layerLinesLineEdit->setFocus();
		    return;
	    }
    }

    if(!isLayerNameValid(layerName))
    {
        ui->voronoiTabWidget->setCurrentPage(0);
        ui->layerNameLineEdit->setFocus();
        return;
    }

    if(!isLayerNameValid(layerLinesName))
    {
        ui->voronoiTabWidget->setCurrentPage(1);
        ui->layerLinesLineEdit->setFocus();
        return;
    }

    if(layerName == layerLinesName)
    {
        QMessageBox::information(this, tr("Information"), tr("Please, define names differents to Layer result and Layer of Lines."));
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
    
    TeLayer* layerDelimiter = getLayer(ui->boxComboBox->currentText().latin1());
    if(layerDelimiter == 0)
    {
        QMessageBox::critical(this, tr("Error"), tr("Error getting the delimiter Layer."));
		return;
    }

    // Verifies is the box chosen is valid
    TeBox b = layerDelimiter->box();
    TeProjection* projFrom = layerDelimiter->projection();
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

	TePointSet ps;
	theme->layer()->getPoints(ps);

    if(ps.empty())
    {
        QMessageBox::critical(this, tr("Error"), tr("Error getting the points of input Theme."));
        return;
    }

    TeWaitCursor wait;

    // Converts x,y to a float array in order to pass to VoronoiDiagramGenerator class
    float* x = new float[ps.size()];
	float* y = new float[ps.size()];
    sort(ps.begin(), ps.end(), XYOrderFunctor()); // need to compare equals points
    x[0] = ps[0].location().x_;
	y[0] = ps[0].location().y_;
    int numPoints = 1;
	for(unsigned int i = 1; i < ps.size(); ++i) // for each point
	{
        if(TeEquals(ps[i-1], ps[i])) // Do not consider equals
            continue;
        // Stores on float array
		x[numPoints] = ps[i].location().x_;
		y[numPoints] = ps[i].location().y_;
        numPoints++;
	}

    // Generates the Voronoi Diagram
	VoronoiDiagramGenerator vdg;
    if(delaunay_)
    {
        vdg.setGenerateDelaunay(delaunay_);
        vdg.setGenerateVoronoi(false);
    }
	vdg.generateVoronoi(x, y, numPoints, b.x1_, b.x2_, b.y1_, b.y2_, 0.0);
	
	delete [] x;
	delete [] y;

	TeLineSet ls;
	float x1, y1, x2, y2;
    vdg.resetVertexPairIterator();
    vdg.resetDelaunayEdgesIterator();
    if(delaunay_ == false)
    {
        while(vdg.getNextVertexPair(x1, y1, x2, y2))
        {
            if(x1 == x2 && y1 == y2)
                continue;

	        TeLine2D l;
	        l.add(TeCoord2D(x1, y1));
	        l.add(TeCoord2D(x2, y2));
	        ls.add(l);
        }
    }
    else
    {
        while(vdg.getNextDelaunay(x1, y1, x2, y2))
        {
            if(x1 == x2 && y1 == y2)
                continue;

	        TeLine2D l;
	        l.add(TeCoord2D(x1, y1));
	        l.add(TeCoord2D(x2, y2));
	        ls.add(l);
        }
    }

    if(ls.empty())
    {
        QMessageBox::information(this, tr("Information"), tr("No line of Voronoi Diagram crosses the box chosen. Please, try another."));
        ui->voronoiTabWidget->setCurrentPage(1);
        ui->boxComboBox->setFocus();
        wait.resetWaitCursor();
        return;
    }

    // Adding the lines of box in order to cut the diagram
    if(delaunay_ == false)
    {
        TePolygon pbox = TeMakePolygon(ls.box());
        TeLinearRing& ring = pbox[0];
        for(unsigned int i = 0; i < ring.size() - 1; ++i)
        {
            TeLine2D line;
            line.add(TeCoord2D(ring[i]));
            line.add(TeCoord2D(ring[i+1]));
            ls.add(line);
        }
    }

    TeLineSet fixedLines;
    TeBreakLines(ls, fixedLines); 

    if(ui->generateLinesCheckBox->isChecked())
		createLayer(layerLinesName, db, theme->layer()->projection(), fixedLines);

    // Adds identifiers to polygonizer control...
    for(unsigned int i = 0; i < fixedLines.size(); ++i)
    {
        std::string sid = Te2String(i);	
	    fixedLines[i].objectId(sid);
    }

    // Gets Polygons!
    TePolygonSet diagram;
    Polygonizer(fixedLines, diagram);
    if(!createLayer(layerName, db, theme->layer()->projection(), diagram))
    {
        wait.resetWaitCursor();
        return;
    }

    wait.resetWaitCursor();

    if(delaunay_ == false)
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

    TeLayerMap& layerMap = plugin_params_->getCurrentDatabasePtr()->layerMap();
	TeLayerMap::iterator layerIt;
	for(layerIt = layerMap.begin(); layerIt != layerMap.end(); ++layerIt)
        ui->boxComboBox->insertItem(QString(layerIt->second->name().c_str()));
    
    ui->boxComboBox->setCurrentText(delimiter.c_str());

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
    
bool VoronoiWindow::createLayer(const std::string& name, TeDatabase* db, TeProjection* proj, TePolygonSet& ps)
{
	TeLayer* layer = createLayer(name, db, proj, TePOLYGONS);
	if(layer == 0)
		return false;

	TeFeatureSet fs;
	TeTable& attrTable = layer->attrTables()[0];
	for(unsigned int i = 0; i < ps.size(); ++i)
	{
		std::string sid = Te2String(i);	
		ps[i].objectId(sid);
		TeTableRow row;
		row.push_back(sid);

		TeMultiPolygon geom(ps[i]);

		TeFeature feature(row, geom);
		fs.add(feature);
	}

	if(!layer->addFeatures(fs, false))
	{
		QMessageBox::critical(this, tr("Error"), tr("Error adding geometries to new layer."));
		db->deleteLayer(layer->id());
		return false;
	}
    
    return true;
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
