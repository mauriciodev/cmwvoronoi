#include <iostream>
#include <QApplication>
#include "UIVoronoi.h"

using namespace std;

#include <qdatetime.h>


bool startUI(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setOrganizationName("INPE");
    app.setApplicationName("CMWVoronoi");
    CMWVDialog mainWin;


    mainWin.show();

    //SimplifyWindow sim;
    //sim.show();
    return app.exec();
}



int main(int argc, char *argv[]){


    return startUI(argc, argv);


    //int limit=91;
    //pointSet.resize(limit)
    //weights.resize(limit);


    return 0;
}

