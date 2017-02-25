/* file apoBaseExeNode.cpp
 *
 * logic parser editor bezier
 *
 * create by duan
 *
 * 2017.2.7
 */

#include "apoBaseExeNode.h"
#include <QPainter>
#include <QMessageBox>
#include <QMouseEvent>



apoBaseParam::apoBaseParam(QWidget *parent, Qt::WindowFlags f) :
	apoBaseSlotCtrl( parent, f)
{
	m_slotType = apoBaseSlotCtrl::SLOT_PARAM;

}
apoBaseParam::apoBaseParam(const QString &text, QWidget *parent, Qt::WindowFlags f) : 
apoBaseSlotCtrl( text, parent, f)
{
	m_slotType = apoBaseSlotCtrl::SLOT_PARAM;
}
apoBaseParam::~apoBaseParam()
{

}


//////////////////////////////////////////////////////////////////////////
apoBaseExeNode::apoBaseExeNode(QWidget *parent, const char *title,const char*name, int param_num) :
	QWidget(parent), m_toNextNode(NULL), 
	m_returnValue(NULL), m_funcName(NULL), m_addNewParam(NULL), m_runInNode(NULL),
	m_name(NULL)
{

	m_disableRetVar = false ;
	m_disableToNext = false;
	m_disableNewParam = false;
	m_disableIn = false;
	m_type = 0;
    init(QString(title),QString(name), param_num) ;
}


apoBaseExeNode::apoBaseExeNode(QWidget *parent) :
	QWidget(parent), m_toNextNode(NULL),
	m_returnValue(NULL), m_funcName(NULL), m_addNewParam(NULL), m_runInNode(NULL),
	m_name(NULL)
{
	m_type = 0;
	m_disableRetVar = false;
	m_disableToNext = false;
	m_disableNewParam = false;
	m_disableIn = false;
}

apoBaseExeNode::~apoBaseExeNode()
{

}


bool apoBaseExeNode::InitCtrl(QWidget *parent, const char *nodeTitle, const char *name, int param_num)
{
	this->setParent(parent);
	init(QString(nodeTitle), QString(name), param_num);
	return true;
}

//call before create ;
void apoBaseExeNode::disableReturnVar()
{
	m_disableRetVar = true;
}
void apoBaseExeNode::disableToNext()
{
	m_disableToNext = true;
}
void apoBaseExeNode::disableNewParam()
{
	m_disableNewParam = true;
}

void apoBaseExeNode::disableConnectIn()
{
	m_disableIn = true;
}


void apoBaseExeNode::setName(const QString &name)
{
	if (m_name){
		m_name->setText(name);
	}
}

void apoBaseExeNode::setTips(const QString &tips)
{
	m_tips = tips;
}

void apoBaseExeNode::addnewClicked()
{
	int paramNum = m_paramVct.size();
	QString text = m_funcName->text();
	QString name;
	if (m_name) {
		name = m_name->text();
	}

	++paramNum;
	init(text,name, paramNum);

	onParamCreated(m_paramVct[paramNum - 1]);
	this->update();

}

void apoBaseExeNode::mouseDoubleClickEvent(QMouseEvent * event)
{
	apoBaseParam *paramCtrl;
	QWidget *w = this->childAt(event->pos());
	if (w == (QWidget*) m_name)	{
		nameCtrlDbClicked();
	}
	else if ((paramCtrl=dynamic_cast<apoBaseParam*>(w) )) {
		for (size_t i = 0; i < m_paramVct.size(); i++)	{
			if (paramCtrl == m_paramVct[i] )	{
				paramCtrlDbClicked(paramCtrl);
				break;
			}

		}
	}
	//QMessageBox::information(this, "window", "double clicked window!");
}

void apoBaseExeNode::paramCtrlDbClicked(apoBaseParam *paramCtrl)
{
	QMessageBox::information(this, "dbclick", "param ctrl");
}

void apoBaseExeNode::nameCtrlDbClicked()
{
	QMessageBox::information(this, "dbclick", "name control");

}


void apoBaseExeNode::onParamCreated(apoBaseParam *paramCtrl)
{

}


void apoBaseExeNode::onInit()
{

}

void apoBaseExeNode::destroy()
{
#define SAFE_CLOSE(a) \
	if(a) a->close() 

	SAFE_CLOSE(m_runInNode);
	SAFE_CLOSE(m_addNewParam);
	SAFE_CLOSE(m_toNextNode);
	SAFE_CLOSE(m_returnValue);

	SAFE_CLOSE(m_funcName);

	SAFE_CLOSE(m_name);
	

	for (int i = 0;  i<m_paramVct.size(); i++){
		SAFE_CLOSE(m_paramVct[i]);
	}

	m_paramVct.clear();
}

void apoBaseExeNode::init(const QString &title, const QString &name, int param_num)
{
	destroy();
    QLabel *label  ;
    int x = 0, y = MARGIN_Y ;  //START coord

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


    //connect in ctrl
	if (!m_disableIn)	{
		CREATE_CTRL_OBJECT(apoBaseSlotCtrl, "->", green, m_runInNode);
		m_runInNode->setSlotType(apoBaseSlotCtrl::SLOT_IN);
	}

    //init title label ;
    x = MARGIN_X;
    y = MARGIN_Y ;
	label = new QLabel(title, this);
	label->move(x + PARAM_CTRL_W, y);
	label->setAttribute(Qt::WA_DeleteOnClose, true);
	label->show();
    m_funcName = label ;	

	//init name 
	if (name.size() > 0){
		y += E_LINE_HEIGHT;
		label = new QLabel(name, this);
		label->move(x + PARAM_CTRL_W, y);
		label->setAttribute(Qt::WA_DeleteOnClose, true);
		label->show();
		m_name = label;

	}
    y+= E_LINE_HEIGHT * 1.5 ;

    //draw param
    x = 0 ;
    for(int i=0 ; i <param_num; ++i) {
		apoBaseParam *label;
		CREATE_CTRL_OBJECT(apoBaseParam, "param", blue, label);
        y+= E_LINE_HEIGHT ;
        m_paramVct.push_back(label);
    }

    //add new-param button
	if (!m_disableNewParam) {
		CREATE_CTRL_OBJECT(QPushButton, "+", green, m_addNewParam);
		connect(m_addNewParam, SIGNAL(clicked()), this, SLOT(addnewClicked()));
		y += E_LINE_HEIGHT;
	}


    if(param_num <2) {
        y += E_LINE_HEIGHT * (2 - param_num) ;
    }
    x += E_LINE_WIDTH ;
	
    m_size.setWidth(x + MARGIN_X);
    m_size.setHeight(y + MARGIN_Y);
	
    //return value
	if (!m_disableRetVar) {
		y -= E_LINE_HEIGHT *0.5;
		CREATE_CTRL_OBJECT(apoBaseSlotCtrl, ">>", yellow, m_returnValue);
		m_returnValue->setSlotType(apoBaseSlotCtrl::SLOT_RETURN_VALUE);

	}
    
    //init toNext label
	if (!m_disableToNext) {
		y -= E_LINE_HEIGHT;
		CREATE_CTRL_OBJECT(apoBaseSlotCtrl, "->", yellow, m_toNextNode);
		m_toNextNode->setSlotType(apoBaseSlotCtrl::SLOT_OUT);

	}
	onInit();
	this->setStyleSheet("background-color:white;");

}


void apoBaseExeNode::paintEvent(QPaintEvent *event)
{
    //draw frame
	this->resize(m_size);
	//this->move(m_pos);
	
	QPainter painter(this);	

	painter.setRenderHint(QPainter::Antialiasing, true);

	painter.setPen(QPen(Qt::red, 1));
	painter.drawLine(QPoint(1, E_LINE_HEIGHT), QPoint(m_size.width() - 3, E_LINE_HEIGHT));

	if (m_name) {
		painter.drawLine(QPoint(1, E_LINE_HEIGHT * 2), QPoint(m_size.width() - 3, E_LINE_HEIGHT * 2));
	}

	painter.setPen(QPen(Qt::red, 2));
	painter.drawRect(1,1,m_size.width()-2, m_size.height() -2);

	if (!m_tips.isEmpty()) {

		int x = MARGIN_X ;
		int y = E_LINE_HEIGHT * 1.5 ;
		if (m_name)		{
			y += E_LINE_HEIGHT;
		}
		painter.setPen(QPen(Qt::lightGray, 2));
		
		QFont font1;
		font1.setFamily(QString::fromUtf8("Harlow Solid Italic"));
		font1.setPointSize(14);
		//font3.setBold(true);
		font1.setWeight(50);
		painter.setFont(font1);
		painter.drawText(x, y, m_tips);
	}
}
