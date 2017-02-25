/* file apoUiMainEditor.h
 *
 * main editor window
 *
 * create by duan
 * 2017.2.9
 */



#include "src/apoUiMainEditor.h"
#include "src/apoBaseExeNode.h"
#include <QMenu>
#include <QContextMenuEvent>
#include <QBoxLayout>

apoUiMainEditor::apoUiMainEditor(QWidget *parent) : 
	QWidget(parent), 
	m_dragSrc(NULL), m_drawingBezier(NULL), m_popSrc(0)
{
	m_curDragType = E_MOUSE_TYPE_MOVE;
	m_popMenuType = E_POP_DEL;

}


void apoUiMainEditor::popMenuAddnewTrigged()
{
	//apoUiExeNode *exeNode = new apoUiExeNode(this, "hello world test1", 3);	
	//apoBaseExeNode *exeNode = new apoBaseExeNode(this, "test-exe-node", "HELLO-WORLD", 2);
	//
	
	static int g_index = 0;
	apoBaseExeNode *exeNode = g_apoCreateExeNode(g_index, this);
	if (exeNode){
		exeNode->move(m_curClicked);
		exeNode->show(); //must call show functiong	
	}
	
	++g_index;
	if (g_index >= EAPO_EXE_NODE_NUMBER){
		g_index = 0;
	}
	
}

void apoUiMainEditor::popMenuDeleteTrigged()
{

}

void apoUiMainEditor::popMenuDisconnectTrigged()
{

}


void apoUiMainEditor::contextMenuEvent(QContextMenuEvent *event)
{
#define SHOW_POP_MENU(_TITLE, _FUNCNAME) \
	do 	{	\
		QMenu *popMenu = new QMenu(this);	\
		QAction *add_node = new QAction(this);	\
		add_node->setText(tr(_TITLE));			\
		popMenu->addAction(add_node);			\
		connect(add_node, &QAction::triggered, this, &_FUNCNAME);	\
		popMenu->exec(QCursor::pos());			\
	} while ( 0 )

	m_popSrc = NULL;

	QWidget *w = this->childAt(event->pos());
	if (w == NULL) {
		m_curClicked = event->pos();
		
		SHOW_POP_MENU("add", apoUiMainEditor::popMenuAddnewTrigged);
	}
	else {
		apoBaseExeNode*pDelNode = dynamic_cast<apoBaseExeNode*>(w);
		if (pDelNode) {
			m_popSrc = w;
			m_popMenuType = E_POP_DEL;
			SHOW_POP_MENU("delete", apoUiMainEditor::popMenuDeleteTrigged);
		}
		else if (dynamic_cast<apoBaseSlotCtrl*>(w))	{

			m_popSrc = w;
			m_popMenuType = E_POP_DISCONNECT;
			SHOW_POP_MENU("disconnect", apoUiMainEditor::popMenuDisconnectTrigged);
		}
	}
	
}

void apoUiMainEditor::mousePressEvent(QMouseEvent *event)
{
	if (Qt::RightButton == event->button())	{
		return;
	}
	

	m_dragSrc = NULL; 
	QWidget *w = this->childAt(event->pos());
	
	if (w == NULL){
		return;
	}
	
	if (dynamic_cast<apoBaseExeNode*>(w))	{
		m_dragSrc = w;
		m_curDragType = E_MOUSE_TYPE_MOVE;
		m_moveOffset = m_dragSrc->mapFromGlobal(event->globalPos());
	}
	else if (dynamic_cast<apoBaseSlotCtrl*>(w)) {
		m_dragSrc = w;

		m_curDragType = E_MOUSE_TYPE_DRAG_DRAW;
		m_drawingBezier = new apoUiBezier(this, event->pos(), event->pos());
	}



    //m_drawingBezier = new apoUiBezier(this, event->pos(), event->pos());
	this->update();
}

void apoUiMainEditor::mouseReleaseEvent(QMouseEvent *event)
{
    if(!m_dragSrc)
        return ;

    if(E_MOUSE_TYPE_MOVE==m_curDragType){

    }
    else {
        QWidget *w = this->childAt(event->pos());

        if (w &&dynamic_cast<QLabel*>(w) ){
            m_drawingBezier->paintTo(event->pos());
            m_beziersVct.push_back(*m_drawingBezier);
        }

        delete m_drawingBezier;
        m_drawingBezier = 0;
        this->update();
    }

    /*if (m_drawingBezier){
		m_drawingBezier->paintTo(event->pos());

		m_beziersVct.push_back(*m_drawingBezier);
		delete m_drawingBezier;
		m_drawingBezier = 0;
		this->update();
    }*/
	m_dragSrc = NULL;
}

void apoUiMainEditor::mouseMoveEvent(QMouseEvent *event)
{
    /*if (m_drawingBezier){
		m_drawingBezier->paintTo(event->pos());
		m_drawingBezier->paintEvent();

		this->update();
    }*/


	if (!m_dragSrc)	{
		return;
	}
	if (m_curDragType == E_MOUSE_TYPE_MOVE)	{
		QPoint newPos = event->pos() - m_moveOffset;
        m_dragSrc->move(newPos);
	}
	else {
		if (m_drawingBezier){
			m_drawingBezier->paintTo(event->pos());
			m_drawingBezier->paintEvent();
		}
	}

    this->update();
}

void apoUiMainEditor::paintEvent(QPaintEvent *event)
{
	if (m_drawingBezier) {
		m_drawingBezier->paintEvent();
	}

	QVector<apoUiBezier>::iterator it;
	for (it = m_beziersVct.begin(); it != m_beziersVct.end(); ++it) {
		it->paintEvent();
	}

//     QPainter painter(this);
//     painter.setPen(Qt::black);
//     painter.drawLine(startLabel->pos(), ctrlLabel1->pos());
//     painter.drawLine(ctrlLabel2->pos(), endLabel->pos());
//     painter.drawLine(endLabel->pos(), ctrlLabel3->pos());
//     painter.drawLine(ctrlLabel4->pos(), endLabel2->pos());
//
//     QPainterPath myPath;
//     myPath.moveTo(startLabel->pos());
//     myPath.cubicTo(ctrlLabel1->pos(), ctrlLabel2->pos(), endLabel->pos());
//     myPath.cubicTo(ctrlLabel3->pos(), ctrlLabel4->pos(), endLabel2->pos());
//     painter.setPen(Qt::red);
//     painter.drawPath(myPath);

    //m_mybezize.paintEvent();
    //m_myExecNode.paintEvent();
}


