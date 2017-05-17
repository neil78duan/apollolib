/* file apoUiDebugger.cpp
*
* debug helper of ui
*
* create by duan
*
* 2017.5.11
*/

#include "apoUiDebugger.h"


DeguggerScriptOwner::DeguggerScriptOwner()
{
	//LogicEngineRoot * root =LogicEngineRoot::get_Instant();
	//root->getGlobalDebugger().setOwner(this);
}

DeguggerScriptOwner::~DeguggerScriptOwner()
{
	//LogicEngineRoot * root = LogicEngineRoot::get_Instant();
	//root->getGlobalDebugger().setOwner(NULL);
}

bool DeguggerScriptOwner::getOtherObject(const char*objName, DBLDataNode &val)
{
	/*if (0 == ndstricmp(objName, "LogFunction")) {
		val.InitSet((void*)ND_LOG_WRAPPER_PRINT(MainWindow));
		return true;
	}
	else*/ if (0 == ndstricmp(objName, "LogFile")) {
		val.InitSet("ndlog.log");
		return true;
	}
	else if (0 == ndstricmp(objName, "LogPath")) {
		val.InitSet("../../log");
		return true;
	}
	else if (0 == ndstricmp(objName, "WritablePath")) {
		val.InitSet("../../log");
		return true;
	}

	else if (0 == ndstricmp(objName, "SelfName")) {
		val.InitSet("apoDebugger");
		return true;
	}
	else if (0 == ndstricmp(objName, "self")) {
		val.InitSet((void*)this, OT_OBJ_BASE_OBJ);
		return true;
	}

	bool ret = ClientMsgHandler::ApoConnectScriptOwner::getOtherObject(objName, val);
	if (ret) {
		return ret;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

DebuggerQObj::DebuggerQObj()
{
}

DebuggerQObj::~DebuggerQObj()
{
}


//////////////////////////////////////////////////////////////////////////


ApoDebugger::ApoDebugger()
{
}

ApoDebugger::~ApoDebugger()
{
}



bool ApoDebugger::onEnterStep(const char *func, const char *node)
{
	m_stepFunc = func;
	m_stepNode = node;
	emit m_obj.stepSignal(m_stepNode.c_str(), m_stepNode.c_str());
	return true;
}

void ApoDebugger::onTerminate()
{
	emit m_obj.terminateSignal();
}
void ApoDebugger::onCommandOk()
{
	emit m_obj.commondOkSignal();
}


void ApoDebugger::onScriptRunOk()
{
	emit m_obj.scriptRunOKSignal();
}


