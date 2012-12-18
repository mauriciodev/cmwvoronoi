#include "UIVoronoi.h"
CMWVDialog::CMWVDialog(QWidget *parent): QDialog(parent) {
    setupUi(this);
}

void CMWVDialog::okPushButton_clicked() {
    GeometryReader pointReader;
    siteVector pointSet;
    weightVector weights;
    obstacleVector obstacles;
    MWVDiagram Diagram;
    pointReader.getPointsFromGDAL(this->themeComboBox->currentText().toStdString(),this->weightComboBox->currentText().toStdString(),pointSet,weights);
    string breakLines=this->breakLinesComboBox->currentText().toStdString();
    QTime t;
    t.restart();
    cmwv_ps::VisibilityConcept vis;
    if (this->visibilityConceptComboBox->currentIndex()==1) {
        vis=cmwv_ps::DePaulo;
    } else {
        vis=cmwv_ps::Wang;
    }
    if (breakLines.size()>0) {
        pointReader.getObstaclesFromGDAL(breakLines,obstacles);
        cmwv_ps CDiagramGenerator;
        CDiagramGenerator.getDiagram(pointSet,weights,obstacles,CDiagramGenerator.getBoundingBox(pointSet,obstacles),Diagram, vis,1);

    } else {
        mwv DiagramGenerator;
        DiagramGenerator.getDiagram(pointSet,weights,DiagramGenerator.getBoundingBox(pointSet),Diagram,0);
    }
    pointReader.exportMWVDiagramToGDAL(Diagram,this->layerNameLineEdit->text().toStdString());




    //cout<< "Time elapsed:" << t.elapsed()/1000.<<endl;
}

void CMWVDialog::themeBrowseClick() {
    string fileName = QFileDialog::getOpenFileName(this,
        tr("Open ShapeFile"), ".", tr("Shapefiles (*.shp)")).toStdString();
    if(fileName!="") {
        this->themeComboBox->addItem(fileName.c_str());
        this->themeComboBox->setCurrentIndex(this->themeComboBox->count()-1);
        GeometryReader g;
        vector<string> weights=g.listWeightAttributes(fileName);
        for (int i=0;i<weights.size();i++) {
            this->weightComboBox->addItem(weights[i].c_str());
        }
    }
}
void CMWVDialog::breaklinesBrowseClick() {
    string fileName = QFileDialog::getOpenFileName(this,
        tr("Open ShapeFile"), ".", tr("Shapefiles (*.shp)")).toStdString();
    if(fileName!="") {
        this->breakLinesComboBox->addItem(fileName.c_str());
        this->breakLinesComboBox->setCurrentIndex(this->breakLinesComboBox->count()-1);
    }
}


void CMWVDialog::resultBrowseClick() {
    string fileName = QFileDialog::getSaveFileName(this,
        tr("Open ShapeFile"), ".", tr("Shapefiles (*.shp)")).toStdString();
    if(fileName!="") {
        this->layerNameLineEdit->setText(fileName.c_str());
    }
}
