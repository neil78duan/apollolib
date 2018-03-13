/* file editorFrame.h
 * editor frame for apollo editor
 *
 * 21017.2.16
 * create by duan
 */

#include "editorFrame.h"
//#include "startdialog.h"
#include <QMessageBox>

EditorFrame::EditorFrame(QWidget *parent) : MainWindow(parent), m_myHost(0)
{

}


void EditorFrame::setHostWidget(QWidget *host)
{
	m_myHost = host;
}

void EditorFrame::closeEvent(QCloseEvent *event)
{
	if (myBase::checkNeedSave() )	{
		/*
		int ret = QMessageBox::question(this, tr("Question"), tr("The file not save,\nDo you save now?"),
			QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
			QMessageBox::Yes);

		if (QMessageBox::Yes == ret) {
			saveCurFile();
		}
		else if (QMessageBox::Cancel == ret) {
			event->ignore();
			return;
		}
		*/
		saveCurFile();
	}
	
	if (m_myHost){
		m_myHost->setVisible(true);
		m_myHost->activateWindow();
	}

}
