#include "cmwv_ps.h"

cmwv_ps::cmwv_ps()
{
}

cmwv_ps::~cmwv_ps() {
    this->_diagram.clear();
    this->_visibleAreas.clear();
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

void cmwv_ps::getDiagram(siteVector &sites, weightVector &weights, obstacleVector obstacles, Bbox_2 extent, MWVDiagram &dominanceAreas, VisibilityConcept concept, int numberOfThreads) {
    cout<<sites.size() << " sites found."<<endl;
    //starting thread pool
    boost::thread_group threads;
    //finding number of cores
    if (numberOfThreads==0) {
        numberOfThreads=boost::thread::hardware_concurrency();
    }

    //creating threads
    int startId,endId, sitesPerGroup;
    sitesPerGroup=sites.size() / numberOfThreads +1;
    cout<< "Starting "<<numberOfThreads<<" threads for computation."<<endl;

    Polygon_2 wholeArea;
    wholeArea=BoxAsPolygon(extent);
    //create visibility areas
    this->_visibleAreas.clear();
    this->_visibleAreas.resize(sites.size());
    for (int i=0;i<numberOfThreads;i++) {
        startId=sitesPerGroup*i;
        endId=sitesPerGroup*(i+1)-1;
        threads.add_thread(new boost::thread(&cmwv_ps::processShadows,this, startId, endId, sites, weights, obstacles, extent, concept));
    }
    threads.join_all();
    //processShadows(0, sites.size(), sites, weights, obstacles, extent, concept);

    cout<< "Creating dominance areas"<<endl;
    //processing dominance areas
    this->_diagram.clear();
    _diagram.resize(sites.size());
    boost::thread_group threads2;
    for (int i=0;i<numberOfThreads;i++) {
        startId=sitesPerGroup*i;
        endId=sitesPerGroup*(i+1)-1;
        threads2.add_thread(new boost::thread(&cmwv_ps::processSites,this, startId, endId, sites, weights, extent));
    }
    threads2.join_all();


    dominanceAreas=this->_diagram;
    this->_diagram.clear();

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

			if (boxVertexes.size()>0){
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
}

void cmwv_ps::obstacleShadowsMauricio(Point_2 &s, obstacle &obstacle, Bbox_2 extent, Polygon_set_2 &shadows ) {
    //Find min and max angles and vertexes ids
    int vmaxId=0, vminId=0,minDistId=0, nElements=0;
    nElements=obstacle.size();
    NT currentTotal=0, currentAngle=0,newTotal=0, maxTotal=0, minTotal=0, minDist=0;
    bool isPolygon=false;
    if (obstacle[0]==obstacle[obstacle.size()-1]) isPolygon=true;
    minDist=CGAL::squared_distance(s,obstacle[0]);
    for (unsigned int i=1; i<nElements;i++) {
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
        if (minDist>CGAL::squared_distance(s,obstacle[i])) {
            minDistId=i;
            minDist=CGAL::squared_distance(s,obstacle[i]);
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
    if (isPolygon==false){
        for (int k=vminId;k!=vmaxId;k+=inc) { //for each pair of lines created
            //cout<<obstacles[j][k] <<", " <<obstacles[j][k+1]<<endl;
            if (obstacle[k]!=obstacle[k+inc]) {
                shadow.push_back(GPS_Segment_2(obstacle[k],obstacle[k+inc]));
            }
        }
    } else {
        //check if the vertex with minimum distance is between the two maximums in the vector
        if( abs(minDistId-vminId)+abs(minDistId-vmaxId)== abs(vminId-vmaxId) ) {
            //between, just walk normally from min to max
            for (int k=vminId;k!=vmaxId;k+=inc) { //for each pair of lines created
                //cout<<obstacles[j][k] <<", " <<obstacles[j][k+1]<<endl;
                if (obstacle[k]!=obstacle[k+inc]) {
                    shadow.push_back(GPS_Segment_2(obstacle[k],obstacle[k+inc]));
                }
            }
        } else { //not between the two, reverse walking using mod to circle around the vector
            for (int k=vminId;k!=vmaxId;k=circularId(k-inc,nElements)) { //for each pair of lines created
                //cout<<obstacles[j][k] <<", " <<obstacles[j][k+1]<<endl;
                int kinc2=circularId(k-inc,nElements);
                if (obstacle[k]!=obstacle[kinc2]) {
                    shadow.push_back(GPS_Segment_2(obstacle[k],obstacle[kinc2]));
                }
            }


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


/*bool cmwv_ps::oneDominance(int i, siteVector &sites, weightVector &weights, Bbox_2 extent) {
    Polygon_2 wholeArea = BoxAsPolygon(extent);
    Polygon_set_2 S;
    cout<<i+1<<" de "<<sites.size()<< endl;
    S.join(wholeArea);
    for (int j=0; j<sites.size();j++) {
        if (i!=j) {
            //cout<<S.number_of_polygons_with_holes()<<endl;
            Polygon_set_2 c;
            TwoSitesDominance(sites[i],weights[i],sites[j],weights[j],c,extent);
            //cout<<S.do_intersect(dominance)<<endl;
            S.intersection(c);
        }
    }
    this->_mutex.lock();
    this->_diagram[i].join(S);
    this->_mutex.unlock();
    //cout << "Polygons "<< i<<": "<<S.number_of_polygons_with_holes() << endl;
    //    (*teste)[i]=i;
    //cout<<i<<",";
    return (S.number_of_polygons_with_holes()>0);
}*/

void cmwv_ps::processSites(int startId, int endId,siteVector sites, weightVector weights, Bbox_2 extent) {
    vector<Polygon_set_2> visibleAreas;
    visibleAreas=this->_visibleAreas;
    for (int i=0;i<visibleAreas.size();i++) {
        cout <<" "<<visibleAreas[i].number_of_polygons_with_holes();
    }
    cout<<endl;
    Polygon_2 wholeArea;
    wholeArea=BoxAsPolygon(extent);
    for (int i=startId;(i<endId+1) && (i<sites.size());i++) {
        if (i>sites.size()-1) break;
        Polygon_set_2 S;
        cout<<i+1<<" de "<<sites.size()<< endl;
        S.join(wholeArea);
        for (unsigned int j=0; j<sites.size();j++) {
            if (i!=j) {
                //cout<<S.number_of_polygons_with_holes()<<endl;
                Polygon_set_2 dominance,jInvisibleArea, constrainedDominance;
                dominance.clear();
                TwoSitesDominance(sites[i],weights[i],sites[j],weights[j],dominance, extent);
                //cout<<S.do_intersect(dominance)<<endl;
                jInvisibleArea.clear();
                jInvisibleArea.join(wholeArea);
                jInvisibleArea.difference(visibleAreas[j]);
                dominance.join(jInvisibleArea);
                constrainedDominance.clear();
                constrainedDominance.join(visibleAreas[i]);
                //cout<<constrainedDominance.number_of_polygons_with_holes()<<endl;
                constrainedDominance.intersection(dominance);
                S.intersection(constrainedDominance);
            }
        }
        this->_mutex.lock();
        this->_diagram[i].join(S);
        this->_mutex.unlock();
        //dominanceAreas.push_back(S);
    }
}

void cmwv_ps::processShadows(int startId, int endId, siteVector &sites, weightVector &weights, obstacleVector &obstacles, Bbox_2 extent, VisibilityConcept concept) {
    Polygon_2 wholeArea;
    wholeArea=BoxAsPolygon(extent);
    cout <<"Processing from "<< startId << " to "<< endId<<endl;
    for (int i=startId;(i<endId+1) && (i<sites.size());i++) { //for each site
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
        this->_mutex.lock();
        this->_visibleAreas[i].join(wholeArea);
        this->_visibleAreas[i].difference(invisibleArea);
        this->_mutex.unlock();
    }
}

int cmwv_ps::circularId(int id, int size) {
    int newId=0;
    newId=id % size;
    if (newId<0) newId+=size;
    return newId;

}

bool cmwv_ps::cleanObstaclesOutsideExtent(obstacleVector &obstacles){

    for (obstacleVector::iterator obsIt=obstacles.begin();obsIt!=obstacles.end();++obsIt) {
        for (obstacle::iterator pointIt=obsIt->begin();pointIt!=obsIt->end();++pointIt) {
            //nothing
        }
    }
}
