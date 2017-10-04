#include <CQChartsSunburstPlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQRotatedText.h>
#include <CGradientPalette.h>

#include <QAbstractItemModel>
#include <QPainter>

namespace {

int colorId   = -1;
int numColors = 0;

int nextColorId() {
  ++colorId;

  if (colorId >= numColors)
    numColors = colorId + 1;

  return colorId;
}

}

//---

CQChartsSunburstPlotType::
CQChartsSunburstPlotType()
{
  addColumnParameter("name" , "Name" , "nameColumn" , "", 0);
  addColumnParameter("value", "Value", "valueColumn", "", 1);
}

CQChartsPlot *
CQChartsSunburstPlotType::
create(CQChartsView *view, QAbstractItemModel *model) const
{
  return new CQChartsSunburstPlot(view, model);
}

//---

CQChartsSunburstPlot::
CQChartsSunburstPlot(CQChartsView *view, QAbstractItemModel *model) :
 CQChartsPlot(view, view->charts()->plotType("sunburst"), model)
{
  // addKey() // TODO

  addTitle();
}

void
CQChartsSunburstPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("", this, "fontHeight");
}

void
CQChartsSunburstPlot::
updateRange()
{
  dataRange_.updateRange(-1, -1);
  dataRange_.updateRange( 1,  1);

  applyDataRange();

  setEqualScale(true);
}

void
CQChartsSunburstPlot::
initObjs(bool force)
{
  if (force) {
    clearPlotObjects();

    dataRange_.reset();
  }

  //---

  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return;
  }

  //---

  if (! plotObjs_.empty())
    return;

  //---

  if (roots_.empty()) {
    CQChartsSunburstRootNode *root = new CQChartsSunburstRootNode;

    roots_.push_back(root);

    QModelIndex index;

    loadChildren(root, index, 1);

    //---

    roots_[0]->packNodes(0.5, 1.0, 0.0, -90, 360);
  }

  //---

  addPlotObjs(roots_[0]);
}

void
CQChartsSunburstPlot::
loadChildren(CQChartsSunburstHierNode *hier, const QModelIndex &index, int depth, int colorId)
{
  if (depth == 3)
    colorId = nextColorId();

  int colorId1 = 0;

  uint nc = model_->rowCount(index);

  for (uint i = 0; i < nc; ++i) {
    QModelIndex index1 = model_->index(i, nameColumn_, index);

    bool ok;

    QString name = CQChartsUtil::modelString(model_, index1, ok);

    //---

    if (model_->rowCount(index1) > 0) {
      CQChartsSunburstHierNode *hier1 = new CQChartsSunburstHierNode(hier, name);

      loadChildren(hier1, index1, depth + 1, colorId);

      colorId1 = hier1->colorId();
    }
    else {
      QModelIndex index2 = model_->index(i, valueColumn_, index);

      bool ok;

      int size = CQChartsUtil::modelInteger(model_, index2, ok);

      if (! ok) size = 1;

      CQChartsSunburstNode *node = new CQChartsSunburstNode(hier, name);

      node->setSize(size);
      node->setColorId(colorId);

      hier->addNode(node);

      colorId1 = node->colorId();
    }
  }

  hier->setColorId(colorId1);
}

void
CQChartsSunburstPlot::
addPlotObjs(CQChartsSunburstHierNode *hier)
{
  for (auto node : hier->getNodes()) {
    addPlotObj(node);
  }

  for (auto hierNode : hier->getChildren()) {
    addPlotObj(hierNode);

    addPlotObjs(hierNode);
  }
}

void
CQChartsSunburstPlot::
addPlotObj(CQChartsSunburstNode *node)
{
  double r1 = node->r();
  double r2 = r1 + node->dr();

  CBBox2D bbox(-r2, -r2, r2, r2);

  CQChartsSunburstNodeObj *obj = new CQChartsSunburstNodeObj(this, bbox, node);

  obj->setId(QString("%1:%2").arg(node->name()).arg(node->size()));

  addPlotObject(obj);
}

void
CQChartsSunburstPlot::
draw(QPainter *p)
{
  initObjs();

  //---

  drawParts(p);
}

void
CQChartsSunburstPlot::
drawNodes(QPainter *p, CQChartsSunburstHierNode *hier)
{
  QFont font = view_->font();

  font.setPointSizeF(fontHeight());

  p->setFont(font);

  //---

  for (auto node : hier->getNodes())
    drawNode(p, node);

  //------

  for (auto hierNode : hier->getChildren()) {
    drawNode(p, hierNode);

    drawNodes(p, hierNode);
  }
}

void
CQChartsSunburstPlot::
drawNode(QPainter *p, CQChartsSunburstNode *node)
{
  if (! node->placed()) return;

  double xc = 0.0;
  double yc = 0.0;

  double r1 = node->r();
  double r2 = r1 + node->dr();

  double px11, py11, px21, py21;
  double px12, py12, px22, py22;

  windowToPixel(xc - r1, yc - r1, px11, py11);
  windowToPixel(xc + r1, yc + r1, px21, py21);
  windowToPixel(xc - r2, yc - r2, px12, py12);
  windowToPixel(xc + r2, yc + r2, px22, py22);

  QRectF qr1(px11, py21, px21 - px11, py11 - py21);
  QRectF qr2(px12, py22, px22 - px12, py12 - py22);

  double a1 = node->a();
  double da = node->da();
  double a2 = a1 + da;

  //a1 = 90 - a1;
  //a2 = 90 - a2;

  QPainterPath path;

  path.arcMoveTo(qr1, a1);

  path.arcTo(qr1, a1, da);
  path.arcTo(qr2, a2, -da);

  path.closeSubpath();

  QColor color = nodeColor(node);

  p->setPen  (textColor(color));
  p->setBrush(color);

  p->drawPath(path);

  double ta = a1 + da/2.0;
  double c  = cos(ta*M_PI/180.0);
  double s  = sin(ta*M_PI/180.0);

  double r3 = (r1 + r2)/2.0;

  double tx = r3*c;
  double ty = r3*s;

  double px, py;

  windowToPixel(tx, ty, px, py);

  QString str = node->name();

  Qt::Alignment align = Qt::AlignHCenter | Qt::AlignVCenter;

  if (c >= 0)
    CQRotatedText::drawRotatedText(p, px, py, str, ta, align);
  else
    CQRotatedText::drawRotatedText(p, px, py, str, ta - 180, align);
}

QColor
CQChartsSunburstPlot::
nodeColor(CQChartsSunburstNode *node) const
{
  return nodeColor(node->colorId());
}

QColor
CQChartsSunburstPlot::
nodeColor(int colorId) const
{
  QColor c(80,80,200);

  return interpPaletteColor((1.0*colorId)/numColors, c);
}

//------

CQChartsSunburstNodeObj::
CQChartsSunburstNodeObj(CQChartsSunburstPlot *plot, const CBBox2D &rect,
                        CQChartsSunburstNode *node) :
 CQChartsPlotObj(rect), plot_(plot), node_(node)
{
}

bool
CQChartsSunburstNodeObj::
inside(const CPoint2D &p) const
{
  double r1 = node_->r();
  double r2 = r1 + node_->dr();

  CPoint2D c(0, 0);

  double r = p.distanceTo(c);

  if (r < r1 || r > r2)
    return false;

  //---

  // check angle
  double a = CQChartsUtil::Rad2Deg(atan2(p.y - c.y, p.x - c.x)); while (a < 0) a += 360.0;

  double a1 = node_->a();
  double a2 = a1 + node_->da();

  while (a1 < 0) a1 += 360.0;
  while (a2 < 0) a2 += 360.0;

  if (a1 > a2) {
    // crosses zero
    if (a >= 0 && a <= a2)
      return true;

    if (a <= 360 && a >= a1)
      return true;
  }
  else {
    if (a >= a1 && a <= a2)
      return true;
  }

  return false;
}

void
CQChartsSunburstNodeObj::
draw(QPainter *p, const CQChartsPlot::Layer &)
{
  QFont font = plot_->view()->font();

  font.setPointSizeF(plot_->fontHeight());

  p->setFont(font);

  //---

  plot_->drawNode(p, node_);
}
