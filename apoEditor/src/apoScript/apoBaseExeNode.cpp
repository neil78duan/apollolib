/* file apoBaseExeNode.cpp
 *
 * logic parser editor bezier
 *
 * create by duan
 *
 * 2017.2.7
 */
#include "apoUiBezier.h"
#include "apoBaseExeNode.h"
#include "apoUiParam.h"
#include "apoScript/apoEditorSetting.h"
#include "logic_parser/logic_compile.h"
#include "logic_parser/logic_editor_helper.h"

#include <QPainter>
#include <QMessageBox>
#include <QMouseEvent>




//////////////////////////////////////////////////////////////////////////
apoBaseExeNode::apoBaseExeNode(QWidget *parent, ndxml *xmlnode, const char*tips) :
QWidget(parent), m_toNextNode(NULL), m_nodeXml(0), m_outParamAddNew(0),
	m_returnValue(NULL), m_title(NULL), m_addNewParam(NULL), m_runInNode(NULL)
{

	m_disableRetVar = false ;
	m_disableToNext = false;
	m_disableNewParam = false;
	m_disableIn = false;
	m_selected = false;
	m_disableNewFuncParam = true;
	m_type = 0;
	m_tips = tips;
	setNodeInfo(parent, xmlnode);
    //init(QString(title), param_num) ;
}


apoBaseExeNode::apoBaseExeNode(QWidget *parent) :
QWidget(parent), m_toNextNode(NULL), m_outParamAddNew(0),
	m_returnValue(NULL), m_title(NULL), m_addNewParam(NULL), m_runInNode(NULL)
{
	m_type = 0;
	m_disableRetVar = false;
	m_disableToNext = false;
	m_disableNewParam = false;
	m_disableIn = false;

	m_selected = false;
}

apoBaseExeNode::~apoBaseExeNode()
{

}


bool apoBaseExeNode::setNodeInfo(QWidget *parent, ndxml *xmlnode, bool showParam)
{
	m_nodeXml = xmlnode;
	

	this->setParent(parent);

	const char *dispName = LogicEditorHelper::_GetXmlName(m_nodeXml, NULL);
	if (dispName){
		const char *pRealDisp = strchr(dispName, '_');
		if (pRealDisp)	{
			++pRealDisp;
			dispName = pRealDisp;
		}
	}
	else {
		dispName = "Common";
	}

	const char *tip = LogicEditorHelper::_GetNodeComment(m_nodeXml);
	if (tip){
		m_tips = tip;
	}
	
	if (showParam){
		if (!_parseParam(xmlnode)) {
			return false;
		}
	}
	if (m_disableNewParam==false) {
		m_disableNewParam = true;
		const char *pIsCreate = ndxml_getattr_val(xmlnode, "create_template");
		if (pIsCreate && ndstristr(pIsCreate, "param")) {
			m_disableNewParam = false;
		}
	}

	init(QString(dispName));
	return true;

}


bool apoBaseExeNode::_parseParam(ndxml *xmlnode)
{
	int subNum = ndxml_getsub_num(xmlnode);
	for (int i = 0; i < subNum; i++) {
		ndxml *xmlParam = ndxml_getnodei(xmlnode, i);
		if (!xmlParam) {
			continue;
		}
		_param2Ctrl(xmlParam, xmlnode);
// 
// 		bool ret =
// 		if (!ret){
// 			return false;
// 		}
	}
	return true;
}


apoBaseParam* apoBaseExeNode::_param2Ctrl(ndxml *xmlParam, ndxml *parent)
{
	if (LogicEditorHelper::CheckHide(xmlParam)){
		return NULL;
	}
	apoEditorSetting*pSetting = apoEditorSetting::getInstant();
	
	const char *paramName = ndxml_getname(xmlParam);
	if (!paramName){
		return NULL;
	}

	const compile_setting *paramSetting = pSetting->getStepConfig(paramName);
	if (!paramSetting){
		return NULL;
	}

	ndxml *refnode = NULL;
	ndxml *myVal = NULL;
	ndxml *xmlName = NULL;

	if (paramSetting->ins_type == E_INSTRUCT_TYPE_PARAM_COLLECT){
		for (int i = 0; i < ndxml_num(xmlParam); i++) {

			ndxml *node1 = ndxml_getnodei(xmlParam, i);
			const char *subName = ndxml_getname(node1);
			if (!subName){
				continue;
			}
			const compile_setting *subSetting = pSetting->getStepConfig(subName);
			if (!subSetting || E_INSTRUCT_TYPE_PARAM != subSetting->ins_type){
				continue;
			}

			const char *isNameParam = ndxml_getattr_val(node1, "is_param_name");
			if (isNameParam && 0==ndstricmp(isNameParam,"yes"))	{
				xmlName = node1;
			}
			else {
				const char *restrictType = ndxml_getattr_val(node1, "restrict");
				if (restrictType) {
					refnode = ndxml_getnode(xmlParam, restrictType);
				}
				myVal = xmlParam;

				myVal = node1;
			}
		}
		parent = xmlParam;
	}
	//step 
	else if (paramSetting->ins_type == E_INSTRUCT_TYPE_PARAM )  {

		const char *restrictType = ndxml_getattr_val(xmlParam, "restrict");
		if (restrictType) {
			refnode = ndxml_getnode(parent, restrictType);
		}
		myVal = xmlParam;		
	}
	else {
		return NULL;
	}

	if (!myVal)	{
		return NULL;
	}

	apoBaseParam *pParamCtrl = new apoBaseParam(this);
	pParamCtrl->setParam(parent,myVal, refnode, xmlName);

	pParamCtrl->resize(PARAM_CTRL_W, PARAM_CTRL_H);
	pParamCtrl->setStyleSheet("QLabel{background-color:yellow;}");
	pParamCtrl->setAttribute(Qt::WA_DeleteOnClose, true);

	m_paramVct.push_back(pParamCtrl);

	return pParamCtrl;

}

// 
// bool apoBaseExeNode::InitCtrl(QWidget *parent, const char *nodeTitle,  int param_num)
// {
// 	this->setParent(parent);
// 	init(QString(nodeTitle), param_num);
// 	return true;
// }

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

void apoBaseExeNode::enableOutParam()
{
	m_disableNewFuncParam = false;
}

void setOutParamNum(int num);

void apoBaseExeNode::setSelected(bool isSelected)
{
	m_selected = isSelected;
}

void apoBaseExeNode::setTitle(const QString &title)
{
	if (m_title){
		m_title->setText(title);
		m_title->adjustSize();
	}
}

QString apoBaseExeNode::getTitle()
{
	if (m_title){
		return m_title->text();
	}
	return QString("Node");
}

void apoBaseExeNode::setTips(const QString &tips)
{
	m_tips = tips;
}

void apoBaseExeNode::onInputValAddClicked()
{
	apoBaseParam *paramCtrl = insertNewParam();
	if (paramCtrl)	{
		onParamCreated(paramCtrl);
		this->update();
		emit NodeAddParamSignal(this);
	} 	
}


bool apoBaseExeNode::isMyParam(apoBaseParam *paramCtrl)
{
	for (int i = 0; i < m_paramVct.size(); i++){
		if (m_paramVct[i] == paramCtrl)	{
			return true;
		}
	}
	return false;
}

bool apoBaseExeNode::closeParam(apoBaseSlotCtrl *param)
{
	bool ret = false;
	for (QVector<apoBaseParam *>::iterator it = m_paramVct.begin(); it != m_paramVct.end(); it++) {
		if ((*it) == param)	{			
			m_paramVct.erase(it);
			param->close();

			ret = true;
			break;
		}
	}
	if (ret) {
		initParamPos();
		return ret;
	}

	QVector<apoBaseSlotCtrl *>::iterator itFnd = m_outParamVct.end();
	for (QVector<apoBaseSlotCtrl *>::iterator it = m_outParamVct.begin(); it != m_outParamVct.end(); it++) {
		if ((*it) == param)	{
			if (param->getConnector() )	{
				return false;
			}
			itFnd = it;
			continue;
		}
		if (itFnd != m_outParamVct.end())	{
			QPoint pos = (*it)->pos();
			pos.setY(pos.y() - E_LINE_HEIGHT);
			(*it)->move(pos);
 		}
	}

	if (itFnd != m_outParamVct.end())	{
		m_outParamVct.erase(itFnd);
		param->close();

		QPoint pos = m_outParamAddNew->pos();
		pos.setY(pos.y() - E_LINE_HEIGHT);
		m_outParamAddNew->move(pos);
		return true;
	}

	return false;
}

apoBaseSlotCtrl* apoBaseExeNode::getFunctionParam(int index)
{
	if (index > 10 || index<= 0)	{
		return NULL;
	}
	
	int num = m_outParamVct.size();
	if (num < index){
		for (int i = 0; i < (index - num) ; i++){
			onAddFunctionInParam();
		}
	}
	return m_outParamVct[index - 1];
}


int apoBaseExeNode::getFunctionParamIndex(apoBaseSlotCtrl* slot)
{
	for (int i = 0; i < m_outParamVct.size(); i++)	{
		if (slot == m_outParamVct[i]){
			return i + 1;
		}
	}
	return -1;
}

apoBaseExeNode *apoBaseExeNode::getMyPreNode()
{
	if (!m_runInNode) {
		return NULL;
	}

	apoUiBezier *pBze = m_runInNode->getConnector();
	if (!pBze){
		return NULL;
	}

	QWidget *w = pBze->getAnotherSlot(m_runInNode);
	if (!w)	{
		return NULL;
	}
	return dynamic_cast<apoBaseExeNode*>(w->parent());		
}

apoBaseExeNode *apoBaseExeNode::getMyNextNode()
{
	if (!m_toNextNode) {
		return NULL;
	}

	apoUiBezier *pBze = m_toNextNode->getConnector();
	if (!pBze){
		return NULL;
	}

	QWidget *w = pBze->getAnotherSlot(m_toNextNode);
	if (!w)	{
		return NULL;
	}
	return dynamic_cast<apoBaseExeNode*>(w->parent());
}

void apoBaseExeNode::onAddFunctionInParam()
{
	//apoBaseSlotCtrl*pParamCtrl;
	//CREATE_CTRL_OBJECT(apoBaseSlotCtrl, ">>", white, return);

	QString str1;
	str1.sprintf("In%d >>", m_outParamVct.size() + 1);
	apoBaseSlotCtrl*pParamCtrl = new apoBaseSlotCtrl(str1,this);	
	pParamCtrl->resize(PARAM_CTRL_W*2, PARAM_CTRL_H);
	pParamCtrl->setStyleSheet("QLabel{background-color:yellow;}");
	pParamCtrl->setAttribute(Qt::WA_DeleteOnClose, true);

	pParamCtrl->setSlotType(apoBaseParam::SLOT_FUNCTION_PARAM);

	QPoint newPos = m_outParamAddNew->pos();
	pParamCtrl->move(newPos.x() , newPos.y());
	pParamCtrl->show();

	newPos.setY(newPos.y() + E_LINE_HEIGHT);
	m_outParamAddNew->move(newPos);

	m_outParamVct.push_back(pParamCtrl);

	int param_num = m_outParamVct.size();
	if (param_num >= 2) {
		m_size.setHeight(m_size.height() + E_LINE_HEIGHT);
	}
}

apoBaseParam* apoBaseExeNode::insertNewParam()
{
	apoEditorSetting*pSetting = apoEditorSetting::getInstant();
	ndxml *newxml = pSetting->AddNewXmlNode(m_nodeXml, (QWidget*) this->parent());
	if (!newxml){
		nd_logerror("create new param error\n");
		return NULL;
	}

	apoBaseParam*pParamCtrl = _param2Ctrl(newxml, m_nodeXml);	
	if (!pParamCtrl) {
		nd_logerror("create new param error\n");
		ndxml_delxml(newxml, m_nodeXml);
		return NULL;
	}
	
	QPoint newPos = m_addNewParam->pos();
	pParamCtrl->move(newPos);
	pParamCtrl->show();

	newPos.setY(newPos.y() + E_LINE_HEIGHT);
	m_addNewParam->move(newPos);

	int param_num = m_paramVct.size();
	if (param_num >=2) {
		m_size.setHeight(m_size.height() + E_LINE_HEIGHT );
		
		if (m_returnValue)	{
			newPos = m_returnValue->pos();
			newPos.setY(newPos.y() + E_LINE_HEIGHT);
			m_returnValue->move(newPos);
		}
	}

	return pParamCtrl;
}

void apoBaseExeNode::mouseDoubleClickEvent(QMouseEvent * event)
{
	apoBaseParam *paramCtrl;
	QWidget *w = this->childAt(event->pos());
	if (w == (QWidget*) m_title)	{
		emit onTitalCtrlDBclicked(w, event);
	}
	else if ((paramCtrl=dynamic_cast<apoBaseParam*>(w) )) {
		for (int i = 0; i < m_paramVct.size(); i++)	{
			if (paramCtrl == m_paramVct[i] )	{
				emit onParamCtrlDBclicked(paramCtrl, event);
				break;
			}
		}
	}
	else {
		emit onDBclicked(this,event);
	}
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

	SAFE_CLOSE(m_title);

	//SAFE_CLOSE(m_name);
	
	//for (int i = 0;  i<m_paramVct.size(); i++){
	//	SAFE_CLOSE(m_paramVct[i]);
	//}

	//m_paramVct.clear();
}

void apoBaseExeNode::init(const QString &title)
{
	destroy();
	apoBaseSlotCtrl *label;
    int x = 0, y = MARGIN_Y ;  //START coord
	
    //connect in ctrl
	if (!m_disableIn)	{
		CREATE_CTRL_OBJECT(apoBaseSlotCtrl, "->", yellow, m_runInNode);
		m_runInNode->setSlotType(apoBaseSlotCtrl::SLOT_RUN_IN);
	}

    //init title label ;
    x = MARGIN_X;
    y = MARGIN_Y ;
	label = new apoBaseSlotCtrl(title, this);
	label->move(x + PARAM_CTRL_W, y);
	label->setAttribute(Qt::WA_DeleteOnClose, true);
	label->show();
	m_title = label;

    y+= E_LINE_HEIGHT * 1.5 ;

    //draw param
    x = 0 ;
	y += MARGIN_Y;

	foreach(apoBaseParam *label, m_paramVct){
		label->move(x, y);
		y += E_LINE_HEIGHT;
		label->show();
	}

    //add new-param button
	if (!m_disableNewParam) {
		CREATE_CTRL_OBJECT(QPushButton, "+", green, m_addNewParam);
		connect(m_addNewParam, SIGNAL(clicked()), this, SLOT(onInputValAddClicked()));
		y += E_LINE_HEIGHT;
	}

	int param_num = m_paramVct.size();
	if (param_num <2) {
        y += E_LINE_HEIGHT * (2 - param_num) ;
    }
    x += E_LINE_WIDTH ;
	
    m_size.setWidth(x + MARGIN_X);
    m_size.setHeight(y + MARGIN_Y);
	
    //init toNext label
	if (!m_disableToNext) {
		y = E_LINE_HEIGHT *2 - MARGIN_Y;
		CREATE_CTRL_OBJECT(apoBaseSlotCtrl, "->", yellow, m_toNextNode);
		m_toNextNode->setSlotType(apoBaseSlotCtrl::SLOT_RUN_OUT);

	}


	//return value
	if (!m_disableRetVar) {
		y = m_size.height() - E_LINE_HEIGHT + MARGIN_Y;
		CREATE_CTRL_OBJECT(apoBaseSlotCtrl, ">>", yellow, m_returnValue);
		m_returnValue->setSlotType(apoBaseSlotCtrl::SLOT_RETURN_VALUE);
	}


	x = E_LINE_WIDTH - PARAM_CTRL_W;
	if (!m_disableNewFuncParam) {
		y += E_LINE_HEIGHT;
		CREATE_CTRL_OBJECT(QPushButton, "+", green, m_outParamAddNew);
		connect(m_outParamAddNew, SIGNAL(clicked()), this, SLOT(onAddFunctionInParam()));
		y += E_LINE_HEIGHT;
	}
	
	onInit();
	this->setStyleSheet("background-color:white;");

}

void apoBaseExeNode::initParamPos()
{
	int x = 0;
	int y = MARGIN_Y + E_LINE_HEIGHT * 1.5;

	//draw param

	foreach(apoBaseParam *label, m_paramVct){
		label->move(x, y);
		y += E_LINE_HEIGHT;
		label->show();
	}

	//add new-param button
	if (m_addNewParam) {
		m_addNewParam->move(x, y);
		m_addNewParam->show();
		y += E_LINE_HEIGHT;
	}

	int param_num = m_paramVct.size();
	if (param_num < 2) {
		y += E_LINE_HEIGHT * (2 - param_num);
	}
	x += E_LINE_WIDTH;

	m_size.setWidth(x + MARGIN_X);
	m_size.setHeight(y + MARGIN_Y);
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

	if (m_selected)	{
		painter.setPen(QPen(Qt::green, 2));
	}
	else {
		painter.setPen(QPen(Qt::red, 2));
	}
	painter.drawRect(1, 1, m_size.width() - 2, m_size.height() - 2);

	//show param value 
	if (m_paramVct.size() > 0) {
		QFont font1;
		font1.setFamily(QString::fromUtf8("Times New Roman"));
		font1.setPointSize(8);
		//font3.setBold(true);
		font1.setWeight(50);

		painter.setPen(QPen(Qt::darkYellow, 1));
		painter.setFont(font1);


		for (QVector<apoBaseParam *>::iterator it = m_paramVct.begin(); it != m_paramVct.end(); ++it){
			QString paramInfo = (*it)->getParamInfo();
			if (paramInfo.isEmpty()) {
				continue;
			}

			QPoint pos = (*it)->pos();
			pos.setX(pos.x() + (*it)->size().width() + 2);
			pos.setY(pos.y() + MARGIN_Y -1);
			painter.drawText(pos, paramInfo);

		}
	}
	
	if (!m_tips.isEmpty()) {

		int x = MARGIN_X ;
		int y = E_LINE_HEIGHT * 1.5 ;
		
		painter.setPen(QPen(Qt::lightGray, 2));
		
		QFont font1;
		font1.setFamily(QString::fromUtf8("Harlow Solid Italic"));
		font1.setPointSize(14);
		//font3.setBold(true);
		font1.setWeight(50);
		painter.setFont(font1);
		painter.drawText(x, y, m_tips);
	}
	trytoDrawConnectSlot();
}


void apoBaseExeNode::trytoDrawConnectSlot()
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen(QPen(Qt::blue, 2));
	
#define DRAW_RECT(_slot)				\
	if ((_slot) && (_slot)->checkInDrag()){			\
		painter.drawRect((_slot)->geometry());\
	}
// 
// 	if (m_toNextNode){
// 		QRect rect = m_toNextNode->geometry();
// 		painter.drawRect(rect);
// 
// 	}
	DRAW_RECT(m_runInNode);
	DRAW_RECT(m_toNextNode);
	DRAW_RECT(m_returnValue);

	for (QVector<apoBaseParam *>::iterator it = m_paramVct.begin(); it != m_paramVct.end(); ++it){
		DRAW_RECT(*it);

	}
}