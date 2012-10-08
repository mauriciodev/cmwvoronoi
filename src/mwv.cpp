#include "mwv.h"

mwv::mwv() {

}



bool mwv::oneDominance(int i, siteVector &sites, weightVector &weights, Bbox_2 extent,MWVDiagram *dominanceAreas) {
    Polygon_2 wholeArea = BoxAsPolygon(extent);
    Polygon_set_2 S;
    cout<<i+1<<" de "<<sites.size()<< endl;
    S.join(wholeArea);
    for (unsigned int j=0; j<sites.size();j++) {
        if (i!=j) {
            //cout<<S.number_of_polygons_with_holes()<<endl;
            Polygon_set_2 c;
            TwoSitesDominance(sites[i],weights[i],sites[j],weights[j],c,extent);
            //cout<<S.do_intersect(dominance)<<endl;
            S.intersection(c);
        }
    }
    (*dominanceAreas)[i].join(S);
    cout << "Polygons "<< i<<": "<<S.number_of_polygons_with_holes() << endl;
    //    (*teste)[i]=i;
    //cout<<i<<",";
	return (S.number_of_polygons_with_holes()>0);
}

void mwv::processSites(int startId, int endId,siteVector &sites, weightVector &weights, Bbox_2 extent, MWVDiagram &dominanceAreas) {
	for (int i=startId;i<endId;i++) {
		oneDominance(i,sites,weights,extent,&dominanceAreas);
    }
}

void mwv::getDiagram(siteVector &sites, weightVector &weights, Bbox_2 extent, MWVDiagram &dominanceAreas) {
    dominanceAreas.resize(sites.size());
	processSites(0,sites.size(),sites,weights,extent,dominanceAreas);
    //starting thread pool
    //boost::asio::io_service io_service;
    //boost::asio::io_service::work work(io_service);
    //boost::thread_group threads;
    //finding number of cores
    int my_thread_count=1;//boost::thread::hardware_concurrency();
    //creating threads
	/*vector<MWVDiagram> threadResults;
	threadResults.resize(my_thread_count);
	vector<boost::thread> threads;
	for (int i=0;i<my_thread_count;i++) {
		int startId,endId, sitesPerGroup;
		sitesPerGroup=sites.size() / my_thread_count +1;
		startId=sitesPerGroup*i;
		endId=sitesPerGroup*(i+1)-1;
		threads.push_back(boost::thread(&mwv::processSites,startId,endId,sites,weights,extent,threadResults[i]));
	}
    cout<<sites.size() << " sites found."<<endl;


	for (unsigned int i=0;i<my_thread_count;i++) {
		dominanceAreas.insert(
    cout<<endl;*/


}



