#ifndef MWV_BASE_H
#define MWV_BASE_H
#include "cmwv_definitions.h"

class mwv_base
{
public:
    mwv_base();
    
    Point_2 representativePoint(Arr::Halfedge_handle heIt);
    void ApoloniusCircle(Point_2 s1, double w1, Point_2 s2, double w2, Curve_2 &curve);
    
    CGAL::Bbox_2 getBoundingBox(siteVector &sites);
};

#endif // MWV_BASE_H
