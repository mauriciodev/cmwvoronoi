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
}


void mwv::getDiagram(siteVector &sites, weightVector &weights, Bbox_2 extent, MWVDiagram &dominanceAreas) {
    dominanceAreas.resize(sites.size());

    //starting thread pool
    //boost::asio::io_service io_service;
    //boost::asio::io_service::work work(io_service);
    //boost::thread_group threads;
    //finding number of cores
    //int my_thread_count=1;//boost::thread::hardware_concurrency();
    //creating threads
    /*for (std::size_t i = 0; i < my_thread_count; ++i)
       threads.create_thread(boost::bind(&boost::asio::io_service::run, &io_service));*/
    cout<<sites.size() << " sites found."<<endl;
    for (int i=0;i<sites.size();i++) {
        oneDominance(i,sites,weights,extent,&dominanceAreas);
        //io_service.post(boost::bind(&mwv::oneDominance, this, i, sites,weights,wholeArea, &dominanceAreas));
    }
    //io_service.post(boost::bind(a_long_running_task, 123));

    //io_service.reset();
    //threads.join_all();


    cout<<endl;


}



