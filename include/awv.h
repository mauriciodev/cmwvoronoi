#ifndef awv_H
#define awv_H

#include <iostream>
#include <vector>
#include "mwv_base.h"
using namespace std;
#include <CGAL/Boolean_set_operations_2.h>
// the number type
#include <CGAL/MP_Float.h>


// example that uses an exact number type

//typedef CGAL::MP_Float AWNT;

// choose the kernel
//#include <CGAL/Simple_cartesian.h>

//typedef CGAL::Simple_cartesian<AWNT>  AWKernel;

// typedefs for the traits and the algorithm

#include <CGAL/Apollonius_graph_2.h>
#include <CGAL/Apollonius_graph_traits_2.h>

typedef CGAL::Apollonius_graph_traits_2<Kernel>   AWTraits;
//typedef AWTraits::Point_2                           AWPoint_2;
typedef CGAL::Apollonius_graph_2<AWTraits>          Apollonius_graph;


class awv{
public:
    awv();
    void addPoint(NT x, NT y, NT weight);
    void getDiagram(siteVector &v, weightVector &w);
    Apollonius_graph graph;


private:


};





#endif// awv_H
