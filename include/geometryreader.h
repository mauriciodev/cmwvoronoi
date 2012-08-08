#ifndef GEOMETRYREADER_H
#define GEOMETRYREADER_H

#include <string>
#include <vector>
using namespace std;
#include "cmwv_definitions.h"
#include "mwv.h"

#include "ogrsf_frmts.h"
#include "cpl_string.h"
#include "cpl_conv.h"




class GeometryReader
{
public:

    //Methods
    GeometryReader();
    void getRandomPoints(int nPoints, siteVector &v, weightVector &w);
    bool getPointsFromGDAL(std::string filename, std::string weightField, siteVector &v, weightVector & w, int sizeLimit=0);
    bool getPointsFromTerraLib(std::string theme);
    void getTestPoints(siteVector &v, weightVector &w);
    bool getObstaclesFromGDAL(std::string filename, obstacleVector &obstacles ,int sizeLimit=0);
    bool exportPointsToGDAL( vector<Point_2> &pointList,std::string filename);
    bool exportArrangementToGDAL(Arr &Arrangement, std::string filename);
    bool exportArrangementFacesToGDAL(Arr &arr, std::string filename, Bbox_2 extent);
    bool exportDiagramToGDAL(std::string filename);
    bool exportMWVDiagramToGDAL(mwv::MWVDiagram &diagram,std::string filename);
    bool arcAsLinestring(Arr::X_monotone_curve_2 curve, vector<double> &outX, vector<double> &outY, double tol=0.1);
    double measureAngle(Point_2 p1, Point_2 p0, Point_2 p2);
    bool isInside(Point_2 p, Bbox_2 box);
    Point_2 constrainInside(Point_2 p, Bbox_2 box);
};


#endif // GEOMETRYREADER_H
