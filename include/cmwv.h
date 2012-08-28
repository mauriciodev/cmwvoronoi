#ifndef cmwv_H
#define cmwv_H

#include <iostream>
#include "mwv.h"
using namespace std;




#include "cmwv_definitions.h"
#include "geometryreader.h"
#include "mwv_base.h"



class cmwv: public mwv_base {
public:

    //METHODS
    cmwv();
    void getVisibilityLines();
    void getVisibilityLinesDePaulo();
    void getArrangement();
    void getArrangement2();
    void getDiagram(siteVector &sites, weightVector &weights, obstacleVector &obstacles, CGAL::Bbox_2 extent);
    //void ApoloniusCircle(Point_2 p1, Point_2 p2, Circle_2 &circle);
    bool visibilityLines(Point_2 p0, obstacle &ob, vector<Segment_2> &visLines,Bbox_2 extent);
    bool visibilityLines_mauricio(Point_2 p0, obstacle &ob, vector<Segment_2> &visLines, Bbox_2 extent);
    CGAL::Bbox_2 getBoundingBox(siteVector &sites,obstacleVector &obstacles);
private:
    bool ProcessVisibilityLine(Arrangement_2::Halfedge &e, siteVector &sites,obstacleVector &obstacles, bool hasHitObstacleLeft=false, bool hasHitObstacleRight=false);


private:
    bool isMWVoronoiSegment(Arrangement_2::Halfedge_handle eit, Arrangement_2 &arrangement, siteVector & sites,weightVector &weights, double tol=0.001);
    bool isObstacleBefore(Arrangement_2::Halfedge_handle e, Point_2 obstaclePoint, Point_2 site);
    bool isObstacleLeft(Line_2 l, Point_2 obs);
    Point_2 vertexAsPoint_2(Arrangement_2::Vertex_handle p) {
        return Point_2(CGAL::to_double(p->point().x()),CGAL::to_double(p->point().y()));
    }
    Point_2 convertPoint(Arrangement_2::Point_2 p){
        return(Point_2(CGAL::to_double(p.x()),CGAL::to_double(p.y())));
    }
    Arrangement_2::Point_2 convertPoint2(Point_2 p){
        //const CGAL::_One_root_point_2<CGAL::Lazy_exact_nt<CGAL::Gmpq>,true> p
        return(Arrangement_2::Point_2(CGAL::to_double(p.x()),CGAL::to_double(p.y())));
    }
    //int numberOfVertices (Circular_arc_2 &arc1, double tol);

};

#endif // cmwv_H
