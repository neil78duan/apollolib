/* file apoUiMainEditor.h
 *
 * main editor window
 *
 * create by duan
 * 2017.2.9
 */



#include "apoScrpit/apoUiMainEditor.h"
#include "apoScrpit/apoBaseExeNode.h"
#include "apoScrpit/mainwindow.h"
#include "logic_parser/logic_editor_helper.h"
#include "listdialog.h"
#include "myqtitemctrl.h"
#include <QMenu>
#include <QContextMenuEvent>
#include <QBoxLayout>
#include <QDockWidget>

using namespace LogicEditorHelper;
/*
need add position xml 
<apoEditorPos kinds="hide" x="100" y="100" offset_x="0"  offset_y="0" />
*/
apoUiMainEditor::apoUiMainEditor(QWidget *parent) : 
QWidget(parent), m_editedFunction(0), m_curDetailXml(0), m_curDetailNode(0),
	m_dragSrc(NULL), m_drawingBezier(NULL), m_popSrc(0)
{
	m_curDragType = E_MOUSE_TYPE_MOVE;
	m_popMenuType = E_POP_DEL;
	m_scriptFileXml = 0;
	m_setting = apoEditorSetting::getInstant();
	//m_editingToNext = 0;
}


bool apoUiMainEditor::showFunction(ndxml *data, ndxml_root *xmlfile)
{
	m_editedFunction = data;
	m_scriptFileXml = xmlfile;

	m_startX = 10;
	m_startY = 20;
	if (!_createFuncEntry(m_editedFunction, QPoint(m_startX, m_startY))) {
		return false;
	}
	
	ndxml *pPoint = ndxml_getnode(m_editedFunction, "apoEditorPos");
	if (pPoint) {
		int x = atoi(ndxml_getattr_val(pPoint, "offset_x"));
		int y = atoi(ndxml_getattr_val(pPoint, "offset_y"));
		dragTo(QPoint(x, y));
	}
	return true;
}


void apoUiMainEditor::clear()
{
	m_curDragType = E_MOUSE_TYPE_MOVE;
	m_popMenuType = E_POP_DEL;

	m_dragSrc = 0;
	m_popSrc = 0;

	m_drawingBezier = 0;
	m_beziersVct.clear();

	m_editedFunction = 0;
	m_startX = 0;
	m_startY = 0;
}

bool apoUiMainEditor::_showBlocks(apoBaseSlotCtrl *inSlot, ndxml *stepsBlocks)
{
	bool ret = false;
	int total = ndxml_getsub_num(stepsBlocks);

	for (int i = 0; i < total; i++) {
		ndxml *xmlStep = ndxml_getnodei(stepsBlocks, i);
		const char *stepInsName = ndxml_getname(xmlStep);
		if (!stepInsName){
			continue;
		}
		const compile_setting* stepInfo = m_setting->getStepConfig(stepInsName);
		if (!stepInfo) {
			continue;
		}

		if (stepInfo->ins_type == E_INSTRUCT_TYPE_COMMENT)	{
			continue;
		}
		else if (stepInfo->ins_type == E_INSTRUCT_TYPE_EXCEPTION_CATCH) {
			int tmpx = m_startX;
			int tmpy = m_startY;
			ret = _createFuncEntry(xmlStep, QPoint(10, 200));
			m_startX = tmpx;
			m_startY = tmpy;
		}
		else if (stepInfo->ins_type == E_INSTRUCT_TYPE_INIT_BLOCK) {

			int tmpx = m_startX;
			int tmpy = m_startY;
			ret = _createFuncEntry(xmlStep, QPoint(10, 400));
			m_startX = tmpx;
			m_startY = tmpy;
		}
		else {
			QPoint pos;
			_getNodePos(xmlStep, pos);
			apoBaseExeNode* pExenode = _showExeNode(inSlot, xmlStep, pos);
			if (!pExenode){
				ret = false;
			}
			else {
				inSlot = pExenode->toNext();
				ret = true;
			}
		}
		if (!ret){
			return false;
		}

	}
	return true;
}
bool apoUiMainEditor::_createFuncEntry(ndxml *stepsBlocks, const QPoint &defaultPos)
{
	QPoint pos;
	//apoBaseSlotCtrl *toNextCtrl = 0;
	if (!_getNodePos(stepsBlocks, pos)) {
		pos = defaultPos;

		m_startX = defaultPos.x() + X_STEP;
		m_startY = defaultPos.y() + Y_STEP;
	}

	apoBaseExeNode* pbeginNode = _showExeNode(NULL, stepsBlocks, pos);
	if (!pbeginNode){
		return false;
	}

	return _showBlocks(pbeginNode->toNext(), stepsBlocks);
}

int apoUiMainEditor::_createParams(ndxml *xmlnode, apoBaseExeNode &exeNode)
{

	for (int i = 0; i < ndxml_getsub_num(xmlnode); i++) {
		ndxml *xmlParam = ndxml_getnodei(xmlnode, i);
		if (!xmlParam) {
			continue;
		}
// 		int ret = param2Stream(xmlParam, stepNode, p, len, &args_num);
// 		if (-1 == ret)	{
// 			return -1;
// 		}
// 		p += ret;
// 		len -= ret;
	}
	return 0;
}
apoBaseExeNode* apoUiMainEditor::_showExeNode(apoBaseSlotCtrl *inSlot, ndxml *exeNode, const QPoint &pos)
{
// 	const char *ctrlName = m_setting->getExeNodeName(ndxml_getname(exeNode));
// 	if (!ctrlName || !*ctrlName) {
// 		ctrlName = "Common";
// 	}

	apoBaseExeNode *nodeCtrl = g_apoCreateExeNode(exeNode, this);
	if (!nodeCtrl){
		return NULL;
	}
	
	//nodeCtrl->setMyUserData(exeNode);

	QObject::connect(nodeCtrl, SIGNAL(onDBclicked(apoBaseExeNode *, QMouseEvent * )),
		this, SLOT(onExenodeDBClicked(apoBaseExeNode *, QMouseEvent *)));

// 	const char *dispName = LogicEditorHelper::_GetXmlName(exeNode, NULL);
// 	if (dispName){
// 		const char *pRealDisp = strchr(dispName, '_');
// 		if (pRealDisp)	{
// 			++pRealDisp;
// 			nodeCtrl->setTitle(pRealDisp);
// 		}
// 		else {
// 			nodeCtrl->setTitle(dispName);
// 
// 		}
// 	}

	//QPoint pt = pos - QPoint(m_centerOffsetX, m_centerOffsetY);

	nodeCtrl->move(pos);
	nodeCtrl->show(); //must call show functiong	
	
	apoBaseSlotCtrl *connectIn = nodeCtrl->inNode();
	
	if (connectIn && inSlot) {
		_connectSlots(inSlot, connectIn);
	}

	return nodeCtrl;
}
bool apoUiMainEditor::_getNodePos(ndxml *exeNode, QPoint &pos)
{
	ndxml *pPoint = ndxml_getnode(exeNode, "apoEditorPos");
	if (pPoint) {
		int x = atoi(ndxml_getattr_val(pPoint, "x"));
		int y = atoi(ndxml_getattr_val(pPoint, "y"));
		pos = QPoint(x, y);
		return true;
	}

	pos = QPoint(m_startX, m_startY);
	m_startX += X_STEP;
	m_startY += Y_STEP;
	return false;
}


bool apoUiMainEditor::_connectSlots(apoBaseSlotCtrl *fromSlot, apoBaseSlotCtrl *toSlot)
{
	QSize size1 = fromSlot->size();
	QSize size2 = toSlot->size();


	QPoint pt1 = fromSlot->mapToGlobal( QPoint(size1.width(), size1.height() / 2));
	QPoint pt2 = toSlot->mapToGlobal( QPoint(0, size2.height() / 2));

	pt1 = this->mapFromGlobal(pt1);
	pt2 = this->mapFromGlobal(pt2);

	apoUiBezier *pbezier = new apoUiBezier(this, pt1, pt2);
	pbezier->setConnector(fromSlot, toSlot);
	m_beziersVct.push_back(*pbezier);
	return true;
}

void apoUiMainEditor::popMenuAddnewTrigged()
{
// 	
// 	static int g_index = 0;
// 	apoBaseExeNode *exeNode = g_apoCreateExeNode(g_index, this);
// 	if (exeNode){
// 		exeNode->move(m_curClicked);
// 		exeNode->show(); //must call show functiong	
// 	}
// 	
// 	++g_index;
// 	if (g_index >= EAPO_EXE_NODE_NUMBER){
// 		g_index = 0;
// 	}
// 	
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


void apoUiMainEditor::dragTo(const QPoint &offset)
{
	
	QObjectList list = children();
	foreach(QObject *obj, list) {
		apoBaseExeNode *pexenode = qobject_cast<apoBaseExeNode*>(obj);
		if (pexenode){
			QPoint curPos = pexenode->pos();
			curPos += offset;
			pexenode->move(curPos);
		}
	}
	
	for (QVector<apoUiBezier>::iterator it = m_beziersVct.begin(); it!=m_beziersVct.end(); it++){
		it->move(offset);
	}
	
	m_offset += offset;
	//scroll(offset.x(), offset.y());
	
	this->update();
}

void apoUiMainEditor::mousePressEvent(QMouseEvent *event)
{
	if (Qt::RightButton == event->button())	{
		return;
	}
	

	m_dragSrc = NULL; 
	QWidget *w = this->childAt(event->pos());
	
	if (w == NULL){
		m_curDragType = E_MOUSE_TYPE_MOVE_VIEW;
		m_moveOffset = event->pos();
		//return;
	}
	
	else if (dynamic_cast<apoBaseExeNode*>(w))	{
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
	if (!m_dragSrc ) {
		if (m_curDragType == E_MOUSE_TYPE_MOVE_VIEW){
			QPoint offsetPt = event->pos() ;
			offsetPt -= m_moveOffset;
			dragTo(offsetPt);
			m_moveOffset = event->pos();
		}
		m_curDragType = E_MOUSE_TYPE_MOVE;
		return;
	}

    if(E_MOUSE_TYPE_MOVE==m_curDragType){

    }
    else {
        QWidget *w = this->childAt(event->pos());

        if (w &&dynamic_cast<QLabel*>(w) ){
			if (w != m_dragSrc ){
				m_drawingBezier->setConnector(m_dragSrc, w);
				m_beziersVct.push_back(*m_drawingBezier);
			}
            //m_drawingBezier->paintTo(event->pos());
        }

        delete m_drawingBezier;
        m_drawingBezier = 0;
        this->update();
    }

	m_dragSrc = NULL;
}

void apoUiMainEditor::mouseMoveEvent(QMouseEvent *event)
{   
	if (!m_dragSrc)	{
		if (m_curDragType == E_MOUSE_TYPE_MOVE_VIEW){
			QPoint offsetPt = event->pos();
			offsetPt -= m_moveOffset;
			dragTo(offsetPt);
			m_moveOffset = event->pos();
		}

		return;
	}
	if (m_curDragType == E_MOUSE_TYPE_MOVE)	{
		QPoint newPos = event->pos() - m_moveOffset;
        m_dragSrc->move(newPos);
	}
	else {
		if (m_drawingBezier){
			m_drawingBezier->paintTo(event->pos());
			m_drawingBezier->paint();
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


void apoUiMainEditor::curDetailChanged(apoBaseExeNode *exenode)
{
	if (m_curDetailNode){
		m_curDetailNode->setSelected(false);
	}
	m_curDetailNode = 0;	
	m_curDetailNode = exenode;

	if (exenode){
		m_curDetailNode->setSelected(true);
		m_curDetailXml = (ndxml *)exenode->getMyUserData();
		if (m_curDetailXml)	{
			QObject *parent = this->parent();
			MainWindow *pMain = dynamic_cast<MainWindow*>(parent);
			if (pMain)		{
				pMain->onShowExeNodeDetail(exenode);
			}
		}
	}
	this->update();

	
}

void apoUiMainEditor::onExenodeDBClicked(apoBaseExeNode *exeNode, QMouseEvent * event)
{
	//get ditail window
	curDetailChanged(exeNode);
}

/*
apoUiDetailView *apoUiMainEditor::getDetailWidget()
{
	QObject *parent = this->parent();
	if (!parent){
		return NULL;
	}

	QDockWidget *pDoc = parent->findChild<QDockWidget*>("DetailView");
	if (!pDoc){
		return NULL;
	}

	return pDoc->findChild<apoUiDetailView*>("xmlDetailTable");

}
*/
//--------------------detail editor-----------------------------
// 
// ndxml* apoUiMainEditor::GetSelXml(apoBaseExeNode* exeNode)
// {
// 	return (ndxml*)exeNode->getMyUserData();
// }
// 
// bool apoUiMainEditor::_GetUserSelEnumVal(const apoUiMainEditor::text_list_t &enum_vals, QString &val)
// {
// 	ListDialog dlg(this);
// 	for (int i = 0; i < enum_vals.size(); i++) {
// 		dlg.m_selList.push_back(enum_vals[i]);
// 	}
// 
// 	dlg.InitList();
// 	dlg.setModal(true);
// 	if (dlg.exec() == QDialog::Accepted) {
// 		return dlg.GetSelectText(val);
// 	}
// 
// 	return false;
// }

// 
// int apoUiMainEditor::DisplaySelXml()
// {
// 	apoUiDetailView *pTable = getDetailWidget();
// 	if (!pTable){
// 		return -1;
// 	}
// 	pTable->showDetail((void*)m_curDetailNode,m_scriptFileXml);
// 
// 	//_initShowCtrl(pTable);
// 	//ShowXMLValue(m_curDetailXml, pTable, (int)CheckExpand(m_curDetailXml));
// 
// 	return 0;
// }

// 
// int apoUiMainEditor::_initShowCtrl(QTableWidget *show_ctrl)
// {
// 	int length = show_ctrl->width();
// 	show_ctrl->clear();
// 
// 	show_ctrl->setColumnCount(2);
// 	show_ctrl->setRowCount(0);
// 
// 	show_ctrl->setColumnWidth(0, length * 0.28);
// 	show_ctrl->setColumnWidth(1, length * 0.68);
// 
// 	QStringList headers;
// 	headers << "Name" << "Value";
// 	show_ctrl->setHorizontalHeaderLabels(headers);
// 	return 0;
// }
// 
// int apoUiMainEditor::ShowXMLValue(ndxml *xml_node, QTableWidget *show_ctrl, int expand)
// {
// 	int i;
// 	const char *pval, *name;
// 
// 	LogicEditorHelper::CXMLAlias &aliasRoot =  m_setting->getAlias();
// 	//size_t param ;
// 	pval = ndxml_getval(xml_node);
// 	if (pval) {
// 		//const char *desc =_GetXmlDesc(xml_node);
// 		name = _GetXmlName(xml_node, &aliasRoot);
// 
// // 		const char *alias = aliasRoot.GetAlia(name);
// // 		if (alias)
// // 			name = alias;
// 
// 		ShowRow(name, pval, xml_node, show_ctrl);
// 
// 	}
// 	for (i = 0; i < ndxml_getattr_num(xml_node); i++) {
// 
// 		name = (char*)ndxml_getattr_name(xml_node, i);
// 		if (!name || CheckReserved(name) >= 0)
// 			continue;
// 		pval = (char*)ndxml_getattr_vali(xml_node, i);
// 		if (pval) {
// 			const char *alias = aliasRoot.GetAlia(name);
// 			if (alias)
// 				name = alias;
// 
// 			ShowRow(name, pval, xml_node, show_ctrl);
// 		}
// 	}
// 
// 	if (expand) {
// 		for (i = 0; i < ndxml_getsub_num(xml_node); i++) {
// 			ndxml *sub_node = ndxml_refsubi(xml_node, i);
// 			if (sub_node && !CheckHide(sub_node)){
// 				ShowXMLValue(sub_node, show_ctrl, expand);
// 			}
// 		}
// 	}
// 	else {
// 		text_vect_t exp_list;
// 		if (get_string_array((char*)ndxml_getattr_val(xml_node, _GetReverdWord(ERT_EXPAND_LIST)), exp_list)) {
// 			for (text_vect_t::iterator it = exp_list.begin(); it != exp_list.end(); it++) {
// 				ndxml *sub_node = ndxml_refsub(xml_node, (*it).c_str());
// 				if (sub_node && !CheckHide(sub_node)){
// 					ShowXMLValue(sub_node, show_ctrl, 1);
// 				}
// 			}
// 		}
// 	}
// 	return 0;
// 
// }
// 
// void apoUiMainEditor::ShowRow(const char *name, const char *val, ndxml* param, QTableWidget *show_ctrl)
// {
// 	int rows = show_ctrl->rowCount();
// 	show_ctrl->setRowCount(rows + 1);
// 
// 	char realval[4096];
// 	const char *p = getDisplayNameFromStr(val, realval, sizeof(realval));
// 
// 	xmlTableItem *tableNode = new xmlTableItem(QString(p));
// 	tableNode->setUserData(param);
// 
// 	ndxml *configRoot = m_setting->getConfig();
// 
// 	if (param) {
// 
// 		int type = GetXmlValType(param, configRoot);
// 		if (type == EDT_BOOL) {
// 			tableNode->setText(QString(val[0] == '0' ? "FALSE" : "TRUE"));
// 		}
// 		else if (type == EDT_ENUM) {
// 			text_list_t enum_texts;
// 			if (getEnumList(param, configRoot, enum_texts) > 0) {
// 				int sel = atoi(val);
// 				if (sel >= 0 && sel < (int)enum_texts.size())
// 					tableNode->setText(enum_texts[sel]);
// 			}
// 
// 		}
// 		else if (type == EDT_KEY_VAL) {
// 			text_list_t key_texts, value_texts;
// 			if (getKeyList(param, configRoot, key_texts) > 0 && getKeyValueList(param, configRoot, value_texts) > 0) {
// 				for (int i = 0; i < (int) value_texts.size(); i++) {
// 					if (val == value_texts[i]) {
// 						if (i < key_texts.size())	{
// 							tableNode->setText(key_texts[i]);
// 							//m_grad->SetItemText(row, 1, (LPCTSTR)key_texts[i]);
// 						}
// 						break;
// 					}
// 
// 				}
// 			}
// 		}
// 	}
// 	show_ctrl->setItem(rows, 0, new xmlTableItem(QString(name)));
// 	show_ctrl->setItem(rows, 1, tableNode);
// 
// 	nd_logdebug("show %s : %s ", name, p);
// 
// 
// }
/*
bool apoUiMainEditor::_beginEdit(int nRow, int nCol)
{
	xmlTableItem *cell = (xmlTableItem*)ui->xmlTable->item(nRow, nCol);
	ndxml *xml = (ndxml *)cell->getUserData();
	if (!xml)	{
		return false;
	}
	bool ret = false;
	QString  str1;
	int type = GetXmlValType(xml, m_config);
	if (type == EDT_IN_FILE || type == EDT_OUT_FILE) {

		if (_GetFileName(type == EDT_IN_FILE, str1, GetXmlParam(xml))) {
			cell->setText(str1);
			if (0 == ndxml_setval(xml, str1.toStdString().c_str())) {
				ret = true;
			}
		}
	}
	else if (EDT_DIR == type) {
		if (_OpenFilter(str1, tr("select direct"))){
			str1 += '/';
			cell->setText(str1);
			if (0 == ndxml_setval(xml, str1.toStdString().c_str())){
				ret = true;
			}
		}
	}
	else if (EDT_USER_DEFINE == type) {

		QString str2;
		const char *pUserparam = ndxml_getattr_val(xml, _GetReverdWord(ERT_USER_DEFINE_PARAM));
		if (_GetUserDefinData(pUserparam, str2)) {
			std::string stdText = str2.toStdString();

			char realval[256];
			str1 = getDisplayNameFromStr(stdText.c_str(), realval, sizeof(realval));
			ret = true;
			cell->setText(str1);
			ndxml_setval(xml, stdText.c_str());
		}
	}
	else if (EDT_BOOL == type) {
		ListDialog dlg(this);
		dlg.m_selList.push_back("FALSE");
		dlg.m_selList.push_back("TRUE");
		dlg.InitList();
		if (dlg.exec() == QDialog::Accepted) {
			dlg.GetSelectText(str1);
			int selval = dlg.GetSelect();

			cell->setText(str1);
			ndxml_setval(xml, selval ? "1" : "0");
			ret = true;
		}

	}
	else if (EDT_ENUM == type)	{
		text_list_t enum_texts;
		if (getEnumList(xml, m_config, enum_texts) > 0) {
			if (_GetUserSelEnumVal(enum_texts, str1)){
				int sel = -1;
				for (int i = 0; i < enum_texts.size(); i++)	{
					if (str1 == enum_texts[i]) {
						sel = i;
						break;
					}
				}
				if (-1 != sel) {
					QString str3;
					str3.sprintf("%d", sel);

					cell->setText(str1);
					ndxml_setval(xml, str3.toStdString().c_str());
					ret = true;
				}

			}
		}
	}
	else if (EDT_KEY_VAL == type)	{
		ret = false;
		text_list_t key_texts;
		if (getKeyList(xml, m_config, key_texts) > 0) {
			if (_GetUserSelEnumVal(key_texts, str1)){
				int sel = -1;
				for (int i = 0; i < key_texts.size(); i++)	{
					if (str1 == key_texts[i]) {
						sel = i;
						break;
					}
				}
				if (-1 != sel) {
					text_list_t value_texts;
					getKeyValueList(xml, m_config, value_texts);
					if (sel < value_texts.size()){

						cell->setText(str1);
						ndxml_setval(xml, value_texts[sel].toStdString().c_str());
						ret = true;
					}
				}
			}
		}
	}
	return ret;
}
*/