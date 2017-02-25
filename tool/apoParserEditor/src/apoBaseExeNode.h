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

#include <QWidget>
#include <QVector>
#include <QLabel>
#include <QPushButton>
#include <QString>


class apoBaseSlotCtrl : public QLabel
{
	Q_OBJECT
public:

	enum eBaseSlotType {
		SLOT_IN,		//connect in
		SLOT_OUT,		// connect-out
		SLOT_RETURN_VALUE,
		SLOT_PARAM,		//param slot
		SLOT_SUB_ENTRY,		//loop 
		SLOT_COND,		// if-else cond
		SLOT_UNKNOWN
	};

	explicit apoBaseSlotCtrl( QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags()) :
		QLabel(parent, f), m_slotType(SLOT_UNKNOWN)
	{

	}

	explicit apoBaseSlotCtrl(const QString &text, QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags()) :
		QLabel(text, parent, f), m_slotType(SLOT_UNKNOWN)
	{

	}
	~apoBaseSlotCtrl()
	{

	}

	eBaseSlotType slotType(){ return m_slotType; }
	void setSlotType(eBaseSlotType stype){  m_slotType = stype; }
protected:
	eBaseSlotType m_slotType;
private:

};


class apoBaseParam : public apoBaseSlotCtrl
{
	Q_OBJECT
public:
	explicit apoBaseParam(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	explicit apoBaseParam(const QString &text, QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~apoBaseParam();

private:

};

class apoBaseExeNode : public QWidget
{
	Q_OBJECT
public:
	explicit apoBaseExeNode(QWidget *parent, const char *nodeTitle, const char *name, int param_num);
	explicit apoBaseExeNode(QWidget *parent =NULL);
	virtual ~apoBaseExeNode();
	
	
	bool InitCtrl(QWidget *parent, const char *nodeTitle, const char *name, int param_num);

	//call before InitCtrl ;
	void disableReturnVar();
	void disableToNext();
	void disableNewParam();
	void disableConnectIn();
	
	void setName(const QString &name);
	void setTips(const QString &tips);

	int getType() { return m_type; }

	enum {
		PARAM_CTRL_W = 15,
		PARAM_CTRL_H = 10,
		MARGIN_X = 10,
		MARGIN_Y = 10,

		E_LINE_HEIGHT = 30,
		E_LINE_WIDTH = 150
	};


public slots:
    void addnewClicked() ;

protected:

	void paintEvent(QPaintEvent *event);
	void mouseDoubleClickEvent(QMouseEvent * event);
	

	virtual void destroy();
	void init(const QString &title ,const QString &name, int param_num);

	virtual void paramCtrlDbClicked(apoBaseParam *paramCtrl);
	virtual void nameCtrlDbClicked();

	virtual void onParamCreated(apoBaseParam *paramCtrl);
	virtual void onInit();
	
	bool m_disableRetVar;
	bool m_disableToNext;
	bool m_disableNewParam;
	bool m_disableIn;

	int m_type;

    QPushButton *m_addNewParam;
	QVector<apoBaseParam *>m_paramVct;
	apoBaseSlotCtrl *m_toNextNode;
	apoBaseSlotCtrl *m_returnValue;
	apoBaseSlotCtrl *m_runInNode;

    QLabel *m_funcName ;

	QLabel *m_name;

    //QPoint m_pos ;
	QSize m_size;

	QString m_tips;

};

#endif // _APOUI_EXENODE_H_
