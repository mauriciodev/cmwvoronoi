#include "mwv.h"

mwv::mwv() {

}



bool mwv::oneDominance(int i, siteVector &sites, weightVector &weights, Bbox_2 extent) {
    Polygon_2 wholeArea = BoxAsPolygon(extent);
    Polygon_set_2 S;
    cout<<i+1<<" de "<<sites.size()<< endl;
    S.join(wholeArea);

    for (int j=0; j<(int)sites.size();j++) {
        if (i!=j) {
            //cout<<S.number_of_polygons_with_holes()<<endl;
            Polygon_set_2 c;
            //Curve_2 c1;
            //if (w1==w2) w1*=0.001; //forcing circular case
            //ApoloniusCircle(sites[i].cgal(),weights[i],sites[j].cgal(),weights[j],c1);
            TwoSitesDominance(sites[i].cgal(),weights[i],sites[j].cgal(),weights[j],c,extent);
            //cout<<S.do_intersect(dominance)<<endl;
            S.intersection(c);
        }
    }
    //this->_mutex.lock();
    int npols=S.number_of_polygons_with_holes();
    boost::mutex::scoped_lock lock(_mutex);
    this->_diagram[i].clear();
    this->_diagram[i].join(S);
    //this->_mutex.unlock();
    //cout << "Polygons "<< i<<": "<<S.number_of_polygons_with_holes() << endl;
    //    (*teste)[i]=i;
    //cout<<i<<",";
    return (npols>0);
}

void mwv::processSites(int startId, int endId,siteVector s, weightVector w, Bbox_2 extent) {
    //s.reserve(s.size());
    //this->_mutex.lock();
    //s=sites;
    //w=weights;
    //this->_mutex.unlock();
    for (unsigned int i=startId;((int)i<endId+1) && (i<s.size());i++) {
        oneDominance(i,s,w,extent);
    }
}
/*void mwv::processSites(int startId, int endId,siteVector &sites, weightVector &weights, Bbox_2 extent) {
    siteVector s;
    s.reserve(endId-startId+1);

    for (unsigned int i=startId;((int)i<endId+1) && (i<sites.size());i++) {
        oneDominance(i,sites,weights,extent);
    }
}*/

void mwv::getDiagram(siteVector &sites, weightVector &weights, Bbox_2 extent, MWVDiagram &dominanceAreas, int numberOfThreads) {
    cout<<sites.size() << " sites found."<<endl;
    this->_diagram.clear();
    this->_diagram.resize(sites.size());
    //processSites(0,sites.size(),sites,weights,extent,dominanceAreas);
    //starting thread pool
    boost::thread_group threads;
    //finding number of cores
    if (numberOfThreads==0) {
        numberOfThreads=boost::thread::hardware_concurrency();
    }
    //force the number of threads for now
    //numberOfThreads=1;

    //creating threads
    int startId,endId, sitesPerGroup;
    sitesPerGroup=sites.size() / numberOfThreads +1;
    cout<< "Starting "<<numberOfThreads<<" threads for computation."<<endl;
    for (int i=0;i<numberOfThreads;i++) {
        startId=sitesPerGroup*i;
        endId=sitesPerGroup*(i+1)-1;
        threads.add_thread(new boost::thread(&mwv::processSites,this,startId,endId,sites,weights,extent));
	}
    threads.join_all();

    dominanceAreas=this->_diagram;
    this->_diagram.clear();
}



