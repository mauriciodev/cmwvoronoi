#ifndef UIVORONOI_H
#define UIVORONOI_H

#include <qdialog.h>
#include "voronoiBaseWindow.h"

namespace Ui {
    class UIVoronoi;
} // namespace Ui

class UIVoronoi : public QDialog
{
    Q_OBJECT

public:
    UIVoronoi(QWidget* parent = 0) : QDialog(parent){};
	~UIVoronoi(){};

public slots:
    virtual void okPushButton_clicked() = 0;
    virtual void generateLinesCheckBox_clicked() = 0;
    virtual void themeComboBox_activated(const QString&) = 0;
    virtual void helpPushButton_clicked() = 0;

public:
    Ui::UIVoronoi *ui;

	
};

#endif // UIVORONOI_H


