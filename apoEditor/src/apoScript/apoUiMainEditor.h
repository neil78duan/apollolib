/* file apoUiMainEditor.h
 *
 * main editor window
 *
 * create by duan
 * 2017.2.9
 */


#ifndef APOUIMAINEDITOR_H
#define APOUIMAINEDITOR_H

#include "apoScript/apoUiExeNode.h"
#include "apoScript/apoUiBezier.h"
#include "apoScript/apoEditorSetting.h"
#include "apoScript/apoUiDetailView.h"
#include "logic_parser/logic_compile.h"

#include <map>
#include <QWidget>
#include <QBoxLayout>
#include <QMouseEvent>
#include <QScrollArea>
#include <QVector>
#include <QPoint>
#include <QTableWidget>

class apoBaseSlotCtrl;

class apoUiMainEditor : public QWidget
{
    Q_OBJECT
public:
	enum { DEFAULT_START_X = 20, DEFAULT_START_Y = 400 };
	typedef QVector<QString> text_list_t;

    explicit apoUiMainEditor(QWidget *parent = 0);
    ~apoUiMainEditor(){}

	bool showFunction(ndxml *data, ndxml_root *xmlfile);
	void clearFunction();
	//void setSettingConfig(apoEditorSetting *setting) { m_setting = setting; }

	void curDetailChanged(apoBaseExeNode *exenode);
	bool setCurDetail(ndxml *xmlNode, bool inError= false);
	bool setCurNodeSlotSelected(ndxml *xmlParam, bool inError=false);
	
	QPoint getExenodeOffset() { return m_offset; }
	float setScale(float scale);
public slots:
	//void onExenodeDBClicked(apoBaseExeNode *exeNode, QMouseEvent * event);
	void onCurNodeChanged();
	void onNodeAddNewParam(apoBaseExeNode *node);

signals:
	void showExenodeSignal(apoBaseExeNode *exeNode);

private:
	void movedInScale(apoBaseExeNode *exeNode, const QPoint &screenPos);
	void savePosition(apoBaseExeNode *exeNode, const QPoint *pos=NULL);
	bool _getNodePos(ndxml *exeNode, QPoint &pos);
	void saveOffset(const QPoint &offset);
	void getOffset(QPoint &offset);

	ndxml *getUnconnectRoot(ndxml* xmlFunc);

	bool _createFuncEntry(ndxml *stepsBlocks, const QPoint &defaultPos);
	bool _showBlocks(apoBaseSlotCtrl *fromSlot, ndxml *stepsBlocks);
	void _showUnConnectBlocks(ndxml *functionXml);
	bool _showSubByslot(apoBaseSlotCtrl *subSlot);
	
	bool _showBools(apoBaseExeNode *entryNode, ndxml *entryXml); //show if else 
	bool _showLoop(apoBaseExeNode *entryNode, ndxml *stepsBlocks);
	bool _showValueComp(apoBaseExeNode *entryNode, ndxml *stepsBlocks);
	bool _showSWitch(apoBaseExeNode *entryNode, ndxml *stepsBlocks);

	apoBaseExeNode* _showExeNode(apoBaseSlotCtrl *fromSlot, ndxml *exeNode, const QPoint &pos);

	int _createParams(ndxml *xmlnode, apoBaseExeNode &exeNode);

	bool pushVarList(ndxml *xmlNode, apoBaseExeNode*nodeCtrl);
	bool _connectSlots(apoBaseSlotCtrl *fromSlot, apoBaseSlotCtrl *toSlot, apoUiBezier::eBezierType type = apoUiBezier:: LineParam );
	bool _connectParam(apoBaseExeNode *preNode, apoBaseExeNode *curNode);
	bool _removeBezier(apoUiBezier *connector);
	bool _removeExenode(apoBaseExeNode *node);
	//disconnect existed connector
	bool _disconnectRunSerq(apoBaseSlotCtrl *fromSlot, apoBaseSlotCtrl *toSlot);

	void _removeGotoConnect(apoBaseSlotCtrl *fromSlot, apoBaseSlotCtrl *toSlot);

	void _disConnectParam(apoBaseExeNode *changedNode);
	void _reConnectParam(apoBaseExeNode *changedNode);
	bool _removeConnector(apoBaseSlotCtrl *slot);
	

	bool testBuildConnector(apoBaseSlotCtrl *fromSlot, apoBaseSlotCtrl *toSlot);
	bool trytoBuildConnector(apoBaseSlotCtrl *fromSlot, apoBaseSlotCtrl *toSlot);
	bool buildParamConnector(apoBaseSlotCtrl *fromSlot, apoBaseSlotCtrl *toSlot);
	bool buildRunSerqConnector(apoBaseSlotCtrl *fromSlot, apoBaseSlotCtrl *toSlot);

	void buildAllGotoLine();
	bool trytoBuildGotoConnector(apoBaseSlotCtrl *fromSlot, apoBaseSlotCtrl *toSlot);

	apoBaseExeNode *createExenode(const QPoint &pos);

    void popMenuAddnewTrigged();
	void popMenuDeleteTrigged(); 
	void popMenuAddLabelTrigged();
	void popMenuDelLabelTrigged();
	void popMenuCloseParamTrigged();
	void popMenuDisconnectTRigged();
	void popMenuCenterTrigged(); //move view to center

    void paintEvent(QPaintEvent *event);
	void contextMenuEvent(QContextMenuEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
	//void wheelEvent(QWheelEvent *event);
	void dragTo(const QPoint &offset);	
	void saveCurPosWithOffset();




	void onFileChanged();
	ndxml *createSubNode(ndxml *xmlRoot);

	enum eDragType{
		E_MOUSE_TYPE_MOVE_NODE,
		E_MOUSE_TYPE_CONNECT_SLOT,
		E_MOUSE_TYPE_MOVE_VIEW,
		E_MOUSE_TYPE_NONE
	};
	//enum ePopMenuType{  E_POP_DEL, E_POP_DISCONNECT };

	enum eDefaultStepSize { X_STEP = apoBaseExeNode::E_LINE_WIDTH + 50, Y_STEP = 20 };

	bool m_bInDrag;
	eDragType m_curDragType; //move subNode or connect-line
	//ePopMenuType m_popMenuType;

	apoBaseSlotCtrl *m_connectToSlot;
	QWidget *m_dragSrc;
	QWidget *m_popSrc;

	QPoint m_moveOffset; //offset step when you move mouse
    QPoint m_curClicked ;

	apoUiBezier *m_drawingBezier;

	float m_scale;
	QVector<apoUiBezier*> m_beziersVct;

	typedef std::map<std::string, apoBaseExeNode*>varinat_map_t;
	varinat_map_t m_varMap;
	varinat_map_t m_labelsMap;
	varinat_map_t m_gotoMap;


	ndxml *m_editedFunction;
	//ndxml *m_curDetailXml;
	ndxml *m_scriptFileXml;

	apoBaseExeNode *m_funcEntry;
	apoBaseExeNode *m_curDetailNode;
	apoEditorSetting *m_setting;

	//editing info
	//apoBaseSlotCtrl *m_editingToNext;  //connector to next exe node 
	int m_startX;
	int m_startY;

	QPoint m_offset;
};


#endif // APOUIMAINEDITOR_H
