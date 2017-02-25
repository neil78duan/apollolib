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

	apoUiBezier(QWidget *parent=NULL);
	apoUiBezier(QWidget *parent, const QPoint &startPos, const QPoint &endPos);
	~apoUiBezier();
	
	void setConnector(QWidget *fromCtrl, QWidget *toCtrl);
    void resetPath( const QPoint &startPos, const QPoint &endPos) ;
	void paintTo(const QPoint &endPos);
    void paintEvent() ;
	void paint();

	void move(const QPoint &offset);
	apoUiBezier &operator=(const apoUiBezier &r);

protected:
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
