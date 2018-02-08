#include <CQChartsImagePlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQStrParse.h>
#include <QPainter>

CQChartsImagePlotType::
CQChartsImagePlotType()
{
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

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsImagePlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *model, const QModelIndex &parent, int row) override {
      for (int col = 0; col < numCols(); ++col) {
        QModelIndex ind = model->index(row, col, parent);

        //---

        bool ok;

        double value = CQChartsUtil::modelReal(model, ind, ok);

        if (! valueSet_) {
          minValue_ = value;
          maxValue_ = value;

          valueSet_ = true;
        }
        else {
          minValue_ = std::min(minValue_, value);
          maxValue_ = std::max(maxValue_, value);
        }
      }

      return State::OK;
    }

    double minValue() const { return minValue_; }
    double maxValue() const { return maxValue_; }

   private:
    CQChartsImagePlot *plot_     { nullptr };
    bool               valueSet_ { false };
    double             minValue_ { 0.0 };
    double             maxValue_ { 0.0 };
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  //---

  minValue_ = visitor.minValue();
  maxValue_ = visitor.maxValue();

  dataRange_.reset();

  int nr = visitor.numRows();
  int nc = visitor.numCols();

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

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsImagePlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *model, const QModelIndex &parent, int row) override {
      x_ = 0.0;

      for (int col = 0; col < numCols(); ++col) {
        QModelIndex ind = model->index(row, col, parent);

        bool ok;

        int value = CQChartsUtil::modelInteger(model, ind, ok);

        //---

        plot_->addImageObj(x_, y_, dx_, dy_, value, ind);

        //---

        x_ += dx_;
      }

      y_ += dy_;

      return State::OK;
    }

   private:
    CQChartsImagePlot *plot_ { nullptr };
    double             x_    { 0.0 };
    double             y_    { 0.0 };
    double             dx_   { 1.0 };
    double             dy_   { 1.0 };
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  //---

  return true;
}

void
CQChartsImagePlot::
addImageObj(double x, double y, double dx, double dy, int value, const QModelIndex &ind)
{
  QModelIndex ind1 = normalizeIndex(ind);

  CQChartsGeom::BBox bbox(x, y, x + dx, y + dy);

  CQChartsImageObj *imageObj = new CQChartsImageObj(this, bbox, value, ind1);

  addPlotObject(imageObj);
}

void
CQChartsImagePlot::
draw(QPainter *painter)
{
  initPlotObjs();

  //---

  drawParts(painter);
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
addSelectIndex()
{
  plot_->addSelectIndex(ind_);
}

bool
CQChartsImageObj::
isIndex(const QModelIndex &ind) const
{
  return (ind == ind_);
}

void
CQChartsImageObj::
draw(QPainter *painter, const CQChartsPlot::Layer &)
{
  CQChartsGeom::BBox prect;

  plot_->windowToPixel(rect(), prect);

  QRectF qrect = CQChartsUtil::toQRect(prect);

  double v = CQChartsUtil::norm(value_, plot_->minValue(), plot_->maxValue());

  QColor c = plot_->interpPaletteColor(v);

  painter->fillRect(qrect, c);
}
