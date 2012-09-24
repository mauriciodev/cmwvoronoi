#include "cmwv_ps.h"

cmwv_ps::cmwv_ps()
{
}


bool cmwv_ps::visibilityLines(Point_2 p0, obstacle &ob, vector<Segment_2> &visLines, Bbox_2 extent) {
    for(obstacle::iterator vertex=ob.begin(); vertex!=ob.end();++vertex) {
        //cout<<(*vertex)<<endl;
        Segment_2 visRay(p0,intersectWithExtent(p0,*vertex, extent));
        //cout<<visRay.source()<<" / "<<visRay.target()<<endl;
        visLines.push_back(visRay);
    }
    return true;
}

bool cmwv_ps::visibilityLines_mauricio(Point_2 p0, obstacle &ob, vector<Segment_2> &visLines, Bbox_2 extent) {
    for(obstacle::iterator vertex=ob.begin(); vertex!=ob.end();++vertex) {
        //cout<<(*vertex)<<endl;
        Segment_2 visRay(p0,intersectWithExtent(p0,*vertex, extent));
        //cout<<visRay.source()<<" / "<<visRay.target()<<endl;
        visLines.push_back(visRay);
    }
    return true;
}

CGAL::Bbox_2 cmwv_ps::getBoundingBox(siteVector &sites,obstacleVector &obstacles){
    siteVector pointSet=sites;
    for (obstacleVector::iterator iObs=obstacles.begin(); iObs!=obstacles.end();++iObs) {
        for(obstacle::iterator iVertex=iObs->begin(); iVertex!=iObs->end(); ++iVertex) {
            pointSet.push_back(*iVertex);
        }
    }

    return mwv_base::getBoundingBox(pointSet);
}

void cmwv_ps::getDiagram2(siteVector &sites, weightVector &weights, obstacleVector obstacles, Bbox_2 extent, MWVDiagram &dominanceAreas) {
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
                Curve_2 c;
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

void cmwv_ps::obstacleShadowsWang(Point_2 &s, obstacle &obstacle, Bbox_2 extent, Polygon_set_2 &shadows ) {
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


