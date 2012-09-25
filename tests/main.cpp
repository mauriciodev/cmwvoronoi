#include "cmwv_ps.h"
#include "geometryreader.h"
#include "mwv.h"
#include <iostream>
using namespace std;

#include <qdatetime.h>


int main(){
    //cmwv generator;
    GeometryReader pointReader;
    siteVector pointSet;
    weightVector weights;
    obstacleVector obstacles;

    //pointReader.getRandomPoints(npoints,pointSet, weights);
    pointReader.getPointsFromGDAL("/home/mauricio/Projetos/MWVoronoi/samples/ConsultaUrg09.shp","QTD_APRESE",pointSet,weights);
    pointReader.getObstaclesFromGDAL("/home/mauricio/Projetos/MWVoronoi/samples/obstacles.shp",obstacles);
    //pointReader.getTestPoints(pointSet,weights);
    cout<< "Found " << pointSet.size() << " weighted points."<<endl;
    QTime t;

    /*t.start();
    mwv DiagramGenerator;
    cout<< "Generating Multiplicative Diagram."<<endl;
    MWVDiagram diagram;
    DiagramGenerator.getDiagram(pointSet, weights,DiagramGenerator.getBoundingBox(pointSet), diagram);
    pointReader.exportMWVDiagramToGDAL(diagram,"mwv");
    pointReader.exportPointsToGDAL(pointSet,"sites");
    cout<< "Time elapsed:" << t.elapsed()/1000.<<endl;*/

    //int limit=91;
    //pointSet.resize(limit)
    //weights.resize(limit);
    t.restart();
    cout<< "Generating Constrained Multiplicative Diagram."<<endl;
    cmwv_ps CDiagramGenerator;
    MWVDiagram cDiagram;
    CDiagramGenerator.getDiagram(pointSet,weights,obstacles,CDiagramGenerator.getBoundingBox(pointSet,obstacles),cDiagram);
    pointReader.exportMWVDiagramToGDAL(cDiagram,"cmwv");
    cout<< "Time elapsed:" << t.elapsed()/1000.<<endl;

    return 0;
}

