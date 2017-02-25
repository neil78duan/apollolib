/* file apoUiMainEditor.h
 *
 * main editor window
 *
 * create by duan
 * 2017.2.9
 */


#ifndef APOUIMAINEDITOR_H
#define APOUIMAINEDITOR_H

#include "apoScrpit/apoUiExeNode.h"
#include "apoScrpit/apoUiBezier.h"
#include "apoScrpit/apoEditorSetting.h"
#include "apoScrpit/apoUiDetailView.h"

#include <QWidget>
#include <QBoxLayout>
#include <QMouseEvent>
#include <QScrollArea>
#include <QVector>
#include <QPoint>
#include <QTableWidget>

class apoUiMainEditor : public QWidget
{
    Q_OBJECT
public:

	typedef QVector<QString> text_list_t;

    explicit apoUiMainEditor(QWidget *parent = 0);
    ~apoUiMainEditor(){}

	bool showFunction(ndxml *data,ndxml_root *xmlfile );
	void setSettingConfig(apoEditorSetting *setting) { m_setting = setting; }

	void clear();
	void curDetailChanged(apoBaseExeNode *exenode);

public slots:
	void onExenodeDBClicked(apoBaseExeNode *exeNode, QMouseEvent * event);

signals:
	void showExenodeDetail(apoBaseExeNode *exeNode);
private:

	bool _createFuncEntry(ndxml *stepsBlocks, const QPoint &defaultPos);
	bool _showBlocks(apoBaseSlotCtrl *inSlot, ndxml *stepsBlocks);
	apoBaseExeNode* _showExeNode(apoBaseSlotCtrl *inSlot, ndxml *exeNode, const QPoint &pos);
	int _createParams(ndxml *xmlnode, apoBaseExeNode &exeNode);
	bool _getNodePos(ndxml *exeNode, QPoint &pos);

	bool _connectSlots(apoBaseSlotCtrl *fromSlot, apoBaseSlotCtrl *toSlot);

    void popMenuAddnewTrigged();

	void popMenuDeleteTrigged();

	void popMenuDisconnectTrigged();

    void paintEvent(QPaintEvent *event);
	void contextMenuEvent(QContextMenuEvent *event);

	
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	//void wheelEvent(QWheelEvent *event);

	void dragTo(const QPoint &offset);

	//apoUiDetailView *getDetailWidget();

	//detail edit
	//int DisplaySelXml();
// 
// 	ndxml* GetSelXml(apoBaseExeNode* exeNode);
// 	bool _GetUserSelEnumVal(const apoUiMainEditor::text_list_t &enum_vals, QString &val);
// 	int _initShowCtrl(QTableWidget *show_ctrl);
// 	int ShowXMLValue(ndxml *xml_node, QTableWidget *show_ctrl, int expand);
// 	void ShowRow(const char *name, const char *val, ndxml* param, QTableWidget *show_ctrl);


	enum eDragType{ E_MOUSE_TYPE_MOVE, E_MOUSE_TYPE_DRAG_DRAW, E_MOUSE_TYPE_MOVE_VIEW};
	enum ePopMenuType{  E_POP_DEL, E_POP_DISCONNECT };

	enum eDefaultStepSize { X_STEP = apoBaseExeNode::E_LINE_WIDTH + 50, Y_STEP = 20 };

	eDragType m_curDragType; //move subNode or connect-line
	ePopMenuType m_popMenuType;

	QWidget *m_dragSrc;
	QWidget *m_popSrc;

	QPoint m_moveOffset;
    QPoint m_curClicked ;

	apoUiBezier *m_drawingBezier;

	QVector<apoUiBezier> m_beziersVct;

	ndxml *m_editedFunction;
	ndxml *m_curDetailXml;
	ndxml *m_scriptFileXml;

	apoBaseExeNode *m_curDetailNode;
	apoEditorSetting *m_setting;

	//editing info
	//apoBaseSlotCtrl *m_editingToNext;  //connector to next exe node 
	int m_startX;
	int m_startY;

	QPoint m_offset;
};


#endif // APOUIMAINEDITOR_H
