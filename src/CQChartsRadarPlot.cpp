#include <CQChartsRadarPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsTextBoxObj.h>
#include <CQChartsTip.h>

#include <QPainter>

CQChartsRadarPlotType::
CQChartsRadarPlotType()
{
}

void
CQChartsRadarPlotType::
addParameters()
{
  startParameterGroup("Radar");

  addColumnParameter ("name" , "Name" , "nameColumn"  );
  addColumnsParameter("value", "Value", "valueColumns", "1").setRequired();

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsRadarPlotType::
description() const
{
  return "<h2>Summary</h2>\n"
         "<p>Draws polygon for each row with a point for each value column.<p>\n"
         "<h2>Columns</h2>\n"
         "<p>The <b>Name</b> column specifies the name for the value set.</p>\n"
         "<p>The column headers specify the name of the indiviidual values.</p>\n";
}

CQChartsPlot *
CQChartsRadarPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsRadarPlot(view, model);
}

//------

CQChartsRadarPlot::
CQChartsRadarPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("radar"), model)
{
  gridData_.color = CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.5);

  setFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));
  setFillAlpha(0.5);

  setBorder(true);
  setFilled(true);

  setTextColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 1));

  addKey();

  addTitle();
}

CQChartsRadarPlot::
~CQChartsRadarPlot()
{
}

//------

void
CQChartsRadarPlot::
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//------

void
CQChartsRadarPlot::
setValueColumn(const CQChartsColumn &c)
{
  if (c != valueColumns_.column()) {
    valueColumns_.setColumn(c);

    updateRangeAndObjs();
  }
}

void
CQChartsRadarPlot::
setValueColumns(const Columns &cols)
{
  if (cols != valueColumns_.columns()) {
    valueColumns_.setColumns(cols);

    updateRangeAndObjs();
  }
}

QString
CQChartsRadarPlot::
valueColumnsStr() const
{
  return valueColumns_.columnsStr();
}

bool
CQChartsRadarPlot::
setValueColumnsStr(const QString &s)
{
  bool rc = true;

  if (s != valueColumnsStr()) {
    rc = valueColumns_.setColumnsStr(s);

    updateRangeAndObjs();
  }

  return rc;
}

const CQChartsColumn &
CQChartsRadarPlot::
valueColumnAt(int i) const
{
  return valueColumns_.getColumn(i);
}

int
CQChartsRadarPlot::
numValueColumns() const
{
  return valueColumns_.count();
}

//------

bool
CQChartsRadarPlot::
isFilled() const
{
  return shapeData_.background.visible;
}

void
CQChartsRadarPlot::
setFilled(bool b)
{
  CQChartsUtil::testAndSet(shapeData_.background.visible, b, [&]() { invalidateLayers(); } );
}

const CQChartsColor &
CQChartsRadarPlot::
fillColor() const
{
  return shapeData_.background.color;
}

void
CQChartsRadarPlot::
setFillColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(shapeData_.background.color, c, [&]() { invalidateLayers(); } );
}

QColor
CQChartsRadarPlot::
interpFillColor(int i, int n)
{
  return fillColor().interpColor(this, i, n);
}

double
CQChartsRadarPlot::
fillAlpha() const
{
  return shapeData_.background.alpha;
}

void
CQChartsRadarPlot::
setFillAlpha(double a)
{
  CQChartsUtil::testAndSet(shapeData_.background.alpha, a, [&]() { invalidateLayers(); } );
}

CQChartsRadarPlot::Pattern
CQChartsRadarPlot::
fillPattern() const
{
  return (Pattern) shapeData_.background.pattern;
}

void
CQChartsRadarPlot::
setFillPattern(Pattern pattern)
{
  if (pattern != (Pattern) shapeData_.background.pattern) {
    shapeData_.background.pattern = (CQChartsFillData::Pattern) pattern;

    invalidateLayers();
  }
}

//------

void
CQChartsRadarPlot::
setAngleStart(double r)
{
  CQChartsUtil::testAndSet(angleStart_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsRadarPlot::
setAngleExtent(double r)
{
  CQChartsUtil::testAndSet(angleExtent_, r, [&]() { updateRangeAndObjs(); } );
}

//----

void
CQChartsRadarPlot::
setGrid(bool b)
{
  CQChartsUtil::testAndSet(gridData_.visible, b, [&]() { invalidateLayers(); } );
}

void
CQChartsRadarPlot::
setGridColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(gridData_.color, c, [&]() { invalidateLayers(); } );
}

QColor
CQChartsRadarPlot::
interpGridColor(int i, int n)
{
  return gridColor().interpColor(this, i, n);
}

void
CQChartsRadarPlot::
setGridAlpha(double r)
{
  CQChartsUtil::testAndSet(gridData_.alpha, r, [&]() { invalidateLayers(); } );
}

void
CQChartsRadarPlot::
setGridWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(gridData_.width, l, [&]() { invalidateLayers(); } );
}

void
CQChartsRadarPlot::
setGridDash(const CQChartsLineDash &d)
{
  CQChartsUtil::testAndSet(gridData_.dash, d, [&]() { invalidateLayers(); } );
}

//------

bool
CQChartsRadarPlot::
isBorder() const
{
  return shapeData_.border.visible;
}

void
CQChartsRadarPlot::
setBorder(bool b)
{
  CQChartsUtil::testAndSet(shapeData_.border.visible, b, [&]() { invalidateLayers(); } );
}

const CQChartsColor &
CQChartsRadarPlot::
borderColor() const
{
  return shapeData_.border.color;
}

void
CQChartsRadarPlot::
setBorderColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(shapeData_.border.color, c, [&]() { invalidateLayers(); } );
}

QColor
CQChartsRadarPlot::
interpBorderColor(int i, int n) const
{
  return borderColor().interpColor(this, i, n);
}

double
CQChartsRadarPlot::
borderAlpha() const
{
  return shapeData_.border.alpha;
}

void
CQChartsRadarPlot::
setBorderAlpha(double a)
{
  CQChartsUtil::testAndSet(shapeData_.border.alpha, a, [&]() { invalidateLayers(); } );
}

const CQChartsLength &
CQChartsRadarPlot::
borderWidth() const
{
  return shapeData_.border.width;
}

void
CQChartsRadarPlot::
setBorderWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(shapeData_.border.width, l, [&]() { invalidateLayers(); } );
}

const CQChartsLineDash &
CQChartsRadarPlot::
borderDash() const
{
  return shapeData_.border.dash;
}

void
CQChartsRadarPlot::
setBorderDash(const CQChartsLineDash &d)
{
  CQChartsUtil::testAndSet(shapeData_.border.dash, d, [&]() { invalidateLayers(); } );
}

//---

const QFont &
CQChartsRadarPlot::
textFont() const
{
  return textData_.font;
}

void
CQChartsRadarPlot::
setTextFont(const QFont &f)
{
  CQChartsUtil::testAndSet(textData_.font, f, [&]() { invalidateLayers(); } );
}

const CQChartsColor &
CQChartsRadarPlot::
textColor() const
{
  return textData_.color;
}

void
CQChartsRadarPlot::
setTextColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(textData_.color, c, [&]() { invalidateLayers(); } );
}

QColor
CQChartsRadarPlot::
interpTextColor(int i, int n) const
{
  return textColor().interpColor(this, i, n);
}

double
CQChartsRadarPlot::
textAlpha() const
{
  return textData_.alpha;
}

void
CQChartsRadarPlot::
setTextAlpha(double a)
{
  CQChartsUtil::testAndSet(textData_.alpha, a, [&]() { invalidateLayers(); } );
}

//----

void
CQChartsRadarPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  // columns
  addProperty("columns", this, "nameColumn"  , "name"     );
  addProperty("columns", this, "valueColumn" , "value"    );
  addProperty("columns", this, "valueColumns", "valuesSet");

  // options
  addProperty("options", this, "angleStart" );
  addProperty("options", this, "angleExtent");

  // grid
  addProperty("grid", this, "grid", "visible");

  addLineProperties("grid", "grid");

  // stroke
  addProperty("stroke", this, "border", "visible");

  addLineProperties("stroke", "border");

  // fill
  addProperty("fill", this, "filled", "visible");

  addFillProperties("fill", "fill");

  // text
  addProperty("text", this, "textFont" , "font" );
  addProperty("text", this, "textColor", "color");
  addProperty("text", this, "textAlpha", "alpha");
}

void
CQChartsRadarPlot::
updateRange(bool apply)
{
  // get values for each row
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsRadarPlot *plot) :
     plot_(plot) {
      nv_ = plot_->numValueColumns();
    }

    State visit(QAbstractItemModel *, const QModelIndex &parent, int row) override {
      for (int iv = 0; iv < nv_; ++iv) {
        const CQChartsColumn &column = plot_->valueColumnAt(iv);

        CQChartsModelIndex ind(row, column, parent);

        double value;

        if (! plot_->columnValue(ind, value))
          continue;

        valueDatas_[iv].add(value);
      }

      return State::OK;
    }

    const ValueDatas &valueDatas() const { return valueDatas_; }

   private:
    CQChartsRadarPlot *plot_ { nullptr };
    int                nv_   { 0 };
    ValueDatas         valueDatas_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  valueDatas_ = visitor.valueDatas();

  //---

  // calc max radius (normalized values)
  valueRadius_ = 0.0;

  int nv = numValueColumns();

  for (int iv = 0; iv < nv; ++iv)
    valueRadius_ = std::max(valueRadius_, valueDatas_[iv].max()/valueDatas_[iv].sum());

  //---

  // set range
  double r = valueRadius_;

  dataRange_.updateRange(-r, -r);
  dataRange_.updateRange( r,  r);

  if (isEqualScale()) {
    double aspect = this->aspect();

    dataRange_.equalScale(aspect);
  }

  //---

  if (apply)
    applyDataRange();
}

//------

CQChartsGeom::BBox
CQChartsRadarPlot::
annotationBBox() const
{
  CQChartsGeom::BBox bbox;

  for (const auto &plotObj : plotObjs_) {
    CQChartsRadarObj *obj = dynamic_cast<CQChartsRadarObj *>(plotObj);

    if (obj)
      bbox += obj->annotationBBox();
  }

  return bbox;
}

//------

bool
CQChartsRadarPlot::
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

  // process model data
  class RadarPlotVisitor : public ModelVisitor {
   public:
    RadarPlotVisitor(CQChartsRadarPlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *, const QModelIndex &ind, int row) override {
      plot_->addRow(ind, row, numRows());

      return State::OK;
    }

   private:
    CQChartsRadarPlot *plot_ { nullptr };
  };

  RadarPlotVisitor radarPlotVisitor(this);

  visitModel(radarPlotVisitor);

  //---

  resetKeyItems();

  //---

  return true;
}

void
CQChartsRadarPlot::
addRow(const QModelIndex &parent, int row, int nr)
{
  bool hidden = isSetHidden(row);

  if (hidden)
    return;

  //---

  // get row name
  bool ok;

  QString name = modelString(row, nameColumn(), parent, ok);

  //---

  // calc polygon angle
  int nv = valueColumns().size();

  double alen = std::min(std::max(angleExtent(), -360.0), 360.0);

  double da = (nv > 2 ? alen/nv : 90.0);

  //---

  // calc polygon points
  QPolygonF                    poly;
  CQChartsRadarObj::NameValues nameValues;

  double a = (nv > 2 ? angleStart() : 0.0);

  for (int iv = 0; iv < nv; ++iv) {
    const CQChartsColumn &valueColumn = valueColumns()[iv];

    //---

    // get column value
    CQChartsModelIndex ind(row, valueColumn, parent);

    double value;

    if (! columnValue(ind, value))
      continue;

    //---

    // get column name
    QString name = modelHeaderString(valueColumn, ok);

    //---

    // set point
    double scale = valueDatas_[iv].sum();

    double ra = CQChartsUtil::Deg2Rad(a);

    double x = value*cos(ra)/scale;
    double y = value*sin(ra)/scale;

    poly << QPointF(x, y);

    //---

    nameValues[name] = value;

    //---

    a -= da;
  }

  //---

  // create object
  QModelIndex nameInd  = modelIndex(row, nameColumn(), parent);
  QModelIndex nameInd1 = normalizeIndex(nameInd);

  CQChartsGeom::BBox bbox(-1, -1, 1, 1);

  CQChartsRadarObj *radarObj =
    new CQChartsRadarObj(this, bbox, name, poly, nameValues, nameInd1, row, nr);

  addPlotObject(radarObj);
}

bool
CQChartsRadarPlot::
columnValue(const CQChartsModelIndex &ind, double &value) const
{
  ColumnType columnType = columnValueType(ind.column);

  value = 1.0;

  if (columnType == ColumnType::INTEGER || columnType == ColumnType::REAL) {
    bool ok;

    value = modelReal(ind.row, ind.column, ind.parent, ok);

    if (! ok || CQChartsUtil::isNaN(value))
      return false;

    if (value <= 0.0)
      return false;
  }
  else {
    bool ok;

    value = modelReal(ind.row, ind.column, ind.parent, ok);

    if (! ok)
      value = 1.0; // string non-real -> 1.0

    if (value <= 0.0)
      value = 1.0;
  }

  return true;
}

void
CQChartsRadarPlot::
addKeyItems(CQChartsPlotKey *key)
{
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsRadarPlot *plot, CQChartsPlotKey *key) :
     plot_(plot), key_(key) {
    }

    State visit(QAbstractItemModel *, const QModelIndex &parent, int row) override {
      bool ok;

      QString name = plot_->modelString(row, plot_->nameColumn(), parent, ok);

      //---

      CQChartsKeyColorBox *color = new CQChartsKeyColorBox(plot_, row, numRows());
      CQChartsKeyText     *text  = new CQChartsKeyText(plot_, name);

      color->setClickHide(true);

      key_->addItem(color, row, 0);
      key_->addItem(text , row, 1);

      return State::OK;
    }

   private:
    CQChartsRadarPlot *plot_ { nullptr };
    CQChartsPlotKey   *key_  { nullptr };
  };

  RowVisitor visitor(this, key);

  visitModel(visitor);

  //---

  key->plot()->updateKeyPosition(/*force*/true);
}

void
CQChartsRadarPlot::
handleResize()
{
  CQChartsPlot::handleResize();

  dataRange_.reset();
}

void
CQChartsRadarPlot::
drawBackground(QPainter *painter)
{
  CQChartsPlot::drawBackground(painter);

  //---

  int nv = valueColumns().size();

  if (! nv)
    return;

  //---

  if      (nv == 1) {
    // TODO
  }
  else if (nv == 2) {
    // TODO
  }
  else if (nv > 2) {
    double alen = std::min(std::max(angleExtent(), -360.0), 360.0);

    double da = alen/nv;

    //---

    // draw grid spokes
    if (isGrid()) {
      QColor gridColor1 = interpGridColor(0, 1);

      gridColor1.setAlphaF(gridAlpha());

      double lw = lengthPixelWidth(gridWidth());

      QPen gpen1(gridColor1);

      gpen1.setWidthF(lw);

      CQChartsUtil::penSetLineDash(gpen1, gridDash());

      painter->setPen(gpen1);

      //---

      double px1, py1;

      windowToPixel(0.0, 0.0, px1, py1);

      double a = angleStart();

      for (int iv = 0; iv < nv; ++iv) {
        double ra = CQChartsUtil::Deg2Rad(a);

        double x = valueRadius_*cos(ra);
        double y = valueRadius_*sin(ra);

        double px2, py2;

        windowToPixel(x, y, px2, py2);

        painter->drawLine(QPointF(px1, py1), QPointF(px2, py2));

        a -= da;
      }
    }

    //---

    QColor gridColor2 = interpGridColor(0, 1);

    QPen gpen2(gridColor2);

    //---

    QColor tc = interpTextColor(0, 1);

    tc.setAlphaF(textAlpha());

    QPen tpen(tc);

    //---

    painter->setFont(textFont());

    int    nl = 5;
    double dr = valueRadius_/nl;

    for (int i = 0; i <= nl; ++i) {
      double r = dr*i;

      double a = angleStart();

      QPolygonF poly;

      for (int iv = 0; iv < nv; ++iv) {
        double ra = CQChartsUtil::Deg2Rad(a);

        double x = r*cos(ra);
        double y = r*sin(ra);

        double px, py;

        windowToPixel(x, y, px, py);

        poly << QPointF(px, py);

        //---

        if (i == nl) {
          painter->setPen(tpen);

          //---

          const CQChartsColumn &valueColumn = valueColumns()[iv];

          bool ok;

          QString name = modelHeaderString(valueColumn, ok);

          Qt::Alignment align = 0;

          if      (CQChartsUtil::isZero(x)) align |= Qt::AlignHCenter;
          else if (x > 0)                   align |= Qt::AlignLeft;
          else if (x < 0)                   align |= Qt::AlignRight;

          if      (CQChartsUtil::isZero(y)) align |= Qt::AlignVCenter;
          else if (y > 0)                   align |= Qt::AlignBottom;
          else if (y < 0)                   align |= Qt::AlignTop;

          CQChartsDrawUtil::drawAlignedText(painter, px, py, name, align, 2, 2);
        }

        //---

        a -= da;
      }

      poly << poly[0];

      //---

      // draw grid polygon

      if (isGrid()) {
        painter->setPen(gpen2);

        painter->drawPolygon(poly);
      }
    }
  }
}

//------

CQChartsRadarObj::
CQChartsRadarObj(CQChartsRadarPlot *plot, const CQChartsGeom::BBox &rect, const QString &name,
                 const QPolygonF &poly, const NameValues &nameValues, const QModelIndex &ind,
                 int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), name_(name), poly_(poly), nameValues_(nameValues),
 ind_(ind), i_(i), n_(n)
{
  assert(i_ >= 0 && i < n_);
}

QString
CQChartsRadarObj::
calcId() const
{
  return QString("poly:%1").arg(i_);
}

QString
CQChartsRadarObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Name", name_);

  for (const auto &nameValue : nameValues_)
    tableTip.addTableRow(nameValue.first, nameValue.second);

  return tableTip.str();
}

bool
CQChartsRadarObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  if      (poly_.size() == 1) {
    const QPointF &p1 = poly_[0]; // circle radius p1.x()

    double r  = std::hypot(p .x  , p .y  );
    double r1 = std::hypot(p1.x(), p1.x());

    return (r < r1);
  }
  else if (poly_.size() == 2) {
    const QPointF &p1 = poly_[0]; // circle radius p1.x() and p2.y()
    const QPointF &p2 = poly_[1];

    double r  = std::hypot(p .x  , p .y  );
    double r1 = std::hypot(p1.x(), p2.y());

    return (r < r1);
  }
  else if (poly_.size() >= 3)
    return poly_.containsPoint(CQChartsUtil::toQPoint(p), Qt::OddEvenFill);
  else
    return false;
}

CQChartsGeom::BBox
CQChartsRadarObj::
annotationBBox() const
{
  CQChartsGeom::BBox bbox;

  return bbox;
}

void
CQChartsRadarObj::
getSelectIndices(Indices &inds) const
{
  for (const auto &valueColumn : plot_->valueColumns())
    addColumnSelectIndex(inds, valueColumn);

  addColumnSelectIndex(inds, ind_.column());
}

void
CQChartsRadarObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid())
    addSelectIndex(inds, ind_.row(), column, ind_.parent());
}

void
CQChartsRadarObj::
draw(QPainter *painter)
{
  if (! poly_.size())
    return;

  //---

  // get pixel origin
  double pxo, pyo;

  plot_->windowToPixel(0.0, 0.0, pxo, pyo);

  //---

  // create pixel polygon
  QPolygonF ppoly;

  for (int i = 0; i < poly_.size(); ++i) {
    double x = poly_[i].x();
    double y = poly_[i].y();

    double px, py;

    plot_->windowToPixel(x, y, px, py);

    ppoly << QPointF(px, py);
  }

  ppoly << ppoly[0];

  //---

  // calc stroke and brush
  QBrush brush;

  if (plot_->isFilled()) {
    QColor c = plot_->interpFillColor(i_, n_);

    c.setAlphaF(plot_->fillAlpha());

    brush.setColor(c);

    brush.setStyle(CQChartsFillPattern::toStyle(
     (CQChartsFillPattern::Type) plot_->fillPattern()));
  }
  else {
    brush.setStyle(Qt::NoBrush);
  }

  QPen pen;

  if (plot_->isBorder()) {
    QColor bc = plot_->interpBorderColor(0, 1);

    bc.setAlphaF(plot_->borderAlpha());

    double bw = plot_->lengthPixelWidth(plot_->borderWidth());

    pen.setColor (bc);
    pen.setWidthF(bw);
  }
  else {
    pen.setStyle(Qt::NoPen);
  }

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  if      (poly_.size() == 1) {
    const QPointF &p1 = ppoly[0]; // circle radius p1.x()

    double r = p1.x() - pxo;

    painter->drawEllipse(QRectF(pxo - r, pyo - r, 2*r, 2*r));
  }
  else if (poly_.size() == 2) {
    const QPointF &p1 = ppoly[0]; // circle radius p1.x() and p2.y()
    const QPointF &p2 = ppoly[1];

    double xr = p1.x() - pxo;
    double yr = p2.y() - pyo;

    painter->drawEllipse(QRectF(pxo - xr, pyo - yr, 2*xr, 2*yr));
  }
  else if (poly_.size() >= 3)
    painter->drawPolygon(ppoly);
}
