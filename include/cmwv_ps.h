#ifndef CMWV_PS_H
#define CMWV_PS_H

#include "mwv_base.h"
using namespace std;
#include "geometryreader.h"

class cmwv_ps : public mwv_base {
public:
    cmwv_ps();
    enum VisibilityConcept {Wang, DePaulo};
    CGAL::Bbox_2 getBoundingBox(siteVector &sites,obstacleVector &obstacles);
    void getDiagram(siteVector &sites, weightVector &weights, obstacleVector obstacles, Bbox_2 extent, MWVDiagram &dominanceAreas, VisibilityConcept concept);

private:
    void obstacleShadowsWang(Point_2 &site, obstacle &obstacle, Bbox_2 extent, Polygon_set_2 &shadows );
    void obstacleShadowsMauricio(Point_2 &site, obstacle &obstacle, Bbox_2 extent, Polygon_set_2 &shadows );

};

#endif // CMWV_PS_H
