/* file apoBaseExeNode.h
 *
 * logic parser editor base exe-node
 *
 * create by duan
 *
 * 2017.2.7
 */

#ifndef _APO_BASE_EXENODE_H_
#define _APO_BASE_EXENODE_H_

#include "apoScrpit/apoUiParam.h"
#include "nd_common/nd_common.h"
#include "apoScrpit/apoUiParam.h"

#include <QWidget>
#include <QVector>
#include <QLabel>
#include <QPushButton>
#include <QString>


#define CREATE_CTRL_OBJECT(_qType,_text, _color, _outputVar) \
	do		{		\
		_qType *ctrl1 = new _qType(QString(_text) , this);	\
		ctrl1->resize(PARAM_CTRL_W, PARAM_CTRL_H);			\
        ctrl1->setStyleSheet("QLabel{background-color:"# _color ";}");	\
		ctrl1->move(x, y);									\
		ctrl1->show();										\
		ctrl1->setAttribute(Qt::WA_DeleteOnClose, true);	\
		_outputVar = ctrl1;									\
	} while (0)

class apoBaseExeNode : public QWidget
{
	Q_OBJECT
public:
	explicit apoBaseExeNode(QWidget *parent, ndxml *xmlnode, const char *tips);
	explicit apoBaseExeNode(QWidget *parent =NULL);
	virtual ~apoBaseExeNode();
	
	bool setNodeInfo(QWidget *parent, ndxml *xmlnode, bool showParam=true);
	void setSelected(bool isSelected);

	//call before InitCtrl ;
	void disableReturnVar();
	void disableToNext();
	void disableNewParam();
	void disableConnectIn();

	void enableOutParam();

	apoBaseSlotCtrl *toNext() { return m_toNextNode; }
	apoBaseSlotCtrl *returnVal(){ return m_returnValue; }
	apoBaseSlotCtrl *inNode() { return m_runInNode; }

	apoBaseExeNode *getMyPreNode();
	apoBaseExeNode *getMyNextNode();


	//void setName(const QString &name);
	void setTitle(const QString &title);
	QString getTitle();
	void setTips(const QString &tips);

	int getType() { return m_type; }
	void *getMyUserData() { return m_nodeXml; }
	void setUserData(void *data) { m_nodeXml = (ndxml *)data; }
	int getParamNum() { return (int)m_paramVct.size(); }
	apoBaseParam *getParam(int index) { return m_paramVct[index]; }

	apoBaseSlotCtrl* getFunctionParam(int index);
	int getFunctionParamIndex(apoBaseSlotCtrl* slot);
	
	bool isMyParam(apoBaseParam *paramCtrl);

	bool closeParam(apoBaseSlotCtrl *param);

	//void setMyUserData(void *userData) { m_myUserData = userData; }

	enum {
		PARAM_CTRL_W = 15,
		PARAM_CTRL_H = 10,
		MARGIN_X = 10,
		MARGIN_Y = 10,

		E_LINE_HEIGHT = 30,
		E_LINE_WIDTH = 175
	};


public slots:
    void onInputValAddClicked() ;
	void onAddFunctionInParam();

signals:
	void onParamCtrlDBclicked(apoBaseParam *paramCtrl, QMouseEvent * event);
	void onTitalCtrlDBclicked(QWidget *titleCtrl, QMouseEvent * event);
	void onDBclicked(apoBaseExeNode *exeNode,QMouseEvent * event);
	void NodeAddParamSignal(apoBaseExeNode *exeNode);
protected:

	void paintEvent(QPaintEvent *event);
	void mouseDoubleClickEvent(QMouseEvent * event);

	void trytoDrawConnectSlot();
	bool _parseParam(ndxml *xmlnode);
	apoBaseParam* _param2Ctrl(ndxml *xmlParam, ndxml *parent);
	

	virtual void destroy();
	void init(const QString &title );
	void initParamPos();
	apoBaseParam* insertNewParam();

	//virtual void paramCtrlDbClicked(apoBaseParam *paramCtrl);
	//virtual void nameCtrlDbClicked();

	virtual void onParamCreated(apoBaseParam *paramCtrl);
	virtual void onInit();
	
	bool m_disableRetVar;
	bool m_disableToNext;
	bool m_disableNewParam;
	bool m_disableIn;
	bool m_disableNewFuncParam;

	bool m_selected;
	int m_type;

    QPushButton *m_addNewParam;
	QPushButton *m_outParamAddNew;

	QVector<apoBaseParam *>m_paramVct;

	QVector<apoBaseSlotCtrl *>m_outParamVct;

	apoBaseSlotCtrl *m_toNextNode;
	apoBaseSlotCtrl *m_returnValue;
	apoBaseSlotCtrl *m_runInNode;

    QLabel *m_title ;

	//QLabel *m_name;

    //QPoint m_pos ;
	QSize m_size;

	QString m_tips;

	//void *m_myUserData;
	
	ndxml *m_nodeXml;

};

#endif // _APOUI_EXENODE_H_
