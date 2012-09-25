#include "cmwv_ps.h"

cmwv_ps::cmwv_ps()
{
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

void cmwv_ps::getDiagram(siteVector &sites, weightVector &weights, obstacleVector obstacles, Bbox_2 extent, MWVDiagram &dominanceAreas) {
    Polygon_2 wholeArea;
    wholeArea=BoxAsPolygon(extent);
    //create visibility areas
    vector<Polygon_set_2> visibleAreas;
    visibleAreas.resize(sites.size());
    for (unsigned int i=0;i<sites.size();i++) { //for each site
        Polygon_set_2 invisibleArea;
        for (unsigned int j=0;j<obstacles.size();j++) { //for each obstacle
            Polygon_set_2 objectShadow;
            //obstacleShadowsWang(sites[i],obstacles[j],extent,objectShadow);
            obstacleShadowsMauricio(sites[i],obstacles[j],extent,objectShadow);
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

void cmwv_ps::obstacleShadowsMauricio(Point_2 &s, obstacle &obstacle, Bbox_2 extent, Polygon_set_2 &shadows ) {
    //Find min and max angles and vertexes ids
    int vmaxId=0, vminId=0;
    NT currentTotal=0, currentAngle=0,newTotal=0, maxTotal=0, minTotal=0;
    for (int i=1; i<obstacle.size();i++) {
        currentAngle=angle(s, obstacle[i], obstacle[i-1]);
        newTotal=currentAngle+newTotal;

        cout<<currentAngle<<" ; "<<newTotal<<" ; " <<minTotal<< " ; "<<maxTotal<<endl;
        if (newTotal<=minTotal) { //this is the new min
            vminId=i;
            minTotal=newTotal;
        }
        if (newTotal>=maxTotal) { //this is the new max
            vmaxId=i;
            maxTotal=newTotal;
        }

    }

    //Polygon_set_2 wholeArea=BoxAsPolygon(extent);
    Polygon_2 shadow;
    Point_2 p1extent=intersectWithExtent(s, obstacle[vminId], extent);
    shadow.push_back(GPS_Segment_2(p1extent,obstacle[vminId]));
    int inc=(vmaxId-vminId)/abs(vmaxId-vminId);
    for (unsigned int k=vminId;k!=vmaxId;k+=inc) { //for each pair of lines created
        //cout<<obstacles[j][k] <<", " <<obstacles[j][k+1]<<endl;

        if (obstacle[k]!=obstacle[k+inc]) {
            shadow.push_back(GPS_Segment_2(obstacle[k],obstacle[k+inc]));
        }

    }
    Point_2 p2extent=intersectWithExtent(s, obstacle[vmaxId], extent);
    shadow.push_back(GPS_Segment_2(obstacle[vmaxId],p2extent));
    shadow.push_back(GPS_Segment_2(p2extent,p1extent));
    cout<<"OBSTACULO:::"<<shadow<<endl;
    if (shadow.orientation()==-1) {
        shadow.reverse_orientation();
    }
    shadows.join(shadow);
}

NT cmwv_ps::angle(Point_2 p0, Point_2 p1, Point_2 p2)  {
    /* Returns angle from -pi to pi*/
    Vector_3 v1(p1.x()-p0.x(),p1.y()-p0.y(),0), v2(p2.x()-p0.x(),p2.y()-p0.y(),0);
    double n1,n2;
    n1=CGAL::to_double(v1.squared_length());
    n2=CGAL::to_double(v2.squared_length());
    n1=sqrt(n1);
    n2=sqrt(n2);

    double c=CGAL::to_double((v1*v2)/(n1*n2));
    //cout<< c<<endl;
    Vector_3 cross=CGAL::cross_product(v1,v2);
    //cout<< cross<<endl;
    NT alpha;
    if(c>1) c=1;
    if(c<-1) c=-1;
    alpha=acos(c);
    //cout<<alpha<<endl;
    if (cross.z()<0) alpha=0-alpha;
    return alpha;
}
