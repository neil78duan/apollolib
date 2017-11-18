#ifndef DRAGTREE_H
#define DRAGTREE_H

#include <QTreeWidget>

class dragTree : public QTreeWidget
{
    Q_OBJECT
public:
    explicit dragTree(QWidget *parent = 0);
    virtual ~dragTree() ;

    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    //void mouseReleaseEvent(QMouseEvent *event);

    //void dragEnterEvent(QDragEnterEvent *event);
    //void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    //void keyPressEvent(QKeyEvent *event);
    //void keyReleaseEvent(QKeyEvent *event);
    void sendDrag(QTreeWidgetItem *from, QTreeWidgetItem *to)
    {
        emit onMydragItem(from, to) ;
    }
 private:
    QPoint m_startDragPoint;
    bool m_inDrag ;
signals:
    void onMydragItem(QTreeWidgetItem *from, QTreeWidgetItem *to )  ;
public slots:
};


#endif // DRAGTREE_H
