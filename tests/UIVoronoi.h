#ifndef UIVORONOI_H
#define UIVORONOI_H
#include "ui_UIVoronoi.h"
#include "cmwv_ps.h"
#include "geometryreader.h"
#include "mwv.h"


using namespace std;
#include <QFileDialog>
#include <qdatetime.h>
#include <QMessageBox>

class CMWVDialog : public QDialog, private  Ui_UIVoronoi
{
Q_OBJECT
public:
CMWVDialog (QWidget *parent=0);
public slots:
void okPushButton_clicked();
void themeBrowseClick();
void breaklinesBrowseClick();
void resultBrowseClick();
void delimiterBrowseClick();
//public signals:
//
//private:
//
};
#endif
