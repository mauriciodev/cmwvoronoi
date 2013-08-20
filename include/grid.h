#ifndef GRID_H
#define GRID_H
#include <vector>
#include <string>
#include <iostream>

using namespace std;

class grid {
public:
    int rows;
    int cols;
    grid(int Cols, int Rows);
    int get(int row,int col) {return row*cols+col;}
    bool set(int row,int col, int value);
    bool setMapping(double minX, double minY, double maxX, double maxY);
    bool mapRowCol(int row, int col, double &x, double &y);
    bool checkExtents(double x, double y);
    static bool test();
    bool saveToGdal(string filename);
private:
    vector<int> outGrid;
    double minX,maxX,minY,maxY;
    double affine[6]; 
}; 


#endif
