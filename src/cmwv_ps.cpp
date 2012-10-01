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

void cmwv_ps::getDiagram(siteVector &sites, weightVector &weights, obstacleVector obstacles, Bbox_2 extent, MWVDiagram &dominanceAreas, VisibilityConcept concept) {
    Polygon_2 wholeArea;
    wholeArea=BoxAsPolygon(extent);
    //create visibility areas
    vector<Polygon_set_2> visibleAreas;
    visibleAreas.resize(sites.size());
    for (unsigned int i=0;i<sites.size();i++) { //for each site
        Polygon_set_2 invisibleArea;
        for (unsigned int j=0;j<obstacles.size();j++) { //for each obstacle
            Polygon_set_2 objectShadow;
            if (concept==Wang) {
                obstacleShadowsWang(sites[i],obstacles[j],extent,objectShadow);
            }
            if (concept==DePaulo) {
                obstacleShadowsMauricio(sites[i],obstacles[j],extent,objectShadow);
            }
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
                Polygon_set_2 dominance,jInvisibleArea, constrainedDominance;
                TwoSitesDominance(sites[i],weights[i],sites[j],weights[j],dominance, extent);
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

void cmwv_ps::obstacleShadowsWang(Point_2 &s, obstacle &obstacles, Bbox_2 extent, Polygon_set_2 &shadows ) {
    //Polygon_set_2 wholeArea=BoxAsPolygon(extent);
    for (unsigned int k=0;k<obstacles.size()-1;k++) { //for each pair of lines created
        //cout<<obstacles[j][k] <<", " <<obstacles[j][k+1]<<endl;
        //create a polygon to represent the shadow of that pair
        if (obstacles[k]!=obstacles[k+1]) {
            double alpha=angle(s,obstacles[k],obstacles[k+1]);
            vector<Point_2> boxVertexes;
            Point_2 minVertex,maxVertex;
            if (alpha>0) {//k+1 is clockwise sucessor of k
                minVertex=obstacles[k];
                maxVertex=obstacles[k+1];
            } else {
                minVertex=obstacles[k+1];
                maxVertex=obstacles[k];
            }
            closePolygon(s,minVertex,maxVertex,extent,boxVertexes);

            Polygon_2 shadow;
            shadow.push_back(GPS_Segment_2(maxVertex,minVertex));
            for (unsigned int i=0;i<boxVertexes.size()-1;i++) {
                shadow.push_back(GPS_Segment_2(boxVertexes[i],boxVertexes[i+1]));
            }

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
    for (unsigned int i=1; i<obstacle.size();i++) {
        currentAngle=angle(s, obstacle[i-1], obstacle[i]);
        newTotal=currentAngle+newTotal;

        //cout<<currentAngle<<" ; "<<newTotal<<" ; " <<minTotal<< " ; "<<maxTotal<<endl;
        if (newTotal<=minTotal) { //this is the new min
            vminId=i;
            minTotal=newTotal;
        }
        if (newTotal>=maxTotal) { //this is the new max
            vmaxId=i;
            maxTotal=newTotal;
        }

    }
    vector<Point_2> boxVertexes;
    closePolygon(s,obstacle[vminId],obstacle[vmaxId],extent,boxVertexes);
    //reverse the points to start from max vertex
    reverse(boxVertexes.begin(),boxVertexes.end());
    //Polygon_set_2 wholeArea=BoxAsPolygon(extent);
    Polygon_2 shadow;
    //Point_2 p1extent=intersectWithExtent(s, obstacle[vminId], extent);
    //shadow.push_back(GPS_Segment_2(p1extent,obstacle[vminId]));
    int inc=(vmaxId-vminId)/abs(vmaxId-vminId);
    for (int k=vminId;k!=vmaxId;k+=inc) { //for each pair of lines created
        //cout<<obstacles[j][k] <<", " <<obstacles[j][k+1]<<endl;

        if (obstacle[k]!=obstacle[k+inc]) {
            shadow.push_back(GPS_Segment_2(obstacle[k],obstacle[k+inc]));
        }

    }
    for (unsigned int i=0;i<boxVertexes.size()-1;i++) {
        shadow.push_back(GPS_Segment_2(boxVertexes[i],boxVertexes[i+1]));
    }
    //Point_2 p2extent=intersectWithExtent(s, obstacle[vmaxId], extent);
    //shadow.push_back(GPS_Segment_2(obstacle[vmaxId],p2extent));
    //shadow.push_back(GPS_Segment_2(p2extent,p1extent));
    //cout<<"OBSTACULO:::"<<shadow<<endl;
    if (shadow.orientation()==-1) {
        shadow.reverse_orientation();
    }
    shadows.join(shadow);
}

