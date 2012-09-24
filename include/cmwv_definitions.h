#ifndef CWVD_DEFINITIONS_H
#define CWVD_DEFINITIONS_H
#include<vector>
#include "mwv_base.h"

//#include <CGAL/Gmpq.h>

//typedef CGAL::Gmpq                                    Number_type;
//#include <CGAL/MP_Float.h>
//#include <CGAL/Quotient.h>
//#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#define myTol 0.001

//#include <CGAL/Cartesian.h>
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
//typedef CGAL::Quotient<CGAL::MP_Float>                      NT;
//typedef CGAL::Cartesian<NT>                                 Kernel;



//Arrangement definitions
typedef CGAL::Arr_circle_segment_traits_2<Kernel>     Segment_Traits_2;
typedef CGAL::Arr_curve_data_traits_2<Segment_Traits_2,edgeData> Data_Traits_2;
typedef Segment_Traits_2::CoordNT                     CoordNT;
typedef Segment_Traits_2::Point_2                     Root_Point_2;
typedef Segment_Traits_2::Curve_2                     Curve_2;
typedef Data_Traits_2::Curve_2                        Data_Curve_2;
typedef CGAL::Arrangement_2<Data_Traits_2>            Arrangement_2;
typedef CGAL::Arr_naive_point_location<Arrangement_2>           Naive_pl;




#endif // CWVD_DEFINITIONS_H
