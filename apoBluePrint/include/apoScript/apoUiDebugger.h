/* file apoUiDebugger.h
 *
 * debug helper of ui
 * 
 * create by duan 
 *
 * 2017.5.11
 */

#ifndef _APOUI_DEBUGGER_H_
#define _APOUI_DEBUGGER_H_


#include "nd_common/nd_common.h"
#include "apoScript/apoEditorSetting.h"
#include "logic_parser/logic_compile.h"
#include "logic_parser/logic_debugger.h"
#include "cli_common/dftCliMsgHandler.h"


#include <QWidget> 

class DeguggerScriptOwner :public  ClientMsgHandler::ApoConnectScriptOwner
{
public:
	DeguggerScriptOwner();
	virtual ~DeguggerScriptOwner();

	bool getOtherObject(const char*objName, DBLDataNode &val);
};


class DebuggerQObj : public QWidget
{
	Q_OBJECT
public:
	DebuggerQObj();
	~DebuggerQObj();


signals:
	void stepSignal(const char *funcName, const char *nodeName);
	void terminateSignal();
	void commondOkSignal();
	void scriptRunOKSignal();

};


class ApoDebugger : public LogicDebugClient
{
public:
	ApoDebugger();
	~ApoDebugger();

	DebuggerQObj m_obj;
private:
	virtual bool onEnterStep(const char *func, const char *node);
	virtual void onTerminate();
	virtual void onCommandOk();
	virtual void onScriptRunOk();

	std::string m_stepFunc;
	std::string m_stepNode;

};



#endif