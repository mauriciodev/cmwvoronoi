#include <voronoidiagramplugin.h>
#include <VoronoiWindow.h>
#include <SimplifyWindow.h>

//Qt include file
#include <qaction.h>
#include <qmessagebox.h>

VoronoiDiagramPlugin::VoronoiDiagramPlugin(PluginParameters* params):
QObject(),
TViewAbstractPlugin(params)
{
	loadTranslationFiles("voronoi_");
}

VoronoiDiagramPlugin::~VoronoiDiagramPlugin()
{
	end();
}

void VoronoiDiagramPlugin::init()
{
	TerraViewBase* tview = getMainWindow();

	try
	{
		if(tview)
		{
            // Voronoi
            /*showVoronoiWinAction_ = new QAction(0);
            showVoronoiWinAction_->setMenuText(tr("Voronoi"));
            showVoronoiWinAction_->setText(tr("Voronoi"));
            showVoronoiWinAction_->setIcon(QIcon(QPixmap(":/images/voronoi.png")));*/

            // Voronoi Ponderado Multiplicativamente 
			showMWVoronoiWinAction_ = new QAction(0);
            showMWVoronoiWinAction_->setMenuText(tr("Multiplicatively Weighted Voronoi Diagram"));
            showMWVoronoiWinAction_->setText(tr("Multiplicatively Weighted Voronoi Diagram"));
			showMWVoronoiWinAction_->setIcon(QIcon(QPixmap(":/images/mwvoronoi.png")));

            /*/ Delaunay
            showDelaunayWinAction_ = new QAction(0);
            showDelaunayWinAction_->setMenuText(tr("Delaunay"));
            showDelaunayWinAction_->setText(tr("Delaunay"));
            showDelaunayWinAction_->setIcon(QIcon(QPixmap(":/images/delaunay.png")));*/

            // Douglas peucker generalization
            showGeneralizationWinAction_ = new QAction(0);
            showGeneralizationWinAction_->setMenuText(tr("Geometry simplification"));
            showGeneralizationWinAction_->setText(tr("Geometry Simplification"));
            showGeneralizationWinAction_->setIcon(QIcon(QPixmap(":/images/delaunay.png")));

			std::string mainMenuName = "TerraViewPlugins.";
			mainMenuName += (std::string)tr("Diagrams");

			QMenu* mnu = getPluginsMenu(mainMenuName);

			if(mnu != 0)
			{
                //mnu->addAction(showVoronoiWinAction_);
                mnu->addAction(showMWVoronoiWinAction_);
                //mnu->addAction(showDelaunayWinAction_);
                mnu->addAction(showGeneralizationWinAction_);
            }

            //connect(showVoronoiWinAction_, SIGNAL(activated()), this, SLOT(showVoronoiWindow()));
            connect(showMWVoronoiWinAction_, SIGNAL(activated()), this, SLOT(showMWVoronoiWindow()));
            //connect(showDelaunayWinAction_, SIGNAL(activated()), this, SLOT(showDelaunayWindow()));
            connect(showGeneralizationWinAction_, SIGNAL(activated()), this, SLOT(showGeneralizationWindow()));
		}
	}
	catch(...)
	{
		QMessageBox::critical(tview, tr("TerraView plug-in error"), tr("Can't create plug-in menu."));
		delete showVoronoiWinAction_;
		showVoronoiWinAction_ = 0;
        delete showMWVoronoiWinAction_;
		showMWVoronoiWinAction_ = 0;
        delete showDelaunayWinAction_;
		showDelaunayWinAction_ = 0;
        delete showGeneralizationWinAction_;
        showGeneralizationWinAction_ = 0;
	}
}

void VoronoiDiagramPlugin::end()
{
	delete showVoronoiWinAction_;
    delete showMWVoronoiWinAction_;
    delete showDelaunayWinAction_;
}

void VoronoiDiagramPlugin::showVoronoiWindow()
{
	VoronoiWindow win(params_);
	win.showWindow();
}

void VoronoiDiagramPlugin::showMWVoronoiWindow()
{
	VoronoiWindow win(params_,MWVoronoi);
	win.showWindow();
}

void VoronoiDiagramPlugin::showDelaunayWindow()
{
	VoronoiWindow win(params_,Delaunay);
	win.showWindow();
}

void VoronoiDiagramPlugin::showGeneralizationWindow()
{
    SimplifyWindow win(params_);
    win.showWindow();
}
