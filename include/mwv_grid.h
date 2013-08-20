#ifndef mwv_grid_H
#define mwv_grid_H

#include <iostream>
#include <vector>

using namespace std;

#include <list>
#include "mwv_base.h"
#include "grid.h"





class mwv_grid : public mwv_base
{
public:

    //METHODS
    mwv_grid();
    void getDiagram(siteVector &sites, weightVector &weights, CGAL::Bbox_2 extent, grid &outgrid  , int NumberOfThreads=0);

    //PROPERTIES


private:




    //int numberOfVertices (Circular_arc_2 &arc1, double tol);

};

#endif // mwv_H
