/* file apoUiExeNode.cpp
 *
 * logic parser editor bezier
 *
 * create by duan
 *
 * 2017.2.7
 */

#include "apoUiExeNode.h"
#include "apoScrpit/apoEditorSetting.h"
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
#include "apoScrpit/apouinodedef.h"
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


apoBaseExeNode *g_apoCreateExeNode(ndxml *xml, QWidget *parent)
{

	apoEditorSetting* p_setting = apoEditorSetting::getInstant();
	const char *ctrlName = p_setting->getExeNodeName(ndxml_getname(xml));
	if (!ctrlName || !*ctrlName) {
		ctrlName = "Common";
	}


	int index = apo_exeNodeGetIndex(ctrlName);
	return g_apoCreateExeNode(index,xml, parent);
}

apoBaseExeNode *g_apoCreateExeNode(int index, ndxml *xml, QWidget *parent)
{

#ifdef APOUI_NODE_ID
#undef APOUI_NODE_ID
#endif
#define APOUI_NODE_ID(id) \
	case EAPO_EXE_NODE_##id :	\
		return new apoUiExenode##id(parent,xml);	\
		break;					\


	switch (index)
	{
#include "apoScrpit/apouinodedef.h"
	default:
		break;
	}

#undef APOUI_NODE_ID

	return NULL;
}
