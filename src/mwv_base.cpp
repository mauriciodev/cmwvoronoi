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

void mwv_base::TwoSitesDominance(Point_2 s1, double w1, Point_2 s2, double w2, Polygon_set_2 &dominance, Bbox_2 extent) {
    dominance.clear();
    Curve_2 c;
    //if (w1==w2) w1*=0.001; //forcing circular case
    ApoloniusCircle(s1,w1,s2,w2,c);
    Polygon_2 wholeArea=BoxAsPolygon(extent);

    if (c.is_circular()) {
        Polygon_2 polC=construct_polygon(c.supporting_circle());
        if (w1<w2) {
            dominance.insert(polC);
        } else {
            dominance.insert(wholeArea);
            dominance.difference(polC);
        }
    } else { //line case
        //dominance.insert(wholeArea);

        Polygon_2 line; // line is a polygon defined by the boundary line and the extent
        Point_2 p1,p2;
        p1=Point_2(CGAL::to_double(c.source().x()),CGAL::to_double(c.source().y()));
        p2=Point_2(CGAL::to_double(c.target().x()),CGAL::to_double(c.target().y()));
        Point_2 p1extent=intersectWithExtent(p2,p1,extent);
        Point_2 p2extent=intersectWithExtent(p1,p2,extent);
        vector<Point_2> boxVertexes;
        //closes the polygon looking from s2 to s1 as if the segment was a breakline
        double alpha=angle(s2,p1extent,p2extent);
        Point_2 pMax, pMin;
        if (alpha>0) { //searching for counter clockwise minimum
            pMax=p2extent;
            pMin=p1extent;
        } else {
            pMax=p1extent;
            pMin=p2extent;
        }

        closePolygon(s2,pMin,pMax,extent,boxVertexes);
        line.push_back(GPS_Segment_2(pMax,pMin));
        for (int i=0;i<boxVertexes.size()-1;i++) {
            if(boxVertexes[i]!=boxVertexes[i+1])
                line.push_back(GPS_Segment_2(boxVertexes[i],boxVertexes[i+1]));
        }
        dominance.insert(line);
        //cout<<"Line: " << dominance.number_of_polygons_with_holes()<<endl;
    }
}

void mwv_base::ApoloniusCircle(Point_2 s1, double w1, Point_2 s2, double w2, Curve_2 &curve) {
    /*!Returns the boundary that divides the dominances of two points*/
    /*FIXME line length*/
    //Aurenhammer's formulae
    double s2y=CGAL::to_double(s2.y());
    double s1y=CGAL::to_double(s1.y());
    double s2x=CGAL::to_double(s2.x());
    double s1x=CGAL::to_double(s1.x());
    if(w1==w2) {
        double mx=CGAL::to_double((s1x+s2x)/2);
        double my=CGAL::to_double((s1y+s2y)/2);
        double dx=CGAL::to_double(s1x-s2x);
        double dy=CGAL::to_double(s1y-s2y);
        double m=atan(-1.*dx/dy);

        double d=100*(dx*dx+dy*dy);

        Point_2 p1(-d*cos(m)+mx,-d*sin(m)+my),p2(d*cos(m)+mx,d*sin(m)+my);
        curve=Segment_2(p1,p2);
        //w1=w2*1.001;
    //}
    } else {
        double cx=(w1*w1*s2x-w2*w2*s1x)/(w1*w1-w2*w2);
        double cy=(w1*w1*s2y-w2*w2*s1y)/(w1*w1-w2*w2);
        //cout<< "Center:" <<CGAL::to_double(cx) << "," << CGAL::to_double(cy)<<endl;
        double d= CGAL::sqrt(CGAL::to_double((s1x-s2x)*(s1x-s2x) + (s1y-s2y)*(s1y-s2y)));

        double r=w1*w2*d/(w1*w1-w2*w2);
        //cout<<LineString "Radius:" <<CGAL::to_double(r)<<endl;
        if (r<0) r=r*-1;
        curve= Circle_2( Kernel::Point_2(cx,cy), r*r);
    }
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

void mwv_base::closePolygon(Point_2 site, Point_2 minVertex, Point_2 maxVertex, Bbox_2 extent, vector<Point_2> &boxVertexes)    {
    /*Closes the polygon defined by a site, an obstacle and the bounding box. the obstacle is defined by maxVertex and minVertex*/
    Point_2 pMinExtent=intersectWithExtent(site, minVertex, extent);
    Point_2 pMaxExtent=intersectWithExtent(site, maxVertex, extent);
    boxVertexes.push_back(minVertex);
    boxVertexes.push_back(pMinExtent);
    vector<Point_2> box;
    box.push_back(Point_2(extent.xmin(),extent.ymin()));
    box.push_back(Point_2(extent.xmax(),extent.ymin()));
    box.push_back(Point_2(extent.xmax(),extent.ymax()));
    box.push_back(Point_2(extent.xmin(),extent.ymax()));
    vector<double> angles;
    //computes the angles between the lower counter clockwise bound and each vertex in the box
    for (vector<Point_2>::iterator pIt=box.begin();pIt!=box.end();++pIt) {
        angles.push_back(reducedAngle(angle(site,minVertex,*pIt)));
    }
    double maxAngle=angle(site,minVertex,maxVertex);
    maxAngle=reducedAngle(maxAngle);
    //searches for what is the counter clockwise sucessor of the minVertex
    int clockWiseSucessor=0;
    for (int i=1;i<4;i++) {
        if (angles[i]<angles[clockWiseSucessor]) {
            clockWiseSucessor=i;
        }
    }
    //add the vertexes in the box, starting from the first sucessor that are before maxVertex
    for (int i=clockWiseSucessor; i<(clockWiseSucessor+4);i++) {
        int id=i % 4; //uses mod to remap the id to the begin of the vector
        if (angles[id]<maxAngle) boxVertexes.push_back(box[id]);
    }
    boxVertexes.push_back(pMaxExtent);
    boxVertexes.push_back(maxVertex);
}

double mwv_base::angle(Point_2 p0, Point_2 p1, Point_2 p2)  {
    /* Returns angle from -pi to pi*/
    Vector_3 v1(p1.x()-p0.x(),p1.y()-p0.y(),0), v2(p2.x()-p0.x(),p2.y()-p0.y(),0);
    double n1,n2;
    n1=CGAL::to_double(v1.squared_length());
    n2=CGAL::to_double(v2.squared_length());
    n1=sqrt(n1);
    n2=sqrt(n2);

    double c=CGAL::to_double((v1*v2)/(n1*n2));
    //cout<< c<<endl;
    Vector_3 cross=CGAL::cross_product(v1,v2);
    //cout<< cross<<endl;
    double alpha;
    if(c>1) c=1;
    if(c<-1) c=-1;
    alpha=acos(c);
    //cout<<alpha<<endl;
    if (cross.z()<0) alpha=0-alpha;
    return alpha;
}

double mwv_base::reducedAngle(double angle) {
    if (angle>2*M_PI) {
        return fmod(angle,2*M_PI);
    } else if (angle<0){
        return fmod(angle,2*M_PI)+2*M_PI;
    } else {
        return angle;
    }
}

