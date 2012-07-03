#ifndef mwv_H
#define mwv_H

#include <iostream>
#include <vector>

using namespace std;
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Gps_circle_segment_traits_2.h>
#include <CGAL/General_polygon_set_2.h>
#include <CGAL/Lazy_exact_nt.h>
#include <CGAL/Boolean_set_operations_2.h>

#include <list>
#include "mwv_base.h"


class mwv : public mwv_base
{
public:

    typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
    typedef Kernel::Point_2                                   Point_2;
    typedef Kernel::Circle_2                                  Circle_2;
    typedef CGAL::Gps_circle_segment_traits_2<Kernel>         Traits_2;

    typedef CGAL::General_polygon_set_2<Traits_2>             Polygon_set_2;
    typedef Traits_2::General_polygon_2                       Polygon_2;
    typedef Traits_2::General_polygon_with_holes_2            Polygon_with_holes_2;
    typedef Traits_2::Curve_2                                 Curve_2;
    typedef Traits_2::X_monotone_curve_2                      X_monotone_curve_2;
    typedef CGAL::Bbox_2                                      Bbox_2;
    typedef vector<Polygon_set_2>                             MWVDiagram;

    //METHODS
    mwv();
    void getDiagram(siteVector &sites, weightVector &weights, CGAL::Bbox_2 extent, MWVDiagram &dominanceAreas);
    bool getDiagram2(siteVector &sites, weightVector &weights, CGAL::Bbox_2 extent);

    //void ApoloniusCircle(Point_2 s1, NT w1, Point_2 s2, NT w2, Curve_2 &curve);
    Polygon_2 construct_polygon (const Circle_2& circle);
    Polygon_2 BoxAsPolygon(const Bbox_2 & box);
    //PROPERTIES


private:




    //int numberOfVertices (Circular_arc_2 &arc1, double tol);

};

#endif // mwv_H
