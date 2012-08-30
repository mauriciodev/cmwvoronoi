#include "cmwv.h"

cmwv::cmwv() {

}

bool cmwv::visibilityLines(Point_2 p0, obstacle &ob, vector<Segment_2> &visLines, Bbox_2 extent) {
    for(obstacle::iterator vertex=ob.begin(); vertex!=ob.end();++vertex) {
        //cout<<(*vertex)<<endl;
        Segment_2 visRay(p0,intersectWithExtent(p0,*vertex, extent));
        //cout<<visRay.source()<<" / "<<visRay.target()<<endl;
        visLines.push_back(visRay);
    }
    return true;
}

bool cmwv::visibilityLines_mauricio(Point_2 p0, obstacle &ob, vector<Segment_2> &visLines, Bbox_2 extent) {
    for(obstacle::iterator vertex=ob.begin(); vertex!=ob.end();++vertex) {
        //cout<<(*vertex)<<endl;
        Segment_2 visRay(p0,intersectWithExtent(p0,*vertex, extent));
        //cout<<visRay.source()<<" / "<<visRay.target()<<endl;
        visLines.push_back(visRay);
    }
    return true;
}



void cmwv::getDiagram(siteVector &sites, weightVector &weights, obstacleVector &obstacles, Bbox_2 extent) {
    //building arrangement of Boundaries
    Arrangement_2 arrangement;
    vector<Arrangement_2::Vertex_handle> siteVertex;
    //Landmarks_pl pl;
    //pl.attach(arrangement);
    cout<<setprecision(15)<<fixed;
    for (unsigned int i=0; i<sites.size();i++) {
        Arrangement_2::Vertex_handle siteV;
        arrangement.insert_in_face_interior(convertPoint2(sites[i]), arrangement.unbounded_face());
        siteVertex.push_back(siteV);
    }
    //adding obstacles
    for (unsigned int i=0;i<obstacles.size();i++) {//for each obstacle
        for (unsigned int j=0;j<obstacles[i].size()-1;j++) { //for each vertex besides the last

            Point_2 p1=obstacles[i][j],p2=obstacles[i][j+1];
            //cout<< p1 <<" / "<< p2<<endl;
            bool areNotEqual=(CGAL::compare(p1.x(), p2.x())!=CGAL::EQUAL) && (CGAL::compare(p1.y(), p2.y())!=CGAL::EQUAL);
            if (areNotEqual){
                Data_Curve_2 c= Data_Curve_2(Segment_2(obstacles[i][j],obstacles[i][j+1]),edgeData(edgeData::BreakLine));
                //cout<<Segment_2(obstacles[i][j],obstacles[i][j+1])<<endl;
                //cout<< obstacles[i][j].x()<<" "<<obstacles[i][j].y()<<" / "<<  obstacles[i][j+1].x()<<" "<<obstacles[i][j+1].y()<<endl;
                insert(arrangement,c);
            }
        }
    }
    //adding visibility lines
    for (unsigned int i=0;i<obstacles.size();i++) {//for each obstacle
        for (unsigned int j=0;j<sites.size();j++) {//for each generator point
            vector<Segment_2> visLines;
            visibilityLines(sites[j],obstacles[i],visLines,extent);
            for (unsigned int k=0;k<visLines.size();k++) {
                //cout<<visLines[k]<<endl;
                Data_Curve_2 c=Data_Curve_2(visLines[k],edgeData(edgeData::VisibilityLine, j, i, k));
                insert(arrangement, c);
            }
            //cout<<visLines[k]<<endl;
            //


        }
    }

    //adding apolonius circles
    for (unsigned int i=0;i<sites.size();i++) {
        cout<<i<<" de "<<sites.size()<< endl;

        for (unsigned int j=i+1; j<sites.size();j++) {

                Data_Curve_2 c;
                ApoloniusCircle(sites[i],weights[i],sites[j],weights[j],c);
                insert(arrangement,c);

        }
    }
    //process visibility lines
    //traverse searching for visibility segments
    //TODO:start with
    Arrangement_2::Halfedge_handle ecit;
    for (int i=0; i<sites.size(); i++) {

        Arrangement_2::Vertex_const_handle    v;
        Arrangement_2::Halfedge_const_handle  e;
        Arrangement_2::Face_const_handle      f;
        Naive_pl         naive_pl (arrangement);
        CGAL::Object locatedObj=naive_pl.locate(convertPoint2(sites[i]));
        if (CGAL::assign(v,locatedObj)) { //found the site
            //traverse its visibility lines
            Arrangement_2::Halfedge_around_vertex_const_circulator first, curr;
            first=curr=v->incident_halfedges();
            do {
                //cout<<curr->curve().data().generatorPointId<<endl;
                //Arrangement_2::Halfedge_const_handle he=curr->handle();
                ProcessVisibilityLine(curr, sites,obstacles);
            } while (++curr!=first);

        } else if (CGAL::assign(e,locatedObj)) { //found an edge
            cout << e->curve().data().generatorPointId<<endl;
        }else if (CGAL::assign(f,locatedObj)) { //found an edge
            cout << "face"<<endl;
            if(f->is_unbounded()) {
                std::cout << "hit nothing." << std::endl;
            } else {

            }
        }
    }
    /*
    for (ecit = arrangement.edges_begin(); ecit != arrangement.edges_end(); ++ecit) {
        if (ecit->curve().data().edgeType==edgeData::VisibilityLine) {
            ProcessVisibilityLine(ecit,sites,obstacles);


            //cout<<"vis "<<generatorId<<endl;
        }
    }*/

    //process obsctables

    //

    cout <<"Arrangement built."<<endl;
    cout <<"Vertices:" << arrangement.number_of_vertices()<<endl;
    cout <<"Edges:" << arrangement.number_of_edges()<<endl;
    //Arrangement_2::Halfedge_handle p0=siteVertex[0];;
    Arrangement_2::Vertex t;

    //cout<< siteVertex[0]->degree () <<endl;


    //Removing non voronoi edges

    Arrangement_2::Halfedge_handle eit;
    Arrangement_2 arr2;
    //int i=0;
    vector<Point_2> representativeList;

    /*for (eit=arrangement.edges_begin();eit!=arrangement.edges_end();++eit) {
        representativeList.push_back(representativePoint(eit));
        if (isMWVoronoiSegment(eit,arrangement,sites,weights)) {
            insert(arr2,eit->curve());
        }
        i++;
    }*/

    //cout <<"Edges:" << arr2.number_of_edges()<<endl;
    GeometryReader teste;
    teste.exportArrangementToGDAL(arrangement, "teste-arr");
    teste.exportArrangementFacesToGDAL(arrangement, "teste-arr-pol",extent);
    teste.exportPointsToGDAL(sites,"sites");

    //teste.exportArrangementToGDAL(arr2, "teste-mwv");
    //teste.exportArrangementFacesToGDAL(arr2, "teste-mwv-pol",extent);

    //teste.exportPointsToGDAL(representativeList, "representativePoints");

}




bool cmwv::isMWVoronoiSegment(Arrangement_2::Halfedge_handle eit, Arrangement_2 &arrangement, siteVector &sites, weightVector &weights, double tol) {
    vector<double> distances;
    distances.reserve(sites.size());
    double dx=0, dy=0,dtarget=0;
    for (unsigned int i=0;i<sites.size(); i++) {
        Point_2 m=representativePoint(eit);
        dx=CGAL::to_double(sites[i].x())-CGAL::to_double(m.x());
        dy=CGAL::to_double(sites[i].y())-CGAL::to_double(m.y());
        dtarget=sqrt(dx*dx+dy*dy)/CGAL::to_double(weights[i]);
        distances.push_back(dtarget);
    }
    sort(distances.begin(),distances.end());
    bool result=(fabs(distances[0]-distances[1])<tol);
    return result;
}


CGAL::Bbox_2 cmwv::getBoundingBox(siteVector &sites,obstacleVector &obstacles){
    siteVector pointSet=sites;
    for (obstacleVector::iterator iObs=obstacles.begin(); iObs!=obstacles.end();++iObs) {
        for(obstacle::iterator iVertex=iObs->begin(); iVertex!=iObs->end(); ++iVertex) {
            pointSet.push_back(*iVertex);
        }
    }

    return mwv_base::getBoundingBox(pointSet);
}

bool cmwv::isObstacleBefore(Arrangement_2::Halfedge_handle e, Point_2 obstaclePoint, Point_2 site) {
    Vector_2 v1(site,obstaclePoint);
    Vector_2 v2(site,vertexAsPoint_2(e->source()));
    Vector_2 v3(site,vertexAsPoint_2(e->target()));
    //checks if obstacle is before the visibility line
    return ( ( v1.squared_length() < v2.squared_length()) && (v1.squared_length() < v3.squared_length()) );


}

bool cmwv::ProcessVisibilityLine(Arrangement_2::Halfedge_around_vertex_const_circulator e, siteVector &sites,obstacleVector &obstacles, bool hasHitObstacleLeft, bool hasHitObstacleRight) {

    int generatorId=e->curve().data().generatorPointId;
    int obstacleId=e->curve().data().obstacleId;
    int obstacleId2=e->curve().data().obstacleSubId;

    //Point_2 obsPoint=obstacles[obstacleId][obstacleId2];
    //bool bIsObstacleBefore= isObstacleBefore(e,obstacles[obstacleId][obstacleId2],sites[generatorId]);
    //e.curve().data().isObstacleBefore=bIsObstacleBefore;

    Arrangement_2::Vertex v2;
    Arrangement_2::Vertex_const_handle v=e->source();
    Arrangement_2::Halfedge_around_vertex_const_circulator nextVisibilityLineEdge=e;
    Line_2 l(convertPoint(e->curve().source()),convertPoint(e->curve().target()));

    cout<<v->point()<<endl;
    Root_Point_2 p0=e->curve().source();
    Arrangement_2::Halfedge_around_vertex_const_circulator first, curr;
    first = curr = v->incident_halfedges();
    do {
        //every line that touches that vertex
        cout <<curr->curve().data().generatorPointId <<endl;
        if (e->curve().data().generatorPointId==curr->curve().data().generatorPointId) {


            if (p0.equals(curr->curve().source())) {
                    //this is the next edge of the same visibility line. save for later to continue processing
                    nextVisibilityLineEdge=curr;
                }

        } else {
                //every line besides the visibility line must be tested for visibility
                if (e->curve().data().edgeType==edgeData::BreakLine) {
                    //TODO check if any line has hit the vertex at the same point
                    //check if it is left or right
                    Point_2 obstaclePoint=convertPoint(curr->curve().target());
                    if (isObstacleLeft(l,obstaclePoint)) {
                        hasHitObstacleLeft=true;
                    } else {
                        hasHitObstacleRight=true;
                    }
                } else {

                }
                //curr->curve().data().maskedSites=e.curve().data().maskedSites;
        }
        /*// Note that the current halfedge is directed from u to v:
        Arrangement_2::Vertex_const_handle u = curr->source();
        std::cout << " (" << u->point() << ")";*/
    }  while (++curr != first);



    //after processing the lines here, continue with the next piece of this visibility line
    /*if (nextVisibilityLineEdge!=e) {
        ProcessVisibilityLine(nextVisibilityLineEdge, sites, obstacles, hasHitObstacleLeft,hasHitObstacleRight);
    }*/
    /*if ((e1->curve().data().edgeType!=edgeData::VisibilityLine) && (e2->curve().data().edgeType!=edgeData::VisibilityLine)) {
        e2->curve().data().maskedSites=e1->curve().data().maskedSites;
    }*/





    return true;

}


bool cmwv::isObstacleLeft(Line_2 l, Point_2 obsVertex) {
    return l.has_on_positive_side(obsVertex);
}
