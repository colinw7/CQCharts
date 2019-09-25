#include <CQChartsRadarPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsTip.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

CQChartsRadarPlotType::
CQChartsRadarPlotType()
{
}

void
CQChartsRadarPlotType::
addParameters()
{
  startParameterGroup("Radar");

  addColumnParameter("name", "Name", "nameColumn").
    setString().setTip("Name column");

  addColumnsParameter("value", "Value", "valueColumns").
   setNumeric().setRequired().setTip("Value column");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsRadarPlotType::
description() const
{
  auto B   = [](const QString &str) { return CQChartsHtml::Str::bold(str); };
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Radar Plot").
    h3("Summary").
     p("Draws polygon for each row with a point for each value column.").
    h3("Columns").
    p("The " + B("Name") + " column specifies the name for the value set.").
    p("The column headers specify the name of the indiviidual values.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/radar.png"));
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
 CQChartsObjShapeData   <CQChartsRadarPlot>(this),
 CQChartsObjTextData    <CQChartsRadarPlot>(this),
 CQChartsObjGridLineData<CQChartsRadarPlot>(this)
{
  NoUpdate noUpdate(this);

  setGridLinesColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 0.5));

  setFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));
  setFillAlpha(0.5);

  setFilled (true);
  setStroked(true);

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

void
CQChartsRadarPlot::
setValueColumns(const CQChartsColumns &c)
{
  CQChartsUtil::testAndSet(valueColumns_, c, [&]() { updateRangeAndObjs(); } );
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
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  CQChartsPlot::addProperties();

  // columns
  addProp("columns", "nameColumn"  , "name"  , "Name column");
  addProp("columns", "valueColumns", "values", "Value columns");

  // options
  addProp("options", "angleStart" , "", "Angle start");
  addProp("options", "angleExtent", "", "Angle extent");

  // grid
  addProp("grid", "gridLines", "visible", "Grid lines visible");

  addLineProperties("grid/stroke", "gridLines", "Grid");

  // fill
  addProp("fill", "filled", "visible", "Fill visible");

  addFillProperties("fill", "fill", "");

  // stroke
  addProp("stroke", "stroked", "visible", "Stroke visible");

  addLineProperties("stroke", "stroke", "");

  // text
  addTextProperties("text", "text", "");
}

CQChartsGeom::Range
CQChartsRadarPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsRadarPlot::calcRange");

  CQChartsRadarPlot *th = const_cast<CQChartsRadarPlot *>(this);

  // get values for each row
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsRadarPlot *plot) :
     plot_(plot) {
      nv_ = plot_->valueColumns().count();
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      for (int iv = 0; iv < nv_; ++iv) {
        const CQChartsColumn &column = plot_->valueColumns().getColumn(iv);

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
    const CQChartsRadarPlot *plot_ { nullptr };
    int                      nv_   { 0 };
    ValueDatas               valueDatas_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  th->valueDatas_ = visitor.valueDatas();

  //---

  // calc max radius (normalized values)
  th->valueRadius_ = 0.0;

  int nv = valueColumns().count();

  for (int iv = 0; iv < nv; ++iv) {
    auto pd = valueDatas_.find(iv);
    if (pd == valueDatas_.end()) continue;

    const ValueData &valueData = (*pd).second;

    th->valueRadius_ = std::max(valueRadius_, valueData.max()/valueData.sum());
  }

  //---

  // set range
  double r = valueRadius_;

  CQChartsGeom::Range dataRange;

  if (r > 0.0) {
    dataRange.updateRange(-r, -r);
    dataRange.updateRange( r,  r);
  }

  if (isEqualScale()) {
    double aspect = this->aspect();

    dataRange.equalScale(aspect);
  }

  return dataRange;
}

//------

CQChartsGeom::BBox
CQChartsRadarPlot::
annotationBBox() const
{
  CQChartsGeom::BBox bbox;

  int nv = valueColumns().count();

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

        CQChartsGeom::Point p1 = windowToPixel(CQChartsGeom::Point(x, y));

        //---

        if (i == nl) {
          const CQChartsColumn &valueColumn = valueColumns().getColumn(iv);

          bool ok;

          QString name = modelHeaderString(valueColumn, ok);

          Qt::Alignment align = 0;

          if      (CMathUtil::isZero(x)) align |= Qt::AlignHCenter;
          else if (x > 0)                align |= Qt::AlignLeft;
          else if (x < 0)                align |= Qt::AlignRight;

          if      (CMathUtil::isZero(y)) align |= Qt::AlignVCenter;
          else if (y > 0)                align |= Qt::AlignBottom;
          else if (y < 0)                align |= Qt::AlignTop;

          QRectF trect =
            CQChartsDrawUtil::calcAlignedTextRect(font, p1.qpoint(), name, align, 2, 2);

          bbox += pixelToWindow(CQChartsGeom::BBox(trect));
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
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsRadarPlot::createObjs");

  NoUpdate noUpdate(this);

  //---

  // process model data
  class RadarPlotVisitor : public ModelVisitor {
   public:
    RadarPlotVisitor(const CQChartsRadarPlot *plot, PlotObjs &objs) :
     plot_(plot), objs_(objs) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      plot_->addRow(data, numRows(), objs_);

      return State::OK;
    }

   private:
    const CQChartsRadarPlot* plot_ { nullptr };
    PlotObjs&                objs_;
  };

  RadarPlotVisitor radarPlotVisitor(this, objs);

  visitModel(radarPlotVisitor);

  //---

  return true;
}

void
CQChartsRadarPlot::
addRow(const ModelVisitor::VisitData &data, int nr, PlotObjs &objs) const
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
  int nv = valueColumns().count();

  double alen = CMathUtil::clamp(angleExtent(), -360.0, 360.0);

  double da = (nv > 2 ? alen/nv : 90.0);

  //---

  // calc polygon points
  QPolygonF                    poly;
  CQChartsRadarObj::NameValues nameValues;

  double a = (nv > 2 ? angleStart() : 0.0);

  for (int iv = 0; iv < nv; ++iv) {
    const CQChartsColumn &valueColumn = valueColumns().getColumn(iv);

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

    auto pd = valueDatas_.find(iv);
    assert(pd != valueDatas_.end());

    const ValueData &valueData = (*pd).second;

    //---

    // set point
    double scale = valueData.sum();

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

  ColorInd is(data.row, nr);

  CQChartsRadarObj *radarObj =
    new CQChartsRadarObj(this, bbox, name, poly, nameValues, nameInd1, is);

  objs.push_back(radarObj);
}

bool
CQChartsRadarPlot::
columnValue(const CQChartsModelIndex &ind, double &value) const
{
  ColumnType columnType = columnValueType(ind.column);

  value = 1.0;

  if (columnType == ColumnType::INTEGER || columnType == ColumnType::REAL) {
    bool ok;

    value = modelReal(ind, ok);

    if (! ok || CMathUtil::isNaN(value))
      return false;

    if (value <= 0.0)
      return false;
  }
  else {
    bool ok;

    value = modelReal(ind, ok);

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
    RowVisitor(const CQChartsRadarPlot *plot, CQChartsPlotKey *key) :
     plot_(plot), key_(key) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      bool ok;

      QString name = plot_->modelString(data.row, plot_->nameColumn(), data.parent, ok);

      //---

      CQChartsRadarPlot *plot = const_cast<CQChartsRadarPlot *>(plot_);

      ColorInd ic(data.row, numRows());

      CQChartsKeyColorBox *color = new CQChartsKeyColorBox(plot, ColorInd(), ColorInd(), ic);
      CQChartsKeyText     *text  = new CQChartsKeyText(plot, name, ic);

      color->setClickable(true);

      key_->addItem(color, data.row, 0);
      key_->addItem(text , data.row, 1);

      return State::OK;
    }

   private:
    const CQChartsRadarPlot* plot_ { nullptr };
    CQChartsPlotKey*         key_  { nullptr };
  };

  RowVisitor visitor(this, key);

  visitModel(visitor);

  //---

  key->plot()->updateKeyPosition(/*force*/true);
}

void
CQChartsRadarPlot::
postResize()
{
  CQChartsPlot::postResize();

  resetDataRange(/*updateRange*/true, /*updateObjs*/false);
}

bool
CQChartsRadarPlot::
hasBackground() const
{
  return true;
}

void
CQChartsRadarPlot::
execDrawBackground(CQChartsPaintDevice *device) const
{
  int nv = valueColumns().count();

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

      device->setPen(gpen1);

      //---

      CQChartsGeom::Point p1 = windowToPixel(CQChartsGeom::Point(0.0, 0.0));

      double a = angleStart();

      for (int iv = 0; iv < nv; ++iv) {
        double ra = CMathUtil::Deg2Rad(a);

        double x = valueRadius_*cos(ra);
        double y = valueRadius_*sin(ra);

        CQChartsGeom::Point p2 = windowToPixel(CQChartsGeom::Point(x, y));

        device->drawLine(device->pixelToWindow(p1.qpoint()),
                         device->pixelToWindow(p2.qpoint()));

        a -= da;
      }
    }

    //---

    QPen gpen2;

    setGridLineDataPen(gpen2, 0, 1);

    //---

    QPen tpen;

    QColor tc = interpTextColor(ColorInd());

    setPen(tpen, true, tc, textAlpha());

    //---

    view()->setPlotPainterFont(this, device, textFont());

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

        CQChartsGeom::Point p1(x, y);

        poly << p1.qpoint();

        //---

        if (i == nl) {
          device->setPen(tpen);

          //---

          const CQChartsColumn &valueColumn = valueColumns().getColumn(iv);

          bool ok;

          QString name = modelHeaderString(valueColumn, ok);

          Qt::Alignment align = 0;

          if      (CMathUtil::isZero(x)) align |= Qt::AlignHCenter;
          else if (x > 0)                align |= Qt::AlignLeft;
          else if (x < 0)                align |= Qt::AlignRight;

          if      (CMathUtil::isZero(y)) align |= Qt::AlignVCenter;
          else if (y > 0)                align |= Qt::AlignBottom;
          else if (y < 0)                align |= Qt::AlignTop;

          CQChartsDrawUtil::drawAlignedText(device, p1.qpoint(), name, align, 2, 2);
        }

        //---

        a -= da;
      }

      poly << poly[0];

      //---

      // draw grid polygon
      if (isGridLines()) {
        device->setPen(gpen2);

        device->drawPolygon(poly);
      }
    }
  }
}

//------

CQChartsRadarObj::
CQChartsRadarObj(const CQChartsRadarPlot *plot, const CQChartsGeom::BBox &rect, const QString &name,
                 const QPolygonF &poly, const NameValues &nameValues, const QModelIndex &ind,
                 const ColorInd &is) :
 CQChartsPlotObj(const_cast<CQChartsRadarPlot *>(plot), rect, is, ColorInd(), ColorInd()),
 plot_(plot), name_(name), poly_(poly), nameValues_(nameValues)
{
  setDetailHint(DetailHint::MAJOR);

  setModelInd(ind);
}

QString
CQChartsRadarObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(is_.i);
}

QString
CQChartsRadarObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Name", name_);

  for (const auto &nameValue : nameValues_)
    tableTip.addTableRow(nameValue.first, nameValue.second);

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

//---

void
CQChartsRadarObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  QString path1 = path + "/" + propertyId();

  model->setObjectRoot(path1, this);

  CQChartsPlotObj::addProperties(model, path1);

  model->addProperty(path1, this, "rect"    )->setDesc("Bounding box");
//model->addProperty(path1, this, "selected")->setDesc("Is selected");

  model->addProperty(path1, this, "name")->setDesc("Name");
}

//---

bool
CQChartsRadarObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  // point
  if      (poly_.size() == 1) {
    const QPointF &p1 = poly_[0]; // circle radius p1.x()

    double r  = std::hypot(p .x  , p .y  );
    double r1 = std::hypot(p1.x(), p1.x());

    return (r < r1);
  }
  // line
  else if (poly_.size() == 2) {
    const QPointF &p1 = poly_[0]; // circle radius p1.x() and p2.y()
    const QPointF &p2 = poly_[1];

    double r  = std::hypot(p .x  , p .y  );
    double r1 = std::hypot(p1.x(), p2.y());

    return (r < r1);
  }
  // polygon
  else if (poly_.size() >= 3) {
    return poly_.containsPoint(p.qpoint(), Qt::OddEvenFill);
  }
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

  addColumnSelectIndex(inds, modelInd().column());
}

void
CQChartsRadarObj::
draw(CQChartsPaintDevice *device)
{
  if (! poly_.size())
    return;

  //---

  // get pixel origin
  CQChartsGeom::Point po = plot_->windowToPixel(CQChartsGeom::Point(0.0, 0.0));

  //---

  // create pixel polygon
  QPolygonF ppoly;

  for (int i = 0; i < poly_.size(); ++i)
    ppoly << plot_->windowToPixel(poly_[i]);

  ppoly << ppoly[0];

  //---

  // calc stroke and brush
  ColorInd colorInd = calcColorInd();

  QPen   pen;
  QBrush brush;

  QColor strokeColor = plot_->interpStrokeColor(colorInd);
  QColor fillColor   = plot_->interpFillColor  (colorInd);

  plot_->setPenBrush(pen, brush,
    plot_->isStroked(), strokeColor, plot_->strokeAlpha(),
    plot_->strokeWidth(), plot_->strokeDash(),
    plot_->isFilled(), fillColor, plot_->fillAlpha(), plot_->fillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  device->setPen  (pen);
  device->setBrush(brush);

  //---

  // draw point
  if      (poly_.size() == 1) {
    const QPointF &p1 = ppoly[0]; // circle radius p1.x()

    double r = p1.x() - po.x;

    QRectF pr(po.x - r, po.y - r, 2*r, 2*r);

    device->drawEllipse(device->pixelToWindow(pr));
  }
  // draw line
  else if (poly_.size() == 2) {
    const QPointF &p1 = ppoly[0]; // circle radius p1.x() and p2.y()
    const QPointF &p2 = ppoly[1];

    double xr = p1.x() - po.x;
    double yr = p2.y() - po.y;

    QRectF pr(po.x - xr, po.y - yr, 2*xr, 2*yr);

    device->drawEllipse(device->pixelToWindow(pr));
  }
  // draw polygon
  else if (poly_.size() >= 3) {
    device->drawPolygon(poly_);
  }
}
