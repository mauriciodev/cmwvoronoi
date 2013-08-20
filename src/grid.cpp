 
#include "grid.h"

grid::grid(int Cols, int Rows) {
    this->outGrid.resize(Rows*Cols);
    this->rows=Rows;
    this->cols=Cols;

}

bool grid::set(int row, int col, int value) {
    unsigned int pos=row*cols+col;
    if (pos<this->outGrid.size()) {
        outGrid[pos=row*cols+col]=value;
        return true;
    } else {return false;  }
}

bool grid::setMapping(double minX, double minY, double maxX, double maxY) {
    this->minX=minX;
    this->minY=minY;
    this->maxX=maxX;
    this->maxY=maxY;
    this->affine[0]=minX;
    this->affine[3]=minY;
    ///this->affine[1];
    this->affine[1]=(maxX-minX)/this->cols;
    this->affine[2]=0;
    this->affine[4]=(maxY-minY)/this->rows;
    this->affine[5]=0;
    return ( (minX<maxX) && (minY <maxY) );
}

bool grid::mapRowCol(int row, int col, double &x, double &y) {
    x=affine[0]+affine[1]*col+affine[2]*row;
    y=affine[3]+affine[4]*col+affine[5]*row;
    return checkExtents(x,y);
}

bool grid::checkExtents(double x, double y)
{
    return ( (x>this->minX) && (x<this->maxX) && (y>this->minY) && (y<this->maxY)  );
}

bool grid::test()
{
    cout<<"Initiating grid class tests"<<endl;
    grid g(10,15);
    g.setMapping(10,20,110,440);

    return true;
}

bool grid::saveToGdal(string filename)
{
    return true;
}
