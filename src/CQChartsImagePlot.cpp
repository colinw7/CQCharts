#include <CQChartsImagePlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <QPainter>

CQChartsImagePlotType::
CQChartsImagePlotType()
{
}

void
CQChartsImagePlotType::
addParameters()
{
  CQChartsPlotType::addParameters();
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
  addTitle();
}

void
CQChartsImagePlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("labels", this, "xLabels"   , "x"    );
  addProperty("labels", this, "yLabels"   , "y"    );
  addProperty("labels", this, "cellLabels", "cell" );
  addProperty("labels", this, "textFont"  , "font" );
  addProperty("labels", this, "textColor" , "color");
  addProperty("labels", this, "textAlpha" , "alpha");
}

//------

const CQChartsColor &
CQChartsImagePlot::
textColor() const
{
  return textData_.color;
}

void
CQChartsImagePlot::
setTextColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(textData_.color, c, [&]() { update(); } );
}

double
CQChartsImagePlot::
textAlpha() const
{
  return textData_.alpha;
}

void
CQChartsImagePlot::
setTextAlpha(double a)
{
  CQChartsUtil::testAndSet(textData_.alpha, a, [&]() { update(); } );
}

QColor
CQChartsImagePlot::
interpTextColor(int i, int n) const
{
  return textColor().interpColor(this, i, n);
}

const QFont &
CQChartsImagePlot::
textFont() const
{
  return textData_.font;
}

void
CQChartsImagePlot::
setTextFont(const QFont &f)
{
  CQChartsUtil::testAndSet(textData_.font, f, [&]() { update(); } );
}

//------

void
CQChartsImagePlot::
updateRange(bool apply)
{
  QAbstractItemModel *model = this->model().data();

  if (! model)
    return;

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsImagePlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *, const QModelIndex &parent, int row) override {
      for (int col = 0; col < numCols(); ++col) {
        bool ok;

        double value = plot_->modelReal(row, col, parent, ok);

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

  // set value range
  minValue_ = visitor.minValue();

  if (xmin())
    minValue_ = *xmin();

  maxValue_ = visitor.maxValue();

  if (xmax())
    maxValue_ = *xmax();

  //---

  dataRange_.reset();

  nr_ = visitor.numRows();
  nc_ = visitor.numCols();

  dataRange_.updateRange(  0,   0);
  dataRange_.updateRange(nc_, nr_);

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

  QAbstractItemModel *model = this->model().data();

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

        double value = plot_->modelReal(row, col, parent, ok);

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
addImageObj(double x, double y, double dx, double dy, double value, const QModelIndex &ind)
{
  QModelIndex ind1 = normalizeIndex(ind);

  CQChartsGeom::BBox bbox(x, y, x + dx, y + dy);

  CQChartsImageObj *imageObj = new CQChartsImageObj(this, bbox, value, ind1);

  addPlotObject(imageObj);
}

//------

void
CQChartsImagePlot::
draw(QPainter *painter)
{
  initPlotObjs();

  //---

  drawParts(painter);

  //---

  if (isXLabels())
    drawXLabels(painter);

  if (isYLabels())
    drawYLabels(painter);
}

void
CQChartsImagePlot::
drawXLabels(QPainter *painter)
{
  painter->setFont(textFont());

  QColor tc = interpTextColor(0, 1);

  tc.setAlphaF(textAlpha());

  QPen tpen(tc);

  painter->setPen(tpen);

  CQChartsTextOptions textOptions;

//textOptions.contrast  = isTextContrast();
//textOptions.formatted = isTextFormatted();
//textOptions.scaled    = isTextScaled();
  textOptions.align     = Qt::AlignRight;
  textOptions.angle     = 90;

  QFontMetricsF fm(textFont());

  double tw = 0.0;
//double th = fm.height();
  double tm = 4;

  for (int col = 0; col < nc_; ++col) {
    bool ok;

    QString name = modelHeaderString(col, Qt::Horizontal, ok);
    if (! name.length()) continue;

    tw = std::max(tw, fm.width(name));
  }

  for (int col = 0; col < nc_; ++col) {
    bool ok;

    QString name = modelHeaderString(col, Qt::Horizontal, ok);
    if (! name.length()) continue;

    double tw1 = fm.width(name);

    QPointF p(col + 0.5, 0);

    QPointF p1 = windowToPixel(p);

    QRectF trect(p1.x() - tw/2, p1.y() - tw1 - tm, tw, tw1);

    drawTextInBox(painter, trect, name, tpen, textOptions);
  }
}

void
CQChartsImagePlot::
drawYLabels(QPainter *painter)
{
  painter->setFont(textFont());

  QColor tc = interpTextColor(0, 1);

  tc.setAlphaF(textAlpha());

  QPen tpen(tc);

  painter->setPen(tpen);

  CQChartsTextOptions textOptions;

//textOptions.contrast  = isTextContrast();
//textOptions.formatted = isTextFormatted();
//textOptions.scaled    = isTextScaled();
  textOptions.align     = Qt::AlignRight;

  QFontMetricsF fm(textFont());

  double tw = 0.0;
  double th = fm.height();
  double tm = 4;

  for (int row = 0; row < nr_; ++row) {
    bool ok;

    QString name = modelHeaderString(row, Qt::Vertical, ok);
    if (! name.length()) continue;

    tw = std::max(tw, fm.width(name));
  }

  for (int row = 0; row < nr_; ++row) {
    bool ok;

    QString name = modelHeaderString(row, Qt::Vertical, ok);
    if (! name.length()) continue;

    QPointF p(0, row + 0.5);

    QPointF p1 = windowToPixel(p);

    QRectF trect(p1.x() - tw - tm, p1.y() - th/2.0, tw, th);

    drawTextInBox(painter, trect, name, tpen, textOptions);
  }
}

//------

CQChartsGeom::BBox
CQChartsImagePlot::
annotationBBox() const
{
  CQChartsGeom::BBox bbox;

  QFontMetricsF fm(textFont());

  double tm = 4;

  if (isXLabels()) {
    double tw = 0.0;

    for (int col = 0; col < nc_; ++col) {
      bool ok;

      QString name = modelHeaderString(col, Qt::Horizontal, ok);
      if (! name.length()) continue;

      tw = std::max(tw, fm.width(name));
    }

    double tw1 = pixelToWindowHeight(tw + tm);

    CQChartsGeom::BBox tbbox(0, -tw1, nc_, 0);

    bbox += tbbox;
  }

  if (isYLabels()) {
    double tw = 0.0;

    for (int row = 0; row < nr_; ++row) {
      bool ok;

      QString name = modelHeaderString(row, Qt::Vertical, ok);
      if (! name.length()) continue;

      tw = std::max(tw, fm.width(name));
    }

    double tw1 = pixelToWindowWidth(tw + tm);

    CQChartsGeom::BBox tbbox(-tw1, 0, 0, nr_);

    bbox += tbbox;
  }

  return bbox;
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

QString
CQChartsImageObj::
calcTipId() const
{
  bool ok;

  QString xname = plot_->modelHeaderString(ind_.column(), ok);
  QString yname = plot_->modelHeaderString(ind_.row(), Qt::Vertical, ok);

  QString tipStr;

  if (xname.length())
    tipStr += xname + " ";

  if (yname.length())
    tipStr += yname + " ";

  tipStr += QString("%1").arg(value_);

  return tipStr.simplified();
}

void
CQChartsImageObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, ind_.column());
}

void
CQChartsImageObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid())
    addSelectIndex(inds, ind_.row(), column, ind_.parent());
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

  QPen   pen;
  QBrush brush(c);

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->fillRect(qrect, brush);

  if (plot_->isCellLabels()) {
    painter->setFont(plot_->textFont());

    QColor tc = plot_->interpTextColor(0, 1);

    tc.setAlphaF(plot_->textAlpha());

    QPen   tpen(tc);
    QBrush tbrush;

    plot_->updateObjPenBrushState(this, tpen, tbrush);

    painter->setPen(tpen);

    QString valueStr = CQChartsUtil::toString(value_);

    CQChartsTextOptions textOptions;

    //textOptions.contrast  = plot_->isTextContrast();
    //textOptions.formatted = plot_->isTextFormatted();
    //textOptions.scaled    = plot_->isTextScaled();
    //textOptions.align     = plot_->textAlign();

    plot_->drawTextInBox(painter, qrect, valueStr, tpen, textOptions);
  }
}
