#include "mwv.h"

mwv::mwv() {

}






void mwv::getDiagram(siteVector &sites, weightVector &weights, Bbox_2 extent, MWVDiagram &dominanceAreas) {
    Polygon_2 wholeArea;
    wholeArea=BoxAsPolygon(extent);
    for (unsigned int i=0;i<sites.size();i++) {
        Polygon_set_2 S;
        cout<<i+1<<" de "<<sites.size()<< endl;
        S.join(wholeArea);
        for (unsigned int j=0; j<sites.size();j++) {
            if (i!=j) {
                //cout<<S.number_of_polygons_with_holes()<<endl;
                Data_Curve_2 c;
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
                    line.push_back(X_monotone_curve_2(p1,p2));
                    dominance.intersection(line);
                    cout<<"Line: " << dominance.number_of_polygons_with_holes()<<endl;
                }
                //cout<<S.do_intersect(dominance)<<endl;
                S.intersection(dominance);
            }
        }
        dominanceAreas.push_back(S);


    }

    //GeometryReader teste;
    //teste.exportArrangementToGDAL(arrangement, "teste-arr");
    //teste.exportArrangementFacesToGDAL(arrangement, "teste-arr-pol",extent);
    //teste.exportPointsToGDAL(sites,"sites");


}


mwv::Polygon_2 mwv::construct_polygon (const Circle_2& circle) {
  // Subdivide the circle into two x-monotone arcs.
  Traits_2 traits;
  Curve_2 curve (circle);
  std::list<CGAL::Object>  objects;
  traits.make_x_monotone_2_object() (curve, std::back_inserter(objects));
  CGAL_assertion (objects.size() == 2);

  // Construct the polygon.
  Polygon_2 pgn;
  X_monotone_curve_2 arc;
  std::list<CGAL::Object>::iterator iter;

  for (iter = objects.begin(); iter != objects.end(); ++iter) {
    CGAL::assign (arc, *iter);
    pgn.push_back (arc);
  }

  return pgn;
}

mwv::Polygon_2 mwv::BoxAsPolygon(const Bbox_2 & extent) {
    Polygon_2 wholeArea;
    Point_2 p1,p2,p3,p4;
    p1=Point_2(extent.xmin(),extent.ymin());
    p2=Point_2(extent.xmax(),extent.ymin());
    p3=Point_2(extent.xmax(),extent.ymax());
    p4=Point_2(extent.xmin(),extent.ymax());


    wholeArea.push_back(X_monotone_curve_2(p1,p2));
    wholeArea.push_back(X_monotone_curve_2(p2,p3));
    wholeArea.push_back(X_monotone_curve_2(p3,p4));
    wholeArea.push_back(X_monotone_curve_2(p4,p1));
    return wholeArea;
}
