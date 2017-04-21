/* file apoUiMainEditor.h
 *
 * main editor window
 *
 * create by duan
 * 2017.2.9
 */



#include "apoScript/apoUiMainEditor.h"
#include "apoScript/apoBaseExeNode.h"
#include "apoScript/mainwindow.h"
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
	m_curDragType = E_MOUSE_TYPE_NONE;
	//m_popMenuType = E_POP_DEL;
	m_scriptFileXml = 0;
	m_funcEntry = NULL;
	m_setting = apoEditorSetting::getInstant();
	m_connectToSlot = NULL;
	//m_editingToNext = 0;
	m_scale = 0;
}


bool apoUiMainEditor::showFunction(ndxml *data, ndxml_root *xmlfile)
{
	clearFunction();
	m_editedFunction = data;
	m_scriptFileXml = xmlfile;

	m_startX = DEFAULT_START_X;
	m_startY = DEFAULT_START_Y;


	QPoint init_offset(0,0);	
	getOffset(init_offset);
	nd_logdebug("load function %s offset(%d,%d)\n", _GetXmlName(data,NULL), init_offset.x(), init_offset.y());

	m_labelsMap.clear();
	m_gotoMap.clear();

	if (!_createFuncEntry(m_editedFunction, QPoint(m_startX, m_startY))) {
		return false;
	}
	buildAllGotoLine();

	m_labelsMap.clear();
	m_gotoMap.clear();

	dragTo(init_offset);

	showNodeDetail(m_funcEntry);
	//update();
	return true;
}


void apoUiMainEditor::clearFunction()
{
	for (int i = 0; i < m_beziersVct.size(); i++){
		delete m_beziersVct[i];
	}
	m_beziersVct.clear();
	m_varMap.clear();
	m_labelsMap.clear();
	m_gotoMap.clear();

	m_curDragType = E_MOUSE_TYPE_NONE;
	//m_popMenuType = E_POP_DEL;

	m_dragSrc = NULL;
	m_popSrc = NULL;

	m_drawingBezier = NULL;

	m_editedFunction = 0;
	m_startX = 0;
	m_startY = 0;

	m_scale = 0;

	m_connectToSlot = NULL;
	m_moveOffset = QPoint(0, 0);
	m_curClicked = QPoint(0, 0);
	m_offset =  QPoint(0, 0);

	m_funcEntry = NULL;
	m_scriptFileXml = NULL;
	m_curDetailNode = NULL;


	QObjectList list = children();
	foreach(QObject *obj, list) {
		QWidget *subWin = qobject_cast<QWidget*>(obj);
		if (subWin){
			subWin->close();
		}
	}
	m_setting = apoEditorSetting::getInstant();
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
		if (CheckHide(xmlStep))	{
			continue;
		}
		const compile_setting* stepInfo = m_setting->getStepConfig(stepInsName);
		if (!stepInfo) {
			continue;
		}

		if (stepInfo->ins_type == E_INSTRUCT_TYPE_COMMENT || stepInfo->ins_type == E_INSTRUCT_TYPE_PARAM_COLLECT)	{
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
			ret = _createFuncEntry(xmlStep, QPoint(10, 20));
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
				//skip connector when this node is newvar
				if (pExenode->toNext() ){
					fromSlot = pExenode->toNext();
				}
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

		else if (stepInfo->ins_type == E_INSTRUCT_TYPE_COLLOCTION || 
			stepInfo->ins_type == E_INSTRUCT_TYPE_STEP_BLOCK) {
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

bool apoUiMainEditor::_showSubByslot(apoBaseSlotCtrl *subSlot)
{
	ndxml *xmlblock = subSlot->getXmlAnchorParent();
	if (!xmlblock){
		nd_logerror("can not get xml display node entry\n");
		return false;
	}
	return _showBlocks(subSlot, xmlblock);
}

bool apoUiMainEditor::_showBools(apoBaseExeNode *entryNode, ndxml *entryXml)
{

	apoUiExenodeBool*pBoolNode = dynamic_cast<apoUiExenodeBool*>(entryNode);
	if (!pBoolNode){
		return false;
	}
	int subX = m_startX;
	int subY = m_startY;

	m_startY = subY - 200;
	m_startX = subX;

	if (!_showSubByslot(pBoolNode->getTrueSlot())) {
		return false;
	}
	
	m_startY = subY;
	m_startX = subX;
	if (!_showSubByslot(pBoolNode->getFalseSlot())) {
		return false;
	}

	return true;
	
}

bool apoUiMainEditor::_showLoop(apoBaseExeNode *entryNode, ndxml *stepsBlocks)
{
	apoUiExenodeLoop*pNode = dynamic_cast<apoUiExenodeLoop*>(entryNode);
	if (!pNode){
		return false;
	}

	int subX = m_startX;
	int subY = m_startY;

	m_startY -= 200;
	bool ret = _showSubByslot(pNode->getLoopSlot());


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
	bool ret = _showSubByslot(pNode->getSubSlot());

	m_startX = subX;
	m_startY = subY;
	return ret;
}

bool apoUiMainEditor::_showSWitch(apoBaseExeNode *entryNode, ndxml *stepsBlocks)
{
	apoUiExenodeSwitch*pNode = dynamic_cast<apoUiExenodeSwitch*>(entryNode);
	if (!pNode){
		return false;
	}

	int subX = m_startX;
	int subY = m_startY;

	int num = pNode->getSubBlockNum();
	for (int i = 0; i < num; i++){
		m_startY = (subY - 200) + i * 200;
		m_startX = subX;
		bool ret = _showSubByslot(pNode->getSubSlot(i));
		if (!ret)	{
			return false;
		}
	}

	_showSubByslot(pNode->getDefault());

	m_startX = subX;
	m_startY = subY;
	return true;
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
	//nd_logdebug("begin show function pos(%d,%d), offset(%d,%d)\n", pos.x(), pos.y(), m_offset.x(), m_offset.y());
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
	m_labelsMap.clear();
	m_gotoMap.clear();
	bool ret = _showBlocks(pbeginNode->toNext(), stepsBlocks);
	if (ret) {
		buildAllGotoLine();
	}

	m_labelsMap.clear();
	m_gotoMap.clear();
	return ret;
}


apoBaseExeNode* apoUiMainEditor::_showExeNode(apoBaseSlotCtrl *fromSlot, ndxml *exeNode, const QPoint &pos)
{
	apoBaseExeNode *nodeCtrl = g_apoCreateExeNode(exeNode, this);
	if (!nodeCtrl){
		return NULL;
	}

	//change current ditail show view
	QObject::connect(nodeCtrl, SIGNAL(NodeAddParamSignal(apoBaseExeNode *)),
		this, SLOT(onNodeAddNewParam(apoBaseExeNode *)));

	
	nodeCtrl->move(pos);
	nodeCtrl->show(); //must call show functiong	
	
	apoBaseSlotCtrl *connectIn = nodeCtrl->inNode();
	
	if (connectIn && fromSlot ) {
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
		ret = _showLoop(nodeCtrl, exeNode);
	}
	else if (nodeCtrl->getType() == EAPO_EXE_NODE_ValueComp) {
		ret = _showValueComp(nodeCtrl, exeNode);
	}
	else if (nodeCtrl->getType() == EAPO_EXE_NODE_Switch) {
		ret = _showSWitch(nodeCtrl, exeNode);
	}


	if (!ret) {		
		nodeCtrl->close();
		return	 NULL;
	}

	if (nodeCtrl->getType() == EAPO_EXE_NODE_NewVar){
		pushVarList(exeNode, nodeCtrl);
	}
	if (nodeCtrl->getLabel()){
		m_labelsMap[nodeCtrl->getLabel()] = nodeCtrl;
	}
	if (nodeCtrl->getGotoLabel()) {
		m_gotoMap[nodeCtrl->getGotoLabel()] = nodeCtrl;
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

// 		nd_logdebug("save %s pos(%s,%s) offset (%d,%d)\n",_GetXmlName(m_editedFunction,NULL),
// 			ndxml_getattr_val(pPoint, "x"), ndxml_getattr_val(pPoint, "y"),
// 			offset.x(), offset.y());
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

ndxml *apoUiMainEditor::getUnconnectRoot(ndxml* xmlFunc)
{
	ndxml *unConnNode = ndxml_getnode(xmlFunc, "unConnect");
	if (!unConnNode) {
		const char *nodeText = "<unConnect kinds=\"hide\" />";
		unConnNode = ndxml_from_text(nodeText);
		ndxml_insert(m_editedFunction, unConnNode);
	}
	else {
		int num = ndxml_getsub_num(unConnNode);
		for (int i = 0; i < num; i++)	{
			ndxml *subnode = ndxml_getnodei(unConnNode, i);
			if (ndxml_getsub_num(subnode)==0)	{
				ndxml_delxml(subnode, unConnNode);
				break;
			}
		}
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
		//nd_logdebug("get position node(%d,%d), offset(%d,%d)\n", pos.x(), pos.y(), m_offset.x(), m_offset.y());
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

void apoUiMainEditor::movedInScale(apoBaseExeNode *exeNode, const QPoint &screenPos)
{
	QPoint newPos = screenPos / m_scale;

	exeNode->setOrgPos(exeNode, newPos);
	//exeNode->move(newPos);
	//exeNode->getOrgPos(exeNode);	//save org pos
	exeNode->move(screenPos);
	
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

	if (m_scale != 0 && m_scale != 1.0f) {
		pos /= m_scale;
		exeNode->setOrgPos(exeNode, pos);
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

	//nd_logdebug("save position node(%d,%d), offset(%d,%d)\n", pos.x(), pos.y(), m_offset.x(), m_offset.y());
	onFileChanged();
}


bool apoUiMainEditor::_connectSlots(apoBaseSlotCtrl *fromSlot, apoBaseSlotCtrl *toSlot, apoUiBezier::eBezierType type)
{
	if (!fromSlot || !toSlot) {
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
		if (!fromSlot->checkValid() || !toSlot->checkValid()) {
			pbezier->setColor(Qt::gray);
		}
		else {
			pbezier->setColor(Qt::darkRed);
		}
		pbezier->setWidth(2);
	}

	pbezier->setType(type);
	fromSlot->setConnector(pbezier);
	if (type != apoUiBezier::LineGoto)	{
		toSlot->setConnector(pbezier);
	}
	else {
		nd_logdebug("create a goto line \n");
	}

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

bool apoUiMainEditor::_removeBezier(apoUiBezier *connector, bool bWithDestroy)
{
	QVector<apoUiBezier*>::iterator it = m_beziersVct.begin();
	for (; it != m_beziersVct.end(); it++)	{
		if ((*it) == connector) {

			if (bWithDestroy) {
				connector->onRemove();
			}

			delete *it;
			m_beziersVct.erase(it);
			return true;
		}
	}
	return false;
}


bool apoUiMainEditor::_removeExenode(apoBaseExeNode *node)
{
	_disConnectParam(node);

	if (node->getType() == EAPO_EXE_NODE_NewVar){
		_disConnectVar(node);
	}

	if (node->getGotoLabel()){
		_disGotoLine(node);
	}

	apoBaseExeNode *myPre = node->getMyPreNode();
	apoBaseExeNode *myNext = node->getMyNextNode();
	
	QObjectList list = node->children();
	foreach(QObject *obj, list) {
		apoBaseSlotCtrl *slot = qobject_cast<apoBaseSlotCtrl*>(obj);
		if (slot){
			_removeConnector(slot);
		}
	}

	
	if (myPre && myNext){
		buildRunSerqConnector(myPre->toNext(), myNext->inNode());
	}

	ndxml *xml = (ndxml*)node->getMyUserData();
	if (xml) {
		ndxml_delxml(xml, NULL);
	}
	node->close();

	this->update();
	onFileChanged();
	return true;
}

void apoUiMainEditor::_disConnectParam(apoBaseExeNode *changedNode, bool bWithDestroy )
{
	for (int i = 0; i < changedNode->getParamNum(); i++) {
		apoBaseParam *paramCtrl = changedNode->getParam(i);
		apoUiBezier *pBze = paramCtrl->getConnector();
		if (pBze) {
			_removeBezier(pBze, bWithDestroy);
		}
	}
}


void apoUiMainEditor::_disConnectVar(apoBaseExeNode *varNode)
{
	QVector<apoUiBezier*>::iterator it = m_beziersVct.begin();
	for (; it != m_beziersVct.end(); )	{
		if ((*it)->getSlot1() == varNode->returnVal())	{
			(*it)->onRemove();
			delete *it;
			it = m_beziersVct.erase(it);
		}
		else {
			++it;
		}		
	}
}

void apoUiMainEditor::_disGotoLine(apoBaseExeNode *labelNode)
{
	QVector<apoUiBezier*>::iterator it = m_beziersVct.begin();
	for (; it != m_beziersVct.end();)	{
		if ((*it)->getSlot2() == labelNode->inNode())	{
			(*it)->onRemove();
			delete *it;
			it = m_beziersVct.erase(it);
		}
		else {
			++it;
		}
	}
}

void apoUiMainEditor::_reConnectParam(apoBaseExeNode *changedNode)
{
	nd_assert(changedNode);
	_disConnectParam(changedNode,false);

	//reconnect 
	_connectParam(changedNode->getMyPreNode(), changedNode);
}

bool apoUiMainEditor::pushVarList(ndxml *xmlNode, apoBaseExeNode*nodeCtrl)
{
// 	ndxml *varName = ndxml_getnode(xmlNode, "param");
// 	if (!varName){
// 		return false;
// 	}
// 	const char *pName = ndxml_getval(varName);
	const char *pName = ((apoUiExenodeNewVar*)nodeCtrl)->getVarName();
	if (pName) {
		m_varMap[pName] = nodeCtrl;
		return true;
	}
	return false;
}


apoBaseExeNode *apoUiMainEditor::createNewVarNode(const QPoint &pos)
{
	ndxml *newxml = m_setting->AddNewXmlByTempName(m_editedFunction, "create_make_var");

	apoBaseExeNode *exeNode = _showExeNode(NULL, newxml, pos);
	if (!exeNode || exeNode->getType() != EAPO_EXE_NODE_NewVar){
		ndxml_delxml(newxml, NULL);
		nd_logerror("create %s Exenode Error \n", _GetXmlName(newxml, NULL));
		return NULL;
	}
	ndxml_disconnect(m_editedFunction, newxml);
	ndxml_insert_after(m_editedFunction, newxml, m_editedFunction);

	if (m_scale != 0 && m_scale != 1.0f){
		movedInScale(exeNode, pos);
		exeNode->setScale(m_scale);
	}

	savePosition(exeNode);
	showNodeDetail(exeNode);
	onFileChanged();
	nd_logmsg("create %s SUCCESS\n", _GetXmlName(newxml, NULL));
	return exeNode;
}

apoBaseExeNode *apoUiMainEditor::createExenode(const QPoint &pos)
{
	ndxml *newxml = m_setting->AddNewXmlNode(m_editedFunction, this);
	if (!newxml) {
		nd_logerror("can not create new node for this function , maybe data is destroyed\n");
		return NULL;
	}

	apoBaseExeNode *exeNode = _showExeNode(NULL, newxml, pos);
	if (!exeNode){
		ndxml_delxml(newxml, NULL);
		nd_logerror("create %s Exenode Error \n", _GetXmlName(newxml, NULL));
		return NULL;
	}

	if (exeNode->getType() == EAPO_EXE_NODE_ModuleInitEntry || exeNode->getType() == EAPO_EXE_NODE_ExceptionEntry){
	}
	else if (exeNode->getType() == EAPO_EXE_NODE_NewVar && !exeNode->inNode()){
		ndxml_disconnect(m_editedFunction, newxml);
		ndxml_insert_after(m_editedFunction, newxml,m_editedFunction);
	}
	else {
		ndxml *unConnNode = getUnconnectRoot(m_editedFunction);
		if (!unConnNode) {
			nd_logerror("can not create unconnected nodes \n");
			ndxml_delxml(newxml, NULL);
			return NULL;
		}
		ndxml_disconnect(m_editedFunction, newxml);
		ndxml_insert(unConnNode, newxml);
	}

	if (m_scale != 0 && m_scale != 1.0f){
		movedInScale(exeNode, pos);
		exeNode->setScale(m_scale);
	}

	savePosition(exeNode);
	showNodeDetail(exeNode);
	onFileChanged();
	nd_logmsg("create %s SUCCESS\n", _GetXmlName(newxml, NULL));
	return exeNode;
}


void apoUiMainEditor::popMenuAddnewTrigged()
{
	createExenode(m_curClicked);
}

void apoUiMainEditor::popMenuDeleteTrigged()
{
	if (m_popSrc) {
		apoBaseExeNode *exeNode = dynamic_cast<apoBaseExeNode*>(m_popSrc);
		if (exeNode && exeNode->isDeletable())	{
			_removeExenode(exeNode);
		}
		else {
			nd_logmsg("this node can not be deleted\n");
		}
		m_popSrc = NULL;
	}

}

void apoUiMainEditor::popMenuAddLabelTrigged()
{
	if (m_popSrc) {
		apoBaseExeNode *exeNode = dynamic_cast<apoBaseExeNode *>(m_popSrc);
		if (exeNode){
			exeNode->addJumpLabel(NULL);
			showNodeDetail(exeNode);
			this->update();
		}
	}
}

void apoUiMainEditor::popMenuDelLabelTrigged()
{
	if (m_popSrc) {
		apoBaseExeNode *exeNode = dynamic_cast<apoBaseExeNode *>(m_popSrc);
		if (exeNode){
			exeNode->delLabel();
			showNodeDetail(exeNode);
			this->update();
		}
	}
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
		parent->closeParam(pslot);
		showNodeDetail(parent);
		this->update();
		onFileChanged();
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
		onFileChanged();
	}
}

void apoUiMainEditor::popMenuCenterTrigged()
{
	//move first not to (20,20) ;
	QPoint startPos = m_funcEntry->pos();
	QPoint newOffset = QPoint(DEFAULT_START_X, DEFAULT_START_Y) - startPos;

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
		
		SHOW_POP_MENU2(QString("New"), apoUiMainEditor::popMenuAddnewTrigged, QString("Moveto default"), apoUiMainEditor::popMenuCenterTrigged);
	}
	else {
		QString title;
		apoBaseExeNode*pDelNode = dynamic_cast<apoBaseExeNode*>(w);
		if (pDelNode) {
			m_popSrc = w;
			title = "Remove ";
			//title += pDelNode->getTitle();

			ndxml *xmlNode = (ndxml *) pDelNode->getMyUserData();
			if (pDelNode->getLabel()) {
				SHOW_POP_MENU2(title, apoUiMainEditor::popMenuDeleteTrigged, QString("Remove label"), apoUiMainEditor::popMenuDelLabelTrigged);
			}
			else if (LogicEditorHelper::_getXmlParamVal(xmlNode, NULL, LogicEditorHelper::ERT_CREATE_LABEL)) {
				SHOW_POP_MENU2(title, apoUiMainEditor::popMenuDeleteTrigged, QString("Add label"), apoUiMainEditor::popMenuAddLabelTrigged);				
			}
			else {
				SHOW_POP_MENU(title, apoUiMainEditor::popMenuDeleteTrigged);

			}
		}
		else if (dynamic_cast<apoBaseSlotCtrl*>(w))	{
			apoBaseSlotCtrl *pslot = (apoBaseSlotCtrl*)w;
			m_popSrc = w;

			if (pslot->getConnector())	{
				title = "Disconnect";
				SHOW_POP_MENU(title, apoUiMainEditor::popMenuDisconnectTRigged);
			}
			else if (pslot->isDelete())	{
				title = "Close ";
				title += pslot->text();
				SHOW_POP_MENU(title, apoUiMainEditor::popMenuCloseParamTrigged);
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
	m_bInDrag = false;
	m_dragSrc = NULL; 
	QWidget *w = this->childAt(event->pos());
	
	if (w == NULL){
		m_curDragType = E_MOUSE_TYPE_MOVE_VIEW;
		m_moveOffset = event->pos();
		//return;
	}	
	else if (dynamic_cast<apoBaseExeNode*>(w))	{
		m_dragSrc = w;
		m_curDragType = E_MOUSE_TYPE_MOVE_NODE;
		m_moveOffset = m_dragSrc->mapFromGlobal(event->globalPos());
	}
	else if (dynamic_cast<apoBaseSlotCtrl*>(w)) {
		m_connectToSlot = NULL;
		m_dragSrc = w;
		
		m_curDragType = E_MOUSE_TYPE_CONNECT_SLOT;		
		m_curClicked = event->pos();
		
	}
	else {
		m_curDragType = E_MOUSE_TYPE_MOVE_VIEW;
		m_moveOffset = event->pos();
	}
	
	this->update();
}

void apoUiMainEditor::mouseMoveEvent(QMouseEvent *event)
{

	if (m_curDragType == E_MOUSE_TYPE_MOVE_VIEW){
		QPoint offsetPt = event->pos();
		offsetPt -= m_moveOffset;
		dragTo(offsetPt);
		m_moveOffset = event->pos();
	}
	else if (m_curDragType == E_MOUSE_TYPE_MOVE_NODE)	{
		nd_assert(m_dragSrc);

		QPoint newPos = event->pos() - m_moveOffset;

		if (m_scale != 0 && m_scale != 1.0f){
			movedInScale(dynamic_cast<apoBaseExeNode*>(m_dragSrc), newPos);
		}
		else {
			m_dragSrc->move(newPos);
		}


	}
	else if (m_curDragType == E_MOUSE_TYPE_CONNECT_SLOT){
		
		if (!m_drawingBezier) {
			
			apoBaseSlotCtrl *pSlot = (apoBaseSlotCtrl*)m_dragSrc;
			m_drawingBezier = new apoUiBezier(this, m_curClicked, event->pos());
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
		
		if (m_drawingBezier){
			m_drawingBezier->paintTo(event->pos());
			m_drawingBezier->paint();
		}

		//highlight aim slot
		QWidget *w = this->childAt(event->pos());
		if (w)	{
			if (w != m_dragSrc){
				m_connectToSlot = dynamic_cast<apoBaseSlotCtrl*>(w);
				if (m_connectToSlot)	{
					if (testBuildConnector(dynamic_cast<apoBaseSlotCtrl*>(m_dragSrc), m_connectToSlot)) {
						m_connectToSlot->setInDrag(true);
					}
					else {
						m_connectToSlot = NULL;
					}
				}
			}

		}
		else {
			if (m_connectToSlot){
				m_connectToSlot->setInDrag(false);
			}
			m_connectToSlot = NULL;
		}
	}
	m_bInDrag = true;
	this->update();
}
void apoUiMainEditor::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_curDragType == E_MOUSE_TYPE_MOVE_VIEW){
		QPoint offsetPt = event->pos();
		offsetPt -= m_moveOffset;
		dragTo(offsetPt);
		m_moveOffset = event->pos();
		saveOffset(m_offset);
	}
	else if (E_MOUSE_TYPE_CONNECT_SLOT == m_curDragType) {
		nd_assert(m_dragSrc);
		QWidget *w = this->childAt(event->pos());
		if (w)	{
			apoBaseSlotCtrl *fromSlot = dynamic_cast<apoBaseSlotCtrl*>(m_dragSrc);
			if (trytoBuildConnector(fromSlot, dynamic_cast<apoBaseSlotCtrl*>(w))) {
				onFileChanged();
			}
			if (fromSlot){
				fromSlot->setInDrag(false);
			}
		}
		else {
			apoBaseSlotCtrl *fromSlot = dynamic_cast<apoBaseSlotCtrl*>(m_dragSrc);
			if (!fromSlot){
				return;
			}
			if (fromSlot->slotType() == apoBaseSlotCtrl::SLOT_RUN_OUT || fromSlot->slotType() == apoBaseSlotCtrl::SLOT_SUB_ENTRY) {
				apoBaseExeNode *newExeNode = createExenode(event->pos());
				if (newExeNode)	{
					if (trytoBuildConnector(fromSlot, newExeNode->inNode())) {
						onFileChanged();
					}
				}
			}
			else if (fromSlot->slotType() == apoBaseSlotCtrl::SLOT_RETURN_VALUE ||
				fromSlot->slotType() == apoBaseSlotCtrl::SLOT_VAR||
				fromSlot->slotType() == apoBaseSlotCtrl::SLOT_FUNCTION_PARAM ) {
				
				apoBaseExeNode *newExeNode = createNewVarNode(event->pos());
				if (newExeNode)	{
					if (trytoBuildConnector(fromSlot, newExeNode->getParam(1))) {
						onFileChanged();
					}
				}
			}
			
			else if (fromSlot->slotType() == apoBaseSlotCtrl::SLOT_NODE_INPUUT_PARAM) {

				apoBaseExeNode *newExeNode = createNewVarNode(event->pos());
				if (newExeNode)	{
					if (trytoBuildConnector(newExeNode->returnVal() ,fromSlot)) {
						onFileChanged();
					}
				}
			}
		}

		if (m_connectToSlot){
			m_connectToSlot->setInDrag(false);
		}
		m_connectToSlot = NULL;

	}
	else if (E_MOUSE_TYPE_MOVE_NODE == m_curDragType) {
		if (!m_bInDrag){
			apoBaseExeNode* clickedNode = dynamic_cast<apoBaseExeNode*>(m_dragSrc);
			if (clickedNode){
				showNodeDetail(clickedNode);
			}
		}
		else {
			savePosition(dynamic_cast<apoBaseExeNode*>(m_dragSrc));
		}
	}
	
	if (m_drawingBezier){
		delete m_drawingBezier;
		m_drawingBezier = 0;
	}
	m_bInDrag = false;
	
	this->update();
	m_dragSrc = NULL;
	m_curDragType = E_MOUSE_TYPE_NONE;
}


void apoUiMainEditor::wheelEvent(QWheelEvent *event)
{
	int delta = event->delta();
	//nd_logmsg("mouse wheel deta=%d\n", delta);
	if (m_scale == 0){
		// first scale 
		saveCurPosWithOffset();
		m_offset = QPoint(0, 0);
		saveOffset(m_offset);
		m_scale = 1.0f;
	}

	float scale = m_scale + (float)delta / 10000.0f;
	setScale(scale);

// 
// 	m_scale = scale;
// 	if (m_scale > 2.0f)	{
// 		m_scale = 2.0f;
// 	}
// 	else if (m_scale < 0.25f) {
// 		m_scale = 0.25f;
// 	}
// 
// 	QObjectList list = children();
// 	foreach(QObject *obj, list) {
// 		apoBaseExeNode *exeNode = qobject_cast<apoBaseExeNode*>(obj);
// 		if (exeNode ){
// 			exeNode->setScale(m_scale);
// 		}
// 	}
// 	this->update();
// 	paintEvent( NULL );
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


void apoUiMainEditor::showNodeDetail(apoBaseExeNode *exenode)
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


bool apoUiMainEditor::setCurDetail(ndxml *xmlNode, bool inError)
{

	QObjectList list = children();
	foreach(QObject *obj, list) {
		apoBaseExeNode *exeNode = qobject_cast<apoBaseExeNode*>(obj);
		if (exeNode && exeNode->getMyUserData()==xmlNode){
			showNodeDetail(exeNode);
			if(inError){
				exeNode->setError(true) ;
			}
			return true;
		}
	}
	return false;
}

bool apoUiMainEditor::setCurNodeSlotSelected(ndxml *xmlParam, bool inError)
{
	if (!m_curDetailNode){
		return false;
	}

	int num = m_curDetailNode->getParamNum();
	for (int i = 0; i < num; i++){
		apoBaseParam *paramSlot = m_curDetailNode->getParam(i);
		if (paramSlot && paramSlot->getValueXml() == xmlParam) {
			
			if(inError) {
				paramSlot->setError(true);
			}
			else {
				paramSlot->setInDrag(true);
			}
			
			return true;
		}
	}

	return false;

}

float apoUiMainEditor::setScale(float scale)
{
	float oldScale = m_scale;
	
	m_scale = scale;
	if (m_scale > 2.0f)	{
		m_scale = 2.0f;
	}
	else if (m_scale < 0.25f) {
		m_scale = 0.25f;
	}

	QObjectList list = children();
	foreach(QObject *obj, list) {
		apoBaseExeNode *exeNode = qobject_cast<apoBaseExeNode*>(obj);
		if (exeNode){
			exeNode->setScale(m_scale);
		}
	}
	this->update();
	paintEvent(NULL);
	return oldScale;
}


const char *apoUiMainEditor::getEditedFunc()
{
	if (m_editedFunction){
		return ndxml_getattr_val(m_editedFunction, "name");
	}
	return NULL;
}

void apoUiMainEditor::onCurNodeChanged()
{
	if (m_curDetailNode && m_curDetailNode->getType() != EAPO_EXE_NODE_Switch){
		m_curDetailNode->renewDisplay();
		_reConnectParam(m_curDetailNode);
		m_curDetailNode->update();
	}
	onFileChanged();
}

void apoUiMainEditor::onNodeAddNewParam(apoBaseExeNode *node)
{
	if (node == m_curDetailNode){
		showNodeDetail(node);
	}
}

void apoUiMainEditor::onFuncNameChanged(ndxml *funcXml)
{
	if (m_editedFunction == funcXml && m_funcEntry){
		m_funcEntry->renewDisplay();
	}
}


bool apoUiMainEditor::testBuildConnector(apoBaseSlotCtrl *fromSlot, apoBaseSlotCtrl *toSlot)
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
			ret = true;
		}
		break;

	case apoBaseSlotCtrl::SLOT_SUB_ENTRY:
	case apoBaseSlotCtrl::SLOT_RUN_OUT:
		if (type2 == apoBaseSlotCtrl::SLOT_RUN_IN)	{
			ret = true;
		}
		break;

	case apoBaseSlotCtrl::SLOT_FUNCTION_PARAM:
	case apoBaseSlotCtrl::SLOT_RETURN_VALUE:
	case apoBaseSlotCtrl::SLOT_VAR:
		if (type2 == apoBaseSlotCtrl::SLOT_NODE_INPUUT_PARAM)	{
			ret = true;
		}
		break;
	case apoBaseSlotCtrl::SLOT_NODE_INPUUT_PARAM:
		if (type2 == apoBaseSlotCtrl::SLOT_RETURN_VALUE ||
			type2 == apoBaseSlotCtrl::SLOT_FUNCTION_PARAM ||
			type2 == apoBaseSlotCtrl::SLOT_VAR
			)	{
			ret = true;
		}
		break;
	case apoBaseSlotCtrl::SLOT_UNKNOWN:
		break;
	default:
		break;
	}

	return ret;
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
	
	apoUiBezier *pBze = toSlot->getConnector();
	if (pBze){
		_removeBezier(pBze,false);
	}
	_connectSlots(fromSlot, toSlot);
	apoBaseExeNode *toCtrl = dynamic_cast<apoBaseExeNode*> (toSlot->parent());
	//
	//change order
	apoBaseExeNode *fromCtrl = dynamic_cast<apoBaseExeNode*> (fromSlot->parent());
	ndxml *fromXml = (ndxml *) fromCtrl->getMyUserData();
	ndxml *toXml = (ndxml *)toCtrl->getMyUserData();

	int fromIndex = ndxml_get_index(ndxml_get_parent(fromXml), fromXml);
	int toIndex = ndxml_get_index(ndxml_get_parent(toXml), toXml);

	if (fromCtrl->getType() == EAPO_EXE_NODE_NewVar&& !fromCtrl->toNext() ){
		if (ndxml_get_parent(fromXml) == ndxml_get_parent(toXml) ) 	{
			if (toIndex < fromIndex) {
				ndxml_disconnect(NULL, fromXml);
				ndxml_insert_before(ndxml_get_parent(toXml), fromXml, toXml);
			}
		}
		else if (_CheckIsChild(fromXml, "unConnect")) {
			ndxml_disconnect(NULL, fromXml);
			ndxml_insert_before(ndxml_get_parent(toXml), fromXml, toXml);
		}
	}

	else if (toCtrl->getType() == EAPO_EXE_NODE_NewVar && !toCtrl->inNode() ){
		
		if (ndxml_get_parent(fromXml) == ndxml_get_parent(toXml)) 	{
			ndxml_disconnect(NULL, toXml);
			ndxml_insert_after(ndxml_get_parent(fromXml), toXml, fromXml);
		}
		else if (fromSlot->slotType() == apoBaseSlotCtrl::SLOT_RETURN_VALUE || _CheckIsChild(toXml, "unConnect")) {

			ndxml_disconnect(NULL, toXml);
			ndxml_insert_after(ndxml_get_parent(fromXml), toXml, fromXml);
		}
	}
	//---end order changed

	showNodeDetail(toCtrl);

	return true;
}


bool apoUiMainEditor::buildRunSerqConnector(apoBaseSlotCtrl *fromSlot, apoBaseSlotCtrl *toSlot)
{
	if (!toSlot->checkConnectIn()) {
		return false;
	}
	//disconnect fromslot
	apoUiBezier *pBze = fromSlot->getConnector();
	apoUiBezier *pBze1 = toSlot->getConnector();

	if (pBze && pBze == pBze1){
		nd_logmsg("alread in connector!\n");
		return false;
	}
	//apoBaseExeNode *fromCtrl = dynamic_cast<apoBaseExeNode*> ();
	apoBaseExeNode *toCtrl = dynamic_cast<apoBaseExeNode*> (toSlot->parent());

	//can not connect self
	if (fromSlot->parent() ==toCtrl)	{

		nd_logmsg("can not connect self!\n");
		return false;
	}

	ndxml *fromXml = fromSlot->getXmlAnchor(); //(ndxml*)fromCtrl->getMyUserData();
	ndxml *toXml =  (ndxml*)toCtrl->getMyUserData();

	//not connect
	ndxml *fromParent = fromSlot->getXmlAnchorParent();  //ndxml_get_parent(fromXml);
	ndxml *toParent = ndxml_get_parent(toXml);


	if (pBze1 && toCtrl->getLabel()){
		return trytoBuildGotoConnector(fromSlot,toSlot);
	}
	else if (pBze1 && fromParent == toParent)	{
		int index1 = ndxml_get_index(fromParent, fromXml);
		int index2 = ndxml_get_index(toParent, toXml);
		if (index1 > index2)	{
			//can not connect to front

			nd_logmsg("can not connect the node front of this!\n");
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
	ndxml *moveInRoot = fromSlot->getXmlAnchorParent();// ndxml_get_parent(fromXml);
	ndxml *insertPosXml = fromSlot->getXmlAnchor();
		
	while (toXml){
		ndxml *movedParent = ndxml_get_parent(toXml);
		ndxml_disconnect(NULL, toXml);
		if (-1 == ndxml_insert_after(moveInRoot, toXml, insertPosXml)) {
			if (movedParent){
				ndxml_insert(movedParent, toXml);
			}
			else {
				ndxml_delxml(toXml, NULL);
			}
			nd_assert(0); 
			nd_logerror("can not connect the node \n");
			return false;
		}
		insertPosXml = toXml;
		toCtrl->onConnected();

		toXml = NULL;
		toCtrl = toCtrl->getMyNextNode();
		if (toCtrl){
			toXml = (ndxml *)toCtrl->getMyUserData();
		}
	}
	
	_connectSlots(fromSlot, toSlot, apoUiBezier::LineRunSerq);

	return true;

}

bool apoUiMainEditor::trytoBuildGotoConnector(apoBaseSlotCtrl *fromSlot, apoBaseSlotCtrl *toSlot)
{
	apoUiBezier *pBze = fromSlot->getConnector();
	//apoUiBezier *pBze1 = toSlot->getConnector();

	apoBaseExeNode *fromCtrl = dynamic_cast<apoBaseExeNode*> (fromSlot->parent());
	apoBaseExeNode *toCtrl = dynamic_cast<apoBaseExeNode*> (toSlot->parent());
	
	if (pBze){
		_disconnectRunSerq(fromSlot, dynamic_cast<apoBaseSlotCtrl*> (pBze->getSlot2()));
		_removeBezier(pBze);
	}

	const char *label = toCtrl->getLabel();
	fromCtrl->addGotoNode(label);

	if (_connectSlots(fromSlot, toSlot, apoUiBezier::LineGoto)) {
		apoUiBezier *bze = m_beziersVct[m_beziersVct.size() - 1];
		bze->setType(apoUiBezier::LineGoto);
	}
	return true;
}


void apoUiMainEditor::buildAllGotoLine()
{
	for (varinat_map_t::iterator it = m_gotoMap.begin(); it != m_gotoMap.end(); ++it){
		const std::string &labels = it->first;
		varinat_map_t::iterator aim_it = m_labelsMap.find(labels);
		if (aim_it == m_labelsMap.end()){
			continue;
		}
		apoBaseExeNode* fromCtrl = it->second;
		apoBaseExeNode* toCtrl = aim_it->second;

		if (!_connectSlots(fromCtrl->toNext(), toCtrl->inNode(), apoUiBezier::LineGoto)) {
			nd_logmsg("connect goto line error (%s -> %s)\n", fromCtrl->getTitle().toStdString().c_str(), toCtrl->getTitle().toStdString().c_str());
		}
	}
}

bool apoUiMainEditor::_removeConnector(apoBaseSlotCtrl *slot)
{
	apoBaseSlotCtrl::eBaseSlotType type = slot->slotType(); 
	if (type == apoBaseSlotCtrl::SLOT_NODE_INPUUT_PARAM||
		type == apoBaseSlotCtrl::SLOT_RETURN_VALUE ||
		type == apoBaseSlotCtrl::SLOT_VAR ||
		type == apoBaseSlotCtrl::SLOT_FUNCTION_PARAM){

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
			return true;
		}
	}
	return false;
}


void apoUiMainEditor::_removeGotoConnect(apoBaseSlotCtrl *fromSlot, apoBaseSlotCtrl *toSlot)
{
	apoBaseExeNode *fromCtrl = dynamic_cast<apoBaseExeNode*> (fromSlot->parent());
	nd_assert(fromCtrl);
	fromCtrl->delGotoNode();
}

//disconnect existed connector
bool apoUiMainEditor::_disconnectRunSerq(apoBaseSlotCtrl *fromSlot, apoBaseSlotCtrl *toSlot)
{
	if (!fromSlot || !toSlot){
		return false;
	}

	apoUiBezier *pbze = fromSlot->getConnector();
	nd_assert(pbze);
	if (pbze->getType() == apoUiBezier::LineGoto){
		_removeGotoConnect(fromSlot, toSlot);
		return true;
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

	ndxml *unConnectXml = getUnconnectRoot(m_editedFunction);
	if (toCtrl->getMyNextNode()){
		ndxml * movetoXml = ndxml_addnode(unConnectXml, "steps_collection", NULL);
		
		while (toCtrl)	{
			
			apoBaseSlotCtrl *_nextSlot = toCtrl->toNext();
			apoBaseExeNode *next = toCtrl->getMyNextNode();

			if (next && _nextSlot) {
				pbze = _nextSlot->getConnector();
				if (pbze->getType() == apoUiBezier::LineGoto){
					_removeGotoConnect(fromSlot, toSlot);
					next = NULL;
				}
			}

			ndxml *movxml = (ndxml *)toCtrl->getMyUserData();
			ndxml_disconnect(NULL, movxml);
			ndxml_insert(movetoXml, movxml);

			toCtrl->onDisconnected();
			toCtrl = next;
		};

	}
	else {
		ndxml_disconnect(NULL, xml);
		ndxml_insert(unConnectXml, xml);
		toCtrl->onDisconnected();
	}
	
	return true;
}
