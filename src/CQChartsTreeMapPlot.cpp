#include <CQChartsTreeMapPlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQUtil.h>
#include <CGradientPalette.h>

#include <QAbstractItemModel>
#include <QPainter>

namespace {

int colorId = 0;

int nextColorId() {
  ++colorId;

  return colorId;
}

}

CQChartsTreeMapPlot::
CQChartsTreeMapPlot(CQChartsView *view, QAbstractItemModel *model) :
 CQChartsPlot(view, model)
{
  setMargins(1, 1, 1, 1);

  addTitle();
}

void
CQChartsTreeMapPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("", this, "fontHeight");
}

void
CQChartsTreeMapPlot::
updateRange()
{
  dataRange_.updateRange(-1, -1);
  dataRange_.updateRange( 1,  1);

  applyDataRange();
}

void
CQChartsTreeMapPlot::
initObjs(bool force)
{
  if (force) {
    clearPlotObjects();

    dataRange_.reset();
  }

  //--

  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return;
  }

  //---

  if (! plotObjs_.empty())
    return;

  //---

  if (! root_)
    initNodes();

  //---

  initObjs(root_, 0);
}

void
CQChartsTreeMapPlot::
initObjs(CQChartsTreeMapHierNode *hier, int depth)
{
  if (hier != root_) {
    CBBox2D rect(hier->x(), hier->y(), hier->x() + hier->w(), hier->y() + hier->h());

    CQChartsTreeMapHierObj *hierObj =
      new CQChartsTreeMapHierObj(this, hier, rect, depth, maxDepth());

    hierObj->setId(QString("%1:%2").arg(hier->name().c_str()).arg(hier->size()));

    addPlotObject(hierObj);
  }

  //---

  for (auto hierNode : hier->getChildren()) {
    initObjs(hierNode, depth + 1);
  }

  //---

  for (auto node : hier->getNodes()) {
    if (! node->placed()) continue;

    //---

    CBBox2D rect(node->x(), node->y(), node->x() + node->w(), node->y() + node->h());

    CQChartsTreeMapObj *obj = new CQChartsTreeMapObj(this, node, rect, depth, maxDepth());

    obj->setId(QString("%1:%2").arg(node->name().c_str()).arg(node->size()));

    addPlotObject(obj);
  }
}

void
CQChartsTreeMapPlot::
initNodes()
{
  root_ = new CQChartsTreeMapHierNode(0, "<root>");

  //---

  QModelIndex index;

  loadChildren(root_, index, 0);

  //---

  root_->setPosition(-1, -1, 2, 2);
}

void
CQChartsTreeMapPlot::
loadChildren(CQChartsTreeMapHierNode *hier, const QModelIndex &index, int depth, int colorId)
{
  maxDepth_ = std::max(maxDepth_, depth + 1);

  //---

  Nodes nodes;

  uint nc = model_->rowCount(index);

  for (uint i = 0; i < nc; ++i) {
    QModelIndex index1 = model_->index(i, 0, index);

    bool ok;

    QString name = CQChartsUtil::modelString(model_, index1, ok);

    //---

    if (model_->rowCount(index1) > 0) {
      CQChartsTreeMapHierNode *hier1 = new CQChartsTreeMapHierNode(hier, name.toStdString());

      int colorId1 = colorId;

      if (hier != root_) {
        if (colorId1 < 0)
          colorId1 = nextColorId();
      }

      loadChildren(hier1, index1, depth + 1, colorId1);
    }
    else {
      if (colorId < 0)
        colorId = nextColorId();

      QModelIndex index2 = model_->index(i, 1, index);

      bool ok;

      int value = CQChartsUtil::modelInteger(model_, index2, ok);

      if (! ok) value = 1;

      CQChartsTreeMapNode *node = new CQChartsTreeMapNode(hier, name.toStdString(), value);

      node->setColorId(colorId);

      maxColorId_ = std::max(maxColorId_, colorId);

      hier->addNode(node);
    }
  }
}

void
CQChartsTreeMapPlot::
draw(QPainter *p)
{
  initObjs();

  //---

  QFont font = view_->font();

  font.setPointSizeF(fontHeight());

  p->setFont(font);

  //---

  drawBackground(p);

  //---

  drawObjs(p);

  //---

  drawTitle(p);
}

QColor
CQChartsTreeMapPlot::
nodeColor(CQChartsTreeMapNode *node) const
{
  QColor c(80,80,200);

  return interpPaletteColor((1.0*node->colorId())/(maxColorId() + 1), c);
}

//------

CQChartsTreeMapHierObj::
CQChartsTreeMapHierObj(CQChartsTreeMapPlot *plot, CQChartsTreeMapHierNode *hier,
                          const CBBox2D &rect, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), hier_(hier), i_(i), n_(n)
{
}

void
CQChartsTreeMapHierObj::
draw(QPainter *p)
{
  QFontMetrics fm(p->font());

  //QColor c = plot_->objectStateColor(this, plot_->hierColor(hier_));
  QColor c = plot_->interpPaletteColor((1.0*i_)/n_);

  QColor c1 = CQUtil::blendColors(c, Qt::white, 0.8);

  QColor tc = plot_->textColor(c1);

  double px1, py1, px2, py2;

  plot_->windowToPixel(hier_->x()             , hier_->y()             , px1, py1);
  plot_->windowToPixel(hier_->x() + hier_->w(), hier_->y() + hier_->h(), px2, py2);

  p->setPen  (tc);
  p->setBrush(c1);

  p->drawRect(CQUtil::toQRect(CBBox2D(px1, py1, px2, py2)));
}

bool
CQChartsTreeMapHierObj::
inside(const CPoint2D &p) const
{
  CBBox2D bbox(hier_->x(), hier_->y(), hier_->x() + hier_->w(), hier_->y() + hier_->h());

  if (bbox.inside(p))
    return true;

  return false;
}

//------

CQChartsTreeMapObj::
CQChartsTreeMapObj(CQChartsTreeMapPlot *plot, CQChartsTreeMapNode *node,
                      const CBBox2D &rect, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), node_(node), i_(i), n_(n)
{
}

void
CQChartsTreeMapObj::
draw(QPainter *p)
{
  QFontMetrics fm(p->font());

//QColor c = plot_->interpPaletteColor((1.0*i_)/n_);
  QColor c = plot_->objectStateColor(this, plot_->nodeColor(node_));

  QColor tc = plot_->textColor(c);

  double px1, py1, px2, py2;

  plot_->windowToPixel(node_->x()             , node_->y()             , px1, py1);
  plot_->windowToPixel(node_->x() + node_->w(), node_->y() + node_->h(), px2, py2);

  p->setPen  (tc);
  p->setBrush(c);

  p->drawRect(CQUtil::toQRect(CBBox2D(px1, py1, px2, py2)));

  //---

  p->setPen(tc);

  plot_->windowToPixel(node_->x() + node_->w()/2, node_->y() + node_->h()/2, px1, py1);

  int len = node_->name().size();

  for (int i = len; i >= 1; --i) {
    std::string name1 = node_->name().substr(0, i);

    int tw = fm.width(name1.c_str());

    if (tw > 2*(px2 - px1)) continue;

    p->drawText(px1 - tw/2, py1 + fm.descent(), name1.c_str());

    break;
  }
}

bool
CQChartsTreeMapObj::
inside(const CPoint2D &p) const
{
  CBBox2D bbox(node_->x(), node_->y(), node_->x() + node_->w(), node_->y() + node_->h());

  if (bbox.inside(p))
    return true;

  return false;
}
