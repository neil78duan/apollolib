#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "src/apoUiMainEditor.h"

#include <QTextEdit>
#include <QDockWidget>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(tr("main editor"));
    apoUiMainEditor *pMainEditor = new apoUiMainEditor(this) ;
    setCentralWidget(pMainEditor);



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionViewList_triggered()
{
	QDockWidget *pDock = this->findChild<QDockWidget*>("FilelistView");
	if (pDock){
		pDock->close();

		return;
	}
	else {
		pDock = new QDockWidget(tr("Filelist Window"), this);
		pDock->setObjectName("FilelistView");
		pDock->setAttribute(Qt::WA_DeleteOnClose, true);

		QTextEdit *dockText1 = new QTextEdit();
		dockText1->setText("First Window,Try to move The title.");
		pDock->setWidget(dockText1);
		addDockWidget(Qt::LeftDockWidgetArea, pDock);

	}
	
}

void MainWindow::on_actionViewOutput_triggered()
{


	QDockWidget *pDock = this->findChild<QDockWidget*>("outputView");
	if (pDock){
		pDock->close();
		return;
	}
	else {
		pDock = new QDockWidget(tr("Output Window"), this);
		pDock->setObjectName("outputView");
		pDock->setAttribute(Qt::WA_DeleteOnClose, true);

		QTextEdit *dockText1 = new QTextEdit();
		dockText1->setText("Second Window,Try to move The title.");
		pDock->setWidget(dockText1);
		addDockWidget(Qt::LeftDockWidgetArea, pDock);

	}

}

