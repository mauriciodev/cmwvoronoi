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
                Data_Curve_2 c= Data_Curve_2(Segment_2(obstacles[i][j],obstacles[i][j+1]),edgeData(edgeData::BreakLine,sites.size()));
                //cout<<Segment_2(obstacles[i][j],obstacles[i][j+1])<<endl;
                //cout<< obstacles[i][j].x()<<" "<<obstacles[i][j].y()<<" / "<<  obstacles[i][j+1].x()<<" "<<obstacles[i][j+1].y()<<endl;
                insert(arrangement,c);
            }
        }
    }
    cout<<"Adding visibility lines"<<endl;
    for (unsigned int i=0;i<sites.size();i++) {//for each generator point
        for (unsigned int j=0;j<obstacles.size();j++) {//for each obstacle
            vector<Segment_2> visLines;
            visibilityLines(sites[i],obstacles[j],visLines,extent);
            for (unsigned int k=0;k<visLines.size();k++) {
                //cout<<visLines[k]<<endl;
                Data_Curve_2 c=Data_Curve_2(visLines[k],edgeData(edgeData::VisibilityLine, sites.size(), i, j, k));
                insert(arrangement, c);
            }
        }
    }


    //process visibility lines
    vector<Arrangement_2::Vertex_handle> vGenerators;
    findGenerators(arrangement,vGenerators,sites);
    for (unsigned int i=0; i<vGenerators.size();i++) {
        //traverse its visibility lines
        Arrangement_2::Halfedge_around_vertex_circulator  first, curr;
        first=curr=vGenerators[i]->incident_halfedges();

        do {
            //cout<<curr->curve().data().generatorPointId<<endl;
            //Arrangement_2::Halfedge_const_handle he=curr->handle();
            curr->curve().data().maskedSites[i]=false;
            ProcessVisibilityLine(curr, sites,obstacles);
        } while (++curr!=first);
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


    //adding apolonius circles
    for (unsigned int i=0;i<sites.size();i++) {
        cout<<i<<" de "<<sites.size()<< endl;

        for (unsigned int j=i+1; j<sites.size();j++) {

                Data_Curve_2 c;
                ApoloniusCircle(sites[i],weights[i],sites[j],weights[j],c);
                insert(arrangement,c);

        }
    }

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
    //teste.exportArrangementToGDAL(arrangement, "teste-arr");
    //teste.exportArrangementFacesToGDAL(arrangement, "teste-arr-pol",extent);
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

bool cmwv::ProcessVisibilityLine(Arrangement_2::Halfedge_around_vertex_circulator e, siteVector &sites,obstacleVector &obstacles, bool hasHitObstacleLeft, bool hasHitObstacleRight) {

    int generatorId=e->curve().data().generatorPointId;
    //int obstacleId=e->curve().data().obstacleId;
    //int obstacleId2=e->curve().data().obstacleSubId;
    bool hasHitObstacleLeftHere=hasHitObstacleLeft, hasHitObstacleRightHere=hasHitObstacleRight;
    //Point_2 obsPoint=obstacles[obstacleId][obstacleId2];
    //bool bIsObstacleBefore= isObstacleBefore(e,obstacles[obstacleId][obstacleId2],sites[generatorId]);
    //e.curve().data().isObstacleBefore=bIsObstacleBefore;

    Arrangement_2::Vertex v2;
    Arrangement_2::Vertex_handle v=e->source();
    Arrangement_2::Halfedge_around_vertex_circulator nextVisibilityLineEdge=e;
    Line_2 l(convertPoint(e->curve().source()),convertPoint(e->curve().target()));

    //cout<<v->point()<<endl;
    Root_Point_2 p0=e->curve().source();
    Arrangement_2::Halfedge_around_vertex_circulator first, curr;
    first = curr = v->incident_halfedges();
    do {
        //Vector_2 v(convertPoint(curr->curve().source()),convertPoint(curr->curve().target()));
        //cout<<l.to_vector()<< " /" << v<<endl;
        //NT angle= CGAL::angle(l.to_vector(),v);
        //cout <<angle<<endl;
        //every line that touches that vertex
        //cout <<curr->curve().data().generatorPointId <<endl;
        if (e->curve().data().generatorPointId==curr->curve().data().generatorPointId) {

            if (!p0.equals(curr->curve().source())) {
                    //this is the next edge of the same visibility line. save for later to continue processing
                    nextVisibilityLineEdge=curr;
                }

        } else {
            //every line besides the visibility line must be tested for visibility
            Point_2 edgePoint=convertPoint(curr->curve().target());
            if (curr->curve().data().edgeType==edgeData::BreakLine) {
                //TODO check if any line has hit the vertex at the same point
                //check if it is left or right
                if (isObstacleLeft(l,edgePoint)) {
                    hasHitObstacleLeftHere=true;
                } else {
                    hasHitObstacleRightHere=true;
                }
            }
            //check if obsctacles that were already hit are up or down
            if (isObstacleLeft(l,edgePoint) && (hasHitObstacleLeft==true)) { //obstacle up
                //edgeData data;
                //data.maskedSites[generatorId]=true;
                curr->curve().data().maskedSites[generatorId]=true;
            } else if ( (!isObstacleLeft(l,edgePoint)) && (hasHitObstacleRight==true)) { //obstacle up
                curr->curve().data().maskedSites[generatorId]=true;
            } else { //obstacle down
                curr->curve().data().maskedSites[generatorId]=false;
            }

            //curr->curve().data().maskedSites=e.curve().data().maskedSites;
        }
        /*// Note that the current halfedge is directed from u to v:
        Arrangement_2::Vertex_const_handle u = curr->source();
        std::cout << " (" << u->point() << ")";*/
    }  while (++curr != first);



    //after processing the lines here, continue with the next piece of this visibility line
    if (nextVisibilityLineEdge!=e) {
        if (hasHitObstacleLeftHere || hasHitObstacleRightHere) nextVisibilityLineEdge->curve().data().maskedSites[generatorId]=true;
        ProcessVisibilityLine(nextVisibilityLineEdge, sites, obstacles, hasHitObstacleLeftHere,hasHitObstacleRightHere);
    }
    /*if ((e1->curve().data().edgeType!=edgeData::VisibilityLine) && (e2->curve().data().edgeType!=edgeData::VisibilityLine)) {
        e2->curve().data().maskedSites=e1->curve().data().maskedSites;
    }*/





    return true;

}
void cmwv::findGenerators(Arrangement_2 &arr, vector<Arrangement_2::Vertex_handle> &vGenerators, siteVector &sites) {
    for (unsigned int i=0; i<sites.size(); i++) { //start at the generators

        Arrangement_2::Vertex_const_handle    v;
        Arrangement_2::Halfedge_const_handle  e;
        Arrangement_2::Face_const_handle      f;
        Naive_pl         naive_pl (arr);
        CGAL::Object locatedObj=naive_pl.locate(convertPoint2(sites[i]));
        //locate the generator
        if (CGAL::assign(v,locatedObj)) { //found the site
            Arrangement_2::Vertex_handle v1=arr.non_const_handle(v);
            vGenerators.push_back(v1);
        } else if (CGAL::assign(e,locatedObj)) { //found an edge
            //cout << e->curve().data().generatorPointId<<endl;
        }else if (CGAL::assign(f,locatedObj)) { //found an edge
            //cout << "face"<<endl;
            if(f->is_unbounded()) {
                std::cout << "hit nothing." << std::endl;
            } else {

            }
        }
    }
}



bool cmwv::isObstacleLeft(Line_2 l, Point_2 obsVertex) {
    return l.has_on_positive_side(obsVertex);
}


void cmwv::getDiagram2(siteVector &sites, weightVector &weights, obstacleVector obstacles, Bbox_2 extent, MWVDiagram &dominanceAreas) {
    Polygon_2 wholeArea;
    wholeArea=BoxAsPolygon(extent);
    //create visibility areas
    vector<Polygon_set_2> visibleAreas;
    visibleAreas.resize(sites.size());
    for (unsigned int i=0;i<sites.size();i++) { //for each site
        Polygon_set_2 invisibleArea;
        for (unsigned int j=0;j<obstacles.size();j++) { //for each obstacle
            Polygon_set_2 objectShadow;
            obstacleShadowsWang(sites[i],obstacles[j],extent,objectShadow);
            invisibleArea.join(objectShadow);
        }
        visibleAreas[i].join(wholeArea);
        visibleAreas[i].difference(invisibleArea);
    }
    GeometryReader teste;
    teste.exportMWVDiagramToGDAL(visibleAreas,"visibility");

    for (unsigned int i=0;i<sites.size();i++) {
        Polygon_set_2 S;
        cout<<i+1<<" de "<<sites.size()<< endl;
        S.join(wholeArea);
        for (unsigned int j=0; j<sites.size();j++) {
            if (i!=j) {
                //cout<<S.number_of_polygons_with_holes()<<endl;
                Data_Curve_2 c;
                ApoloniusCircle(sites[i],weights[i],sites[j],weights[j],c);
                Polygon_set_2 dominance,jInvisibleArea, constrainedDominance;
                dominance.clear();
                if (c.is_circular()) {
                    Polygon_2 polC=construct_polygon(c.supporting_circle());
                    if (weights[i]<weights[j]) {
                        dominance.insert(polC);
                    } else {
                        dominance.insert(wholeArea);
                        dominance.difference(polC);
                    }
                } else { //line case
                    dominance.insert(wholeArea);
                    Polygon_2     line; // line is a polygon defined by 2 points
                    Point_2 p1,p2;
                    p1=Point_2(CGAL::to_double(c.source().x()),CGAL::to_double(c.source().y()));
                    p2=Point_2(CGAL::to_double(c.target().x()),CGAL::to_double(c.target().y()));

                    line.push_back(GPS_Traits_2::X_monotone_curve_2(p1,p2));
                    dominance.intersection(line);
                    cout<<"Line: " << dominance.number_of_polygons_with_holes()<<endl;
                }
                //cout<<S.do_intersect(dominance)<<endl;
                jInvisibleArea.join(wholeArea);
                jInvisibleArea.difference(visibleAreas[j]);
                dominance.join(jInvisibleArea);
                constrainedDominance.join(visibleAreas[i]);
                constrainedDominance.intersection(dominance);
                S.intersection(constrainedDominance);
            }
        }
        dominanceAreas.push_back(S);


    }

    //GeometryReader teste;
    //teste.exportArrangementToGDAL(arrangement, "teste-arr");
    teste.exportMWVDiagramToGDAL(dominanceAreas,"cmwv");
    //teste.exportPointsToGDAL(sites,"sites");
}


void cmwv::obstacleShadowsWang(Point_2 &s, obstacle &obstacle, Bbox_2 extent, Polygon_set_2 &shadows ) {
    //Polygon_set_2 wholeArea=BoxAsPolygon(extent);
    for (unsigned int k=0;k<obstacle.size()-1;k++) { //for each pair of lines created
        //cout<<obstacles[j][k] <<", " <<obstacles[j][k+1]<<endl;
        //create a polygon to represent the shadow of that pair
        if (obstacle[k]!=obstacle[k+1]) {

            Polygon_2 shadow;
            Point_2 p1extent=intersectWithExtent(s, obstacle[k], extent);
            Point_2 p2extent=intersectWithExtent(s, obstacle[k+1], extent);
            shadow.push_back(GPS_Segment_2(obstacle[k],obstacle[k+1]));
            shadow.push_back(GPS_Segment_2(obstacle[k+1], p2extent));
            shadow.push_back(GPS_Segment_2(p2extent, p1extent));
            shadow.push_back(GPS_Segment_2(p1extent, obstacle[k]));
            if (shadow.orientation()==-1) {
                shadow.reverse_orientation();
            }
            //complete the shadow with the bounding box vertex
            //cout<< shadow<<endl;
            shadows.join(shadow);
        }
    }
}


Point_2 cmwv::representativePoint(Arrangement_2::Halfedge_handle eit) {
    Vector_2 v1,v2; //middle
    Point_2 res;
    //cout << eit->curve().source <<", "<< eit->curve().target() <<", " << m <<endl ;
    Vector_2 m((CGAL::to_double(eit->curve().source().x())+CGAL::to_double(eit->curve().target().x()))/2,(CGAL::to_double(eit->curve().source().y())+CGAL::to_double(eit->curve().target().y()))/2);
    //cout<< m<<endl;
    if (eit->curve().is_circular()) {
        //middle arc point
        Circle_2 c=eit->curve().supporting_circle();
        Vector_2 vCenter(CGAL::to_double(c.center().x()),CGAL::to_double(c.center().y()));
        double radius=sqrt(CGAL::to_double(c.squared_radius()));
        Vector_2 u=m-vCenter;

        double norm=sqrt(CGAL::to_double(u.squared_length()));
        if (norm==0.) {
            double y=CGAL::to_double(eit->curve().source().y())-CGAL::to_double(eit->curve().target().y());
            double x=CGAL::to_double(eit->curve().source().x())-CGAL::to_double(eit->curve().target().x());
            if (y==0)  {
                u=Vector_2(0,1);
            } else {
                u=Vector_2(1,-x/y);
            }
            norm=sqrt(CGAL::to_double(u.squared_length()));
        }
        Vector_2 u0(CGAL::to_double(eit->curve().source().x()),CGAL::to_double(eit->curve().source().y()));
        u0=u0-vCenter;
        double cross=CGAL::to_double(u0.x()*u.y()-u0.y()*u.x());
        if (cross<0) {
            //it is clockwise, fix needed
            u=u*(-1.);
        }
        Vector_2 vres=u*(1/norm)*radius+vCenter;
        res=Point_2(vres.x(),vres.y());


    } else {
        res=Point_2(m.x(),m.y());
    }
    return res;
}

void cmwv::ApoloniusCircle(Point_2 s1, double w1, Point_2 s2, double w2, Data_Curve_2 &curve, int nSites) {
    Circle_2 c;
    mwv_base::ApoloniusCircle(s1,w1,s2,w2,c,nSites);
    curve =Data_Curve_2(c,edgeData(edgeData::DominanceArc));
}
