#include <CQChartsContourPlot.h>
#include <CQChartsContour.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsTip.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

#include <QMenu>

CQChartsContourPlotType::
CQChartsContourPlotType()
{
}

void
CQChartsContourPlotType::
addParameters()
{
  CQChartsPlotType::addParameters();
}

QString
CQChartsContourPlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Contour Plot").
    h3("Summary").
     p("Draws contour from x, y, z points.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/contour.png"));
}

CQChartsPlot *
CQChartsContourPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsContourPlot(view, model);
}

//------

CQChartsContourPlot::
CQChartsContourPlot(View *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("delaunay"), model),
 CQChartsObjContourShapeData<CQChartsContourPlot>(this)
{
}

CQChartsContourPlot::
~CQChartsContourPlot()
{
  term();
}

//---

void
CQChartsContourPlot::
init()
{
  CQChartsPlot::init();

  //---

  NoUpdate noUpdate(this);

  setContourFillColor(Color(Color::Type::PALETTE));

  addAxes();

  addTitle();
}

void
CQChartsContourPlot::
term()
{
  delete contour_;
}

//---

void
CQChartsContourPlot::
setSolid(bool b)
{
  CQChartsUtil::testAndSet(solid_, b, [&]() { drawObjs(); } );
}

void
CQChartsContourPlot::
setNumContourLevels(int n)
{
  CQChartsUtil::testAndSet(numContourLevels_, n, [&]() { drawObjs(); } );
}

//---

void
CQChartsContourPlot::
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  addBaseProperties();

  // solid
  addProp("contour", "solid"           , "solid"    , "Solid contour lines");
  addProp("contour", "numContourLevels", "numLevels", "Number of contour levels");

  // contour
  addFillProperties("contour/fill"  , "contourFill"  , "Contour polygon fill");
  addLineProperties("contour/stroke", "contourStroke", "Contour polygon stroke");
}

CQChartsGeom::Range
CQChartsContourPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsContourPlot::calcRange");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsContourPlot *>(this);

  th->clearErrors();

  //---

  // calc data range (x, y, z values)
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsContourPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      auto *plot = const_cast<CQChartsContourPlot *>(plot_);

      // first row is y values (skip first column)
      if (data.row == 0) {
        for (int col = 1; col < numCols(); ++col) {
          ModelIndex columnModelInd(plot, data.row, CQChartsColumn(col), data.parent);

          bool ok;

          double y = plot_->modelReal(columnModelInd, ok);

          if (ok && ! CMathUtil::isNaN(y))
            yValueRange_.add(y);
        }
      }
      // remaining rows are x values (first column)
      else {
        ModelIndex columnModelInd(plot, data.row, CQChartsColumn(0), data.parent);

        bool ok;

        double x = plot_->modelReal(columnModelInd, ok);

        if (ok && ! CMathUtil::isNaN(x))
          xValueRange_.add(x);

        for (int col = 1; col < numCols(); ++col) {
          ModelIndex columnModelInd(plot, data.row, CQChartsColumn(col), data.parent);

          bool ok;

          double z = plot_->modelReal(columnModelInd, ok);

          if (ok && ! CMathUtil::isNaN(z))
            zValueRange_.add(z);
        }
      }

      return State::OK;
    }

    double minXValue() const { return xValueRange_.min(0.0); }
    double maxXValue() const { return xValueRange_.max(1.0); }
    double minYValue() const { return yValueRange_.min(0.0); }
    double maxYValue() const { return yValueRange_.max(1.0); }
    double minZValue() const { return zValueRange_.min(0.0); }
    double maxZValue() const { return zValueRange_.max(1.0); }

   private:
    const CQChartsContourPlot* plot_ { nullptr };
    RMinMax                    xValueRange_;
    RMinMax                    yValueRange_;
    RMinMax                    zValueRange_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  //---

  // set value range
  double xmin = visitor.minXValue();
  double ymin = visitor.minYValue();
  double xmax = visitor.maxXValue();
  double ymax = visitor.maxYValue();

  Range dataRange(xmin, ymin, xmax, ymax);;

  //---

  th->nx_ = visitor.numProcessedRows() - 1;
  th->ny_ = visitor.numCols() - 1;

  return dataRange;
}

bool
CQChartsContourPlot::
createObjs(PlotObjs &) const
{
  CQPerfTrace trace("CQChartsContourPlot::createObjs");

  NoUpdate noUpdate(this);

  //---

  auto *th = const_cast<CQChartsContourPlot *>(this);

  delete th->contour_;

  th->contour_ = new CQChartsContour(th);

  std::vector<double> x, y, z;

  x.resize(nx_);
  y.resize(ny_);
  z.resize(nx_*ny_);

  //---

  // create points for original data points
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsContourPlot *plot, std::vector<double> &x,
               std::vector<double> &y, std::vector<double> &z) :
     plot_(plot), x_(x), y_(y), z_(z) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      auto *plot = const_cast<CQChartsContourPlot *>(plot_);

      // first row is y values (skip first column)
      if (data.row == 0) {
        for (int col = 1; col < numCols(); ++col) {
          ModelIndex columnModelInd(plot, data.row, CQChartsColumn(col), data.parent);

          bool ok;

          double y = plot_->modelReal(columnModelInd, ok);

          if (! ok || CMathUtil::isNaN(y))
            y = 0.0;

          y_[iy_++] = y;
        }
      }
      // remaining rows are x values (first column) and z values
      else {
        ModelIndex columnModelInd(plot, data.row, CQChartsColumn(0), data.parent);

        bool ok;

        double x = plot_->modelReal(columnModelInd, ok);

        if (! ok || CMathUtil::isNaN(x))
          x = 0.0;

         x_[ix_++] = x;

        for (int col = 1; col < numCols(); ++col) {
          ModelIndex columnModelInd(plot, data.row, CQChartsColumn(col), data.parent);

          bool ok;

          double z = plot_->modelReal(columnModelInd, ok);

          if (! ok || CMathUtil::isNaN(z))
            z = 0.0;

          z_[iz_++] = z;
        }
      }

      return State::OK;
    }

   private:
    const CQChartsContourPlot* plot_ { nullptr };
    std::vector<double>&       x_;
    std::vector<double>&       y_;
    std::vector<double>&       z_;
    int                        ix_ { 0 };
    int                        iy_ { 0 };
    int                        iz_ { 0 };
  };

  RowVisitor visitor(this, x, y, z);

  visitModel(visitor);

  //---

  contour_->setData(&x[0], &y[0], &z[0], nx_, ny_);

  return true;
}

//------

bool
CQChartsContourPlot::
addMenuItems(QMenu *)
{
  return true;
}

//------

bool
CQChartsContourPlot::
hasBackground() const
{
  return isLayerActive(CQChartsLayer::Type::BACKGROUND);
}

bool
CQChartsContourPlot::
hasForeground() const
{
  return false;
}

void
CQChartsContourPlot::
execDrawBackground(CQChartsPaintDevice *device) const
{
  device->save();

  setClipRect(device);

  drawContour(device);

  device->restore();
}

void
CQChartsContourPlot::
execDrawForeground(CQChartsPaintDevice *) const
{
}

void
CQChartsContourPlot::
drawContour(CQChartsPaintDevice *device) const
{
  contour_->setSolid(isSolid());
  contour_->setNumContourLevels(numContourLevels());

  contour_->drawContour(device);
}
