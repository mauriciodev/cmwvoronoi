#include "cmwv_ps.h"
#include "geometryreader.h"
#include "mwv.h"
#include <iostream>
using namespace std;

#include <qdatetime.h>

void testApolonius() {
    mwv test;
    Bbox_2 extent(-2,-2,2,2);

    Polygon_set_2 result;
    //test.ApoloniusCircle(Point_2(-1,0),2,Point_2(1,0),2,result, extent);
    test.TwoSitesDominance(Point_2(-1,0),2,Point_2(1,0),2,result, extent);
    //test.closePolygon(Point_2(0,0), Point_2(1,1), Point_2(-1,1),extent,result );
    //cout<<result<<endl;

}

void testPolygonClosing() {
    vector<Point_2> result;
    Bbox_2 extent(-2,-2,2,2);
    cmwv_ps test;
    test.closePolygon(Point_2(0,0), Point_2(1,1), Point_2(-1,1),extent,result );
    for (vector<Point_2>::iterator pIt=result.begin();pIt!=result.end();++pIt) {
        cout<<(*pIt)<<endl;
    }
}

int main(){
    //cmwv generator;
    testApolonius();
    //testPolygonClosing();
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
    CDiagramGenerator.getDiagram(pointSet,weights,obstacles,CDiagramGenerator.getBoundingBox(pointSet,obstacles),cDiagram, cmwv_ps::DePaulo);
    pointReader.exportMWVDiagramToGDAL(cDiagram,"cmwv");
    cout<< "Time elapsed:" << t.elapsed()/1000.<<endl;

    return 0;
}

