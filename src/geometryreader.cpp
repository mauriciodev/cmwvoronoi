#include "geometryreader.h"

GeometryReader::GeometryReader()
{
    OGRRegisterAll();
}

bool GeometryReader::getExtent(std::string filename, Bbox_2 &box) {
    OGRDataSource       *poDS;
    poDS = OGRSFDriverRegistrar::Open( filename.c_str(), FALSE );
    if( poDS == NULL ) return false;
    OGRLayer  *poLayer= poDS->GetLayer(0);
    if( poLayer == NULL ) return false;
    OGREnvelope env;
    poLayer->GetExtent(&env);
    box=Bbox_2(env.MinX,env.MinY,env.MaxX,env.MaxY);
    return true;
}

void GeometryReader::getTestPoints(siteVector &v, weightVector &w) {
    v.push_back(Point_2(1,1));
    v.push_back(Point_2(10,0));
    v.push_back(Point_2(5,5));
    w.push_back(5);
    w.push_back(10);
    w.push_back(5);
}

void GeometryReader::getRandomPoints(int nPoints, siteVector &v, weightVector &w) {
    CGAL::Random generatorOfgenerator;
    int random_seed = generatorOfgenerator.get_int(0, 123456);
    std::cout << "random_seed = " << random_seed << std::endl;
    CGAL::Random theRandom(random_seed);
    double random_max = 0;
    double random_min = -45;
    double x1, y1,w1;

    for (int i = 0; i < 10; i++) {
        x1 = theRandom.get_double(random_min,random_max);
        y1 = theRandom.get_double(random_min,random_max);
        w1 = theRandom.get_double(10,100);
        std::cout << x1 << " " << y1 <<" "<<  w1<< std::endl;
        Point_2 s(x1,y1);
        //s.weight=w1;
        w.push_back(w1);
        v.push_back(s);
    }
}

bool GeometryReader::getPointsFromGDAL(std::string filename, std::string weightField, siteVector &sites, weightVector & w, int sizeLimit) {
    OGRDataSource       *poDS;
    poDS = OGRSFDriverRegistrar::Open( filename.c_str(), FALSE );
    if( poDS == NULL ) return false;
    OGRLayer  *poLayer= poDS->GetLayer(0);
    if( poLayer == NULL ) return false;
    OGRFeature *poFeature;

    OGRFeatureDefn *fields=poLayer->GetLayerDefn();
    int iField=fields->GetFieldIndex(weightField.c_str());
    OGRGeometry *poGeometry;
    poLayer->ResetReading();
    int i=0;
    while( ( (poFeature = poLayer->GetNextFeature()) != NULL ) && ((sizeLimit==0) || (i<sizeLimit))) {
        i++;
        w.push_back(poFeature->GetFieldAsDouble(iField)) ;
        poGeometry = poFeature->GetGeometryRef();
        //cout<<poGeometry->getGeometryType()<<endl;
        //cout<<wkbPoint<<endl;
        if( poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbPoint )  {
           OGRPoint *poPoint = (OGRPoint *) poGeometry;
           //cout<< poPoint->getX()<<", "<<poPoint->getY()<<endl;
           sites.push_back(Point_2(poPoint->getX(),poPoint->getY()));
        } else  {
           printf( "no point geometry\n" );
        }
    }

    return true;
}

/*bool GeometryReader::exportArrangementToGDAL(Arrangement_2 &arr, std::string filename) {
    const char *pszDriverName = "ESRI Shapefile";
    OGRSFDriver *poDriver;
    poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
    if( poDriver == NULL )    {
        printf( "%s driver not available.\n", pszDriverName );
        return 1;
    }
    char** options=NULL;
    options=CSLAddNameValue(options,"OVERWRITE","YES");

    OGRDataSource *poDS;
    poDS = poDriver->CreateDataSource( filename.c_str(), options );
    if( poDS == NULL )    {
        printf( "Creation of output file failed.\n" );
        return  1 ;
    }
    OGRLayer *poLayer;
    if (poDS->GetLayerByName(filename.c_str())) {
        poDS->DeleteLayer(0);
    }
    poLayer = poDS->CreateLayer( filename.c_str(), NULL, wkbLineString, NULL );

    if( poLayer == NULL ) {
        printf( "Layer creation failed.\n" );
        return 1;
    }

    OGRFieldDefn oField( "edgetype", OFTString );
    oField.SetWidth(16);
    if( poLayer->CreateField( &oField ) != OGRERR_NONE )
    {
        printf( "Creating field failed.\n" );
        exit( 1 );
    }
    OGRFieldDefn oField2( "idgen", OFTInteger);
    if( poLayer->CreateField( &oField2 ) != OGRERR_NONE )
    {
        printf( "Creating field failed.\n" );
        exit( 1 );
    }
    OGRFieldDefn oField3( "obstacleUp", OFTInteger);
    if( poLayer->CreateField( &oField3 ) != OGRERR_NONE )
    {
        printf( "Creating field failed.\n" );
        exit( 1 );
    }
    int nsites;
    nsites=arr.edges_begin()->curve().data().maskedSites.size();
    for (unsigned int i=0;i<nsites;i++) {
        std::stringstream ss;
        ss<<"site"<<i;
        OGRFieldDefn oField3(ss.str().c_str(), OFTInteger);
        if( poLayer->CreateField( &oField3 ) != OGRERR_NONE )
        {
            printf( "Creating field failed.\n" );
            exit( 1 );
        }
    }

    OGRFeature *poFeature;

    Arrangement_2::Edge_const_iterator eit;
    //Arrangement_2::Halfedge_handle eit;
    std::cout << arr.number_of_edges() << " edges:" << std::endl;
    const char * edgetypes[] = {"VisibilityLine", "DominanceArc", "BreakLine"};

    for (eit = arr.edges_begin(); eit != arr.edges_end(); ++eit) {
        poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
        vector<double>x,y;
        arcAsLinestring(eit->curve(),x,y);


        OGRLineString line;
        for(unsigned int i=0; i<x.size();i++) {
            if( ! ((x[i]!=x[i]) || (y[i]!=y[i]) ) ) {
                line.addPoint(x[i],y[i]);
            }
        }
        if (x.size()>0) {
            poFeature->SetGeometry( &line );
            edgeData fields=eit->curve().data();
            //cout<<edgetypes[fields.edgeType]<<endl;

            poFeature->SetField("edgetype",edgetypes[fields.edgeType]);
            poFeature->SetField("idgen",int(fields.generatorPointId));
            //poFeature->SetField("obsBefore",fields.isObstacleBefore);
            for (unsigned int i=0;i<fields.maskedSites.size();i++) {
                std::stringstream ss;
                ss<<"site"<<i;
                poFeature->SetField(ss.str().c_str(),int(fields.maskedSites[i]));
            }
            if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )  {
                printf( "Failed to create feature in shapefile.\n" );
                exit( 1 );
            }
        }
        OGRFeature::DestroyFeature( poFeature );

    }
    OGRDataSource::DestroyDataSource( poDS );
    return 0;
}*/



double GeometryReader::measureAngle(Point_2 p1, Point_2 p0, Point_2 p2) {

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

bool GeometryReader::exportPointsToGDAL(vector<Point_2> &pointList,std::string filename) {
    const char *pszDriverName = "ESRI Shapefile";
    OGRSFDriver *poDriver;
    poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
    if( poDriver == NULL )    {
        printf( "%s driver not available.\n", pszDriverName );
        return 1;
    }
    char** options=NULL;
    options=CSLAddNameValue(options,"OVERWRITE","YES");

    OGRDataSource *poDS;
    poDS = poDriver->CreateDataSource( filename.c_str(), options );
    if( poDS == NULL )    {
        printf( "Creation of output file failed.\n" );
        return  1 ;
    }
    OGRLayer *poLayer;
    if (poDS->GetLayerByName(filename.c_str())) {
        poDS->DeleteLayer(0);
    }
    poLayer = poDS->CreateLayer( filename.c_str(), NULL, wkbPoint, NULL );

    if( poLayer == NULL ) {
        printf( "Layer creation failed.\n" );
        return 1;
    }
    OGRFeature *poFeature;

        //Arrangement_2::Halfedge_handle eit;

    vector<Point_2>::iterator pIt;
    for (pIt=pointList.begin(); pIt!=pointList.end(); ++pIt) {
        poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );

        OGRPoint p;
        p.setX(CGAL::to_double(pIt->x()));
        p.setY(CGAL::to_double(pIt->y()));

        poFeature->SetGeometry( &p );
        if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )  {
            printf( "Failed to create feature in shapefile.\n" );
            exit( 1 );
        }

        OGRFeature::DestroyFeature( poFeature );

    }
    OGRDataSource::DestroyDataSource( poDS );
    return 0;
}

/*bool GeometryReader::exportArrangementFacesToGDAL(Arrangement_2 &arr, std::string filename,Bbox_2 extent) {
    //FIXME handle holes
    const char *pszDriverName = "ESRI Shapefile";
    OGRSFDriver *poDriver;
    poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
    if( poDriver == NULL )    {
        printf( "%s driver not available.\n", pszDriverName );
        return 1;
    }
    char** options=NULL;
    options=CSLAddNameValue(options,"OVERWRITE","YES");

    OGRDataSource *poDS;
    poDS = poDriver->CreateDataSource( filename.c_str(), options );
    if( poDS == NULL )    {
        printf( "Creation of output file failed.\n" );
        return  1 ;
    }
    OGRLayer *poLayer;
    if (poDS->GetLayerByName(filename.c_str())) {
        poDS->DeleteLayer(0);
    }
    poLayer = poDS->CreateLayer( filename.c_str(), NULL, wkbPolygon, NULL );

    if( poLayer == NULL ) {
        printf( "Layer creation failed.\n" );
        return 1;
    }
    OGRFeature *poFeature;

    Arrangement_2::Face_const_handle fit;
    std::cout << arr.number_of_faces() << " faces:" << std::endl;
    for (fit = arr.faces_begin(); fit != arr.faces_end(); ++fit) {
        poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
        Arrangement_2::Edge_const_iterator eit;
        OGRPolygon pol;
        OGRLinearRing ring;

        if (fit->is_unbounded()) {
            //use the bounding box
            double xmin=extent.xmin();
            double ymin=extent.ymin();
            double xmax=extent.xmax();
            double ymax=extent.ymax();
            ring.addPoint(xmin,ymin);
            ring.addPoint(xmin,ymax);
            ring.addPoint(xmax,ymax);
            ring.addPoint(xmax,ymin);
            std::cout << "Unbounded face. " << std::endl;
        } else {
            Arrangement_2::Ccb_halfedge_const_circulator curr = fit->outer_ccb();
            do  {

                //curr->curve();
                vector<double>x,y;
                arcAsLinestring(curr->curve(),x,y);
                double xi,yi;
                for(unsigned int i=0; i<x.size();i++) {
                    if( ! ((x[i]!=x[i]) || (y[i]!=y[i]) ) ) { //not nan
                        Point_2 contrained=constrainInside(Point_2(x[i],y[i]), extent);
                        xi=CGAL::to_double(contrained.x());
                        yi=CGAL::to_double(contrained.y());
                        ring.addPoint(xi,yi);
                        //if (isInside(Point_2(x[i],y[i]),extent)) {

                        //}
                    }
                }


            } while (++curr != fit->outer_ccb());
        }
        pol.addRing(&ring);
        for (eit = arr.edges_begin(); eit != arr.edges_end(); ++eit) {

            vector<double>x,y;
            arcAsLinestring(eit,x,y);


            OGRLineString line;
            for(unsigned int i=0; i<x.size();i++) {
                if( ! ((x[i]!=x[i]) || (y[i]!=y[i]) ) ) {
                    line.addPoint(x[i],y[i]);
                }
            }
        //if (pol.>0) {
            poFeature->SetGeometry( &pol );
            if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )  {
                printf( "Failed to create feature in shapefile.\n" );
                exit( 1 );
            }
        //}

        OGRFeature::DestroyFeature( poFeature );

    }
    OGRDataSource::DestroyDataSource( poDS );
    CPLFree(options);
    return 0;
}*/

bool GeometryReader::isInside(Point_2 p, Bbox_2 box) {
    double px,py;
    px=CGAL::to_double(p.x());
    py=CGAL::to_double(p.y());
    return ((px>box.xmin()) && (px<box.xmax()) && (py>box.ymin()) && (py<box.ymax()));
}

Point_2 GeometryReader::constrainInside(Point_2 p, Bbox_2 box) {
    double px,py;
    px=CGAL::to_double(p.x());
    py=CGAL::to_double(p.y());
    if (px<box.xmin()) px=box.xmin();
    if (px>box.xmax()) px=box.xmax();
    if (py<box.ymin()) py=box.ymin();
    if (py>box.ymax()) py=box.ymax();
    return Point_2(px,py);
}


bool GeometryReader::exportMWVDiagramToGDAL(MWVDiagram &diagram,std::string filename) {
    //FIXME handle holes
    const char *pszDriverName = "ESRI Shapefile";
    OGRSFDriver *poDriver;
    poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
    if( poDriver == NULL )    {
        printf( "%s driver not available.\n", pszDriverName );
        return 1;
    }
    char** options=NULL;
    options=CSLAddNameValue(options,"OVERWRITE","YES");

    OGRDataSource *poDS;
    poDS = poDriver->CreateDataSource( filename.c_str(), options );
    if( poDS == NULL )    {
        printf( "Creation of output file failed.\n" );
        return  1 ;
    }
    OGRLayer *poLayer;
    if (poDS->GetLayerByName(filename.c_str())) {
        poDS->DeleteLayer(0);
    }
    poLayer = poDS->CreateLayer( filename.c_str(), NULL, wkbMultiPolygon, NULL );

    if( poLayer == NULL ) {
        printf( "Layer creation failed.\n" );
        return 1;
    }
    OGRFeature *poFeature;


    std::cout << diagram.size() << " areas created." << std::endl;
    for (MWVDiagram::iterator polIt=diagram.begin(); polIt!=diagram.end(); ++polIt) {
        poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
        std::list<Polygon_with_holes_2> res;
        std::list<Polygon_with_holes_2>::const_iterator it;
        //cout << "Polygons: "<<polIt->number_of_polygons_with_holes() << endl;

        polIt->polygons_with_holes (std::back_inserter (res));
        mwv_base mwvHelper;
        OGRMultiPolygon mpol;
        for (it=res.begin();it!=res.end();++it) {
            OGRPolygon pol;
            OGRLinearRing ring;
            Polygon_2::Curve_const_iterator cIt;
            //cout << "Curves: "<<it->outer_boundary().number << endl;
            for(cIt=it->outer_boundary().curves_begin(); cIt!=it->outer_boundary().curves_end();++cIt) {

                vector<double>x,y;
                mwvHelper.arcAsLinestring(*cIt,x,y);

                for(unsigned int i=0; i<x.size()-1;i++) {
                    if( ! ((x[i]!=x[i]) || (y[i]!=y[i]) ) ) { //not nan
                        ring.addPoint(x[i],y[i]);
                    }
                }


            }
            OGRPoint p0;
            ring.getPoint(0,&p0);
            ring.addPoint(&p0);
            pol.addRing(&ring); //if (ring.IsValid())
            //cout<<ring.getNumPoints()<<endl;
            mwv_base mwvHelper;
            //reading holes
            Polygon_with_holes_2::Hole_const_iterator hit;
            //std::cout << "  " << it->number_of_holes() << " holes:" << std::endl;
            for (hit = it->holes_begin(); hit != it->holes_end(); ++hit) {
                OGRLinearRing innerRing;
                for(cIt=hit->curves_begin(); cIt!=hit->curves_end();++cIt) {
                    vector<double>x,y;

                    mwvHelper.arcAsLinestring(*cIt,x,y);
                    for(unsigned int i=0; i<x.size()-1;i++) {
                        if( ! ((x[i]!=x[i]) || (y[i]!=y[i]) ) ) { //not nan
                            innerRing.addPoint(x[i],y[i]);
                        }
                    }
                }
                OGRPoint p0;
                innerRing.getPoint(0,&p0);
                innerRing.addPoint(&p0);
                pol.addRing(&innerRing);

            }
            if (pol.getBoundary()->IsValid()) {
                mpol.addGeometry(&pol);
            }
        }

        poFeature->SetGeometry( &mpol );
        if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )  {
            printf( "Failed to create feature in shapefile.\n" );
            exit( 1 );
        }
        OGRFeature::DestroyFeature( poFeature );

    }
    OGRDataSource::DestroyDataSource( poDS );
    CPLFree(options);
    return 0;
}

bool GeometryReader::getObstaclesFromGDAL(std::string filename, obstacleVector &obstacles, int sizeLimit) {
    OGRDataSource *poDS;
    poDS = OGRSFDriverRegistrar::Open( filename.c_str(), FALSE );
    if( poDS == NULL ) return false;
    OGRLayer  *poLayer= poDS->GetLayer(0);
    if( poLayer == NULL ) return false;
    OGRFeature *poFeature;

    OGRGeometry *poGeometry;
    poLayer->ResetReading();
    int i=0;
    while( ( (poFeature = poLayer->GetNextFeature()) != NULL ) && ((sizeLimit==0) || (i<sizeLimit))) {
        i++;
        poGeometry = poFeature->GetGeometryRef();
        //cout<<poGeometry->getGeometryType()<<endl;
        //cout<<wkbPoint<<endl;
        if( poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbLineString )  {
           OGRLineString *poLine = (OGRLineString *) poGeometry;
           OGRPoint poPoint;
           obstacle ob;
           for (int j=0; j<poLine->getNumPoints(); j++) {
               poLine->getPoint(j,&poPoint);
               ob.push_back(Point_2(poPoint.getX(),poPoint.getY()));
           }
           //cout<< poPoint->getX()<<", "<<poPoint->getY()<<endl;
           obstacles.push_back(ob);
        } else  {
           printf( "no point geometry\n" );
        }
    }

    return true;

}

vector<string> GeometryReader::listWeightAttributes(std::string filename) {
    OGRDataSource *poDS;
    vector<string> result;
    poDS = OGRSFDriverRegistrar::Open( filename.c_str(), FALSE );
    if( poDS == NULL ) return result;
    OGRLayer  *poLayer= poDS->GetLayer(0);
    if( poLayer == NULL ) return result;
    OGRFeatureDefn *attributes=poLayer->GetLayerDefn();
    for (int i=0;i<attributes->GetFieldCount();i++) {
        OGRFieldType t=attributes->GetFieldDefn(i)->GetType();
        if ((t==OFTInteger) || (t==OFTReal)) {
            result.push_back(attributes->GetFieldDefn(i)->GetNameRef());
        }
    }

    return result;
}

bool GeometryReader::exportAGToGDAL(Apollonius_graph &ag,std::string filename) {
    //FIXME handle holes
    const char *pszDriverName = "ESRI Shapefile";
    OGRSFDriver *poDriver;
    poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName );
    if( poDriver == NULL )    {
        printf( "%s driver not available.\n", pszDriverName );
        return 1;
    }
    char** options=NULL;
    options=CSLAddNameValue(options,"OVERWRITE","YES");

    OGRDataSource *poDS;
    poDS = poDriver->CreateDataSource( filename.c_str(), options );
    if( poDS == NULL )    {
        printf( "Creation of output file failed.\n" );
        return  1 ;
    }
    OGRLayer *poLayer;
    if (poDS->GetLayerByName(filename.c_str())) {
        poDS->DeleteLayer(0);
    }
    poLayer = poDS->CreateLayer( filename.c_str(), NULL, wkbMultiPolygon, NULL );

    if( poLayer == NULL ) {
        printf( "Layer creation failed.\n" );
        return 1;
    }
    OGRFeature *poFeature;


    std::cout << ag.number_of_faces() << " areas created." << std::endl;
    for (Apollonius_graph::Vertex_handle vIt=ag.all_vertices_begin();vIt!=ag.all_vertices_end();++vIt) {
        Apollonius_graph::Vertex v;

    }
    /*for (Apollonius_graph::Face_handle fIt=ag.finite_faces_begin(); fIt!=ag.finite_faces_end();++fIt) {
        poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
        AWTraits::Object_2 fDual=ag.dual(fIt);
        Site_2 pDual=fDual;*/



        /*std::list<Polygon_with_holes_2> res;
        std::list<Polygon_with_holes_2>::const_iterator it;
        //cout << "Polygons: "<<polIt->number_of_polygons_with_holes() << endl;

        polIt->polygons_with_holes (std::back_inserter (res));
        mwv_base mwvHelper;
        OGRMultiPolygon mpol;
        for (it=res.begin();it!=res.end();++it) {
            OGRPolygon pol;
            OGRLinearRing ring;
            Polygon_2::Curve_const_iterator cIt;
            //cout << "Curves: "<<it->outer_boundary().number << endl;
            for(cIt=it->outer_boundary().curves_begin(); cIt!=it->outer_boundary().curves_end();++cIt) {

                vector<double>x,y;
                mwvHelper.arcAsLinestring(*cIt,x,y);

                for(unsigned int i=0; i<x.size()-1;i++) {
                    if( ! ((x[i]!=x[i]) || (y[i]!=y[i]) ) ) { //not nan
                        ring.addPoint(x[i],y[i]);
                    }
                }


            }
            OGRPoint p0;
            ring.getPoint(0,&p0);
            ring.addPoint(&p0);
            pol.addRing(&ring); //if (ring.IsValid())
            //cout<<ring.getNumPoints()<<endl;
            mwv_base mwvHelper;
            //reading holes
            Polygon_with_holes_2::Hole_const_iterator hit;
            //std::cout << "  " << it->number_of_holes() << " holes:" << std::endl;
            for (hit = it->holes_begin(); hit != it->holes_end(); ++hit) {
                OGRLinearRing innerRing;
                for(cIt=hit->curves_begin(); cIt!=hit->curves_end();++cIt) {
                    vector<double>x,y;

                    mwvHelper.arcAsLinestring(*cIt,x,y);
                    for(unsigned int i=0; i<x.size()-1;i++) {
                        if( ! ((x[i]!=x[i]) || (y[i]!=y[i]) ) ) { //not nan
                            innerRing.addPoint(x[i],y[i]);
                        }
                    }
                }
                OGRPoint p0;
                innerRing.getPoint(0,&p0);
                innerRing.addPoint(&p0);
                pol.addRing(&innerRing);

            }
            if (pol.getBoundary()->IsValid()) {
                mpol.addGeometry(&pol);
            }
        }

        poFeature->SetGeometry( &mpol );
        if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )  {
            printf( "Failed to create feature in shapefile.\n" );
            exit( 1 );
        }*/
        /*OGRFeature::DestroyFeature( poFeature );

    }*/
    OGRDataSource::DestroyDataSource( poDS );
    CPLFree(options);
    return 0;
}
