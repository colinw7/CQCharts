#include <CQChartsImagePlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsRenderer.h>
#include <CGradientPalette.h>
#include <CQStrParse.h>

CQChartsImagePlotType::
CQChartsImagePlotType()
{
  addParameters();
}

void
CQChartsImagePlotType::
addParameters()
{
}

CQChartsPlot *
CQChartsImagePlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsImagePlot(view, model);
}

//------

CQChartsImagePlot::
CQChartsImagePlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("image"), model)
{
}

void
CQChartsImagePlot::
addProperties()
{
  CQChartsPlot::addProperties();
}

void
CQChartsImagePlot::
updateRange(bool apply)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  //---

  dataRange_.reset();

  minValue_ = 0.0;
  maxValue_ = 0.0;

  //---

  int nr = model->rowCount   (QModelIndex());
  int nc = model->columnCount(QModelIndex());

  for (int r = 0; r < nr; ++r) {
    for (int c = 0; c < nc; ++c) {
      QModelIndex ind = model->index(r, c);

      //---

      bool ok;

      double value = CQChartsUtil::modelReal(model, ind, ok);

      if (r == 0 && c == 0) {
        minValue_ = value;
        maxValue_ = value;
      }
      else {
        minValue_ = std::min(minValue_, value);
        maxValue_ = std::max(maxValue_, value);
      }
    }
  }

  dataRange_.updateRange( 0,  0);
  dataRange_.updateRange(nc, nr);

  //---

  if (apply)
    applyDataRange();
}

bool
CQChartsImagePlot::
initObjs()
{
  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return false;
  }

  //---

  if (! plotObjs_.empty())
    return false;

  //---

  QAbstractItemModel *model = this->model();

  if (! model)
    return false;

  //---

  int nr = model->rowCount   (QModelIndex());
  int nc = model->columnCount(QModelIndex());

  double x = 0.0;
  double y = 0.0;

  //double dx = (nc > 0 ? 1.0/nc : 0.0);
  //double dy = (nr > 0 ? 1.0/nr : 0.0);
  double dx = 1;
  double dy = 1;

  for (int r = 0; r < nr; ++r) {
    x = 0.0;

    for (int c = 0; c < nc; ++c) {
      QModelIndex ind = model->index(r, c);

      QModelIndex ind1 = normalizeIndex(ind);

      //---

      bool ok;

      int value = CQChartsUtil::modelInteger(model, ind, ok);

      CQChartsGeom::BBox bbox(x, y, x + dx, y + dy);

      CQChartsImageObj *imageObj = new CQChartsImageObj(this, bbox, value, ind1);

      addPlotObject(imageObj);

      //---

      x += dx;
    }

    y += dy;
  }

  //---

  return true;
}

void
CQChartsImagePlot::
draw(CQChartsRenderer *renderer)
{
  initPlotObjs();

  //---

  drawParts(renderer);
}

//------

CQChartsImageObj::
CQChartsImageObj(CQChartsImagePlot *plot, const CQChartsGeom::BBox &rect,
                 double value, const QModelIndex &ind) :
 CQChartsPlotObj(plot, rect), plot_(plot), value_(value), ind_(ind)
{
}

QString
CQChartsImageObj::
calcId() const
{
  return QString("%1").arg(value_);
}

void
CQChartsImageObj::
mousePress(const CQChartsGeom::Point &)
{
  plot_->beginSelect();

  plot_->addSelectIndex(ind_);

  plot_->endSelect();
}

bool
CQChartsImageObj::
isIndex(const QModelIndex &ind) const
{
  return (ind == ind_);
}

void
CQChartsImageObj::
draw(CQChartsRenderer *renderer, const CQChartsPlot::Layer &)
{
  CQChartsGeom::BBox prect;

  plot_->windowToPixel(rect(), prect);

  QRectF qrect = CQChartsUtil::toQRect(prect);

  double v = CQChartsUtil::norm(value_, plot_->minValue(), plot_->maxValue());

  QColor c = plot_->interpPaletteColor(v);

  renderer->fillRect(qrect, c);
}
