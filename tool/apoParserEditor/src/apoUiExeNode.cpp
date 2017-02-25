/* file apoUiExeNode.cpp
 *
 * logic parser editor bezier
 *
 * create by duan
 *
 * 2017.2.7
 */

#include "apoUiExeNode.h"
#include <QPainter>
#include <QMessageBox>
#include <QMouseEvent>


static int apo_exeNodeGetIndex(const char *name)
{


#ifdef APOUI_NODE_ID
#undef APOUI_NODE_ID
#endif
#define APOUI_NODE_ID(id) #id,

	static const char* exeClassname[] = {
#include "src/apouinodedef.h"
	};
#undef APOUI_NODE_ID
	int number = sizeof(exeClassname) / sizeof(exeClassname[0]);
	for (int i = 0; i < number; i++) {
		if (strcmp(name, exeClassname[i]) == 0) {
			return i;
		}
	}
	return 0;
	
}


apoBaseExeNode *g_apoCreateExeNode(const char *objName, QWidget *parent)
{

	int index = apo_exeNodeGetIndex(objName);
	return g_apoCreateExeNode(index, parent);
}

apoBaseExeNode *g_apoCreateExeNode(int index, QWidget *parent)
{

#ifdef APOUI_NODE_ID
#undef APOUI_NODE_ID
#endif
#define APOUI_NODE_ID(id) \
	case EAPO_EXE_NODE_##id :	\
		return new apoUiExenode##id(parent);	\
		break;					\


	switch (index)
	{
#include "src/apouinodedef.h"
	default:
		break;
	}

#undef APOUI_NODE_ID
	return NULL;
}
