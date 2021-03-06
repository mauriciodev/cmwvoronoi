#include "mwv_base.h"


bool coord::operator==(coord const &c2) {
    return ((this->x==c2.x) && (this->y==c2.y));
}

void coord::operator=(coord const &c2) {
    this->x=c2.x;
    this->y=c2.y;
}

bool coord::operator!=(coord const &c2) {
    return (!((*this)==c2));
}

Point_2 coord::cgal() {
    return(Point_2(x,y));
}


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
            //FIXME This is causing a problem with terraview
            return p1;
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
        //cout<< p1<<", "<<p1extent<<", "<<p2<<", "<<p2extent<<endl;
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
        if (boxVertexes.size()>0) {
            line.push_back(GPS_Segment_2(pMax,pMin));
            for (unsigned int i=0;i<(boxVertexes.size()-1);i++) {
                if(boxVertexes[i]!=boxVertexes[i+1])
                    line.push_back(GPS_Segment_2(boxVertexes[i],boxVertexes[i+1]));
            }
            dominance.insert(line);
        }
        //cout<<"Line: " << dominance.number_of_polygons_with_holes()<<endl;
    }
}

void mwv_base::ApoloniusCircle(Point_2 s1, double w1, Point_2 s2, double w2, Curve_2 &curve) {
    /*!Returns the boundary that divides the dominances of two points*/
    /*FIXME line length*/
    //Aurenhammer's formulae
    NT s2y=s2.y();
    NT s1y=s1.y();
    NT s2x=s2.x();
    NT s1x=s1.x();
    /*double s2y=CGAL::to_double(s2.y());
    double s1y=CGAL::to_double(s1.y());
    double s2x=CGAL::to_double(s2.x());
    double s1x=CGAL::to_double(s1.x());*/
    if(w1==w2) {
        NT mx=(s1x+s2x)/2;
        NT my=(s1y+s2y)/2;
        NT dx=s1x-s2x;
        NT dy=s1y-s2y;
        Point_2 p1,p2;
        NT d=(dx*dx+dy*dy)/1000.;
        if (dy!=0) {
            double m=atan(-1.*CGAL::to_double(dx/dy));
            p1=Point_2(-d*cos(m)+mx,-d*sin(m)+my);
            p2=Point_2(d*cos(m)+mx,d*sin(m)+my);
        } else {
            p1=Point_2(mx,-d+my);
            p2=Point_2(mx,d+my);
        }
        curve=Segment_2(p1,p2);
        //w1=w2*1.001;
    //}
    } else {
        double den=1/CGAL::to_double(w1*w1-w2*w2);
        NT cx=(w1*w1*s2x-w2*w2*s1x)*den;
        NT cy=(w1*w1*s2y-w2*w2*s1y)*den;
        //double cx=(w1*w1*s2x-w2*w2*s1x)/(w1*w1-w2*w2);
        //double cy=(w1*w1*s2y-w2*w2*s1y)/(w1*w1-w2*w2);
        //cout<< "Center:" <<CGAL::to_double(cx) << "," << CGAL::to_double(cy)<<endl;
        double d= CGAL::sqrt(CGAL::to_double((s1x-s2x)*(s1x-s2x) + (s1y-s2y)*(s1y-s2y)));

        double r=w1*w2*d*den;
        //cout<<LineString "Radius:" <<CGAL::to_double(r)<<endl;
        if (r<0) r=r*-1;
        curve= Circle_2( Kernel::Point_2(cx,cy), r*r);
    }
}

CGAL::Bbox_2 mwv_base::getBoundingBox(siteVector &sites) {
    double minx,maxx,miny,maxy;
    siteVector::iterator sIt=sites.begin();
    minx=maxx=sIt->x;
    miny=maxy=sIt->y;

    for (sIt=sites.begin(); sIt!=sites.end(); ++sIt) {
        if (sIt->x>maxx) maxx=sIt->x;
        if (sIt->x<minx) minx=sIt->x;
        if (sIt->y>maxy) maxy=sIt->y;
        if (sIt->y<miny) miny=sIt->y;
    }
    double tolx=(maxx-minx)/10.;
    double toly=(maxy-miny)/10.;
    if (tolx==0) {tolx=toly;}
    else if (toly==0) { toly=tolx; }
    //else toly=tolx=10;

    //std::cout <<minx-tolx << miny-toly << maxx+tolx << maxy+toly<<std::endl;
    //toly=tolx=10000;
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

bool mwv_base::closePolygon(Point_2 site, Point_2 minVertex, Point_2 maxVertex, Bbox_2 extent, vector<Point_2> &boxVertexes)    {
    /*Closes the polygon defined by a site, an obstacle and the bounding box. the obstacle is defined by maxVertex and minVertex*/
	//checks if the points are inside the box
	if (!(isPointInBox(site,extent) && isPointInBox(minVertex,extent) && isPointInBox(maxVertex,extent)) ) return false;
	//extends the lines between the site and the vertexes
	Point_2 pMinExtent=intersectWithExtent(site, minVertex, extent);
    Point_2 pMaxExtent=intersectWithExtent(site, maxVertex, extent);
	//start building the polygon
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
	return (boxVertexes.size()>=4);
}

double mwv_base::isPointInBox(Point_2 p, Bbox_2 box) {
    bool xTest=(p.x()>=box.xmin()) && (p.x()<=box.xmax());
    bool yTest=(p.y()>=box.ymin()) && (p.y()<=box.ymax());
    return (xTest && yTest);

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
	/*! Returns an angle between 0 and 2*pi. */
    if (angle>2*M_PI) {
        return fmod(angle,2*M_PI);
    } else if (angle<0){
        return fmod(angle,2*M_PI)+2*M_PI;
    } else {
        return angle;
    }
}

bool mwv_base::arcAsLinestring(GPS_Segment_2 curve, vector<double> &outX, vector<double> &outY, double tol) {
    //FIXME number of vertexes
    if (curve.is_circular()) {
    //if (false){

        double r=CGAL::sqrt(CGAL::to_double(curve.supporting_circle().squared_radius()));
        double cx=CGAL::to_double(curve.supporting_circle().center().x());
        double cy=CGAL::to_double(curve.supporting_circle().center().y());

        Point_2 center=Point_2(cx,cy);

        Point_2 p1=Point_2(CGAL::to_double(curve.source().x()),CGAL::to_double(curve.source().y()));
        Point_2 p2=Point_2(CGAL::to_double(curve.target().x()),CGAL::to_double(curve.target().y()));

        if (curve.orientation()==CGAL::CLOCKWISE) {
            Point_2 pAux=p2;
            p2=p1;
            p1=pAux;
        }	
        double ang0=angle(center,Point_2(cx+1,cy),p1);
        double ang=angle(center,p1,p2);
        double maxTheta=0;
        double a=1-tol/r;
        if(fabs(a)>1) a=1.;
        maxTheta=2.*acos(a);
        int steps;
        if (ang==0.) {
            steps=1;
        } else {
            steps=ceil(fabs(ang)/maxTheta)+1;
        }
        if (steps<5) steps=5;
        //steps=10;

        double angStep=0,xi=0,yi=0;
        for (int i=0; i<steps+1; i++) {
            angStep=ang0+ang/(steps)*i;
            xi=cx+r*cos(angStep);
            yi=cy+r*sin(angStep);
            outX.push_back(xi);
            outY.push_back(yi);
        }
        if (curve.orientation()==CGAL::CLOCKWISE) {
            //ang*=-1.;
            reverse(outX.begin(),outX.end());
            reverse(outY.begin(),outY.end());
        }

    }
    if ( (!curve.is_circular()) || (outX.size()==0) || (outY.size()==0)) {
        outX.push_back(CGAL::to_double(curve.source().x()));
        outY.push_back(CGAL::to_double(curve.source().y()));
        outX.push_back(CGAL::to_double(curve.target().x()));
        outY.push_back(CGAL::to_double(curve.target().y()));
    }
    if ((outX.size()>0) && (outY.size()>0)) {
        return true;
    } else {
        return false;
    }


}


double mwv_base::measureAngle(Point_2 p1, Point_2 p0, Point_2 p2) {

        double x1=CGAL::to_double(p1.x());
        double x2=CGAL::to_double(p2.x());
        double y1=CGAL::to_double(p1.y());
        double y2=CGAL::to_double(p2.y());
        double cx=CGAL::to_double(p0.x());
        double cy=CGAL::to_double(p0.y());
        Vector_2 u1(x1-cx,y1-cy),u2(x2-cx,y2-cy);

        double scalar=CGAL::to_double(u1*u2);
        double norm1=sqrt(CGAL::to_double(u1*u1));
        double norm2=sqrt(CGAL::to_double(u2*u2));
        double cosTheta=scalar/norm1/norm2;
        double cross=CGAL::to_double(u1.x()*u2.y()-u1.y()*u2.x());
        double sinTheta=cross/norm1/norm2;
        double angle;
        if (cosTheta< -1.) {
            angle=M_PI;
        } else if (cosTheta>1. ) {
            angle=0;
        } else {
            angle=acos(cosTheta);
            if (sinTheta<0) {
                angle=2*M_PI-angle;
            }
        }
        return angle;
}

bool mwv_base::isPolygon(obstacle &obs) {
    Point_2 p;
    double length1=obs[0].distance(obs[1]);
    //NT length1=CGAL::squared_distance(obs[0],obs[1]);
    double lengthFirstLast=obs[0].distance(obs[obs.size()-1]);
    //NT lengthFirstLast=CGAL::squared_distance(obs[0],obs[obs.size()-1]);
    if (lengthFirstLast<length1/100000.) {
        return true;
    } else {
        return false;
    }
}

void mwv_base::douglasPeucker(obstacle::iterator obsStart, obstacle::iterator obsEnd, obstacle &obsOut, double tol) {
    /*!Douglas peucker linestring simplification*/
    NT dmax=0;
    obstacle::iterator maxCoord=obsStart;
    Point_2 p1(obsStart->x,obsStart->y),p2((obsEnd-1)->x,(obsEnd-1)->y);
    Line_2 base(p1,p2);
    //cout<<base<<endl;
    for(obstacle::iterator it= obsStart; it!=obsEnd;++it) {
        NT d=CGAL::squared_distance(base,Point_2(it->x,it->y));
        if (d>dmax) {
            maxCoord=it;
            dmax=d;
        }
    }
    //cout<<int(maxCoord-obsStart)<<endl
    // If max distance is greater than epsilon, recursively simplify
    if (dmax >= (tol*tol)) {
        obstacle part1,part2;
        douglasPeucker(obsStart,maxCoord+1, part1,tol);
        douglasPeucker(maxCoord,obsEnd,part2,tol);
        obsOut.reserve(part1.size()+part2.size());
        obsOut.insert(obsOut.end(),part1.begin(),part1.end());
        obsOut.insert(obsOut.end(),part2.begin()+1,part2.end());
    } else {
        obsOut.clear();
        obsOut.push_back(*obsStart);
        obsOut.push_back(*(obsEnd-1));
    }

}

