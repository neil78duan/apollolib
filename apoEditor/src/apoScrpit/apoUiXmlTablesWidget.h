/* file apoUiXmlTablesWidget.h
*
* apoUi xml table editor 
*
* create by duan 
*
* 2017.2.24
*/

#ifndef _APOUI_XMLTABLES_WIDGET_H_
#define _APOUI_XMLTABLES_WIDGET_H_

#include "myqtitemctrl.h"
#include "logic_parser/logic_editor_helper.h"
#include "listdialog.h"
#include "apoScrpit/apoEditorSetting.h"
#include "apoScrpit/apoUiXmlTablesWidget.h"

#include <QWidget>
#include <QTableWidget>
#include <QResizeEvent>
#include <QVector>


#define UNEDIT_CTRL(itemCtrl)	 itemCtrl->setFlags(itemCtrl->flags() & (~Qt::ItemIsEditable))
class apoUiXmlTableWidget : public QTableWidget
{
	Q_OBJECT
public:
	explicit apoUiXmlTableWidget(QWidget *parent = Q_NULLPTR);
	apoUiXmlTableWidget(int rows, int columns, QWidget *parent = Q_NULLPTR);
	~apoUiXmlTableWidget();


	virtual bool showDetail(void *data, ndxml_root *xmlfile);
	void setDisplayFile(ndxml_root *filexml) { m_rootFile = filexml; }


public slots:
	void onCellClicked(int row, int column);
	void oCellChanged(int row, int column);

	//signal
signals:
	void xmlDataChanged();
protected:

	void preShow();
	void postShow();
	void dataChangedSignal()
	{
		emit xmlDataChanged();
	}

	bool _beginEdit(int nRow, int nCol);
	bool _GetUserDefinData(const char *pUserParam, QString &val);
	bool _GetUserSelEnumVal(const apoEditorSetting::text_vct_t &enum_vals, QString &val);
	bool _GetFileName(bool bOpen, QString & strFile, const char *default_file);
	bool _OpenFilter(QString &strPath, const QString &tip);

	virtual bool onChanged(int row, int column, const char *xmlRealValue);
	ndxml *_getXml(int row, int column);
	apoEditorSetting *m_rootSetting;

	ndxml_root *m_rootFile;		//be edited file 
	ndxml_root *m_config;	//setting.xml file
	ndxml *m_editedNode;

	LogicEditorHelper::CXMLAlias *m_alias;
	bool m_beginEditor;
private:

};


/*

*/

#endif // !_APOUI_XMLTABLES_WIDGET_H_
