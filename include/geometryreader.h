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

#include <CGAL/Random.h>
#include <CGAL/point_generators_2.h>


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
    bool exportArrangementToGDAL(Arrangement_2 &Arr, std::string filename);
    bool exportArrangementFacesToGDAL(Arrangement_2 &Arr, std::string filename, Bbox_2 extent);
    bool exportDiagramToGDAL(std::string filename);
    bool exportMWVDiagramToGDAL(MWVDiagram &diagram,std::string filename);
    bool arcAsLinestring(Arrangement_2::X_monotone_curve_2 curve, vector<double> &outX, vector<double> &outY, double tol=0.1);
    double measureAngle(Point_2 p1, Point_2 p0, Point_2 p2);
    bool isInside(Point_2 p, Bbox_2 box);
    Point_2 constrainInside(Point_2 p, Bbox_2 box);
};


#endif // GEOMETRYREADER_H
