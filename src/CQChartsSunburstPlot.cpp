#include <CQChartsSunburstPlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQRotatedText.h>
#include <CQUtil.h>
#include <CGradientPalette.h>

#include <QAbstractItemModel>
#include <QPainter>

namespace {

int numColors = 30;

int colorId = -1;

int nextColorId() {
  ++colorId;

  if (colorId >= numColors)
    colorId = 0;

  return colorId;
}

}

//---

CQChartsSunburstPlot::
CQChartsSunburstPlot(CQChartsView *view, QAbstractItemModel *model) :
 CQChartsPlot(view, model)
{
  range_.setWindowRange(-1, 1, 1, -1);

  init();
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
init()
{
  RootNode *root = new RootNode;

  roots_.push_back(root);

  QModelIndex index;

  loadChildren(root, index, 1);

  //--

  roots_[0]->packNodes(0.5, 1.0, 0.0, -90, 360);
}

void
CQChartsSunburstPlot::
loadChildren(HierNode *hier, const QModelIndex &index, int depth, int colorId)
{
  if (depth == 2)
    colorId = nextColorId();

  int colorId1 = 0;

  uint nc = model_->rowCount(index);

  for (uint i = 0; i < nc; ++i) {
    QModelIndex index1 = model_->index(i, 0, index);

    bool ok;

    QString name = CQChartsUtil::modelString(model_, index1, ok);

    //---

    if (model_->rowCount(index1) > 0) {
      HierNode *hier1 = new HierNode(hier, name);

      loadChildren(hier1, index1, depth + 1, colorId);

      colorId1 = hier1->colorId();
    }
    else {
      QModelIndex index2 = model_->index(i, 1, index);

      bool ok;

      int size = CQChartsUtil::modelInteger(model_, index2, ok);

      if (! ok) size = 1;

      Node *node = new Node(hier, name);

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
draw(QPainter *p)
{
  QFont font = view_->font();

  font.setPointSizeF(fontHeight());

  p->setFont(font);

  //---

  drawBackground(p);

  //---

  for (uint i = 0; i < roots_.size(); ++i)
    drawNodes(p, roots_[i]);
}

void
CQChartsSunburstPlot::
drawNodes(QPainter *p, HierNode *hier)
{
  const Nodes &nodes = hier->getNodes();

  for (auto node : nodes)
    drawNode(p, node);

  //------

  const HierNode::Children &children = hier->getChildren();

  for (auto hierNode : children) {
    drawNode(p, hierNode);

    drawNodes(p, hierNode);
  }
}

void
CQChartsSunburstPlot::
drawNode(QPainter *p, Node *node)
{
  if (! node->placed()) return;

  double xc = 0.0;
  double yc = 0.0;

  double r1 = node->r();
  double r2 = node->r() + node->dr();

  double px11, py11, px21, py21;
  double px12, py12, px22, py22;

  range_.windowToPixel(xc - r1, yc - r1, &px11, &py11);
  range_.windowToPixel(xc + r1, yc + r1, &px21, &py21);
  range_.windowToPixel(xc - r2, yc - r2, &px12, &py12);
  range_.windowToPixel(xc + r2, yc + r2, &px22, &py22);

  QRectF qr1(px11, py21, px21 - px11, py11 - py21);
  QRectF qr2(px12, py22, px22 - px12, py12 - py22);

  double a1 = node->a();
  double a2 = node->a() + node->da();
  double da = node->da();

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

  double ta = node->a() + node->da()/2.0;
  double c  = cos(ta*M_PI/180.0);
  double s  = sin(ta*M_PI/180.0);

  double tx = (r1 + 0.01)*c;
  double ty = (r1 + 0.01)*s;

  double px, py;

  range_.windowToPixel(tx, ty, &px, &py);

  QString str = node->name();

  if (c >= 0)
    CQRotatedText::drawRotatedText(p, px, py, str, ta, Qt::AlignLeft | Qt::AlignVCenter);
  else
    CQRotatedText::drawRotatedText(p, px, py, str, 180.0 + ta, Qt::AlignRight | Qt::AlignVCenter);
}

QColor
CQChartsSunburstPlot::
nodeColor(Node *node) const
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
