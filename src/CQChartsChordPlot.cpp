#include <CQChartsChordPlot.h>
#include <CQChartsView.h>
#include <CQChartsRotatedTextBoxObj.h>
#include <CQChartsModelDetails.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsTip.h>
#include <CQCharts.h>
#include <CQChartsValueSet.h>
#include <CQChartsNamePair.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

//---

CQChartsChordPlotType::
CQChartsChordPlotType()
{
}

void
CQChartsChordPlotType::
addParameters()
{
  startParameterGroup("Chord");

  addColumnParameter("link" , "Link" , "linkColumn" );
  addColumnParameter("value", "Value", "valueColumn");
  addColumnParameter("group", "Group", "groupColumn");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsChordPlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Chord Plot").
    h3("Summary").
     p("Draw connections using radial plot with sized path arcs.").
     p("The size of each arc is equivalent to the number of connections from that section.").
    h3("Columns").
     p("The link column specifies the node name and index (row) and the group column "
       "specifies the parent group. The remaining columns contain the connection value for "
       "each connected node i.e. the should be N rows and N + 1 or N + 2 columns depending "
       "on whether a group is specified").
    h3("Options").
     p("The inner radius (0-1) can be specified to adjust the with of the ring and connection "
       "area. The radius for the label can be specified ((0-1) inside, >1 outside) and the "
       "nodes can be sorted by value or use the original model order").
    h3("Customization").
     p("The stroke style, segment fill alpha, and arc fill alpha can be specified. The start angle "
       "and gap between nodes (in degress) can be specified. The label text style can be "
       "specified.").
    h3("Limitations").
     p("A user defined range cannot be specified, no axes or key is supported, logarithmic "
       "values are not allowed and probing is not available.").
    h3("Example").
     p(IMG("images/chord_plot.png"));
}

bool
CQChartsChordPlotType::
isColumnForParameter(CQChartsModelColumnDetails *columnDetails,
                     CQChartsPlotParameter *parameter) const
{
  if (parameter->name() == "link") {
    if (columnDetails->type() == CQChartsPlot::ColumnType::NAME_PAIR)
      return true;

    return false;
  }

  return CQChartsPlotType::isColumnForParameter(columnDetails, parameter);
}

CQChartsPlot *
CQChartsChordPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsChordPlot(view, model);
}

//---

CQChartsChordPlot::
CQChartsChordPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("chord"), model),
 CQChartsObjStrokeData<CQChartsChordPlot>(this)
{
  NoUpdate noUpdate(this);

  textBox_ = new CQChartsRotatedTextBoxObj(this);

  setStrokeAlpha(0.3);

  setLayerActive(CQChartsLayer::Type::FG_PLOT, true);

  addTitle();
}

CQChartsChordPlot::
~CQChartsChordPlot()
{
  delete textBox_;
}

//---

void
CQChartsChordPlot::
setLinkColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(linkColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsChordPlot::
setValueColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsChordPlot::
setGroupColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(groupColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsChordPlot::
setSorted(bool b)
{
  CQChartsUtil::testAndSet(sorted_, b, [&]() { updateObjs(); } );
}

void
CQChartsChordPlot::
setInnerRadius(double r)
{
  CQChartsUtil::testAndSet(innerRadius_, r, [&]() { drawObjs(); } );
}

void
CQChartsChordPlot::
setLabelRadius(double r)
{
  CQChartsUtil::testAndSet(labelRadius_, r, [&]() { drawObjs(); } );
}

//---

void
CQChartsChordPlot::
setSegmentAlpha(double r)
{
  CQChartsUtil::testAndSet(segmentAlpha_, r, [&]() { drawObjs(); } );
}

void
CQChartsChordPlot::
setArcAlpha(double r)
{
  CQChartsUtil::testAndSet(arcAlpha_, r, [&]() { drawObjs(); } );
}

void
CQChartsChordPlot::
setGapAngle(double r)
{
  CQChartsUtil::testAndSet(gapAngle_, r, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsChordPlot::
setStartAngle(double r)
{
  CQChartsUtil::testAndSet(startAngle_, r, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsChordPlot::
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  auto addStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                          const QString &desc) {
    CQPropertyViewItem *item = addProp(path, name, alias, desc);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  //---

  CQChartsPlot::addProperties();

  // columns
  addProp("columns", "linkColumn" , "link" , "Link column");
  addProp("columns", "valueColumn", "value", "Value column");
  addProp("columns", "groupColumn", "group", "Grouping column");

  // options
  addProp("options", "sorted"     , "", "Sort values by size");
  addProp("options", "innerRadius", "", "Radius of inside of outer strip");

  // stroke
  addLineProperties("stroke", "stroke", "");

  // segment
  addStyleProp("segment/fill", "segmentAlpha", "alpha", "Alpha of segment fill");

  // arc
  addProp("arc", "gapAngle"  , "gapAngle"  , "Angle for gap between strip segements");
  addProp("arc", "startAngle", "startAngle", "Angle for first strip segment");

  addStyleProp("arc/fill", "arcAlpha"  , "alpha"     , "Alpha for arc fill");

  // labels
  textBox_->addTextDataProperties(propertyModel(), "labels", "Labels");

  addProp("labels", "labelRadius", "radius", "Radius for segment label");

  QString labelBoxPath = "labels/box";

  textBox_->CQChartsBoxObj::addProperties(propertyModel(), labelBoxPath, "Labels");
}

CQChartsGeom::Range
CQChartsChordPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsChordPlot::calcRange");

  double r = 1.0;

  r = std::max(r, labelRadius());

  CQChartsGeom::Range dataRange;

  dataRange.updateRange(-r, -r);
  dataRange.updateRange( r,  r);

  if (isEqualScale()) {
    double aspect = this->aspect();

    dataRange.equalScale(aspect);
  }

  return dataRange;
}

CQChartsGeom::BBox
CQChartsChordPlot::
annotationBBox() const
{
  CQChartsGeom::BBox bbox;

  for (const auto &plotObj : plotObjs_) {
    CQChartsChordObj *obj = dynamic_cast<CQChartsChordObj *>(plotObj);

    if (obj)
      bbox += obj->textBBox();
  }

  return bbox;
}

//------

bool
CQChartsChordPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsChordPlot::createObjs");

  NoUpdate noUpdate(this);

  //---

  if (valueColumn().isValid())
    return initHierObjs(objs);

  return initTableObjs(objs);
}

bool
CQChartsChordPlot::
initTableObjs(PlotObjs &objs) const
{
  using RowData = std::vector<QVariant>;

  struct IndRowData {
    QModelIndex ind;
    RowData     rowData;
  };

  using IndRowDatas = std::vector<IndRowData>;

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsChordPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      int nc = numCols();

      IndRowData indRowData;

      indRowData.rowData.resize(nc);

      for (int col = 0; col < numCols(); ++col) {
        QModelIndex ind = plot_->modelIndex(data.row, col, data.parent);

        if (col == 0)
          indRowData.ind = ind;

        bool ok;

        indRowData.rowData[col] = plot_->modelValue(data.row, col, data.parent, ok);
      }

      indRowDatas_.push_back(indRowData);

      return State::OK;
    }

    const IndRowDatas &indRowDatas() const { return indRowDatas_; }

   private:
    const CQChartsChordPlot* plot_ { nullptr };
    IndRowDatas              indRowDatas_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  //---

  const IndRowDatas &indRowDatas = visitor.indRowDatas();

  int nr = indRowDatas.size();
  int nc = (nr > 0 ? indRowDatas[0].rowData.size() : 0);

  int numExtraColumns = 0;

  if (linkColumn ().isValid()) ++numExtraColumns;
  if (groupColumn().isValid()) ++numExtraColumns;

  int nv = std::min(nr, nc - numExtraColumns);

  //---

  using Datas = std::vector<CQChartsChordData>;

  Datas datas;

  datas.resize(nv);

  //---

  CQChartsValueSet groupValues(this);

  if (groupColumn().isValid() && groupColumn().column() < nv) {
    int igroup = groupColumn().column();

    for (int row = 0; row < nv; ++row) {
      QVariant group = indRowDatas[row].rowData[igroup];

      groupValues.addValue(group);
    }
  }

  //---

  int nv1 = 0;

  double total = 0.0;

  for (int row = 0; row < nv; ++row) {
    CQChartsChordData &data = datas[row];

    data.setFrom(row);

    //---

    const QModelIndex &ind = indRowDatas[row].ind;

    //---

    // set link
    if (linkColumn().isValid() && linkColumn().column() < nv) {
      QModelIndex linkInd  = modelIndex(ind.row(), linkColumn(), ind.parent());
      QModelIndex linkInd1 = normalizeIndex(linkInd);

      QVariant var = indRowDatas[row].rowData[linkColumn().column()];

      QString link;

      CQChartsVariant::toString(var, link);

      data.setName(link);

      data.setInd(linkInd1);
    }

    //---

    // set group
    if (groupColumn().isValid() && groupColumn().column() < nv) {
      int igroup = groupColumn().column();

      QModelIndex groupInd  = modelIndex(ind.row(), groupColumn(), ind.parent());
      QModelIndex groupInd1 = normalizeIndex(groupInd);

      QVariant groupVar = indRowDatas[row].rowData[igroup];

      QString groupStr;

      CQChartsVariant::toString(groupVar, groupStr);

      int ig = groupValues.iset(groupVar);
      int ng = groupValues.numUnique();

      data.setGroup(CQChartsChordData::Group(groupStr, ig, ng));

      data.setInd(groupInd1);
    }

    //---

    // get values
    int col1 = 0;

    for (int col = 0; col < nv; ++col) {
      if (col == linkColumn() || col == groupColumn())
        continue;

      //---

      bool ok;

      double value = CQChartsVariant::toReal(indRowDatas[row].rowData[col], ok);

      //---

      if (CMathUtil::isZero(value))
        continue;

      data.addValue(col1, value);

      ++col1;
    }

    //---

    // add to total
    double total1 = data.total();

    if (! CMathUtil::isZero(total1))
      ++nv1;

    total += total1;
  }

  //---

  // sort
  if (isSorted()) {
    std::sort(datas.begin(), datas.end(),
      [](const CQChartsChordData &lhs, const CQChartsChordData &rhs) {
        return lhs.total() < rhs.total();
      });

    for (auto &data : datas)
      data.sort();
  }

  //---

  // 360 degree circle, minus gap angle degrees per set
  double gap = this->gapAngle();

  double drange = 360 - nv1*gap;

  while (drange <= 0) {
    gap /= 2.0;

    drange = 360 - nv1*gap;
  }

  // divide remaining degrees by total to get value->degrees factor
  CQChartsChordPlot *th = const_cast<CQChartsChordPlot *>(this);

  th->valueToDegrees_ = drange/total;

  //---

  double angle1 = this->startAngle();

  for (int row = 0; row < nv; ++row) {
    CQChartsChordData &data = datas[row];

    double total1 = data.total();

    if (CMathUtil::isZero(total1))
      continue;

    double dangle = -valueToDegrees(total1);

    double angle2 = angle1 + dangle;

    data.setAngles(angle1, dangle);

    angle1 = angle2 - gap;
  }

  //---

  for (int row = 0; row < nv; ++row) {
    CQChartsChordData &data = datas[row];

    CQChartsGeom::BBox rect(-1, -1, 1, 1);

    ColorInd ig(data.group().i, data.group().n);
    ColorInd iv(row, nv);

    CQChartsChordObj *obj = new CQChartsChordObj(this, rect, data, ig, iv);

    objs.push_back(obj);
  }

  //---

  return true;
}

bool
CQChartsChordPlot::
initHierObjs(PlotObjs &objs) const
{
  CQChartsValueSet groupValues(this);

  if (groupColumn().isValid())
    addColumnValues(groupColumn(), groupValues);

  //---

  using NameDataMap = std::map<QString,CQChartsChordData>;

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsChordPlot *plot, CQChartsValueSet &groupValues) :
     plot_(plot), groupValues_(groupValues) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      bool ok1, ok2;

      QString linkStr = plot_->modelString(data.row, plot_->linkColumn (), data.parent , ok1);
      double  value   = plot_->modelReal  (data.row, plot_->valueColumn(), data.parent, ok2);

      if (! ok1 || ! ok2)
        return State::SKIP;

      //---

      // decode link into src and dest
      CQChartsNamePair linkPair(linkStr);

      if (! linkPair.isValid())
        return State::SKIP;

      QModelIndex linkInd  = plot_->modelIndex(data.row, plot_->linkColumn(), data.parent);
      QModelIndex linkInd1 = plot_->normalizeIndex(linkInd);

      QString srcStr  = linkPair.name1();
      QString destStr = linkPair.name2();

      // find src (create if doesn't exist)
      auto ps = nameDataMap_.find(srcStr);

      if (ps == nameDataMap_.end()) {
        ps = nameDataMap_.insert(ps, NameDataMap::value_type(srcStr, CQChartsChordData()));

        (*ps).second.setFrom(nameDataMap_.size() - 1);
        (*ps).second.setName(srcStr);
        (*ps).second.setInd (linkInd1);
      }

      // find src (create if doesn't exist)
      auto pd = nameDataMap_.find(destStr);

      if (pd == nameDataMap_.end()) {
        pd = nameDataMap_.insert(pd, NameDataMap::value_type(destStr, CQChartsChordData()));

        (*pd).second.setFrom(nameDataMap_.size() - 1);
        (*pd).second.setName(destStr);
        (*pd).second.setInd (linkInd1);
      }

      // create link from src to dest for value
      (*ps).second.addValue((*pd).second.from(), value);

      //---

      // set group if specified
      if (plot_->groupColumn().isValid()) {
        //int igroup = plot_->groupColumn().column();

        bool ok;

        QVariant groupVar = plot_->modelValue(data.row, plot_->groupColumn(), data.parent, ok);

        QString groupStr;

        CQChartsVariant::toString(groupVar, groupStr);

        int ig = groupValues_.iset(groupVar);
        int ng = groupValues_.numUnique();

        (*ps).second.setGroup(CQChartsChordData::Group(groupStr, ig, ng));
      }

      return State::OK;
    }

    const NameDataMap &nameDataMap() const { return nameDataMap_; }

   private:
    const CQChartsChordPlot* plot_ { nullptr };
    CQChartsValueSet&        groupValues_;
    NameDataMap              nameDataMap_;
  };

  RowVisitor visitor(this, groupValues);

  visitModel(visitor);

  //---

  const NameDataMap &nameDataMap = visitor.nameDataMap();

  using Datas = std::vector<CQChartsChordData>;

  Datas datas;

  double total = 0.0;

  for (const auto &nameData : nameDataMap) {
    const CQChartsChordData &data = nameData.second;

    datas.push_back(data);

    total += data.total();
  }

  int nv = datas.size();

  //---

  if (isSorted()) {
    std::sort(datas.begin(), datas.end(),
      [](const CQChartsChordData &lhs, const CQChartsChordData &rhs) {
        if (lhs.group().value() != rhs.group().value())
          return lhs.group().value() < rhs.group().value();

        return lhs.total() < rhs.total();
      });

    for (auto &data : datas)
      data.sort();
  }
  else {
    std::sort(datas.begin(), datas.end(),
      [](const CQChartsChordData &lhs, const CQChartsChordData &rhs) {
        if (lhs.group().value() != rhs.group().value())
          return lhs.group().value() < rhs.group().value();

        return lhs.from() < rhs.from();
      });

    for (auto &data : datas)
      data.sort();
  }

  //---

  // 360 degree circle, minus gap angle degrees per set
  double gap = this->gapAngle();

  double drange = 360 - nv*gap;

  while (drange <= 0) {
    gap /= 2.0;

    drange = 360 - nv*gap;
  }

  // divide remaining degrees by total to get value->degrees factor
  CQChartsChordPlot *th = const_cast<CQChartsChordPlot *>(this);

  th->valueToDegrees_ = drange/total;

  //---

  double angle1 = this->startAngle();

  for (int row = 0; row < nv; ++row) {
    CQChartsChordData &data = datas[row];

    double total1 = data.total();

    double dangle = -valueToDegrees(total1);

    double angle2 = angle1 + dangle;

    data.setAngles(angle1, dangle);

    angle1 = angle2 - gap;
  }

  //---

  for (int row = 0; row < nv; ++row) {
    CQChartsChordData &data = datas[row];

    CQChartsGeom::BBox rect(-1, -1, 1, 1);

    ColorInd ig(data.group().i, data.group().n);
    ColorInd iv(row, nv);

    CQChartsChordObj *obj = new CQChartsChordObj(this, rect, data, ig, iv);

    objs.push_back(obj);
  }

  //---

  return true;
}

void
CQChartsChordPlot::
postResize()
{
  CQChartsPlot::postResize();

  resetDataRange(/*updateRange*/true, /*updateObjs*/false);
}

//---

void
CQChartsChordPlot::
write(std::ostream &os, const QString &varName, const QString &modelName) const
{
  CQChartsPlot::write(os, varName, modelName);

  textBox_->write(os, varName);
}

//------

CQChartsChordObj::
CQChartsChordObj(const CQChartsChordPlot *plot, const CQChartsGeom::BBox &rect,
                 const CQChartsChordData &data, const ColorInd &ig, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsChordPlot *>(plot), rect, ColorInd(), ig, iv),
 plot_(plot), data_(data)
{
  setDetailHint(DetailHint::MAJOR);

  setModelInd(data.ind());
}

QString
CQChartsChordObj::
calcId() const
{
  if (data_.group().str != "")
    return QString("%1:%2:%3:%4").arg(typeName()).arg(data_.name()).
             arg(data_.group().str).arg(iv_.i);
  else
    return QString("%1:%2:%3").arg(typeName()).arg(data_.name()).arg(iv_.i);
}

QString
CQChartsChordObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Name", data_.name());

  if (data_.group().str != "")
    tableTip.addTableRow("Group", data_.group().str);

  tableTip.addTableRow("Total", data_.total());

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

bool
CQChartsChordObj::
inside(const CQChartsGeom::Point &p) const
{
  double r = std::hypot(p.x, p.y);

  double ro = 1.0;
  double ri = std::min(std::max(plot_->innerRadius(), 0.01), 1.0);

  if (r < ri || r > ro)
    return false;

  //---

  // check angle
  double a = CMathUtil::Rad2Deg(atan2(p.y, p.x));
  a = CMathUtil::normalizeAngle(a);

  double a1 = CMathUtil::normalizeAngle(data_.angle());
  double a2 = CMathUtil::normalizeAngle(a1 + data_.dangle());

  if (a1 < a2) {
    // crosses zero
    if (a >= 0 && a <= a1)
      return true;

    if (a <= 360 && a >= a2)
      return true;
  }
  else {
    if (a >= a2 && a <= a1)
      return true;
  }

  return false;
}

void
CQChartsChordObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->linkColumn ());
  addColumnSelectIndex(inds, plot_->groupColumn());
}

void
CQChartsChordObj::
draw(CQChartsPaintDevice *device)
{
  // calc inner outer arc rectangles
  double ro = 1.0;
  double ri = std::min(std::max(plot_->innerRadius(), 0.01), 1.0);

  CQChartsGeom::Point po1 = plot_->windowToPixel(CQChartsGeom::Point(-ro, -ro));
  CQChartsGeom::Point po2 = plot_->windowToPixel(CQChartsGeom::Point( ro,  ro));
  CQChartsGeom::Point pi1 = plot_->windowToPixel(CQChartsGeom::Point(-ri, -ri));
  CQChartsGeom::Point pi2 = plot_->windowToPixel(CQChartsGeom::Point( ri,  ri));

  CQChartsGeom::Point c  = CQChartsGeom::Point(0, 0);
//CQChartsGeom::Point pc = plot_->windowToPixel(c);

  QRectF porect = QRectF(po1.qpoint(), po2.qpoint()).normalized();
  QRectF pirect = QRectF(pi1.qpoint(), pi2.qpoint()).normalized();

  QRectF irect = device->pixelToWindow(pirect);
  QRectF orect = device->pixelToWindow(porect);

  //---

  double angle1 = data_.angle();
  double dangle = data_.dangle();
  double angle2 = angle1 + dangle;

  // create value set segment arc path
  QPainterPath path;

  path.arcMoveTo(device->pixelToWindow(porect), -angle1);

  path.arcTo(orect, -angle1, -dangle);
  path.arcTo(irect, -angle2,  dangle);

  path.closeSubpath();

  //---

  // set pen and brush
  // TODO: separate segment stroke/fill control
  QPen pen;

  QColor segmentStrokeColor = plot_->interpStrokeColor(ColorInd());

  plot_->setPen(pen, true, segmentStrokeColor, plot_->strokeAlpha(),
                plot_->strokeWidth(), plot_->strokeDash());

  double gval = data_.group().value();

  ColorInd colorInd = calcColorInd();

  QColor fromColor;

  if (plot_->colorType() == CQChartsPlot::ColorType::AUTO) {
    if (gval >= 0.0)
      fromColor = plot_->blendGroupPaletteColor(gval, iv_.value(), 0.1);
    else
      fromColor = plot_->interpPaletteColor(colorInd);
  }
  else
    fromColor = plot_->interpPaletteColor(colorInd);

  double fromAlpha = 1.0;

  if (! isInside() && ! isSelected())
    fromAlpha = plot_->segmentAlpha();

  QBrush brush;

  plot_->setBrush(brush, true, fromColor, fromAlpha, CQChartsFillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  device->setPen  (pen);
  device->setBrush(brush);

  //---

  // draw path
  device->drawPath(path);

  //---

  // draw arcs between value sets

  QColor arcStrokeColor = plot_->interpStrokeColor(ColorInd());

  int from = data_.from();

  for (const auto &value : data_.values()) {
    // get arc angles
    double a1, da1;

    valueAngles(value.to, a1, da1);

    if (CMathUtil::isZero(da1))
      continue;

    CQChartsChordObj *toObj = dynamic_cast<CQChartsChordObj *>(plotObject(value.to));

    double a2, da2;

    toObj->valueAngles(from, a2, da2);

    //if (CMathUtil::isZero(da2))
    //  continue;

    double a11 = a1 + da1;
    double a21 = a2 + da2;

    //---

    // create path
    QPainterPath path;

    path.arcMoveTo(irect, -a1 );   QPointF p1 = path.currentPosition();
    path.arcMoveTo(irect, -a11);   QPointF p2 = path.currentPosition();
    path.arcMoveTo(irect, -a2 ); //QPointF p3 = path.currentPosition();
    path.arcMoveTo(irect, -a21);   QPointF p4 = path.currentPosition();

    //--

    if (from != value.to) {
      path.moveTo(p1);
      path.quadTo(c.qpoint(), p4);
      path.arcTo (irect, -a21, da2);
      path.quadTo(c.qpoint(), p2);
      path.arcTo (irect, -a11, da1);

      path.closeSubpath();
    }
    else {
      path.moveTo(p1);
      path.quadTo(c.qpoint(), p2);
      path.arcTo (irect, -a11, da1);

      path.closeSubpath();
    }

    //---

    // set pen and brush
    // TODO: separate arc stroke/fill control
    QPen pen;

    plot_->setPen(pen, true, arcStrokeColor, plot_->strokeAlpha(),
                  plot_->strokeWidth(), plot_->strokeDash());

    ColorInd toColorInd = toObj->calcColorInd();

    QColor toColor = plot_->interpPaletteColor(toColorInd);

    QColor c = CQChartsUtil::blendColors(fromColor, toColor, 0.5);

    double alpha = 1.0;

    if (! isInside() && ! isSelected())
      alpha = plot_->arcAlpha();

    QBrush brush;

    plot_->setBrush(brush, true, c, alpha, CQChartsFillPattern());

    device->setPen  (pen);
    device->setBrush(brush);

    //---

    // draw path
    device->drawPath(path);
  }
}

void
CQChartsChordObj::
drawFg(CQChartsPaintDevice *device) const
{
  if (data_.name() == "")
    return;

  if (! plot_->textBox()->isTextVisible())
    return;

  // get total (skip if zero)
  double total = data_.total();

  if (CMathUtil::isZero(total))
    return;

  // calc label radius
  double ro = 1.0;
  double ri = std::min(std::max(plot_->innerRadius(), 0.01), 1.0);

  double lr = plot_->labelRadius();

  double lr1 = ri + lr*(ro - ri);

  lr1 = std::max(lr1, 0.01);

  // calc label angle
  double angle1 = data_.angle();
  double dangle = data_.dangle();
  double angle2 = angle1 + dangle;

  double ta = CMathUtil::avg(angle1, angle2);

  //---

  // set connection line pen
  // TODO: separate text and line pen control
  ColorInd colorInd = calcColorInd();

  QPen lpen;

  QColor bg = plot_->interpPaletteColor(colorInd);

  plot_->setPen(lpen, true, bg, 1.0);

  //---

  // draw text using line pen
  QPointF center(0, 0);

  plot_->textBox()->drawConnectedRadialText(device, center, ro, lr1, ta, data_.name(),
                                            lpen, /*isRotated*/false);
}

CQChartsGeom::BBox
CQChartsChordObj::
textBBox() const
{
  double ro = 1.0;
  double ri = std::min(std::max(plot_->innerRadius(), 0.01), 1.0);

  //---

  double angle1 = data_.angle();
  double dangle = data_.dangle();
  double angle2 = angle1 + dangle;

  //---

  if (! plot_->textBox()->isTextVisible())
    return CQChartsGeom::BBox();

  double total = data_.total();

  if (CMathUtil::isZero(total))
    return CQChartsGeom::BBox();

  //---

  // calc box of text on arc center line
  double lr = plot_->labelRadius();

  double ta = CMathUtil::avg(angle1, angle2);

  QPointF center(0, 0);

  double lr1 = ri + lr*(ro - ri);

  lr1 = std::max(lr1, 0.01);

  CQChartsGeom::BBox tbbox;

  plot_->textBox()->calcConnectedRadialTextBBox(center, ro, lr1, ta, data_.name(),
                                                /*isRotated*/false, tbbox);

  return tbbox;
}

CQChartsChordObj *
CQChartsChordObj::
plotObject(int ind) const
{
  for (int i = 0; i < plot_->numPlotObjects(); ++i) {
    CQChartsChordObj *obj = dynamic_cast<CQChartsChordObj *>(plot_->plotObject(i));

    if (obj->data().from() == ind)
      return obj;
  }

  return nullptr;
}

void
CQChartsChordObj::
valueAngles(int ind, double &a, double &da) const
{
  a = data_.angle();

  for (const auto &value : data_.values()) {
    da = -plot_->valueToDegrees(value.value);

    if (ind == value.to)
      return;

    a += da;
  }

  da = 0.0;
}
