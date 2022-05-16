#include "mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	setGeometry(300, 150, 800, 450);

	imagewidget_ = new ImageWidget();
	setCentralWidget(imagewidget_);

	CreateActions();
	CreateMenus();
	CreateToolBars();
	CreateStatusBar();
}

MainWindow::~MainWindow()
{

}

void MainWindow::closeEvent(QCloseEvent* e)
{

}

void MainWindow::paintEvent(QPaintEvent* paintevent)
{

}

void MainWindow::CreateActions()
{

	action_open_ = new QAction(QIcon("./images/open.jpg"), tr("&Open..."), this);
	action_open_->setShortcuts(QKeySequence::Open);
	action_open_->setStatusTip(tr("Open an existing file"));
	connect(action_open_, &QAction::triggered, imagewidget_, &ImageWidget::Open);

	action_save_ = new QAction(QIcon("./images/save.jpg"), tr("&Save"), this);
	action_save_->setShortcuts(QKeySequence::Save);
	action_save_->setStatusTip(tr("Save the document to disk"));
	connect(action_save_, &QAction::triggered, imagewidget_, &ImageWidget::Save);
	// connect ...

	action_saveas_ = new QAction(tr("Save &As..."), this);
	action_saveas_->setShortcuts(QKeySequence::SaveAs);
	action_saveas_->setStatusTip(tr("Save the document under a new name"));
	connect(action_saveas_, &QAction::triggered, imagewidget_, &ImageWidget::SaveAs);

	action_restore_ = new QAction(tr("Restore"), this);
	action_restore_->setStatusTip(tr("Show origin image"));
	connect(action_restore_, &QAction::triggered, imagewidget_, &ImageWidget::Restore);

	action_setPolygon_ = new QAction(tr("InitialZeroLevel"), this);
	action_setPolygon_->setStatusTip(tr("Initial zero level set"));
	connect(action_setPolygon_, &QAction::triggered, imagewidget_, &ImageWidget::initialPolygon);

	action_active_contour_ = new QAction(tr("Active Contour"), this);
	action_active_contour_->setStatusTip(tr("Find Boundary"));
	connect(action_active_contour_, &QAction::triggered, imagewidget_, &ImageWidget::actContour);
}

void MainWindow::CreateMenus()
{
	menu_file_ = menuBar()->addMenu(tr("&File"));
	menu_file_->setStatusTip(tr("File menu"));
	menu_file_->addAction(action_open_);
	menu_file_->addAction(action_save_);
	menu_file_->addAction(action_saveas_);
}

void MainWindow::CreateToolBars()
{
	toolbar_file_ = addToolBar(tr("File"));
	toolbar_file_->addAction(action_open_);
	toolbar_file_->addAction(action_save_);

	// Add separator in toolbar 
	toolbar_file_->addSeparator();
	toolbar_file_->addAction(action_setPolygon_);
	toolbar_file_->addAction(action_restore_);
	toolbar_file_->addAction(action_active_contour_);
	
}

void MainWindow::CreateStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}
