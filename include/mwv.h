#ifndef mwv_H
#define mwv_H

#include <iostream>
#include <vector>

using namespace std;
#include <CGAL/Boolean_set_operations_2.h>

#include <list>
#include "mwv_base.h"


class mwv : public mwv_base
{
public:

    //METHODS
    mwv();
    void getDiagram(siteVector &sites, weightVector &weights, CGAL::Bbox_2 extent, MWVDiagram &dominanceAreas, int NumberOfThreads=0);
    bool getDiagram2(siteVector &sites, weightVector &weights, CGAL::Bbox_2 extent);
    bool oneDominance(int i, siteVector &sites, weightVector &weights, Bbox_2 extent);//
    void processSites(int startId, int endIt,siteVector s, weightVector w, Bbox_2 extent);
    //void ApoloniusCircle(Point_2 s1, NT w1, Point_2 s2, NT w2, Curve_2 &curve);

    //PROPERTIES


private:




    //int numberOfVertices (Circular_arc_2 &arc1, double tol);

};

#endif // mwv_H
