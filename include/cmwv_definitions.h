#ifndef CWVD_DEFINITIONS_H
#define CWVD_DEFINITIONS_H
#include<vector>

//#include <CGAL/Gmpq.h>

//typedef CGAL::Gmpq                                    Number_type;

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Random.h>
#include <CGAL/point_generators_2.h>

#include <CGAL/Cartesian.h>
#include <CGAL/Arr_circle_segment_traits_2.h>
//#include <CGAL/Arrangement_2.h>
#include <CGAL/Arrangement_with_history_2.h>

//typedef CGAL::Cartesian<Number_type>                  Kernel;
typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef Kernel::Circle_2                              Circle_2;
typedef Kernel::Line_2                                Line_2;
typedef Kernel::Segment_2                             Segment_2;
typedef CGAL::Arr_circle_segment_traits_2<Kernel>     Traits_2;
typedef Traits_2::CoordNT                             CoordNT;
typedef Kernel::Point_2                             Point_2;
typedef Traits_2::Curve_2                             Curve_2;
typedef CGAL::Arrangement_with_history_2<Traits_2>    Arrangement_2;
typedef Kernel::Vector_2                              Vector_2;
typedef CGAL::Bbox_2                                  Bbox_2;
typedef Kernel::Ray_2                                 Ray_2;
typedef Kernel::Iso_rectangle_2                       Rectangle_2;
#define Arr Arrangement_2
//#define NT Number_type




//CMWV Definitions
typedef std::vector<double> weightVector;
typedef std::vector<Point_2> siteVector;
typedef std::vector<Point_2> obstacle;
typedef std::vector<obstacle> obstacleVector;


#endif // CWVD_DEFINITIONS_H
