#include "awv.h"

void awv::addPoint(NT x, NT y, NT weight) {
    Apollonius_graph::Site_2 site(Point_2(x,y),weight);
    this->graph.insert(site);

}



awv::awv() {

}

void awv::getDiagram(siteVector &v, weightVector &w) {
    for (unsigned int i=0;i<v.size();i++) {
        Apollonius_graph::Site_2 site(v[i],w[i]);
        this->graph.insert(site);
    }

}
