

#include "logic_parser/logic_editor_helper.h"
#include "apoEditorSetting.h"

#include "runFuncDialog.h"
#include "ui_runFuncDialog.h"

using namespace LogicEditorHelper;
RunFuncDialog::RunFuncDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RunFuncDialog)
{
    ui->setupUi(this);
	
}

RunFuncDialog::~RunFuncDialog()
{
    delete ui;
}


QString RunFuncDialog::getArgs()
{
	return ui->editArgs->text();
}
QString RunFuncDialog::getFunc()
{
	return ui->comboFunc->currentText();
}

bool RunFuncDialog::initFunctionList(ndxml *xmlFile, const char *dftFunc)
{
#define INSERT_FUNCTIONS(_xml, _collectName) \
	do 	{	\
		ndxml *xmlBlock = ndxml_getnode(_xml,_collectName) ;	\
		if(xmlBlock) { insertFunctions( xmlBlock) ; }			\
	} while (0)

	INSERT_FUNCTIONS(xmlFile, "baseFunction");
	INSERT_FUNCTIONS(xmlFile, "message_handler");
	INSERT_FUNCTIONS(xmlFile, "baseFunction");

	if (dftFunc){
		ui->comboFunc->setCurrentText(QString(dftFunc));
	}
	return true;
}


bool RunFuncDialog::insertFunctions(ndxml *xmlFuncCollection)
{

	apoEditorSetting *setting = apoEditorSetting::getInstant();
	for (int i = 0; i < ndxml_num(xmlFuncCollection); i++){
		
		ndxml *funcNode = ndxml_getnodei(xmlFuncCollection, i);
		const char *stepName = ndxml_getname(funcNode);
		const compile_setting* stepSetting = setting->getStepConfig(stepName);

		if (!stepSetting)	{
			continue;
		}

		if (stepSetting->ins_type == E_INSTRUCT_TYPE_STEP_BLOCK){
			insertFunctions(funcNode);
		}
		else if (E_INSTRUCT_TYPE_FUNCTION == stepSetting->ins_type) {

			const char *name = (char*)ndxml_getattr_val(funcNode, "name");
			if (name && *name)	{
				ui->comboFunc->addItem(QString(name));
			}
		}
		

	}
	return true;
}
/*
{
	ListDialog dlg(this);
	ndxml *node = ndxml_getnode(m_rootFile, "baseFunction");
	if (node)	{
		for (int i = 0; i < ndxml_num(node); i++){
			ndxml *funcNode = ndxml_getnodei(node, i);
			const char *name = (char*)ndxml_getattr_val(funcNode, _GetReverdWord(ERT_NAME));
			if (!name) {
				name = (char*)ndxml_getname(funcNode);
			}
			dlg.m_selList.push_back(QString(name));
		}
	}
	const apoEditorSetting::text_vct_t  *pList = m_rootSetting->getUserdefList(pUserParam);
	if (pList)	{
		for (int i = 0; i < pList->size(); i++) {
			dlg.m_selList.push_back((*pList)[i]);
		}
	}

	dlg.InitList();
	if (dlg.exec() == QDialog::Accepted) {
		return  dlg.GetSelectText(val);
	}

}*/