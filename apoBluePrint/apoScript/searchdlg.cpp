#include "apoScript/searchdlg.h"
#include "ui_searchdlg.h"
#include "logic_parser/logic_compile.h"
#include "apoScript/apoEditorSetting.h"
#include <QMessageBox>
#include <string>

SearchDlg::SearchDlg(ndxml *xmlFile, QWidget *parent) :m_xmlFile(xmlFile),
    QDialog(parent),
    ui(new Ui::SearchDlg)
{
	ndxml_initroot(&m_xmlResult);
    ui->setupUi(this);

	ui->searchEdit->setFocus();
}

SearchDlg::~SearchDlg()
{
    delete ui;
}

void SearchDlg::on_SearchButton_clicked()
{
	QString str1 = ui->searchEdit->text();
	if (str1.size() == 0){

		QMessageBox::warning(this, "searched", "Please input text !", QMessageBox::Ok);
		ui->searchEdit->setFocus();
		return;
	}
	std::string stdText = str1.toStdString();
	searchText(stdText.c_str());
	QDialog::accept();
}

void SearchDlg::on_CancelButton_clicked()
{
	QDialog::reject();
}


ndxml *SearchDlg::getResult()
{
	if (ndxml_getsub_num(&m_xmlResult) == 0)	{
		return NULL;
	}
	return &m_xmlResult;
}

int SearchDlg::searchOnlyCurrent(ndxml *xml,const char *aimText)
{
	const char *pval = ndxml_getname(xml);
	if (ndstristr(pval, aimText)) {
		storeSearchValue(xml);
		return 1;
	}
	pval = ndxml_getattr_val(xml,"name");
	if (pval){
		if (ndstristr(pval, aimText)) {
			storeSearchValue(xml);
			return 1;
		}
	}

	pval = ndxml_getval(xml);
	if (pval){
		if (ndstristr(pval, aimText)) {
			storeSearchValue(xml);
			return 1;
		}
	}
	return 0;
}

void SearchDlg::storeSearchValue(ndxml *curNode)
{
	std::string dispName  ;
	ndxml *parent = ndxml_get_parent(curNode);
	ndxml *node = curNode;

	while (parent) {
		std::string myName = LogicEditorHelper::_GetXmlName(node,NULL);
		dispName = myName + dispName;
		
		node = parent;
		parent = ndxml_get_parent(node);
		if (parent)	{
			dispName = '_' + dispName;
		}
	}
	char value[1024];
	LogicCompiler::getFuncStackInfo(curNode, value, sizeof(value));

	ndxml *val = ndxml_addnode(&m_xmlResult, "result", value);
	if (val){
		ndxml_setattrval(val, "name", dispName.c_str());
	}

}
int SearchDlg::searchText(const char *aimText)
{
	for (int x = 0; x < ndxml_num(m_xmlFile); x++){
		ndxml *node = ndxml_getnodei(m_xmlFile, x);
		if (LogicCompiler::isFileInfo(node)) {
			continue;
		}
		searchFromXml(node,aimText);
	}

	return 0;
}
int SearchDlg::searchFromXml(ndxml *xml, const char *aimText)
{
	int ret;
	apoEditorSetting *pSetting = apoEditorSetting::getInstant();
	nd_assert(pSetting);

	for (int i = 0; i < ndxml_num(xml); i++) {
		ndxml *node = ndxml_getnodei(xml, i);	
		const char *stepInsName = ndxml_getname(node);
		if (!stepInsName){
			continue;
		}
		if (LogicEditorHelper::CheckHide(node)){
			continue;
		}
		const compile_setting* compileInfo = pSetting->getStepConfig(stepInsName);
		if (!compileInfo){
			continue; 
		}
		ret = 0;
		instructType instType = (instructType) compileInfo->ins_type;
		switch (instType)
		{
		case E_INSTRUCT_TYPE_COMMENT:
		case E_INSTRUCT_TYPE_LABEL:
		case E_INSTRUCT_TYPE_GOTO:
		case E_INSTRUCT_TYPE_PARAM:
		case E_INSTRUCT_TYPE_PARAM_COLLECT:
			searchOnlyCurrent(node, aimText);
			break;

		case E_INSTRUCT_TYPE_FUNCTION:
		case E_INSTRUCT_TYPE_CMD:
			ret = searchOnlyCurrent(node, aimText);
			if (!ret)	{
				ret = searchFromXml(node, aimText);
			}
			break;
		case E_INSTRUCT_TYPE_COLLOCTION:
		case E_INSTRUCT_TYPE_CASE_ENTRY:
		case E_INSTRUCT_TYPE_STEP_BLOCK:
		case E_INSTRUCT_TYPE_INIT_BLOCK:
		case E_INSTRUCT_TYPE_SUB_ENTRY:
		case E_INSTRUCT_TYPE_EXCEPTION_CATCH:
		case E_INSTRUCT_TYPE_LOOP:
			ret = searchFromXml(node,aimText);
			break;

		case E_INSTRUCT_TYPE_DELAY_COMPILE:
		case E_INSTRUCT_TYPE_REF_FROM_PARENT:
		case E_INSTRUCT_TYPE_FUNCTION_INFO:
		case E_INSTRUCT_TYPE_USER_DEFINE:
			continue;
			
		default:
			break;
		}
	}
	return 0;
}