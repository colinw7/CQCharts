#include <CQChartsRadarPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsDrawUtil.h>
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
 CQChartsPlot(view, view->charts()->plotType("radar"), model),
 CQChartsPlotShapeData   <CQChartsRadarPlot>(this),
 CQChartsPlotTextData    <CQChartsRadarPlot>(this),
 CQChartsPlotGridLineData<CQChartsRadarPlot>(this)
{
  setGridLinesColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.5));

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
  addProperty("grid", this, "gridLines", "visible");

  addLineProperties("grid", "gridLines");

  // stroke
  addProperty("stroke", this, "border", "visible");

  addLineProperties("stroke", "border");

  // fill
  addProperty("fill", this, "filled", "visible");

  addFillProperties("fill", "fill");

  // text
  addTextProperties("text", "text");
}

void
CQChartsRadarPlot::
calcRange()
{
  // get values for each row
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsRadarPlot *plot) :
     plot_(plot) {
      nv_ = plot_->numValueColumns();
    }

    State visit(QAbstractItemModel *, const VisitData &data) override {
      for (int iv = 0; iv < nv_; ++iv) {
        const CQChartsColumn &column = plot_->valueColumnAt(iv);

        CQChartsModelIndex ind(data.row, column, data.parent);

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
}

//------

CQChartsGeom::BBox
CQChartsRadarPlot::
annotationBBox() const
{
  CQChartsGeom::BBox bbox;

  int nv = valueColumns().size();

  // add corner labels
  if (nv > 2) {
    double alen = CMathUtil::clamp(angleExtent(), -360.0, 360.0);

    double da = alen/nv;

    //---

    QFont font = view()->plotFont(this, textFont());

    int    nl = 5;
    double dr = valueRadius_/nl;

    for (int i = 0; i <= nl; ++i) {
      double r = dr*i;

      double a = angleStart();

      for (int iv = 0; iv < nv; ++iv) {
        double ra = CMathUtil::Deg2Rad(a);

        double x = r*cos(ra);
        double y = r*sin(ra);

        double px, py;

        windowToPixel(x, y, px, py);

        //---

        if (i == nl) {
          const CQChartsColumn &valueColumn = valueColumns()[iv];

          bool ok;

          QString name = modelHeaderString(valueColumn, ok);

          Qt::Alignment align = 0;

          if      (CMathUtil::isZero(x)) align |= Qt::AlignHCenter;
          else if (x > 0)                align |= Qt::AlignLeft;
          else if (x < 0)                align |= Qt::AlignRight;

          if      (CMathUtil::isZero(y)) align |= Qt::AlignVCenter;
          else if (y > 0)                align |= Qt::AlignBottom;
          else if (y < 0)                align |= Qt::AlignTop;

          QRectF trect = CQChartsDrawUtil::alignedTextRect(font, px, py, name, align, 2, 2);

          bbox += pixelToWindow(CQChartsUtil::fromQRect(trect));
        }

        //---

        a -= da;
      }
    }
  }

  //---

  // add objects
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

    State visit(QAbstractItemModel *, const VisitData &data) override {
      plot_->addRow(data, numRows());

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
addRow(const ModelVisitor::VisitData &data, int nr)
{
  bool hidden = isSetHidden(data.row);

  if (hidden)
    return;

  //---

  // get row name
  bool ok;

  QString name = modelString(data.row, nameColumn(), data.parent, ok);

  //---

  // calc polygon angle
  int nv = valueColumns().size();

  double alen = CMathUtil::clamp(angleExtent(), -360.0, 360.0);

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
    CQChartsModelIndex ind(data.row, valueColumn, data.parent);

    double value;

    if (! columnValue(ind, value))
      continue;

    //---

    // get column name
    QString name = modelHeaderString(valueColumn, ok);

    //---

    // set point
    double scale = valueDatas_[iv].sum();

    double ra = CMathUtil::Deg2Rad(a);

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
  QModelIndex nameInd  = modelIndex(data.row, nameColumn(), data.parent);
  QModelIndex nameInd1 = normalizeIndex(nameInd);

  CQChartsGeom::BBox bbox(-1, -1, 1, 1);

  CQChartsRadarObj *radarObj =
    new CQChartsRadarObj(this, bbox, name, poly, nameValues, nameInd1, data.row, nr);

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

    if (! ok || CMathUtil::isNaN(value))
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

    State visit(QAbstractItemModel *, const VisitData &data) override {
      bool ok;

      QString name = plot_->modelString(data.row, plot_->nameColumn(), data.parent, ok);

      //---

      CQChartsKeyColorBox *color = new CQChartsKeyColorBox(plot_, data.row, numRows());
      CQChartsKeyText     *text  = new CQChartsKeyText(plot_, name, data.row, numRows());

      color->setClickHide(true);

      key_->addItem(color, data.row, 0);
      key_->addItem(text , data.row, 1);

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

bool
CQChartsRadarPlot::
hasBackground() const
{
  return true;
}

void
CQChartsRadarPlot::
drawBackground(QPainter *painter)
{
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
    double alen = CMathUtil::clamp(angleExtent(), -360.0, 360.0);

    double da = alen/nv;

    //---

    // draw grid spokes
    if (isGridLines()) {
      QPen gpen1;

      setGridLineDataPen(gpen1, 0, 1);

      painter->setPen(gpen1);

      //---

      double px1, py1;

      windowToPixel(0.0, 0.0, px1, py1);

      double a = angleStart();

      for (int iv = 0; iv < nv; ++iv) {
        double ra = CMathUtil::Deg2Rad(a);

        double x = valueRadius_*cos(ra);
        double y = valueRadius_*sin(ra);

        double px2, py2;

        windowToPixel(x, y, px2, py2);

        painter->drawLine(QPointF(px1, py1), QPointF(px2, py2));

        a -= da;
      }
    }

    //---

    QPen gpen2;

    setGridLineDataPen(gpen2, 0, 1);

    //---

    QPen tpen;

    QColor tc = interpTextColor(0, 1);

    setPen(tpen, true, tc, textAlpha(), CQChartsLength("0px"));

    //---

    view()->setPlotPainterFont(this, painter, textFont());

    int    nl = 5;
    double dr = valueRadius_/nl;

    for (int i = 0; i <= nl; ++i) {
      double r = dr*i;

      double a = angleStart();

      QPolygonF poly;

      for (int iv = 0; iv < nv; ++iv) {
        double ra = CMathUtil::Deg2Rad(a);

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

          if      (CMathUtil::isZero(x)) align |= Qt::AlignHCenter;
          else if (x > 0)                align |= Qt::AlignLeft;
          else if (x < 0)                align |= Qt::AlignRight;

          if      (CMathUtil::isZero(y)) align |= Qt::AlignVCenter;
          else if (y > 0)                align |= Qt::AlignBottom;
          else if (y < 0)                align |= Qt::AlignTop;

          CQChartsDrawUtil::drawAlignedText(painter, px, py, name, align, 2, 2);
        }

        //---

        a -= da;
      }

      poly << poly[0];

      //---

      // draw grid polygon
      if (isGridLines()) {
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

  if (! visible())
    return bbox;

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
  QPen   pen;
  QBrush brush;

  plot_->setPenBrush(pen, brush,
    plot_->isBorder(), plot_->interpBorderColor(i_, n_), plot_->borderAlpha(),
    plot_->borderWidth(), plot_->borderDash(),
    plot_->isFilled(), plot_->interpFillColor(i_, n_), plot_->fillAlpha(),
    plot_->fillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

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
  else if (poly_.size() >= 3) {
    painter->drawPolygon(ppoly);
  }
}
