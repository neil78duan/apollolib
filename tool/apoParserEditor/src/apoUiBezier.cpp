/* file apoUiBezier.h
*
* logic parser editor bezier
*
* create by duan
*
* 2017.2.10
*/


#include "apoUiBezier.h"
#include <QPainter>


apoUiBezier::apoUiBezier( QWidget *parent, const QPoint &startPos, const QPoint &endPos):
    m_parent(parent)
{
    initPos(startPos, endPos);
}

apoUiBezier::apoUiBezier(): m_parent(NULL)
{

}

apoUiBezier::~apoUiBezier()
{
}

apoUiBezier &apoUiBezier::operator=(const apoUiBezier &r)
{
#define COPY_FROM_R(_a) _a = r._a ; 
	COPY_FROM_R(m_parent);
	COPY_FROM_R(m_start);
	COPY_FROM_R(m_end);

	COPY_FROM_R(m_ctrlPos1);
	COPY_FROM_R(m_ctrlPos2);
	return *this;
}

void apoUiBezier::resetPath( const QPoint &startPos, const QPoint &endPos)
{
    initPos(startPos, endPos) ;
}

void apoUiBezier::paintTo(const QPoint &endPos)
{
	initPos(m_start, endPos);
}

void apoUiBezier::initPos(const QPoint &start,const QPoint &end)
{
    m_start = start ;
    m_end = end ;
	
	m_ctrlPos1.setX(m_start.x() + 50);
	m_ctrlPos1.setY(m_start.y() );

	m_ctrlPos2.setX(end.x() - 50);
	m_ctrlPos2.setY(end.y() );

// 	float x = 0;
// 	if (m_end.x() > start.x()) {
// 		x = (m_end.x() - start.x()) / 2 + start.x();
// 	}
// 	else {
// 		x = (start.x() - m_end.x()) / 2 + m_end.x();
// 	}
// 
//     m_ctrlPos1.setX(x);
//     m_ctrlPos2.setX(x);
// 
//     m_ctrlPos2.setY(start.y());
//     m_ctrlPos2.setY(end.y());

}

void apoUiBezier::paintEvent()
{
    QPainter painter(m_parent);
    QPainterPath myPath;

    myPath.moveTo(m_start);
    myPath.cubicTo(m_ctrlPos1, m_ctrlPos2, m_end) ;
    painter.setPen(Qt::red);
    painter.drawPath(myPath);

// 
// 
// 	painter.setPen(QPen(Qt::green, 5));
// 	painter.drawPoint(m_start);
// 	painter.drawPoint(m_ctrlPos1);
// 	painter.drawPoint(m_ctrlPos2);
// 	painter.drawPoint(m_end);

}
