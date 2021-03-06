#include "cmwv_ps.h"
#include "geometryreader.h"
#include "mwv.h"
#include <iostream>
#include <QApplication>
#include "grid.h"
#include "UIVoronoi.h"
//#include "voronoiBaseWindow.h"
//#include "awv.h"
#include "mwv_grid.h"
using namespace std;

#include <qdatetime.h>

/*void testAWV(siteVector &pointSet, weightVector &weights) {
    awv test;
    test.getDiagram(pointSet,weights);
    GeometryReader gr;
    gr.exportAGToGDAL(test.graph,"awv.shp");
}*/

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

void testMWV(siteVector pointSet, weightVector weights) {
    GeometryReader pointReader;
    cout<< "Found " << pointSet.size() << " weighted points."<<endl;
    QTime t;

    mwv DiagramGenerator;
    cout<< "Generating Multiplicative Diagram."<<endl;
    MWVDiagram diagram;
    t.start();
    DiagramGenerator.getDiagram(pointSet, weights,DiagramGenerator.getBoundingBox(pointSet), diagram);
    //DiagramGenerator.getDiagram(pointSet, weights,DiagramGenerator.getBoundingBox(pointSet), diagram,1);
    cout<< "Time elapsed:" << t.elapsed()/1000.<<endl;
    //diagram.clear();
    t.restart();

    //cout<< "Time elapsed:" << t.elapsed()/1000.<<endl;
    pointReader.exportMWVDiagramToGDAL(diagram,"mwv");
    //pointReader.exportPointsToGDAL(pointSet,"sites");

}
void testCMWV(siteVector &pointSet, weightVector &weights, obstacleVector &obstacles) {
    GeometryReader pointReader;
    QTime t;
    t.restart();
    cout<< "Generating Constrained Multiplicative Diagram."<<endl;
    cmwv_ps CDiagramGenerator;
    MWVDiagram cDiagram;
    CDiagramGenerator.getDiagram(pointSet,weights,obstacles,CDiagramGenerator.getBoundingBox(pointSet,obstacles),cDiagram, cmwv_ps::DePaulo);
    pointReader.exportMWVDiagramToGDAL(cDiagram,"cmwv-depaulo");
    cout<< "Time elapsed:" << t.elapsed()/1000.<<endl;
    GeometryReader teste;
    teste.exportMWVDiagramToGDAL(CDiagramGenerator._visibleAreas,"visibility-depaulo");
//x    CDiagramGenerator.getDiagram(pointSet,weights,obstacles,CDiagramGenerator.getBoundingBox(pointSet,obstacles),cDiagram, cmwv_ps::Wang,1);
//    pointReader.exportMWVDiagramToGDAL(cDiagram,"cmwv-wang");
//    teste.exportMWVDiagramToGDAL(CDiagramGenerator._visibleAreas,"visibility-wang");
    //GeometryReader teste;
    //teste.exportArrangementToGDAL(arrangement, "teste-arr");
    //teste.exportMWVDiagramToGDAL(dominanceAreas,"cmwv");
    //teste.exportPointsToGDAL(sites,"sites");
}




int main(int argc, char *argv[]){

    //cmwv generator;
    //cout << -11 % 10<<endl;
    grid::test();


    testApolonius();
    //testPolygonClosing();
    GeometryReader pointReader;
    Bbox_2 box;
    pointReader.getExtent("/home/mauricio/Desktop/INPE/dissertacao/dados/pontos_vendas_pt.shp",box);
    siteVector pointSet;
    weightVector weights;
    obstacleVector obstacles;

    pointReader.getPointsFromGDAL("/home/mauricio/dissertacao/dados/pv_subset2.shp","DDD_ANO",pointSet,weights);
    mwv_grid testMGRID;
    grid g(100,100);
    //testMGRID.getDiagram(pointSet,weights,testMGRID.getBoundingBox(pointSet),g);


    //pointReader.getRandomPoints(npoints,pointSet, weights);
    //pointReader.getPointsFromGDAL("C:/dados/INTCIR10.shp","frequencia",pointSet,weights);
    //string c="/media/7C340CAA340C6A0A/";
    //pointReader.getPointsFromGDAL("/home/mauricio/dissertacao/dados/point_sample.shp","frequencia",pointSet,weights);
    pointReader.getPointsFromGDAL("/home/mauricio/dissertacao/dados/INTCIR10.shp","frequencia",pointSet,weights);
    //pointReader.getPointsFromGDAL("/home/mauricio/Desktop/INPE/dissertacao/dados/point_sample.shp","frequencia",pointSet,weights);
    //pointReader.getObstaclesFromGDAL("/home/mauricio/Desktop/INPE/dissertacao/dados/breaklines_lin.shp",obstacles);
    pointReader.getObstaclesFromGDAL("/home/mauricio/dissertacao/dados/breaklines_lin.shp",obstacles);
    //testAWV(pointSet,weights);

    /*testMWV(pointSet,weights);
    testMWV(pointSet,weights);
    testMWV(pointSet,weights);
    testMWV(pointSet,weights);
    testMWV(pointSet,weights);
    testMWV(pointSet,weights);
    testMWV(pointSet,weights);
    testMWV(pointSet,weights);
    testMWV(pointSet,weights);*/
    mwv teste;


    pointReader.getPointsFromGDAL("/home/mauricio/Projetos/MWVoronoi/samples/ConsultaUrg09.shp","QTD_APRESE",pointSet,weights);
    //pointReader.getObstaclesFromGDAL("/home/mauricio/Projetos/MWVoronoi/samples/obstacles.shp",obstacles);

    //pointReader.getTestPoints(pointSet,weights);




    //pointReader.getPointsFromGDAL("/home/mauricio/Desktop/INPE/dissertacao/exemplo/pontos2.shp","frequencia",pointSet,weights);
    //pointReader.getObstaclesFromGDAL("/home/mauricio/Desktop/INPE/dissertacao/exemplo/barreirasL.shp",obstacles);
    //testMWV(pointSet,weights);
    testCMWV(pointSet,weights,obstacles);

    //return startUI(argc, argv);


    //int limit=91;
    //pointSet.resize(limit)
    //weights.resize(limit);


    return 0;
}

