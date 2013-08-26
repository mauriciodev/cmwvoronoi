#include "mwv_grid.h"

mwv_grid::mwv_grid() {

}

void mwv_grid::getDiagram(siteVector &sites, weightVector &weights, CGAL::Bbox_2 extent, grid &outgrid, int NumberOfThreads)
{
    outgrid.setMapping(extent.xmin(),extent.ymin(),extent.xmax(),extent.ymax());
    cout<<sites.size() << " sites found."<<endl;
    double x,y,dx,dy,d;
    for (int i=0; i<outgrid.cols;i++) {
        for (int j=0; j<outgrid.cols;j++) {
            outgrid.mapRowCol(i,j,x,y);
            double minDist=DBL_MAX;
            int minK=0;
            for (unsigned int k=0; k<sites.size();k++) {
                //weighted distance
                dx=CGAL::to_double(x-sites[k].x);
                dy=CGAL::to_double(y-sites[k].y);
                d=sqrt(dx*dx+dy*dy)/weights[k];
                if(d<minDist) {
                    minDist=d;
                    minK=k;
                }

            }
            outgrid.set(i,j,minK);
        }
    }
    //processSites(0,sites.size(),sites,weights,extent,dominanceAreas);
    //starting thread pool
    /*boost::thread_group threads;
    //finding number of cores
    if (numberOfThreads==0) {
        numberOfThreads=boost::thread::hardware_concurrency();
    }
    //force the number of threads for now
    numberOfThreads=1;

    //creating threads
    int startId,endId, sitesPerGroup;
    sitesPerGroup=sites.size() / numberOfThreads +1;
    cout<< "Starting "<<numberOfThreads<<" threads for computation."<<endl;
    for (int i=0;i<numberOfThreads;i++) {
        startId=sitesPerGroup*i;
        endId=sitesPerGroup*(i+1)-1;
        threads.add_thread(new boost::thread(&mwv::processSites,this,startId,endId,sites,weights,extent));
	}
    threads.join_all();*/

}



