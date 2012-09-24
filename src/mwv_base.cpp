#include "mwv_base.h"

mwv_base::mwv_base()
{
}

Point_2 mwv_base::intersectWithExtent(Point_2 p0, Point_2 p1, Bbox_2 extent) {
    //Vector_2 diagonal(Point_2(extent.xmin(), extent.ymin()), Point_2(extent.xmax(), extent.ymax()));
    //double maxLength=2.*diagonal.squared_length();

    Ray_2 ray(p0,p1);
    Point_2 pBoundary;
    Rectangle_2 box(Point_2(extent.xmin(),extent.ymin()),Point_2(extent.xmax(),extent.ymax()));
    //std::cout<<extent<<std::endl;
    CGAL::Object intersection_obj = CGAL::intersection(ray,box);
    //std::cout<<ray<<std::endl;
    if (const Point_2 *ipoint = CGAL::object_cast<Point_2 >(&intersection_obj)) {

        // handle the point intersection case with *ipoint.
        // point is inside the extent
        //std::cout<<(*ipoint)<<std::endl;
        return *ipoint;

    } else
        if (const Segment_2 *iseg = CGAL::object_cast<Segment_2>(&intersection_obj)) {
            // handle the segment intersection case with *iseg.
            //point is outside the extent
            return iseg->target();
        } else {
            // handle the no intersection case.
            return Point_2(0,0);
        }


    //CGAL::assign(pBoundary,intersection_obj);



}

void mwv_base::ApoloniusCircle(Point_2 s1, double w1, Point_2 s2, double w2, Curve_2 &curve, int nSites) {
    /*FIXME line length*/
    //Aurenhammer's formulae
    double s2y=CGAL::to_double(s2.y());
    double s1y=CGAL::to_double(s1.y());
    double s2x=CGAL::to_double(s2.x());
    double s1x=CGAL::to_double(s1.x());
    if(w1==w2) {
        /*double mx=CGAL::to_double((s1x+s2x)/2);
        double my=CGAL::to_double((s1y+s2y)/2);
        double dx=CGAL::to_double(s1x-s2x);
        double dy=CGAL::to_double(s1y-s2y);
        double m=atan(-1.*dx/dy);

        double d=100*(dx*dx+dy*dy);

        Kernel::Point_2 p1(-d*cos(m)+mx,-d*sin(m)+my),p2(d*cos(m)+mx,d*sin(m)+my);
        curve=Segment_2(p1,p2);*/
        w1=w2*1.001;
    }
    //} else {
        double cx=(w1*w1*s2x-w2*w2*s1x)/(w1*w1-w2*w2);
        double cy=(w1*w1*s2y-w2*w2*s1y)/(w1*w1-w2*w2);
        //cout<< "Center:" <<CGAL::to_double(cx) << "," << CGAL::to_double(cy)<<endl;
        double d= CGAL::sqrt(CGAL::to_double((s1x-s2x)*(s1x-s2x) + (s1y-s2y)*(s1y-s2y)));

        double r=w1*w2*d/(w1*w1-w2*w2);
        //cout<<LineString "Radius:" <<CGAL::to_double(r)<<endl;
        if (r<0) r=r*-1;
        curve= Circle_2( Kernel::Point_2(cx,cy), r*r);
    //}
}

CGAL::Bbox_2 mwv_base::getBoundingBox(siteVector &sites) {
    double minx,maxx,miny,maxy;
    siteVector::iterator sIt=sites.begin();
    minx=maxx=CGAL::to_double(sIt->x());
    miny=maxy=CGAL::to_double(sIt->y());

    for (sIt=sites.begin(); sIt!=sites.end(); ++sIt) {
        if (CGAL::to_double(sIt->x())>maxx) maxx=CGAL::to_double(sIt->x());
        if (CGAL::to_double(sIt->x())<minx) minx=CGAL::to_double(sIt->x());
        if (CGAL::to_double(sIt->y())>maxy) maxy=CGAL::to_double(sIt->y());
        if (CGAL::to_double(sIt->y())<miny) miny=CGAL::to_double(sIt->y());
    }
    double tolx=(maxx-minx)/10.;
    double toly=(maxy-miny)/10.;
    if (tolx==0) {tolx=toly;}
    else if (toly==0) { toly=tolx; }
    //else toly=tolx=10;

    //std::cout <<minx-tolx << miny-toly << maxx+tolx << maxy+toly<<std::endl;
    return Bbox_2(minx-tolx,miny-toly,maxx+tolx,maxy+toly);
}




Polygon_2 mwv_base::construct_polygon (const Circle_2& circle) {
  // Subdivide the circle into two x-monotone arcs.
  GPS_Traits_2 traits;
  Curve_2 curve (circle);
  std::list<CGAL::Object>  objects;
  traits.make_x_monotone_2_object() (curve, std::back_inserter(objects));
  CGAL_assertion (objects.size() == 2);

  // Construct the polygon.
  Polygon_2 pgn;
  GPS_Segment_2 arc;
  std::list<CGAL::Object>::iterator iter;

  for (iter = objects.begin(); iter != objects.end(); ++iter) {
    CGAL::assign (arc, *iter);
    pgn.push_back (arc);
  }

  return pgn;
}

Polygon_2 mwv_base::BoxAsPolygon(const Bbox_2 & extent) {
    Polygon_2 wholeArea;
    Point_2 p1,p2,p3,p4;
    p1=Point_2(extent.xmin(),extent.ymin());
    p2=Point_2(extent.xmax(),extent.ymin());
    p3=Point_2(extent.xmax(),extent.ymax());
    p4=Point_2(extent.xmin(),extent.ymax());


    wholeArea.push_back(GPS_Segment_2(p1,p2));
    wholeArea.push_back(GPS_Segment_2(p2,p3));
    wholeArea.push_back(GPS_Segment_2(p3,p4));
    wholeArea.push_back(GPS_Segment_2(p4,p1));
    return wholeArea;
}
