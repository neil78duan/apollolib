#include "dragtree.h"
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>

dragTree::dragTree(QWidget *parent) : QTreeWidget(parent)
{
    m_inDrag = false ;
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setAcceptDrops(true);
    this->setDragEnabled(true);
    this->setStyleSheet("QTreeWidget::item:selected{ background-color: rgb(150, 0, 0)}");
}


dragTree::~dragTree()
{
}

//void dragTree::keyPressEvent(QKeyEvent *event)
//{
//  if (event->key() == Qt::Key_Control)
//  {
//    m_CtrlPressed = true;
//  }
//}

//void dragTree::keyReleaseEvent(QKeyEvent *event)
//{
//  if (event->key() == Qt::Key_Control)
//  {
//    m_CtrlPressed = false;
//  }
//}

void dragTree::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        do {
            m_startDragPoint = event->pos() ;
        }while(0) ;
    }
    QTreeWidget::mousePressEvent(event);
    m_inDrag = false ;
}

void dragTree::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        int dragDistance = (event->pos() - m_startDragPoint).manhattanLength();
        if (dragDistance > QApplication::startDragDistance()){
            m_inDrag = true ;
        }
    }
    QTreeWidget::mouseMoveEvent(event);

}

//void dragTree::mouseReleaseEvent(QMouseEvent *event)
//{
//    if(m_inDrag) {
//        do {
//            QTreeWidgetItem *currentItem = this->itemAt(event->pos());
//            QTreeWidgetItem *startItem = this->itemAt(m_startDragPoint);

//            if(!currentItem || !startItem || currentItem==startItem) {
//                break;
//            }

//            sendDrag(startItem, currentItem) ;

//        }while(0) ;

//    }
//    m_inDrag = false ;
//    QTreeWidget::mouseReleaseEvent(event);
//}

//void dragTree::dragMoveEvent(QDragMoveEvent *event)
//{

//  QWidget *source =  qobject_cast<MyTreeWidget *>(event->source());
//  if (source /*&& source != this*/)
//  {
//    const TreeItemMimeData *pMimeData = (const TreeItemMimeData *)(event->mimeData());
//    const QTreeWidgetItem *item = pMimeData->DragItemData();
//    QTreeWidgetItem *currentItem = this->itemAt(event->pos());
//    if (currentItem == item)           //不允许拖回到原来的item
//    {
//      event->ignore();
//    }
//    else
//    {
//      setCurrentItem(currentItem);
//      if (m_CtrlPressed)
//      {
//        event->setDropAction(Qt::CopyAction);
//      }
//      else
//      {
//        event->setDropAction(Qt::MoveAction);
//      }
//      //event->setDropAction(Qt::MoveAction);
//      event->accept();
//    }

//  }
//}

//void dragTree::dragEnterEvent(QDragEnterEvent *event)
//{
//  QWidget *source =  qobject_cast<MyTreeWidget *>(event->source());
//  if (source /*&& source != this*/)
//  {
//    if (m_CtrlPressed)
//    {
//      event->setDropAction(Qt::CopyAction);
//    }
//    else
//    {
//      event->setDropAction(Qt::MoveAction);
//    }
//    /*event->setDropAction(Qt::MoveAction);  */
//    event->accept();
//  }
//}

void dragTree::dropEvent(QDropEvent *event)
{
    do {
        if(!m_inDrag)
            break ;
        QTreeWidgetItem *currentItem = this->itemAt(event->pos());
        QTreeWidgetItem *startItem = this->itemAt(m_startDragPoint);

        if(!currentItem || !startItem || currentItem==startItem) {
            break;
        }

        sendDrag(startItem, currentItem) ;
        event->accept();

    }while(0) ;
    QTreeWidget::dropEvent(event);
}

//void dragTree::performDrag()
//{
//  QTreeWidgetItem *item = currentItem();
//  if (item)
//  {

//    TreeItemMimeData *mimeData = new TreeItemMimeData;
//    mimeData->SetDragData("ItemMimeData",item);

//    QDrag *drag = new QDrag(this);
//    drag->setMimeData(mimeData);
//    drag->setPixmap(QPixmap(":/DragDropDemo/Resources/Mail.png"));
//    if (m_CtrlPressed)
//    {
//      drag->exec(Qt::CopyAction);
//    }
//    else
//    {
//      drag->exec(Qt::MoveAction);
//      delete item;
//    }

//  }
//}
