#include "SimplifyWindow.h" 

SimplifyWindow::SimplifyWindow(PluginParameters* pp) :  voronoiBaseWindow(pp), plugin_params_(pp) {
    setupUi(this);
}

void SimplifyWindow::pushButton_clicked() {
    std::string inputThemeName=this->themeComboBox->currentText().toStdString();
    double tol=this->lineEdit->text().toDouble();
    std::string outThemeName=this->lineEdit_2->text().toStdString();


    TeDatabase* db = plugin_params_->getCurrentDatabasePtr();

    TeLayer* inputLayer=getLayer(inputThemeName);
    TeProjection* proj = inputLayer->projection();



    if (inputLayer->geomRep() & TeLINES) {
        TeLineSet lines,outputLines;
        inputLayer->getLines(lines);
        for (TeLineSet::iterator lineIt=lines.begin();lineIt!=lines.end();++lineIt) {
            TeLine2D newline;
            newline.copyElements(*lineIt);

            TeLineSimplify(newline,tol,TeLength(newline));
        }
        createLayer(outThemeName,db,proj,outputLines);
    }
    if (inputLayer->geomRep() & TePOLYGONS) {
        TePolygonSet pols,outputPolygons;
        inputLayer->getPolygons(pols);
        for (TePolygonSet::iterator polIt=pols.begin();polIt!=pols.end();++polIt) {
            TePolygon newpol;
            for (TePolygon::iterator ringIt=polIt->begin();ringIt!=polIt->end();++ringIt) {
                TeLinearRing newring;
                newring.copyElements(*ringIt);
                TeLineSimplify(newring,tol,TeLength(newring));
                newpol.add(newring);
            }
            outputPolygons.add(newpol);
        }
        createLayer(outThemeName,db,proj,outputPolygons);
    }
    QMessageBox::information(this, tr("Information"), tr("Simplification process finished!"));
    plugin_params_->updateTVInterface();
}

void SimplifyWindow::loadThemes() {

}

void SimplifyWindow::showWindow() {
    if(plugin_params_->getCurrentDatabasePtr() == 0)
    {
        QMessageBox::critical(this, tr("Error"), tr("Connect to a database first."));
        return;
    }

    this->themeComboBox->clear();
    TeLayerMap& layerMap = plugin_params_->getCurrentDatabasePtr()->layerMap();
    TeLayerMap::iterator layerIt;

    TeLayerMap& layerMap2 = plugin_params_->getCurrentDatabasePtr()->layerMap();
    for(layerIt = layerMap2.begin(); layerIt != layerMap2.end(); ++layerIt) {
        if ((layerIt->second->geomRep() & TeLINES) || (layerIt->second->geomRep() & TePOLYGONS) )
            this->themeComboBox->insertItem(QString(layerIt->second->name().c_str()));
    }



    this->exec();
}

