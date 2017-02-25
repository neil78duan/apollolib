/* file apoUiMainEditor.h
 *
 * main editor window
 *
 * create by duan
 * 2017.2.9
 */


#ifndef APOUIMAINEDITOR_H
#define APOUIMAINEDITOR_H

#include "src/apoUiExeNode.h"
#include "src/apoUiBezier.h"
#include <QWidget>
#include <QBoxLayout>
#include <QMouseEvent>
#include <QScrollArea>
#include <QVector>

class apoUiMainEditor : public QWidget
{
    Q_OBJECT
public:
    explicit apoUiMainEditor(QWidget *parent = 0);
    ~apoUiMainEditor(){}


public slots:

private:

    void popMenuAddnewTrigged();

	void popMenuDeleteTrigged();

	void popMenuDisconnectTrigged();

    void paintEvent(QPaintEvent *event);
	void contextMenuEvent(QContextMenuEvent *event);

	
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

	enum eDragType{ E_MOUSE_TYPE_MOVE, E_MOUSE_TYPE_DRAG_DRAW};
	enum ePopMenuType{  E_POP_DEL, E_POP_DISCONNECT };

	eDragType m_curDragType; //move subNode or connect-line
	ePopMenuType m_popMenuType;

	QWidget *m_dragSrc;
	QWidget *m_popSrc;

	QPoint m_moveOffset;
    QPoint m_curClicked ;

	apoUiBezier *m_drawingBezier;

	QVector<apoUiBezier> m_beziersVct;

};

#endif // APOUIMAINEDITOR_H
