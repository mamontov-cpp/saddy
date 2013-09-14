#include "gridnodedge.h"
#include "constants.h"

DECLARE_SOBJ_INHERITANCE(GridNodeEdge, sad::BasicNode)

GridNodeEdge::GridNodeEdge(p2d::Body * b1, p2d::Body * b2)
{
	m_b1 = b1;
	m_b2 = b2;
	m_ad = new Sprite2DAdapter(NULL, hRectF(), hRectF());
	Sprite2DAdapter::Options * o = p2d::app::Constants<GridNodeEdge>::sprite();
	this->m_ad->set(*o);
    delete o;
}


void GridNodeEdge::render()
{
	p2d::Point p1 = m_b1->currentShape()->center();
	p2d::Point p2 = m_b2->currentShape()->center();
	this->m_ad->makeSpanBetweenPoints(p2d::app::Constants<GridNodeEdge>::sourceRect(), p1, p2);
	this->m_ad->render();
}
