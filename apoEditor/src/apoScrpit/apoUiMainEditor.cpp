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
#include "logic_parser/logicDataType.h"
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
unconnected node 
<unConnect kinds="hide"  />

*/
apoUiMainEditor::apoUiMainEditor(QWidget *parent) : 
QWidget(parent), m_editedFunction(0),  m_curDetailNode(0),
	m_dragSrc(NULL), m_drawingBezier(NULL), m_popSrc(0)
{
	m_curDragType = E_MOUSE_TYPE_MOVE;
	//m_popMenuType = E_POP_DEL;
	m_scriptFileXml = 0;
	m_funcEntry = NULL;
	m_setting = apoEditorSetting::getInstant();
	//m_editingToNext = 0;
}


bool apoUiMainEditor::showFunction(ndxml *data, ndxml_root *xmlfile)
{
	m_editedFunction = data;
	m_scriptFileXml = xmlfile;

	m_startX = 10;
	m_startY = 20;
	for (int i = 0; i < m_beziersVct.size(); i++)	{
		delete m_beziersVct[i];
	}
	m_varMap.clear();
	m_funcEntry = NULL;

	QPoint init_offset(0,0);
	m_offset = init_offset;

	getOffset(init_offset);

	nd_logdebug("load offset(%d,%d)\n", init_offset.x(), init_offset.y());

	if (!_createFuncEntry(m_editedFunction, QPoint(m_startX, m_startY))) {
		return false;
	}
	dragTo(init_offset);

	return true;
}


bool apoUiMainEditor::showCompileError(ndxml_root *xmlfile, stackIndex_vct &errStackIndex)
{

	return false;
}

void apoUiMainEditor::clear()
{
	m_curDragType = E_MOUSE_TYPE_MOVE;
	//m_popMenuType = E_POP_DEL;

	m_dragSrc = 0;
	m_popSrc = 0;

	m_drawingBezier = 0;
	for (int i = 0; i < m_beziersVct.size(); i++){
		delete m_beziersVct[i];
	}
	m_beziersVct.clear();

	m_editedFunction = 0;
	m_startX = 0;
	m_startY = 0;
}

bool apoUiMainEditor::_showBlocks(apoBaseSlotCtrl *fromSlot, ndxml *stepsBlocks)
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
			ret = _createFuncEntry(xmlStep, QPoint(10, 300));
			m_startX = tmpx;
			m_startY = tmpy;
		}
		else if (stepInfo->ins_type == E_INSTRUCT_TYPE_INIT_BLOCK) {

			int tmpx = m_startX;
			int tmpy = m_startY;
			ret = _createFuncEntry(xmlStep, QPoint(10, 500));
			m_startX = tmpx;
			m_startY = tmpy;
		}
		else {
			QPoint pos;
			_getNodePos(xmlStep, pos);
			apoBaseExeNode* pExenode = _showExeNode(fromSlot, xmlStep, pos);
			if (!pExenode){
				ret = false;
			}
			else {				
				fromSlot = pExenode->toNext();
				ret = true;
			}
		}
		if (!ret){
			return false;
		}

	}
	return true;
}

void apoUiMainEditor::_showUnConnectBlocks(ndxml *functionXml)
{
	ndxml *unConnBlock = ndxml_getnode(functionXml, "unConnect");
	if (!unConnBlock){
		return;
	}
	int total = ndxml_getsub_num(unConnBlock);

	std::vector<ndxml *> delBufs;
	for (int i = 0; i < total; i++) {
		ndxml *xmlStep = ndxml_getnodei(unConnBlock, i);
		const char *stepInsName = ndxml_getname(xmlStep);
		if (!stepInsName){
			continue;
		}
		const compile_setting* stepInfo = m_setting->getStepConfig(stepInsName);
		if (!stepInfo || stepInfo->ins_type == E_INSTRUCT_TYPE_COMMENT) {
			continue;
		}

		else if (stepInfo->ins_type == E_INSTRUCT_TYPE_STEP_COLLECTION) {
			if (ndxml_getsub_num(xmlStep)<=0){
				delBufs.push_back(xmlStep);
				continue;;
			}
			_showBlocks(NULL, xmlStep);
		}
		else {
			QPoint pos;
			_getNodePos(xmlStep, pos);
			_showExeNode(NULL, xmlStep, pos);
		}
	}

	for (size_t i = 0; i < delBufs.size(); i++)	{
		ndxml_delxml(delBufs[i], functionXml);
	}
}

bool apoUiMainEditor::_showBools(apoBaseExeNode *entryNode, ndxml *entryXml)
{
	bool ret = false;
	int total = ndxml_getsub_num(entryXml);

	apoUiExenodeBool*pBoolNode = dynamic_cast<apoUiExenodeBool*>(entryNode);
	if (!pBoolNode){
		return false;
	}
	int subX = m_startX;
	int subY = m_startY;

	for (int i = 0; i < total; i++) {
		ndxml *xmlStep = ndxml_getnodei(entryXml, i);
		const char *stepInsName = ndxml_getname(xmlStep);
		if (!stepInsName){
			continue;
		}
		const compile_setting* stepInfo = m_setting->getStepConfig(stepInsName);
		if (!stepInfo || stepInfo->ins_type != E_INSTRUCT_TYPE_CASE_ENTRY) {
			continue;
		}

		ndxml *xmlCond = ndxml_getnode(xmlStep, "condition");
		if (!xmlCond){
			continue; 
		}
		int isTrue = ndxml_getval_int(xmlCond);
		if (isTrue)	{
			m_startY = subY - 200;
			m_startX = subX;
			ret = _showBlocks(pBoolNode->getTrueSlot(),xmlStep);
		}
		else {
			m_startY = subY ;
			m_startX = subX;
			ret = _showBlocks(pBoolNode->getFalseSlot(), xmlStep);
		}
		if (!ret){
			return false;
		}
	}
	return ret;
}

bool apoUiMainEditor::_showLoops(apoBaseExeNode *entryNode, ndxml *stepsBlocks)
{
	apoUiExenodeLoop*pNode = dynamic_cast<apoUiExenodeLoop*>(entryNode);
	if (!pNode){
		return false;
	}

	int subX = m_startX;
	int subY = m_startY;

	m_startY -= 200;
	bool ret = _showBlocks(pNode->getLoopSlot(), stepsBlocks);


	m_startX = subX;
	m_startY = subY;
	return ret;
}

bool apoUiMainEditor::_showValueComp(apoBaseExeNode *entryNode, ndxml *stepsBlocks)
{
	apoUiExenodeValueComp*pNode = dynamic_cast<apoUiExenodeValueComp*>(entryNode);
	if (!pNode){
		return false;
	}

	int subX = m_startX;
	int subY = m_startY;

	m_startY -= 200;
	bool ret = _showBlocks(pNode->getSubSlot(), stepsBlocks);

	m_startX = subX;
	m_startY = subY;
	return ret;
}

bool apoUiMainEditor::_createFuncEntry(ndxml *stepsBlocks, const QPoint &defaultPos)
{
	bool noInidPos = false;
	QPoint pos;
	//apoBaseSlotCtrl *toNextCtrl = 0;
	if (!_getNodePos(stepsBlocks, pos)) {
		pos = defaultPos;
		m_startX = defaultPos.x() + X_STEP;
		m_startY = defaultPos.y() + Y_STEP;
		noInidPos = true;
	}

	apoBaseExeNode* pbeginNode = _showExeNode(NULL, stepsBlocks, pos);
	if (!pbeginNode){
		return false;
	}
	if (noInidPos){
		savePosition(pbeginNode, &pos);
	}
	//show unconnected nodes
	_showUnConnectBlocks(stepsBlocks);

	if (!m_funcEntry && dynamic_cast<apoUiExenodeFuncEntry*>(pbeginNode))	{
		m_funcEntry = pbeginNode;
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
apoBaseExeNode* apoUiMainEditor::_showExeNode(apoBaseSlotCtrl *fromSlot, ndxml *exeNode, const QPoint &pos)
{
	apoBaseExeNode *nodeCtrl = g_apoCreateExeNode(exeNode, this);
	if (!nodeCtrl){
		return NULL;
	}

	if (nodeCtrl->getType() == EAPO_EXE_NODE_NewVar){
		pushVarList(exeNode, nodeCtrl);
	}

	QObject::connect(nodeCtrl, SIGNAL(onDBclicked(apoBaseExeNode *, QMouseEvent * )),
		this, SLOT(onExenodeDBClicked(apoBaseExeNode *, QMouseEvent *)));

	QObject::connect(nodeCtrl, SIGNAL(NodeAddParamSignal(apoBaseExeNode *)),
		this, SLOT(onNodeAddNewParam(apoBaseExeNode *)));

	
	nodeCtrl->move(pos);
	nodeCtrl->show(); //must call show functiong	
	
	apoBaseSlotCtrl *connectIn = nodeCtrl->inNode();
	
	if (connectIn && fromSlot) {
		_connectSlots(fromSlot, connectIn, apoUiBezier::LineRunSerq);
	}
	if (fromSlot){
		apoBaseExeNode *preNode = dynamic_cast<apoBaseExeNode *>( fromSlot->parent() );
		if (preNode){
			_connectParam(preNode, nodeCtrl);
		}
	}
	// show sub nodes
	bool ret = true;
	if (nodeCtrl->getType() == EAPO_EXE_NODE_Bool) {
		ret = _showBools(nodeCtrl, exeNode);
	}
	else if (nodeCtrl->getType() == EAPO_EXE_NODE_Loop) {
		ret = _showLoops(nodeCtrl, exeNode);
	}
	else if (nodeCtrl->getType() == EAPO_EXE_NODE_ValueComp) {
		ret = _showValueComp(nodeCtrl, exeNode);
	}

	if (!ret) {		
		nodeCtrl->close();
		return	 NULL;
	}

	return nodeCtrl;
}

void apoUiMainEditor::saveOffset(const QPoint &offset)
{
	if (!m_editedFunction)	{
		return;
	}
	ndxml *pPoint = ndxml_getnode(m_editedFunction, "apoEditorPos");
	if (pPoint){
		char buf[20];
		snprintf(buf, sizeof(buf), "%d", offset.x());
		ndxml_setattrval(pPoint, "offset_x", buf);

		snprintf(buf, sizeof(buf), "%d", offset.y());
		ndxml_setattrval(pPoint, "offset_y", buf);

		nd_logdebug("save offset (%d,%d)\n", offset.x(), offset.y());
	}

	onFileChanged();
}

void apoUiMainEditor::getOffset(QPoint &offset)
{
	if (!m_editedFunction)	{
		return;
	}
	ndxml *pPoint = ndxml_getnode(m_editedFunction, "apoEditorPos");
	if (pPoint){
		const char *p = ndxml_getattr_val(pPoint, "offset_x");
		if (p)	{
			offset.setX(atoi(p));
		}
		p = ndxml_getattr_val(pPoint, "offset_y");
		if (p)	{
			offset.setY(atoi(p));
		}

	}
}

ndxml *apoUiMainEditor::getUnconnectRoot()
{
	ndxml *unConnNode = ndxml_getnode(m_editedFunction, "unConnect");
	if (!unConnNode) {
		const char *nodeText = "<unConnect kinds=\"hide\" />";
		unConnNode = ndxml_from_text(nodeText);
	}
	return unConnNode;
}
bool apoUiMainEditor::_getNodePos(ndxml *exeNode, QPoint &pos)
{
	ndxml *pPoint = ndxml_getnode(exeNode, "apoEditorPos");
	if (pPoint) {
		int x = atoi(ndxml_getattr_val(pPoint, "x"));
		int y = atoi(ndxml_getattr_val(pPoint, "y"));
		pos = QPoint(x, y);
		//pos += m_offset;


		nd_logdebug("get position node(%d,%d), offset(%d,%d)\n", pos.x(), pos.y(), m_offset.x(), m_offset.y());
		return true;
	}
	pos = QPoint(m_startX, m_startY);
	m_startX += X_STEP;
	m_startY += Y_STEP;

	apoBaseExeNode node;
	node.setUserData(exeNode);
	savePosition(&node, &pos);

	return false;
}
void apoUiMainEditor::savePosition(apoBaseExeNode *exeNode,  const QPoint *pNewPos )
{
	if (!exeNode)	{
		return;
	}
	QPoint pos;
	if (pNewPos)	{
		pos = *pNewPos;
	}
	else {
		pos = exeNode->pos();
		pos -= m_offset;
	}

	ndxml *xml = (ndxml *)exeNode->getMyUserData();
	if (!xml) {
		return;
	}
	ndxml *pPoint = ndxml_getnode(xml, "apoEditorPos");
	if (pPoint) {
		char buf[20];
		snprintf(buf, sizeof(buf), "%d", pos.x());
		ndxml_setattrval(pPoint, "x", buf);

		snprintf(buf, sizeof(buf), "%d", pos.y());
		ndxml_setattrval(pPoint, "y",buf );
	}
	else {

		char buf[128];

		snprintf(buf, sizeof(buf), "<apoEditorPos kinds=\"hide\" x=\"%d\" y=\"%d\" offset_x=\"0\"  offset_y=\"0\" />",
			pos.x(), pos.y());
		
		ndxml *newNode = ndxml_from_text(buf);
		if (newNode){
			ndxml_insert_ex(xml, newNode, 0);
		}
	}

	nd_logdebug("save position node(%d,%d), offset(%d,%d)\n", pos.x(), pos.y(), m_offset.x(), m_offset.y());
	onFileChanged();
}


bool apoUiMainEditor::_connectSlots(apoBaseSlotCtrl *fromSlot, apoBaseSlotCtrl *toSlot, apoUiBezier::eBezierType type)
{
	if (!fromSlot) {
		return false;
	}
		
	QSize size1 = fromSlot->size();
	QSize size2 = toSlot->size();


	QPoint pt1 = fromSlot->mapToGlobal( QPoint(size1.width(), size1.height() / 2));
	QPoint pt2 = toSlot->mapToGlobal( QPoint(0, size2.height() / 2));

	pt1 = this->mapFromGlobal(pt1);
	pt2 = this->mapFromGlobal(pt2);

	apoUiBezier *pbezier = new apoUiBezier(this, pt1, pt2);
	if (type == apoUiBezier::LineParam)	{
		pbezier->setColor(Qt::green);
		pbezier->setWidth(1);
	}
	else{
		pbezier->setColor(Qt::darkRed);
		pbezier->setWidth(2);
	}

	fromSlot->setConnector(pbezier);
	toSlot->setConnector(pbezier);

	pbezier->setAnchor(fromSlot, toSlot);
	m_beziersVct.push_back(pbezier);
	return true;
}

bool apoUiMainEditor::_connectParam(apoBaseExeNode *preNode, apoBaseExeNode *curNode)
{
	int paramNum = curNode->getParamNum();
	for (int  i = 0; i < paramNum; i++)	{
		apoBaseParam *param = curNode->getParam(i);
		int paramType = param->getParamType();

		if (paramType == OT_LAST_RET)	{
			if (preNode) {
				_connectSlots(preNode->returnVal(), param);
			}
			
		}
		else if (paramType == OT_VARIABLE) {
			const char *pVarName = param->getValue();
			if (pVarName && * pVarName)	{
				varinat_map_t::iterator it = m_varMap.find(pVarName);
				if (it !=m_varMap.end()) {
					_connectSlots(it->second->returnVal(), param);
				}

			}
		}
		else if (paramType == OT_PARAM) {
			// need to connect function param
			ndxml *xmlVal = param->getValueXml();
			if (xmlVal)	{
				int param_index = ndxml_getval_int(xmlVal);

				apoBaseSlotCtrl* pslot = m_funcEntry->getFunctionParam(param_index);
				if (pslot)	{
					_connectSlots(pslot, param);
				}
			}
		}
		
	}
	return true;
}

bool apoUiMainEditor::_removeBezier(apoUiBezier *connector)
{
	QVector<apoUiBezier*>::iterator it = m_beziersVct.begin();
	for (; it != m_beziersVct.end(); it++)	{
		if ((*it) == connector) {
			delete *it;
			m_beziersVct.erase(it);
			return true;
		}
	}
	return false;
}


void apoUiMainEditor::_disConnect(apoBaseExeNode *changedNode)
{
	for (int i = 0; i < changedNode->getParamNum(); i++) {
		apoBaseParam *paramCtrl = changedNode->getParam(i);
		apoUiBezier *pBze = paramCtrl->getConnector();
		if (pBze) {
			_removeBezier(pBze);
		}
	}
}
void apoUiMainEditor::_reConnectParam(apoBaseExeNode *changedNode)
{
	nd_assert(changedNode);
	_disConnect(changedNode);

	//reconnect 
	_connectParam(changedNode->getMyPreNode(), changedNode);
}

bool apoUiMainEditor::pushVarList(ndxml *xmlNode, apoBaseExeNode*nodeCtrl)
{
	ndxml *varName = ndxml_getnode(xmlNode, "param");
	if (!varName){
		return false;
	}
	const char *pName = ndxml_getval(varName);

	m_varMap[pName] = nodeCtrl;
	return true;		 
}


void apoUiMainEditor::popMenuAddnewTrigged()
{
	ndxml *newxml = m_setting->AddNewXmlNode(m_editedFunction, this);
	if (!newxml) {
		nd_logerror("can not create new node for this function , maybe data is destroyed\n");
		return;
	}
	ndxml_disconnect(m_editedFunction, newxml);

	ndxml *unConnNode = getUnconnectRoot();
	if (!unConnNode) {		
		ndxml_insert(m_editedFunction, unConnNode);
	}
	

	apoBaseExeNode *exeNode = g_apoCreateExeNode(newxml, this);
	if (!exeNode){
		ndxml_delxml(newxml, NULL);
		nd_logerror("create %s Exenode Error \n", _GetXmlName(newxml, NULL));
		return;
	}
	ndxml_insert(unConnNode, newxml);
	exeNode->move(m_curClicked);
	exeNode->show(); //must call show functiong	
	onFileChanged();
	nd_logmsg("create %s SUCCESS\n", _GetXmlName(newxml, NULL));

	//nd_logdebug("current cursor pos(%d,%d)\n", m_curClicked.x(), m_curClicked.y());
// 	
// 	static int g_index = 0;
// 	
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

void apoUiMainEditor::popMenuCloseParamTrigged()
{
	apoBaseSlotCtrl *pslot = dynamic_cast<apoBaseSlotCtrl*>(m_popSrc);
	m_popSrc = NULL;
	if (pslot==NULL){
		return ;
	}
	apoBaseExeNode *parent = dynamic_cast<apoBaseExeNode *>(pslot->parent());
	if (parent)	{
		_disConnect(parent);
		parent->closeParam(pslot);
		//reconnect 
		_connectParam(parent->getMyPreNode(), parent);
		this->update();

	}
}

void apoUiMainEditor::popMenuDisconnectTRigged()
{
	apoBaseSlotCtrl *pslot = dynamic_cast<apoBaseSlotCtrl*>(m_popSrc);
	m_popSrc = NULL;
	if (pslot == NULL){
		return;
	}
	if (_removeConnector(pslot)) {
		this->update();
	}
}

void apoUiMainEditor::popMenuCenterTrigged()
{
	//move first not to (20,20) ;
	QPoint startPos = m_funcEntry->pos();
	QPoint newOffset = QPoint(20, 20) - startPos;

	dragTo(newOffset);
	saveCurPosWithOffset();

	m_offset = QPoint(0, 0);
	saveOffset(m_offset);
	//nd_logdebug("center ()\n");
}

void apoUiMainEditor::contextMenuEvent(QContextMenuEvent *event)
{
#define SHOW_POP_MENU(_TITLE, _FUNCNAME) \
	do 	{	\
		QMenu *popMenu = new QMenu(this);		\
		QAction *add_node = new QAction(_TITLE, this);	\
		popMenu->addAction(add_node);			\
		connect(add_node, &QAction::triggered, this, &_FUNCNAME);	\
		popMenu->exec(QCursor::pos());			\
	} while ( 0 )

#define SHOW_POP_MENU2(_TITLE, _FUNCNAME,_TITLE2, _FUNCNAME2) \
	do 	{	\
		QMenu *popMenu = new QMenu(this);		\
		QAction *add_node = new QAction(this);	\
		add_node->setText(_TITLE);				\
		popMenu->addAction(add_node);			\
		connect(add_node, &QAction::triggered, this, &_FUNCNAME);	\
		QAction *add_node1 = new QAction(_TITLE2, this);			\
		popMenu->addAction(add_node1);			\
		connect(add_node1, &QAction::triggered, this, &_FUNCNAME2);	\
		popMenu->exec(QCursor::pos());			\
	} while ( 0 )

	m_popSrc = NULL;

	QWidget *w = this->childAt(event->pos());
	if (w == NULL) {
		m_curClicked = event->pos();
		
		SHOW_POP_MENU2(QString("New"), apoUiMainEditor::popMenuAddnewTrigged, QString("Move default"), apoUiMainEditor::popMenuCenterTrigged);
	}
	else {
		QString title;
		apoBaseExeNode*pDelNode = dynamic_cast<apoBaseExeNode*>(w);
		if (pDelNode) {
			m_popSrc = w;
			title = "Remove ";
			title + pDelNode->getTitle();
			SHOW_POP_MENU(title, apoUiMainEditor::popMenuDeleteTrigged);
		}
		else if (dynamic_cast<apoBaseSlotCtrl*>(w))	{
			apoBaseSlotCtrl *pslot = (apoBaseSlotCtrl*)w;
			m_popSrc = w;

			if (pslot->slotType() == apoBaseSlotCtrl::SLOT_FUNCTION_PARAM)	{
				title = "Close ";
				title += pslot->text(); 
				SHOW_POP_MENU(title, apoUiMainEditor::popMenuCloseParamTrigged);
			}
			else if (pslot->slotType() == apoBaseSlotCtrl::SLOT_NODE_INPUUT_PARAM)	{
				title = "Close ";
				apoBaseParam *param = (apoBaseParam*)pslot;
				title += param->getParamName();
				SHOW_POP_MENU2(title, apoUiMainEditor::popMenuCloseParamTrigged, QString("Disconnect"), apoUiMainEditor::popMenuDisconnectTRigged);				
			}
			else {
				title = "Disconnect";
				SHOW_POP_MENU(title, apoUiMainEditor::popMenuDisconnectTRigged);
			}

		}
	}
	
}


void apoUiMainEditor::onFileChanged()
{
	MainWindow *pMain = dynamic_cast<MainWindow*> (this->parent());
	if (pMain)	{
		pMain->onFileChanged();
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
	
	for (QVector<apoUiBezier*>::iterator it = m_beziersVct.begin(); it!=m_beziersVct.end(); it++){
		(*it)->move(offset);
	}
	
	m_offset += offset;
	//scroll(offset.x(), offset.y());
	//nd_logdebug("drag to (%d,%d)\n", offset.x(), offset.y());
	this->update();
}

void apoUiMainEditor::saveCurPosWithOffset()
{
	QObjectList list = children();
	foreach(QObject *obj, list) {
		apoBaseExeNode *pexenode = qobject_cast<apoBaseExeNode*>(obj);
		if (pexenode){
			QPoint curPos = pexenode->pos();
			savePosition(pexenode,&curPos);
		}
	}
	onFileChanged();
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
		apoBaseSlotCtrl *pSlot = (apoBaseSlotCtrl*)w;

		m_curDragType = E_MOUSE_TYPE_DRAG_DRAW;
		m_drawingBezier = new apoUiBezier(this, event->pos(), event->pos());
		apoBaseSlotCtrl::eBaseSlotType type = pSlot->slotType();
		if (type == apoBaseSlotCtrl::SLOT_RUN_IN || type == apoBaseSlotCtrl::SLOT_RUN_OUT || type == apoBaseSlotCtrl::SLOT_SUB_ENTRY){
			m_drawingBezier->setColor(Qt::darkRed);
			m_drawingBezier->setWidth(2);
		}
		else {
			m_drawingBezier->setColor(Qt::green);
			m_drawingBezier->setWidth(1);
		}
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

			saveOffset(m_offset);
		}
		m_curDragType = E_MOUSE_TYPE_MOVE;
		return;
	}

	if (E_MOUSE_TYPE_DRAG_DRAW == m_curDragType) {
		QWidget *w = this->childAt(event->pos());
		if (trytoBuildConnector(dynamic_cast<apoBaseSlotCtrl*>(m_dragSrc), dynamic_cast<apoBaseSlotCtrl*>(w))) {
			onFileChanged();
		}

		if (m_drawingBezier){
			delete m_drawingBezier;
			m_drawingBezier = 0;
		}
	}
	else if (E_MOUSE_TYPE_MOVE == m_curDragType) {
		QPoint pos = m_dragSrc->pos();
		nd_logdebug("node move to(%d,%d)\n", pos.x(), pos.y());
		savePosition(dynamic_cast<apoBaseExeNode*>(m_dragSrc));
	}

	this->update();
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
		
		//savePosition(dynamic_cast<apoBaseExeNode*>(m_dragSrc));
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

	QVector<apoUiBezier*>::iterator it;
	for (it = m_beziersVct.begin(); it != m_beziersVct.end(); ++it) {
		(*it)->paintEvent();
	}

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
		emit showExenodeSignal(m_curDetailNode);
	}
	this->update();

	
}

void apoUiMainEditor::onExenodeDBClicked(apoBaseExeNode *exeNode, QMouseEvent * event)
{
	//get ditail window
	curDetailChanged(exeNode);
}

void apoUiMainEditor::onCurNodeChanged()
{
	if (m_curDetailNode){
		_reConnectParam(m_curDetailNode);
		m_curDetailNode->update();
	}
}

void apoUiMainEditor::onNodeAddNewParam(apoBaseExeNode *node)
{
	if (node == m_curDetailNode){
		curDetailChanged(node);
	}
}


bool apoUiMainEditor::trytoBuildConnector(apoBaseSlotCtrl *fromSlot, apoBaseSlotCtrl *toSlot)
{
	bool ret = false;
	if (!fromSlot || !toSlot || fromSlot == toSlot) {
		return false;
	}	
	apoBaseSlotCtrl::eBaseSlotType type1 = fromSlot->slotType();
	apoBaseSlotCtrl::eBaseSlotType type2 = toSlot->slotType();

	switch (type1)
	{
	case apoBaseSlotCtrl::SLOT_RUN_IN:
		if (type2 == apoBaseSlotCtrl::SLOT_RUN_OUT || type2 == apoBaseSlotCtrl::SLOT_SUB_ENTRY)	{
			ret = buildRunSerqConnector(toSlot, fromSlot);
		}
		break;

	case apoBaseSlotCtrl::SLOT_SUB_ENTRY:
	case apoBaseSlotCtrl::SLOT_RUN_OUT:
		if (type2 == apoBaseSlotCtrl::SLOT_RUN_IN)	{
			ret = buildRunSerqConnector(fromSlot, toSlot);
		}
		break;

	case apoBaseSlotCtrl::SLOT_FUNCTION_PARAM:
	case apoBaseSlotCtrl::SLOT_RETURN_VALUE:
	case apoBaseSlotCtrl::SLOT_VAR:
		if (type2 == apoBaseSlotCtrl::SLOT_NODE_INPUUT_PARAM)	{
			ret = buildParamConnector(fromSlot, toSlot);
		}
		break;
	case apoBaseSlotCtrl::SLOT_NODE_INPUUT_PARAM:
		if (type2 == apoBaseSlotCtrl::SLOT_RETURN_VALUE ||
			type2 == apoBaseSlotCtrl::SLOT_FUNCTION_PARAM||
			type2 == apoBaseSlotCtrl::SLOT_VAR
			)	{
			ret = buildParamConnector(toSlot, fromSlot);
		}
		break;
	case apoBaseSlotCtrl::SLOT_UNKNOWN:
		break;
	default:
		break;
	}

	return ret;
}

bool apoUiMainEditor::buildParamConnector(apoBaseSlotCtrl *fromSlot, apoBaseSlotCtrl *toSlot)
{
	if (!toSlot->onConnectIn(fromSlot)) {
		return false;
	}
	_connectSlots(fromSlot, toSlot);
	return true;
}


bool apoUiMainEditor::buildRunSerqConnector(apoBaseSlotCtrl *fromSlot, apoBaseSlotCtrl *toSlot)
{
	if (!toSlot->checkConnectIn()) {
		return false;
	}
	//disconnect fromslot
	apoUiBezier *pBze = fromSlot->getConnector();
	apoUiBezier *pBze1 = fromSlot->getConnector();

	if (pBze && pBze == pBze1){
		return false;
	}
	apoBaseExeNode *fromCtrl = dynamic_cast<apoBaseExeNode*> (fromSlot->parent());
	apoBaseExeNode *toCtrl = dynamic_cast<apoBaseExeNode*> (toSlot->parent());

	//can not connect self
	if (fromCtrl == toCtrl)	{
		return false;
	}

	ndxml *fromXml = (ndxml*)fromCtrl->getMyUserData();
	ndxml *toXml = (ndxml*)toCtrl->getMyUserData();

	//not connect
	ndxml *fromParent = ndxml_get_parent(fromXml);
	ndxml *toParent = ndxml_get_parent(toXml);


	if (fromParent ==toParent)	{
		int index1 = ndxml_get_index(fromParent, fromXml);
		int index2 = ndxml_get_index(toParent, toXml);
		if (index1 < index2)	{
			//can not connect to front
			return false;
		}
	}


	if (pBze){
		_disconnectRunSerq(fromSlot, dynamic_cast<apoBaseSlotCtrl*> (pBze->getSlot2()));
		_removeBezier(pBze);
	}
	if (pBze1){
		_disconnectRunSerq(dynamic_cast<apoBaseSlotCtrl*> (pBze1->getSlot1()), toSlot);
		_removeBezier(pBze1);
	}
	

	//build new connection
	ndxml *moveInRoot = ndxml_get_parent(fromXml);
	int insertPos = ndxml_get_index(moveInRoot, fromXml) + 1;

	if (fromXml == m_editedFunction){
		moveInRoot = m_editedFunction;
		insertPos = -1;
	}

	if (toCtrl->getMyNextNode()){
		apoBaseExeNode *moveCtrl = toCtrl;

		while (moveCtrl)	{
			apoBaseExeNode *next = moveCtrl->getMyNextNode();
			ndxml *movxml = (ndxml *)moveCtrl->getMyUserData();
			ndxml_disconnect(NULL, movxml);
			ndxml_insert_ex(moveInRoot, toXml, insertPos);
			insertPos++;
			toCtrl = next;
		};

	}
	else {
		ndxml_disconnect(NULL, toXml);
		ndxml_insert_ex(moveInRoot, toXml, insertPos);
	}

	_connectSlots(fromSlot, toSlot, apoUiBezier::LineRunSerq);

	return true;

}


bool apoUiMainEditor::_removeConnector(apoBaseSlotCtrl *slot)
{
	apoBaseSlotCtrl::eBaseSlotType type = slot->slotType(); 
	if (type == apoBaseSlotCtrl::SLOT_NODE_INPUUT_PARAM){
		apoUiBezier *pconn = slot->getConnector();
		if (pconn) {
			_removeBezier(pconn);
		}
		return slot->onDisconnect();
	}
	else {
		apoUiBezier *pbze = slot->getConnector();
		if (!pbze){
			return false;
		}
		if (_disconnectRunSerq((apoBaseSlotCtrl *)pbze->getSlot1(), (apoBaseSlotCtrl *)pbze->getSlot2())){
			this->_removeBezier(pbze);
		}
	}
	return false;
}


//disconnect existed connector
bool apoUiMainEditor::_disconnectRunSerq(apoBaseSlotCtrl *fromSlot, apoBaseSlotCtrl *toSlot)
{
	if (!fromSlot || !toSlot){
		return false;
	}
	apoBaseExeNode *toCtrl = dynamic_cast<apoBaseExeNode*> (toSlot->parent());
	if (!toCtrl)	{
		return false;
	}
	ndxml *xml = (ndxml *) toCtrl->getMyUserData();
	if (!xml)	{
		return false;
	}
	ndxml *parentXml = ndxml_get_parent(xml);
	if (!parentXml)	{
		return false;
	}

	ndxml *unConnectXml = getUnconnectRoot();
	if (toCtrl->getMyNextNode()){
		ndxml * movetoXml = ndxml_addnode(unConnectXml, "steps_collection", NULL);
		
		while (toCtrl)	{
			apoBaseExeNode *next = toCtrl->getMyNextNode();
			ndxml *movxml = (ndxml *)toCtrl->getMyUserData();
			ndxml_disconnect(NULL, movxml);
			ndxml_insert(movetoXml, movxml);
			toCtrl = next;
		};

	}
	else {
		ndxml_disconnect(NULL, xml);
		ndxml_insert(unConnectXml, xml);
	}
	
	return true;
}
