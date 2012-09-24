#ifndef CMWV_PS_H
#define CMWV_PS_H

#include "mwv_base.h"
using namespace std;
#include "geometryreader.h"

class cmwv_ps : public mwv_base {
public:
    cmwv_ps();
    bool visibilityLines(Point_2 p0, obstacle &ob, vector<Segment_2> &visLines,Bbox_2 extent);
    bool visibilityLines_mauricio(Point_2 p0, obstacle &ob, vector<Segment_2> &visLines, Bbox_2 extent);
    CGAL::Bbox_2 getBoundingBox(siteVector &sites,obstacleVector &obstacles);
    void getDiagram2(siteVector &sites, weightVector &weights, obstacleVector obstacles, Bbox_2 extent, MWVDiagram &dominanceAreas);
private:
    void obstacleShadowsWang(Point_2 &site, obstacle &obstacle, Bbox_2 extent, Polygon_set_2 &shadows );

};

#endif // CMWV_PS_H
