#ifndef MYQTITEMCTRL_H
#define MYQTITEMCTRL_H

/* define my item of qtree or qgrid*/


#include <QTreeWidgetItem>
#include <QTableWidgetItem>

class xmlTreeItem : public  QTreeWidgetItem
{
    typedef QTreeWidgetItem myBase ;
public :
    explicit xmlTreeItem(int type = Type):myBase(type),userData(0)  { }
    explicit xmlTreeItem(const QStringList &strings, int type = Type):myBase(strings, type),userData(0){   }
    explicit xmlTreeItem(QTreeWidget *view, int type = Type):myBase(view, type),userData(0){   }
    xmlTreeItem(QTreeWidget *view, const QStringList &strings, int type = Type):myBase(view,strings, type),userData(0){   }
    xmlTreeItem(QTreeWidget *view, QTreeWidgetItem *after, int type = Type):myBase(view, after, type),userData(0){   }
    explicit xmlTreeItem(QTreeWidgetItem *parent, int type = Type):myBase(parent, type),userData(0){   }
    xmlTreeItem(QTreeWidgetItem *parent, const QStringList &strings, int type = Type):myBase( parent, strings, type),userData(0){   }
    xmlTreeItem(QTreeWidgetItem *parent, QTreeWidgetItem *after, int type = Type):myBase(parent, after, type),userData(0){   }
    xmlTreeItem(const QTreeWidgetItem &other):myBase(other),userData(0){   }
    virtual ~xmlTreeItem(){   }
    void setUserData(void *data) {
        userData = data ;
    }

    void *getUserData() {
        return userData ;
    }

private:
    void *userData ;
} ;

class xmlTableItem : public QTableWidgetItem
{

    typedef QTableWidgetItem myBase ;
public:
	explicit xmlTableItem(int type = Type) : myBase(type), userData(0), attrName(0){ }
	explicit xmlTableItem(const QString &text, int type = Type) : myBase(text, type), userData(0), attrName(0){}
	explicit xmlTableItem(const QIcon &icon, const QString &text, int type = Type) : myBase(icon, text, type), userData(0), attrName(0){}
	xmlTableItem(const QTableWidgetItem &other) : myBase(other), userData(0), attrName(0){}
    virtual ~xmlTableItem (){}

    void setUserData(void *data) {
        userData = data ;
    }

    void *getUserData() {
        return userData ;
    }
	const char* getAttrName() { return attrName; }
	void setAttrName(const char *name){ attrName = name;}

private:
    void *userData ;
	const char *attrName;
};


#endif // MYQTITEMCTRL_H
