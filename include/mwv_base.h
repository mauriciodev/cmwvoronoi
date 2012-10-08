#ifndef MWV_BASE_H
#define MWV_BASE_H

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef Kernel::FT                                    NT;
//typedef Kernel::
//basic geometries
typedef Kernel::Circle_2                              Circle_2;
typedef Kernel::Line_2                                Line_2;
typedef Kernel::Segment_2                             Segment_2;
typedef Kernel::Vector_2                              Vector_2;
typedef Kernel::Vector_3                              Vector_3;
typedef Kernel::Ray_2                                 Ray_2;
typedef Kernel::Iso_rectangle_2                       Rectangle_2;
typedef Kernel::Point_2                               Point_2;
typedef CGAL::Bbox_2                                  Bbox_2;


#include <CGAL/General_polygon_set_2.h>
#include <CGAL/Gps_circle_segment_traits_2.h>
typedef CGAL::Gps_circle_segment_traits_2<Kernel>                 GPS_Traits_2;
typedef CGAL::General_polygon_set_2<GPS_Traits_2>                 Polygon_set_2;
typedef GPS_Traits_2::General_polygon_2                           Polygon_2;
typedef GPS_Traits_2::General_polygon_with_holes_2                Polygon_with_holes_2;
typedef GPS_Traits_2::Curve_2                                     Curve_2;
typedef std::vector<Polygon_set_2>                                MWVDiagram;
typedef GPS_Traits_2::X_monotone_curve_2                          GPS_Segment_2;


//CMWV Definitions
using namespace std;
typedef std::vector<double> weightVector;
typedef std::vector<Point_2> siteVector;
typedef std::vector<Point_2> obstacle;
typedef std::vector<obstacle> obstacleVector;


class mwv_base
{
public:
    mwv_base();
    void ApoloniusCircle(Point_2 s1, double w1, Point_2 s2, double w2, Curve_2 &circleOutput);
    void TwoSitesDominance(Point_2 s1, double w1, Point_2 s2, double w2, Polygon_set_2 &output, Bbox_2 extent);
    void closePolygon(Point_2 site, Point_2 minVertex, Point_2 maxVertex, Bbox_2 extent, vector<Point_2> &boxVertexes);
    CGAL::Bbox_2 getBoundingBox(siteVector &sites);
    Point_2 intersectWithExtent(Point_2 p0, Point_2 p1, Bbox_2 extent);
    Polygon_2 construct_polygon (const Circle_2& circle);
    Polygon_2 BoxAsPolygon(const Bbox_2 & box);
	bool arcAsLinestring(GPS_Segment_2 curve, vector<double> &outX, vector<double> &outY, double tol=0.1);
    double angle(Point_2 p0, Point_2 p1, Point_2 p2);
	double measureAngle(Point_2 p1, Point_2 p0, Point_2 p2);
    double reducedAngle(double angle);
};

#endif // MWV_BASE_H
