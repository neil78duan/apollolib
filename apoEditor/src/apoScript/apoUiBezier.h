/* file apoUiBezier.h
 *
 * logic parser editor bezier
 *
 * create by duan
 *
 * 2017.2.10
 */


#ifndef APOUIBEZIER_H
#define APOUIBEZIER_H


#include <QWidget>
#include <QMouseEvent>
#include <QLabel>

//class LogicParserEditor bezier
// draw a bzeize line from startPos to endPos
class apoUiBezier
{
public:
	enum eBezierType {
		LineParam, LineRunSerq,LineGoto
	};
	apoUiBezier(QWidget *parent=NULL);
	apoUiBezier(QWidget *parent, const QPoint &startPos, const QPoint &endPos);
	~apoUiBezier();
	
	void setAnchor(QWidget *fromCtrl, QWidget *toCtrl);
    void resetPath( const QPoint &startPos, const QPoint &endPos) ;
	void paintTo(const QPoint &endPos);
    void paintEvent() ;
	void paint();

	void move(const QPoint &offset);
	apoUiBezier &operator=(const apoUiBezier &r);

	void setColor(Qt::GlobalColor color){ m_color = color; }
	void setDebug(bool bDebug = false);
	void setWidth(int wid) { m_lineWid = wid; }
	void setType(eBezierType type) { m_type = type; }
	eBezierType getType() { return m_type; }

	QWidget *getSlot1(){ return m_startCtrl; }
	QWidget *getSlot2(){ return m_endCtrl; }

	QWidget *getAnotherSlot(QWidget *curSlot){ return curSlot == m_startCtrl? m_endCtrl: m_startCtrl; }

	void onRemove();

protected:
	Qt::GlobalColor m_color;
	int m_lineWid;
	eBezierType m_type;
	QWidget *m_startCtrl;
	QWidget *m_endCtrl;

private:

	void initCtrl();
    void initPos(const QPoint &start,const QPoint &end) ;

    QWidget *m_parent ;
    QPoint m_start ;
    QPoint m_end ;

    QPoint m_ctrlPos1 ;
    QPoint m_ctrlPos2 ;
};

#endif // APOUIBEZIER_H
