/* file apoUiParam.h
 *
 * define param node 
 *
 * create by duan 
 *
 * 2017.2.20
 */

#ifndef _APO_UI_PARAM_H_
#define _APO_UI_PARAM_H_

#include "nd_common/nd_common.h"
#include <QWidget> 
#include <QLabel>
#include <QEvent>

class apoUiBezier;
class apoBaseSlotCtrl : public QLabel
{
	Q_OBJECT
public:

	enum eBaseSlotType {
		SLOT_RUN_IN,		//connect in
		SLOT_RUN_OUT,		// connect-out
		SLOT_RETURN_VALUE,
		SLOT_NODE_INPUUT_PARAM,		//param slot
		SLOT_FUNCTION_PARAM,
		SLOT_SUB_ENTRY,		//loop 
		SLOT_VAR ,			//new var
		SLOT_UNKNOWN
	};

	explicit apoBaseSlotCtrl(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags()) :
		QLabel(parent, f),
		m_inDrag(false), m_valid(true),m_showError(false),
		m_slotType(SLOT_UNKNOWN),m_myConnect(0)
	{
		m_xmlAnchorParent = 0;
		m_xmlAnchor = 0;
	}

	explicit apoBaseSlotCtrl(const QString &text, QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags()) :
		QLabel(text, parent, f),
		m_inDrag(false), m_valid(true),m_showError(false),
		m_slotType(SLOT_UNKNOWN),m_myConnect(0)
	{
		m_xmlAnchorParent = 0;
		m_xmlAnchor = 0;
	}
	~apoBaseSlotCtrl()
	{

	}

	bool checkInDrag() { return m_inDrag; }
	void setInDrag(bool inDrag = true) { m_inDrag = inDrag; }
	
	void setError(bool isError=true) {m_showError = isError;}
	bool checkInError() {return m_showError;}

	bool checkValid() { return m_valid; }
	void setValid(bool enable = true);
	

	eBaseSlotType slotType(){ return m_slotType; }
	void setSlotType(eBaseSlotType stype){ m_slotType = stype; }

	//void setUserdata(void *data) { m_UserData = data; }
	//void *getUserdata() { return m_UserData; }

	apoUiBezier *getConnector() { return m_myConnect; }
	void setConnector(apoUiBezier * bezier){ m_myConnect = bezier; }


	ndxml * getXmlAnchorParent() { return  m_xmlAnchorParent; }
	ndxml * getXmlAnchor() { return m_xmlAnchor; }

	void setXmlAnchorParent(ndxml *xml) {  m_xmlAnchorParent = xml; }
	void setXmlAnchor(ndxml *xml) { m_xmlAnchor= xml; }


	virtual bool checkConnectIn();
	virtual bool onConnectIn(apoBaseSlotCtrl*fromSlot);
	virtual bool onDisconnect();

	virtual bool isDelete();
	virtual QString getParamInfo();
	
protected:
	bool event(QEvent *e);

	bool m_inDrag;
	bool m_valid; //not in using
	bool m_showError ;		//show compiled error
	eBaseSlotType m_slotType;
	apoUiBezier *m_myConnect;

	ndxml *m_xmlAnchorParent;
	ndxml *m_xmlAnchor;
private:

};


class apoBaseParam : public apoBaseSlotCtrl
{
	Q_OBJECT
public:
	explicit apoBaseParam(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	explicit apoBaseParam(const QString &text, QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~apoBaseParam();

	virtual QString getParamInfo();

	void setIndex(int index) { m_index = index; }
	void setParam(ndxml *parent, ndxml *param, ndxml *paramRef, ndxml *varname = NULL);

	int getParamType();
	const char *getValue();

	QString getParamName();
	QString getDisplayValue();
	QString getTypeName();

	//void setParamName(const char *name) { m_varName = name; }


	ndxml *getTypeXml(){ return m_reference; }
	ndxml *getNameXml(){ return m_varName; }
	ndxml *getValueXml(){ return m_xml; }
	ndxml *getParentXml() { return m_parent; }



	virtual bool checkConnectIn();
	virtual bool onConnectIn(apoBaseSlotCtrl*fromSlot);

	virtual bool onDisconnect();

	virtual bool isDelete();
	//bool checkNameEdited();
	//bool checkTypeEdited();
protected:
	int m_index;
	//ndxml *m_parentXml;
	ndxml *m_varName;
	ndxml *m_xml;
	ndxml *m_reference;
	ndxml *m_parent;
private:

};


#endif
