#include <CQChartsSunburstPlot.h>
#include <CQChartsWindow.h>
#include <CQRotatedText.h>

#include <QAbstractItemModel>
#include <QPainter>

namespace {

QColor bg_colors[] = {
  // blue
  QColor(0x31,0x82,0xBD),
  QColor(0x6B,0xAE,0xD6),
  QColor(0x9E,0xCA,0xE1),
  QColor(0xC6,0xDB,0xEF),

  // orange
  QColor(0xE6,0x55,0x0D),
  QColor(0xFD,0x8D,0x3C),
  QColor(0xFD,0xAE,0x6B),
  QColor(0xFD,0xD0,0xA2),

  // green
  QColor(0x31,0xA3,0x54),
  QColor(0x74,0xC4,0x76),
  QColor(0xA1,0xD9,0x9B),
  QColor(0xC7,0xE9,0xC0),

  // purple
  QColor(0x75,0x6B,0xB1),
  QColor(0x9E,0x9A,0xC8),
  QColor(0xBC,0xBD,0xDC),
  QColor(0xDA,0xDA,0xEB),

  // gray
  QColor(0x63,0x63,0x63),
  QColor(0x96,0x96,0x96),
  QColor(0xBD,0xBD,0xBD),
  QColor(0xD9,0xD9,0xD9),
};

int num_bg_colors = 30;

int colorId = -1;

int nextColorId() {
  ++colorId;

  if (colorId >= num_bg_colors)
    colorId = 0;

  return colorId;
}

}

//---

CQChartsSunburstPlot::
CQChartsSunburstPlot(CQChartsWindow *window, QAbstractItemModel *model) :
 CQChartsPlot(window, model)
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

  QColor c;

  uint nc = model_->rowCount(index);

  for (uint i = 0; i < nc; ++i) {
    QModelIndex index1 = model_->index(i, 0, index);

    QVariant var1 = model_->data(index1);

    QString name = var1.toString();

    //---

    if (model_->rowCount(index1) > 0) {
      HierNode *hier1 = new HierNode(hier, name);

      loadChildren(hier1, index1, depth + 1, colorId);

      c = hier1->color();
    }
    else {
      QModelIndex index2 = model_->index(i, 1, index);

      QVariant var2 = model_->data(index2);

      int size = atoi(var2.toString().toLatin1().constData());

      Node *node = new Node(hier, name);

      node->setSize(size);
      node->setColor(bg_colors[colorId]);

      hier->addNode(node);

      c = node->color();
    }
  }

  hier->setColor(c.lighter(110));
}

#if 0
void
CQChartsSunburstPlot::
resizeEvent(QResizeEvent *e)
{
  range_.setPixelRange(0, 0, width() - 1, height() - 1);

  CQChartsPlot::resizeEvent(e);
}
#endif

void
CQChartsSunburstPlot::
draw(QPainter *p)
{
  QFont font = window_->font();

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

  QColor nodeColor = node->color();

  p->setPen  (textColor(nodeColor));
  p->setBrush(nodeColor);

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
    CQRotatedText::drawRotatedText(p, px, py, str, ta,
                                   Qt::AlignLeft | Qt::AlignVCenter);
  else
    CQRotatedText::drawRotatedText(p, px, py, str, 180.0 + ta,
                                   Qt::AlignRight | Qt::AlignVCenter);
}

QColor
CQChartsSunburstPlot::
textColor(const QColor &bg) const
{
  int g = qGray(bg.red(), bg.green(), bg.blue());

  return (g > 128 ? QColor(0,0,0) : QColor(255, 255, 255));
}
