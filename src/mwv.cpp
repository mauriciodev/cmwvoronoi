#include "mwv.h"

mwv::mwv() {

}



bool mwv::oneDominance(int i, siteVector &sites, weightVector &weights, Polygon_2 wholeArea,MWVDiagram *dominanceAreas) {
    Polygon_set_2 S;
    cout<<i+1<<" de "<<sites.size()<< endl;
    S.join(wholeArea);
    for (unsigned int j=0; j<sites.size();j++) {
        if (i!=j) {
            //cout<<S.number_of_polygons_with_holes()<<endl;
            Curve_2 c;
            ApoloniusCircle(sites[i],weights[i],sites[j],weights[j],c);
            Polygon_set_2 dominance;
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
                line.push_back(GPS_Segment_2(p1,p2));
                dominance.intersection(line);
                cout<<"Line: " << dominance.number_of_polygons_with_holes()<<endl;
            }
            //cout<<S.do_intersect(dominance)<<endl;
            S.intersection(dominance);
        }
    }
    (*dominanceAreas)[i].join(S);
    cout << "Polygons "<< i<<": "<<S.number_of_polygons_with_holes() << endl;
    //    (*teste)[i]=i;
    //cout<<i<<",";
}


void mwv::getDiagram(siteVector &sites, weightVector &weights, Bbox_2 extent, MWVDiagram &dominanceAreas) {
    Polygon_2 wholeArea;
    wholeArea=BoxAsPolygon(extent);
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
        oneDominance(i,sites,weights,wholeArea,&dominanceAreas);
        //io_service.post(boost::bind(&mwv::oneDominance, this, i, sites,weights,wholeArea, &dominanceAreas));
    }
    //io_service.post(boost::bind(a_long_running_task, 123));

    //io_service.reset();
    //threads.join_all();


    for (unsigned int i=0;i<sites.size();i++) {
    //    oneDominance(i,sites,weights,wholeArea,&dominanceAreas);
        //cout << teste[i]<<",";
        //cout <<dominanceAreas[i].polygons_with_holes().Size()<<endl;
    }
    cout<<endl;

    //GeometryReader teste;
    //teste.exportArrangementToGDAL(arrangement, "teste-arr");
    //teste.exportArrangementFacesToGDAL(arrangement, "teste-arr-pol",extent);
    //teste.exportPointsToGDAL(sites,"sites");


}



