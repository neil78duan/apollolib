#include "xmldialog.h"
#include "ui_xmldialog.h"
#include "logic_parser/dbl_mgr.h"

#include "listdialog.h"
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTreeWidget>
#include "dragtree.h"

void _xml_errorout(const char *errdesc)
{
    nd_logerror("parse xml error %s", errdesc) ;
}

static int GetCreateName(ndxml *xml_template, char *buf, size_t size)
{
    char *p;
    p = (char*)ndxml_getattr_val(xml_template, "name");
    if (!p) {
        return -1;
    }
    else {
        strncpy(buf, p, size);
    }
    return 0;

}

static ndxml * CreateByTemplate(ndxml *root, ndxml *xml_template)
{
    char *val,*p ;
    char name[128] ;
    if (0!=GetCreateName(xml_template, name, sizeof(name))){
        return NULL;
    }
    val = (char*)ndxml_getattr_val(xml_template, "value");

    ndxml *new_xml = ndxml_addsubnode(root,name, val) ;
    if(!new_xml)
        return NULL ;

    ndxml *attr_xml =ndxml_refsub(xml_template, "attribute") ;
    if(attr_xml){
        for(int i=0; i<ndxml_getsub_num(attr_xml); i++) {
            ndxml *sub_node = ndxml_refsubi(attr_xml,i) ;
            if(sub_node) {
                p = (char*)ndxml_getattr_val(sub_node, "name");
                val = (char*)ndxml_getattr_val(sub_node, "value");
                if(p && val) {
                    ndxml_addattrib(new_xml,p,val);
                }
            }
        }
    }

    ndxml *child_node = ndxml_refsub(xml_template,"sub_node") ;
    if(child_node) {
        for(int i=0; i<ndxml_getsub_num(child_node); i++) {
            ndxml *sub_template = ndxml_refsubi(child_node,i) ;
            if(sub_template)
                CreateByTemplate(new_xml, sub_template) ;
        }
    }
    return new_xml ;
}

static void SetXmlName(ndxml *xml_node, ndxml *xmlParent)
{
    const char *pAutoIndex = ndxml_getattr_val(xmlParent, "auto_index");
    const char *pName = ndxml_getattr_val(xml_node, "name");
    if (pAutoIndex && pName ){
        int a = atoi(pAutoIndex);
        char buf[128];
        snprintf(buf, sizeof(buf), "%s%s", pName, pAutoIndex);
        ndxml_setattrval(xml_node, "name", buf);

        ++a;
        snprintf(buf, sizeof(buf), "%d", a);
        ndxml_setattrval(xmlParent, "auto_index", buf);
    }

}

XMLDialog::XMLDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XMLDialog)
{
    ui->setupUi(this);
    ui->xmlTree->setColumnCount(1);
    m_curItem = 0 ;
    m_dragItem = 0 ;

    m_stat = 0 ;
    m_inEditing = 0 ;
    m_inInit = 0 ;
    m_treeInInit = 0 ;


    m_myOKFunc = 0;
    m_myCancelFunc=0;
    QObject::connect(ui->xmlTree,  &dragTree::onMydragItem , this , &XMLDialog::DragCallBack);
}

XMLDialog::~XMLDialog()
{
    delete ui;
}

void XMLDialog::setButtonText(const char *okText, const char *cancelText)
{
    ui->ButtonCancel->setText(cancelText);
    ui->ButtonOK->setText(okText );
}

bool XMLDialog::preInit(dlgAccCallBack initF, dlgAccCallBack acceptF, dlgAccCallBack rejectF)
{
    if(initF) {
        initF(this) ;
    }

    m_myOKFunc = acceptF;
    m_myCancelFunc = rejectF;
    return true ;
}

bool XMLDialog::_GetFileName(bool bOpen, QString & strFile, const char *default_file)
{
    if (bOpen) {
        strFile = QFileDialog::getOpenFileName(this, tr("open file"), ".",  tr("Allfile(*.*);;xmlfile(*.xml)"));
    }
    else {
        strFile = QFileDialog::getSaveFileName(this, tr("open file"), ".",  tr("Allfile(*.*);;xmlfile(*.xml)"));
    }

    if(strFile.isNull()) {
        strFile = default_file ;
        return false ;
    }
    return true ;

}

bool XMLDialog::_OpenFilter(QString &strPath,const QString &tip)
{
    strPath = QFileDialog::getExistingDirectory(this,tip,".");
    if(strPath.isNull()) {
        return false ;
    }
    return true ;
}

bool XMLDialog::CreateXmlTree(ndxml_root *xml_root)
{
    ui->xmlTree->clear();

    m_treeInInit = 1 ;
    xmlTreeItem *root = new xmlTreeItem(ui->xmlTree, QStringList(QString(m_title.c_str())));

    root->setUserData(NULL);

    int num =ndxml_num(xml_root) ;
    for(int i=0;i<num; i++) {
        ndxml *node = ndxml_getnodei(xml_root,i) ;
        if (!node || CheckHide(node))
            continue ;
        InitTreeNode(node,root) ;
    }
    root->setExpanded(true);

    for(int i=0 ; i<root->childCount(); i++ ) {
        QTreeWidgetItem *pChild = root->child(i) ;
        ExpandTree(pChild) ;
    }

    m_curItem = 0 ;
    m_treeInInit = 0;
    return true ;
}

bool XMLDialog::ExpandTree(QTreeWidgetItem* hItem)
{
    if (!hItem){
        return false;
    }
    ndxml* xml = GetSelXml(hItem) ;
    if(!xml) {
        return false ;
    }

    bool ret = false;
    const char *pExpVal = ndxml_getattr_val(xml, _GetReverdWord(ERT_EXPAND_STAT));
    if (pExpVal && pExpVal[0]=='1'){
        ret = true;
        hItem->setExpanded(true);
        for(int i=0 ; i<hItem->childCount(); i++ ) {
            QTreeWidgetItem *pChild = hItem->child(i) ;
            ExpandTree(pChild) ;
            //pChild->setExpanded(true);

        }

    }

    return ret;
}

QTreeWidgetItem* XMLDialog::InitTreeNode(ndxml *xml_node,QTreeWidgetItem *hParent)
{
    xmlTreeItem *leaf =new xmlTreeItem(hParent, QStringList(QString(_GetXmlName(xml_node,&m_alias))));

    leaf->setFlags(leaf->flags()| (Qt::ItemIsEditable | Qt::ItemIsDragEnabled) );

    leaf->setUserData(xml_node);
    if(CheckExpand(xml_node)) {
        return leaf;
    }
    text_vect_t exp_list;
    get_string_array((char*)ndxml_getattr_val(xml_node, _GetReverdWord(ERT_EXPAND_LIST)), exp_list);

    int num =ndxml_getsub_num(xml_node);
    for(int i=0;i<num; i++) {
        ndxml *node = ndxml_refsubi(xml_node,i) ;
        if(!node)
            continue ;
        const char *pName = ndxml_getname(node);
        if (CheckHide(node) || std::find(exp_list.begin(), exp_list.end(),std::string(pName))!=exp_list.end())
            continue ;

        InitTreeNode(node,leaf) ;
    }

    return leaf;
}

#define BACKUP_NAME "xmlcfgbackup.xml"
void XMLDialog::SetXML(ndxml_root *config_xml, ndxml_root *data_root, const char *title)
{
    m_root = data_root ;
    nd_chdir(nd_getcwd());
    ndxml_save(data_root, BACKUP_NAME);

    m_config = config_xml;
    ndxml *alias = ndxml_getnode(m_config, "alias");
    if(alias)
        m_alias.Create(alias) ;
	if (!title || !*title){
		m_title = "xml_root";
	}
	else {
		m_title = title;
	}
    CreateXmlTree(m_root) ;
}
// 
// void XMLDialog::SetMessageID(ndxml_root *funclist_xml)
// {
//     ndxml *funcroot = ndxml_getnode(funclist_xml, "MessageDefine");
//     char buf[256];
//     if (funcroot) {
//         for (int i = 0; i < ndxml_num(funcroot); i++){
//             ndxml *fnode = ndxml_getnodei(funcroot, i);
//             const char *pDispname = ndxml_getattr_val(fnode, "comment");
//             const char *pRealVal = ndxml_getattr_val(fnode, "id");
//             const char *p = buildDisplaNameValStr(pRealVal, pDispname, buf, sizeof(buf));
// 
//             m_messageList.push_back(QString(p));
// 
//         }
//     }
// }
// 
// void XMLDialog::SetEventsID(ndxml_root *funclist_xml)
// {
//     ndxml *funcroot = ndxml_getnode(funclist_xml, "events_id");
//     char buf[256];
//     if (funcroot) {
//         for (int i = 0; i < ndxml_num(funcroot); i++){
//             ndxml *fnode = ndxml_getnodei(funcroot, i);
//             const char *pDispname = ndxml_getattr_val(fnode, "desc");
//             const char *pRealVal = ndxml_getval(fnode);
// 
//             const char *p = buildDisplaNameValStr(pRealVal, pDispname, buf, sizeof(buf));
//             m_eventsList.push_back(QString(p));
// 
//         }
//     }
// }

bool XMLDialog::loadUserdefDisplayList(ndxml_root &xmlNameList, const char *name)
{
	text_list_t textList;

	ndxml *funcroot = ndxml_getnode(&xmlNameList, name);
	char buf[256];
	if (funcroot) {
		for (int i = 0; i < ndxml_num(funcroot); i++){
			ndxml *fnode = ndxml_getnodei(funcroot, i);
			const char *pDispname = ndxml_getattr_val(fnode, "desc");
			const char *pRealVal = ndxml_getval(fnode);

			const char *p = buildDisplaNameValStr(pRealVal, pDispname, buf, sizeof(buf));
			textList.push_back(QString(p));

		}
		addDisplayNameList(name, textList);
		return true;
	}
	return false;
}


bool XMLDialog::addDisplayNameList(const char *name, text_list_t &text_list)
{
	std::pair< text_map_t::iterator, bool> ret = m_userDefineList.insert(std::make_pair(std::string(name), text_list));
	return ret.second;
}


bool XMLDialog::_GetUserDefinData(const char *pUserParam, QString &val)
{
    if (0==ndstricmp((char*)pUserParam,"func_list") ) {
        //m_root

        ListDialog dlg (this);
        ndxml *node = ndxml_getnode(m_root, "baseFunction");
        if (node)	{
            for (int i = 0; i < ndxml_num(node); i++){
                ndxml *funcNode = ndxml_getnodei(node,i);
                const char *name = (char*)ndxml_getattr_val(funcNode, _GetReverdWord(ERT_NAME));
                if (!name) {
                    name = (char*)ndxml_getname(funcNode);
                }
                dlg.m_selList.push_back(QString(name));
            }
        }
		text_map_t::const_iterator it = m_userDefineList.find(pUserParam);
		if (it != m_userDefineList.end()) {
			const text_list_t &exterFunclist = it->second;
			for (int i = 0; i < exterFunclist.size(); i++) {
				dlg.m_selList.push_back(exterFunclist[i]);
			}
		}

//         for (int i = 0; i < m_cPPfunclist.size(); i++) {
//             dlg.m_selList.push_back(m_cPPfunclist[i]);
//         }

        dlg.InitList();
        if (dlg.exec() == QDialog::Accepted) {
            return  dlg.GetSelectText(val) ;
        }

    }
//     else if (0 == ndstricmp((char*)pUserParam, "msg_list")) {
//         //m_root
//         return _GetUserSelEnumVal(m_messageList, val);
// 
//     }
//     else if (0 == ndstricmp((char*)pUserParam, "event_list")) {
//         return _GetUserSelEnumVal(m_eventsList, val);
//     }

    else if (0 == ndstricmp((char*)pUserParam, "dbl_excel")) {
        DBLDatabase *pdbl = DBLDatabase::get_Instant();
        if (!pdbl )	{
            nd_logerror("need load excel data") ;
            return false;
        }
        text_list_t texts;
        DBLDatabase::table_vct_t::iterator it = pdbl->m_tables.begin();
        for (; it!=pdbl->m_tables.end() ;it++)	{

            texts.push_back(QString(it->first.c_str()));
        }
        return _GetUserSelEnumVal(texts, val);
    }
    else if (0 == ndstricmp((char*)pUserParam, "dbl_excel_col")) {
        DBLDatabase *pdbl = DBLDatabase::get_Instant();
        if (!pdbl)	{
            nd_logerror("need load excel data") ;
            return false;
        }

        ndxml *xml = GetSelXml(m_curItem);
        if (!xml)	{
            return false;
        }
        ndxml *tablexml = ndxml_getnode(xml, "tablename");
        if (!tablexml)	{
            nd_logerror(" excel table data") ;
            return false;
        }
        const char *pname = ndxml_getval(tablexml);
        if (0==ndstricmp((char*)pname,(char*)"none"))	{

            nd_logerror("param not be none") ;
            return false;
        }
        DBLTable *ptable = pdbl->FindTable(pname);
        if (!ptable){
            nd_logerror("table not found") ;
            return false;
        }

        text_list_t texts;
        int total = ptable->GetCols();
        for (int i = 0; i < total; i++)	{
            if (pdbl->GetEncodeType() == E_SRC_CODE_UTF_8) {
                char buf[128];
                nd_utf8_to_gbk(ptable->GetColName(i), buf, sizeof(buf));
                texts.push_back(QString(buf));
            }
            else {
                texts.push_back(QString(ptable->GetColName(i)));
            }
        }

        return _GetUserSelEnumVal(texts, val);
    }
	else {
		text_map_t::const_iterator it = m_userDefineList.find(pUserParam);
		if (it != m_userDefineList.end()) {
			return _GetUserSelEnumVal(it->second, val);
		}
	}

    return false;
}
ndxml* XMLDialog::GetSelXml(QTreeWidgetItem* hItem)
{
    xmlTreeItem *node = (xmlTreeItem*)hItem ;
    return (ndxml*)node->getUserData() ;
    //return (ndxml*) (m_pTree->GetItemData(hItem) );
}

bool XMLDialog::_GetUserSelEnumVal(const text_list_t &enum_vals, QString &val)
{
    ListDialog dlg(this) ;
    for (int i = 0; i < enum_vals.size(); i++) {
        dlg.m_selList.push_back(enum_vals[i]);
    }

    dlg.InitList();
	dlg.setModal(true);
    if (dlg.exec() == QDialog::Accepted) {
        return dlg.GetSelectText(val) ;
    }

    return false;
}


int XMLDialog::DisplaySelXml()
{
    QTreeWidgetItem *hItem =  ui->xmlTree->currentItem() ;
    if(!hItem)
        return -1;
    ndxml* xml = GetSelXml(hItem) ;
    if(!xml)
        return -1;

    m_inInit = 0 ;
    _initShowCtrl(ui->xmlTable) ;
    ShowXMLValue(xml,ui->xmlTable, (int)CheckExpand(xml)) ;

    m_inInit = 1 ;
    return 0;
}


int XMLDialog::_initShowCtrl(QTableWidget *show_ctrl )
{
    int length = show_ctrl->width() ;
    show_ctrl->clear();

    show_ctrl->setColumnCount(2);
    show_ctrl->setRowCount(0);

    show_ctrl->setColumnWidth(0, length * 0.28);
    show_ctrl->setColumnWidth(1, length * 0.68);

    QStringList headers;
    headers << "Name" << "Value" ;
    show_ctrl->setHorizontalHeaderLabels(headers);
    return 0 ;
}

int XMLDialog::ShowXMLValue(ndxml *xml_node,QTableWidget *show_ctrl, int expand )
{
    int i;
    const char *pval , *name;
    //size_t param ;
    pval = ndxml_getval(xml_node) ;
    if(pval) {
        //const char *desc =_GetXmlDesc(xml_node);
        name = _GetXmlName(xml_node, &m_alias) ;

        const char *alias = m_alias.GetAlia(name) ;
        if(alias)
            name = alias ;

        ShowRow(name, pval, xml_node,show_ctrl) ;

    }
    for(i=0; i<ndxml_getattr_num(xml_node); i++) {

        name = (char*)ndxml_getattr_name(xml_node,i) ;
        if(!name || CheckReserved(name)>=0)
            continue;
        pval = (char*)ndxml_getattr_vali(xml_node, i);
        if(pval) {
            const char *alias = m_alias.GetAlia(name) ;
            if(alias)
                name = alias ;

            ShowRow(name, pval, xml_node,show_ctrl) ;
        }
    }

    if(expand) {
        for(i=0; i<ndxml_getsub_num(xml_node); i++) {
            ndxml *sub_node = ndxml_refsubi(xml_node,i) ;
            if(sub_node && !CheckHide(sub_node) ){
                ShowXMLValue(sub_node,show_ctrl,expand ) ;
            }
        }
    }
    else {
        text_vect_t exp_list;
        if (get_string_array((char*)ndxml_getattr_val(xml_node, _GetReverdWord(ERT_EXPAND_LIST)), exp_list)) {
            for (text_vect_t::iterator it = exp_list.begin(); it != exp_list.end(); it++) {
                ndxml *sub_node = ndxml_refsub(xml_node, (*it).c_str());
                if (sub_node && !CheckHide(sub_node)){
                    ShowXMLValue(sub_node, show_ctrl, 1);
                }
            }
        }
    }
    return 0;

}

void XMLDialog::ShowRow(const char *name, const char *val, ndxml* param,QTableWidget *show_ctrl)
{
    int rows = show_ctrl->rowCount() ;
    show_ctrl->setRowCount(rows+1);

    char realval[4096];
    const char *p = getDisplayNameFromStr(val, realval, sizeof(realval));

    xmlTableItem *tableNode = new xmlTableItem(QString(p)) ;
    tableNode->setUserData(param);


    if(param) {
        
        int type =  GetXmlValType(param, m_config) ;
        if(type==EDT_BOOL) {
            tableNode->setText(QString(val[0]=='0' ? "FALSE":"TRUE"));
        }
        else if(type==EDT_ENUM) {
            text_list_t enum_texts;
            if (getEnumList(param, m_config, enum_texts) > 0) {
                int sel = atoi(val);
                if (sel >= 0 && sel < (int)enum_texts.size() )
                    tableNode->setText(enum_texts[sel]);
            }

        }
        else if (type == EDT_KEY_VAL) {
            text_list_t key_texts,value_texts;
            if (getKeyList(param, m_config, key_texts) > 0 && getKeyValueList(param, m_config, value_texts)> 0) {
                for (size_t i = 0; i < value_texts.size(); i++) {
                    if (val == value_texts[i]) {
                        if (i < key_texts.size())	{
                            tableNode->setText(key_texts[i]);
                            //m_grad->SetItemText(row, 1, (LPCTSTR)key_texts[i]);
                        }
                        break;
                    }

                }
            }
        }
    }
    show_ctrl->setItem(rows, 0, new xmlTableItem(QString(name)) );
    show_ctrl->setItem(rows, 1, tableNode );

    nd_logdebug("show %s : %s ", name,p);


}

bool XMLDialog::_beginEdit(int nRow, int nCol)
{
    xmlTableItem *cell = (xmlTableItem*) ui->xmlTable->item( nRow, nCol) ;
    ndxml *xml = (ndxml *)cell->getUserData() ;
    if (!xml)	{
        return false;
    }
    bool ret = false;
    QString  str1;
    int type = GetXmlValType(xml, m_config);
    if (type == EDT_IN_FILE || type == EDT_OUT_FILE) {

        if (_GetFileName(type == EDT_IN_FILE, str1, GetXmlParam(xml))) {
            if (0 == ndxml_setval(xml, str1.toStdString().c_str())) {
                ret = true;
            }
        }
    }
    else if (EDT_DIR == type) {
        if (_OpenFilter(str1, tr("select direct"))){
            str1 += '/' ;
            if (0 == ndxml_setval(xml, str1.toStdString().c_str()) ){
                ret = true;
            }
        }
    }
    else if (EDT_USER_DEFINE == type) {

        QString str2;
        const char *pUserparam = ndxml_getattr_val(xml, _GetReverdWord(ERT_USER_DEFINE_PARAM));
        if (_GetUserDefinData(pUserparam, str2)) {
            std::string stdText = str2.toStdString() ;
            if (0 == ndxml_setval(xml, stdText.c_str())) {
                char realval[256];
                const char *p = getDisplayNameFromStr(stdText.c_str(), realval, sizeof(realval));
                str1 = p;
                ret = true;
            }
        }
    }
    else if(EDT_BOOL == type) {
        ListDialog dlg(this) ;
        dlg.m_selList.push_back("FALSE");
        dlg.m_selList.push_back("TRUE");
        dlg.InitList();
        if (dlg.exec() == QDialog::Accepted) {
            dlg.GetSelectText(str1) ;
            int selval = dlg.GetSelect() ;
            ndxml_setval(xml,selval ? "1":"0");
            ret = true ;
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
                    ndxml_setval(xml,str3.toStdString().c_str());
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
                        ndxml_setval(xml, value_texts[sel].toStdString().c_str() );
                        ret = true;
                    }
                    //else {
                       // AfxMessageBox("≈‰÷√Œƒº˛¥ÌŒÛ,key∫Õvalue≥§∂»≤ª“ª÷¬");
                    //}
                }
            }
        }
    }

    if (ret) {

        cell->setText(str1) ;
        //CfgChanged();
    }
    return ret;

}

void XMLDialog::OnPopInsertNode()
{
    xmlTreeItem *curItem = (xmlTreeItem*) ui->xmlTree->currentItem() ;
    if(!curItem)
        return ;
    ndxml *xml =(ndxml *) curItem->getUserData();
    if(!xml) {
        return ;
    }

    ndxml *create_template = GetCreateTemplate(xml, m_config) ;
    if(!create_template) {
        return ;
    }

    int tempType = 0;
    char *p = (char*)ndxml_getattr_val(create_template, "create_type");
    if (p)	{
        tempType = atoi(p);
    }

    if (tempType == TEMPLATE_LIST){
        ndxml *temp_root = ndxml_getnode(m_config, _GetReverdWord(ERT_TEMPLATE));
        if (!temp_root)
            return;
        std::vector<const char*> creat_temp_list;
        ListDialog dlg(this);
        for (int i = 0; i < ndxml_getsub_num(create_template); ++i){
            ndxml *subnode = ndxml_refsubi(create_template,i);
            const char *temp_real_name = ndxml_getval(subnode);
            ndxml *temp_node = ndxml_refsub(temp_root, temp_real_name);
            QString str1;
            if (temp_node){
                str1 = _GetXmlName(temp_node, &m_alias);
            }
            else{
                str1 = temp_real_name;
            }

            dlg.m_selList.push_back(str1);
            creat_temp_list.push_back(temp_real_name);
        }

        dlg.InitList();
        if (dlg.exec() != QDialog::Accepted) {
            return  ;
        }

        int seled = dlg.GetSelect();
        if (seled==-1) {
            return;
        }
        create_template = ndxml_refsub(temp_root, creat_temp_list[seled]);
        if (!create_template){
            return;
        }
        p = (char*)ndxml_getattr_val(create_template, "create_type");
        if (p)	{
            tempType = atoi(p);
        }
    }

    ndxml *new_xml=NULL;
    if (tempType == TEMPLATE_CREAT){
        new_xml = CreateByTemplate(xml, create_template);
        if (new_xml) {
            SetXmlName(new_xml, xml);
            CfgChanged();
            QTreeWidgetItem* hNodeNew = InitTreeNode(new_xml, curItem);
            if (hNodeNew) {
                ui->xmlTree->setCurrentItem(hNodeNew);
            }
        }
    }
    else if (tempType == TEMPLATE_DIRECT) {
        for (int i = 0; i< ndxml_num(create_template); i++)	{
            ndxml *sub1 = ndxml_refsubi(create_template, i);
            new_xml = ndxml_copy(sub1);

            if (new_xml) {
                ndxml_insert(xml, new_xml);
                SetXmlName(new_xml, xml);

                CfgChanged();
                QTreeWidgetItem* hNodeNew = InitTreeNode(new_xml, curItem);
                if (hNodeNew) {
                    ui->xmlTree->setCurrentItem(hNodeNew);
                }
            }
        }
    }

}
ndxml *XMLDialog::GetCurrentXml()
{
    xmlTreeItem *curItem = (xmlTreeItem*) ui->xmlTree->currentItem() ;
    if(!curItem)
        return NULL;
    return (ndxml *) curItem->getUserData();
}

void XMLDialog::OnPopXmlDel()
{
    xmlTreeItem *curItem = (xmlTreeItem*) ui->xmlTree->currentItem() ;
    if(!curItem)
        return ;
    ndxml *xml =(ndxml *) curItem->getUserData();
    if(xml) {
        if(! CheckCanDelete(xml) ) {
            nd_logerror("this item not be deleted");
            return ;
        }
    }
    else {
        return ;
    }

    xmlTreeItem *hParent = (xmlTreeItem*)curItem->parent();
    if(!hParent ) {
        return ;
    }
    int ret =  QMessageBox::question(this,tr("Question"), tr("Do you delete?"),
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::Yes);
    if (QMessageBox::No ==ret ) {
        return ;
    }
    ndxml *xml_parent = (ndxml *) hParent->getUserData();
    if (xml_parent) {
        ndxml_delxml(xml,xml_parent);
        CreateXmlTree(m_root);
        //CfgChanged();
    }

}
void XMLDialog::on_xmlTree_itemClicked(QTreeWidgetItem *item, int column)
{
    if(item == (QTreeWidgetItem *) m_curItem) {
        ui->xmlTree->editItem(item);
        m_curItem = (xmlTreeItem*)item ;
    }
    else {
        m_curItem = (xmlTreeItem*)item ;
        DisplaySelXml() ;
    }
}

void XMLDialog::on_xmlTable_cellClicked(int row, int column)
{
    m_inInit = 1 ;
    m_dragItem = 0 ;
    nd_logdebug(" clicked cell (%d, %d)", row, column) ;
    _beginEdit(row, column) ;
}

void XMLDialog::on_xmlTable_itemChanged(QTableWidgetItem *item)
{
    if(m_inInit==0) {
        return ;
    }
    int ret = -1 ;
    xmlTableItem *pItem = dynamic_cast<xmlTableItem*>(item) ;
    if(!pItem) {
        return ;
    }

    ndxml *xml =(ndxml *) pItem->getUserData()  ;
    if(xml){
        QString str1 =  pItem->text() ;
        if (GetXmlValType(xml, m_config) == EDT_ENUM) {
            int sel = -1;
            text_list_t enum_texts;
            if (getEnumList(xml, m_config, enum_texts) > 0) {
                for (int i = 0; i < enum_texts.size(); i++)	{
                    if (str1 == QString(enum_texts[i]) ) {
                        sel = i;
                        break;
                    }
                }
            }
            if (-1 != sel) {
                str1.sprintf("%d", sel);
                ret = ndxml_setval(xml, str1.toStdString().c_str());
            }

        }
        else if (GetXmlValType(xml, m_config) == EDT_KEY_VAL) {
            int sel = -1;
            text_list_t enum_texts;
            if (getEnumList(xml, m_config, enum_texts) > 0) {
                for (int i = 0; i < enum_texts.size(); i++)	{
                    if (str1 == enum_texts[i]) {
                        sel = i;
                        break;
                    }
                }
            }
            if (-1 != sel) {
                text_list_t value_texts;
                getKeyValueList(xml, m_config, value_texts);
                if (sel < value_texts.size()){
                    ndxml_setval(xml, value_texts[sel].toStdString().c_str());
                    ret = true;
                }
                //else {
                //    AfxMessageBox("≈‰÷√Œƒº˛¥ÌŒÛ,key∫Õvalue≥§∂»≤ª“ª÷¬");
                //}
            }

        }
        else if(!str1.isNull()) {
            ret = ndxml_setval(xml, str1.toStdString().c_str()) ;
            CfgChanged() ;
            ret = 0;
        }
    }
}

void XMLDialog::on_xmlTree_customContextMenuRequested(const QPoint &pos)
{
    xmlTreeItem* curItem = (xmlTreeItem*) ui->xmlTree->itemAt(pos) ;
    if(!curItem)
        return ;

    ui->xmlTree->setCurrentItem(curItem);
    //m_curItem = curItem ;

    ndxml *xml =(ndxml *) curItem->getUserData() ;
    if(!xml) {
        return ;
    }

    QMenu *pop_menu = new QMenu(this);

    ndxml *create_template = GetCreateTemplate(xml, m_config) ;
    bool addOk = false ;

    if(create_template) {
        QAction *add_node = new QAction(this);
        add_node->setText(tr("add"));
        pop_menu->addAction(add_node) ;

        connect(add_node, &QAction::triggered, this , &XMLDialog::OnPopInsertNode) ;
        addOk = true ;
    }
    if(CheckCanDelete(xml)) {

        QAction *del_node = new QAction(this);
        del_node->setText(tr("del"));
        pop_menu->addAction(del_node) ;
        connect(del_node, &QAction::triggered, this , &XMLDialog::OnPopXmlDel) ;
        addOk = true ;
    }

    if(addOk) {
        pop_menu->exec(QCursor::pos());
    }
    else {
        delete pop_menu ;
    }
}

void XMLDialog::on_xmlTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    //m_curItem =(xmlTreeItem*) current ;
    DisplaySelXml() ;
}


void XMLDialog::on_xmlTree_itemCollapsed(QTreeWidgetItem *item)
{
    if(m_treeInInit)
        return ;
    xmlTreeItem* curItem = dynamic_cast<xmlTreeItem*>(item);
    if(!curItem)
        return ;
    ndxml *xml = (ndxml *) curItem->getUserData() ;
    if (xml) {
        ndxml_setattrval(xml, "expand_stat", "0");
        nd_logmsg("Collapsed %s",  _GetXmlName(xml, &m_alias) ) ;
        CfgChanged();
    }

}

void XMLDialog::on_xmlTree_itemExpanded(QTreeWidgetItem *item)
{
    if(m_treeInInit)
        return ;

    xmlTreeItem* curItem = dynamic_cast<xmlTreeItem*>(item);
    if(!curItem)
        return ;
    ndxml *xml = (ndxml *) curItem->getUserData() ;
    if (xml) {
        ndxml_setattrval(xml, "expand_stat", "1");
        nd_logmsg("expand %s",  _GetXmlName(xml, &m_alias) ) ;
        CfgChanged();
    }

}

void XMLDialog::on_xmlTree_itemChanged(QTreeWidgetItem *item, int column)
{
    xmlTreeItem* curItem = dynamic_cast<xmlTreeItem*>(item);
    if(!curItem)
        return ;
    ndxml *xml = (ndxml *) curItem->getUserData() ;
    if (xml) {
        QString text1 = item->text(column) ;
        ndxml_setattrval(xml, "name",text1.toStdString().c_str()) ;
        nd_logmsg("itemchange %s : %s",  _GetXmlName(xml, &m_alias) , text1.toStdString().c_str()) ;
        CfgChanged();
    }
    m_inEditing = 0 ;

}

////drag and drop
//void XMLDialog::on_xmlTree_itemEntered(QTreeWidgetItem *item, int column)
//{
//    m_dragItem = 0 ;
//    xmlTreeItem* curItem = dynamic_cast<xmlTreeItem*>(item);
//    if(!curItem)
//        return ;
//    ndxml *xml = (ndxml *) curItem->getUserData() ;
//    if (xml) {
//        m_dragItem = curItem ;
//        QString text1 = item->text(column) ;
//        nd_logmsg("begining drag item %s ... ",  _GetXmlName(xml, &m_alias)) ;
//    }

//}


//void XMLDialog::mouseReleaseEvent(QMouseEvent *event)
//{
//    if(m_dragItem) {
//        xmlTreeItem* curItem = (xmlTreeItem*) ui->xmlTree->itemAt(event->pos()) ;
//        if(curItem) {
//            TreeDragCallback(m_dragItem,curItem) ;
//        }
//        m_dragItem = NULL ;
//    }
//    else {
//        QDialog::mouseReleaseEvent(event);
//    }
//}

void XMLDialog::DragCallBack(QTreeWidgetItem* from, QTreeWidgetItem* to)
{
    TreeDragCallback((xmlTreeItem*) from, (xmlTreeItem*) to) ;
}

bool XMLDialog::TreeDragCallback(xmlTreeItem*  hFrom, xmlTreeItem* hTo)
{
    xmlTreeItem *parent1 = (xmlTreeItem *) hFrom->parent() ;
    xmlTreeItem *parent2 = (xmlTreeItem *) hTo->parent();
    if (parent2!=parent1){
        return false;
    }
    ndxml *xmlparent = GetSelXml(parent1);
    ndxml*xmlFrom = GetSelXml(hFrom);
    ndxml*xmlTo = GetSelXml(hTo);
    if (!xmlparent|| !xmlFrom || !xmlTo)	{
        return false;
    }

    nd_logmsg("drag  item %s to %s",  _GetXmlName(xmlFrom, &m_alias) , _GetXmlName(xmlTo, &m_alias)) ;

    const char *pEnableDrag = ndxml_getattr_val(xmlparent, "enable_drag");
    if (!pEnableDrag){
        return false;
    }
    if (0!=ndstricmp(pEnableDrag,"yes")){
        return false;
    }

    int index = ndxml_get_index(xmlparent, xmlTo);
    int src_index = ndxml_get_index(xmlparent, xmlFrom);
    if (-1==index || -1==src_index){
        return false;
    }
    if (-1==ndxml_remove(xmlFrom, xmlparent)){
        return false;
    }
    if (src_index > index) {
        index -= 1;
    }
    if (-1 == ndxml_insert_ex(xmlparent, xmlFrom, index)) {
        return false;
    }

    CreateXmlTree(m_root);
    CfgChanged();
    return true;
}


void XMLDialog::on_ButtonOK_clicked()
{
    if(m_myOKFunc) {
        bool ret = m_myOKFunc(this) ;
        if(ret) {
            return ;
        }
    }
    QDialog::accept() ;
}

void XMLDialog::on_ButtonCancel_clicked()
{
    if(m_myCancelFunc) {
        bool ret = m_myCancelFunc(this) ;
        if(ret) {
            return ;
        }
    }
    QDialog::reject() ;

}
