#include "SimplifyWindow.h"


/*SimplifyWindow::SimplifyWindow(QWidget *parent) : QDialog(parent) //PluginParameters* pp,QWidget *parent) : QDialog(parent) //plugin_params_(pp),
{
    setupUi(this); // this sets up GUI
    //this->plugin_params_=pp;
}*/

SimplifyWindow::SimplifyWindow()  //PluginParameters* pp: plugin_params_(pp)
{
    /*! Selects what aditional parameters each diagram needs.*/
    //ui = (new Ui::UISimplify);
    //this->setupUi();
    this->exec();
}

void SimplifyWindow::ShowWindow(){
    /*if(plugin_params_->getCurrentDatabasePtr() == 0)
    {
        QMessageBox::critical(this, tr("Error"), tr("Connect to a database first."));
        return;
    }

    ui->themeComboBox->clear();

    TeThemeMap& themeMap = plugin_params_->getCurrentDatabasePtr()->themeMap();
    TeThemeMap::iterator themeIt;
    std::string delimiter;
    for(themeIt = themeMap.begin(); themeIt != themeMap.end(); ++themeIt)
    {
        TeTheme* t = dynamic_cast<TeTheme*>(themeIt->second);
        if(t == 0)
            continue;
        if(t->layer()->geomRep() & TePOINTS)
        if ((t->layer()->geomRep() & TeLINES) || (t->layer()->geomRep() & TePOLYGONS) )
        {
            if(delimiter.empty())
                delimiter = t->layer()->name();
            ui->themeComboBox->insertItem(QString(t->name().c_str()));
        }
    }

    if(ui->themeComboBox->count() == 0)
    {
        QMessageBox::information(this, tr("Information"), tr("No Theme with polygon/line representation found."));
        return;
    }

    */
    this->exec();
}

void SimplifyWindow::pushButton_clicked() {
/*
    std::string layerName = (std::string)ui->lineEdit_2->text();
    if(layerName.empty())
    {
        QMessageBox::information(this, tr("Information"), tr("Please, define a name to result Layer."));
        ui->lineEdit_2->setFocus();
        return;
    }


    //if(!VoronoiWindow::isLayerNameValid(layerName)) return;

    TeDatabase* db = plugin_params_->getCurrentDatabasePtr();
*/
    /*TeTheme* theme; = VoronoiWindow::getTheme(ui->themeComboBox->currentText().latin1());
    if(theme == 0)
    {
        QMessageBox::critical(this, tr("Error"), tr("Error getting the input Theme."));
        return;
    }*/
/*
    TePrecision::instance().setPrecision(TeGetPrecision(theme->layer()->projection()));

    stringstream ss;
    ss<< ((std::string)ui->lineEdit_2->text());

    double threshold;
    ss>>threshold;
*/
}

void loadThemes() {

}
