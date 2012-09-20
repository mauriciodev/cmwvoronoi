#include "cmwv.h"
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
    t.start();
    /*mwv DiagramGenerator;
    //
    cout<< "Generating diagram."<<endl;
    mwv::MWVDiagram diagram;
    DiagramGenerator.getDiagram(pointSet, weights,DiagramGenerator.getBoundingBox(pointSet), diagram);
    pointReader.exportMWVDiagramToGDAL(diagram,"mwv");
    pointReader.exportPointsToGDAL(pointSet,"sites");
    */
    int limit=91;
    pointSet.resize(limit);
    weights.resize(limit);
    cmwv CDiagramGenerator;
    MWVDiagram outputDiagram;
    CDiagramGenerator.getDiagram2(pointSet,weights,obstacles,CDiagramGenerator.getBoundingBox(pointSet,obstacles),outputDiagram);

    cout<< "Time elapsed:" << t.elapsed()/1000.<<endl;
    //pointReader.exportMWVDiagramToGDAL(diagram,"mwv");
    return 0;
}

