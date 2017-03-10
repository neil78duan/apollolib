/* file editorFrame.h
 * editor frame for apollo editor
 *
 * 21017.2.16
 * create by duan
 */

#ifndef EDITORFRAME_H
#define EDITORFRAME_H

#include "apoScript/mainwindow.h"

class EditorFrame : public MainWindow
{
    Q_OBJECT
public:
    explicit EditorFrame(QWidget *parent = 0);

    void setHostWidget(QWidget *host) ;

	typedef MainWindow myBase;
signals:

public slots:

private:
	virtual void closeEvent(QCloseEvent *event);

	QWidget *m_myHost;
};

#endif // EDITORFRAME_H
