#ifndef MWV_BASE_H
#define MWV_BASE_H
#include "cmwv_definitions.h"

class mwv_base
{
public:
    mwv_base();
    void ApoloniusCircle(Point_2 s1, double w1, Point_2 s2, double w2, Data_Curve_2 &curve);
    Point_2 representativePoint(Arrangement_2::Halfedge_handle heIt);
    CGAL::Bbox_2 getBoundingBox(siteVector &sites);
    Point_2 intersectWithExtent(Point_2 p0, Point_2 p1, Bbox_2 extent);

};

#endif // MWV_BASE_H
