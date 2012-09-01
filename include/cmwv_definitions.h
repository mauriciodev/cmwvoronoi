#ifndef CWVD_DEFINITIONS_H
#define CWVD_DEFINITIONS_H
#include<vector>

//#include <CGAL/Gmpq.h>

//typedef CGAL::Gmpq                                    Number_type;
#include <CGAL/MP_Float.h>
#include <CGAL/Quotient.h>
//#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#define myTol 0.001

#include <CGAL/Cartesian.h>
#include <CGAL/Arr_circle_segment_traits_2.h>
#include <CGAL/Arrangement_2.h>
#include <CGAL/Arrangement_with_history_2.h>
#include <CGAL/Arr_curve_data_traits_2.h>
#include <CGAL/Arr_naive_point_location.h>
#include <vector>


class edgeData {
public:
    enum EdgeType {VisibilityLine, DominanceArc, BreakLine};
    EdgeType edgeType;
    int generatorPointId,obstacleId,obstacleSubId;
    std::vector<bool> maskedSites;
    edgeData() {};
    edgeData(EdgeType t, int nSites=0, int generatorId=-1, int obstacleId=-1,int obstacleSubId=-1) {
        this->edgeType=t;
        this->generatorPointId=generatorId;
        this->obstacleId=obstacleId;
        this->obstacleSubId=obstacleSubId;
        this->maskedSites.resize(nSites);
    }

};

//
//typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef CGAL::Quotient<CGAL::MP_Float>                      NT;
typedef CGAL::Cartesian<NT>                                 Kernel;

//basic geometries
typedef Kernel::Circle_2                              Circle_2;
typedef Kernel::Line_2                                Line_2;
typedef Kernel::Segment_2                             Segment_2;
typedef Kernel::Vector_2                              Vector_2;
typedef Kernel::Ray_2                                 Ray_2;
typedef Kernel::Iso_rectangle_2                       Rectangle_2;
typedef Kernel::Point_2                               Point_2;
typedef CGAL::Bbox_2                                  Bbox_2;

//Arrangement definitions
typedef CGAL::Arr_circle_segment_traits_2<Kernel>     Segment_Traits_2;
typedef CGAL::Arr_curve_data_traits_2<Segment_Traits_2,edgeData> Data_Traits_2;
typedef Segment_Traits_2::CoordNT                     CoordNT;
typedef Segment_Traits_2::Point_2                     Root_Point_2;
typedef Segment_Traits_2::Curve_2                     Curve_2;
typedef Data_Traits_2::Curve_2                        Data_Curve_2;
typedef CGAL::Arrangement_2<Data_Traits_2>            Arrangement_2;
typedef CGAL::Arr_naive_point_location<Arrangement_2>           Naive_pl;




//CMWV Definitions
typedef std::vector<double> weightVector;
typedef std::vector<Point_2> siteVector;
typedef std::vector<Point_2> obstacle;
typedef std::vector<obstacle> obstacleVector;


#endif // CWVD_DEFINITIONS_H
