#include <CQChartsChordPlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>
#include <CQChartsValueSet.h>
#include <QPainter>

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

  addColumnParameter("name" , "Name" , "nameColumn" );
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
  return "<h2>Summary</h2>\n"
         "<p>Draw connections using radial plot with sized path arcs.</p>\n";
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
 CQChartsPlotStrokeData<CQChartsChordPlot>(this)
{
  textBox_ = new CQChartsRotatedTextBoxObj(this);

  setBorderAlpha(0.3);

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
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
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
  CQChartsUtil::testAndSet(innerRadius_, r, [&]() { invalidateLayers(); } );
}

void
CQChartsChordPlot::
setLabelRadius(double r)
{
  CQChartsUtil::testAndSet(labelRadius_, r, [&]() { invalidateLayers(); } );
}

//---

void
CQChartsChordPlot::
setSegmentAlpha(double r)
{
  CQChartsUtil::testAndSet(segmentAlpha_, r, [&]() { invalidateLayers(); } );
}

void
CQChartsChordPlot::
setArcAlpha(double r)
{
  CQChartsUtil::testAndSet(arcAlpha_, r, [&]() { invalidateLayers(); } );
}

//---

void
CQChartsChordPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "nameColumn" , "name" );
  addProperty("columns", this, "valueColumn", "value");
  addProperty("columns", this, "groupColumn", "group");

  addProperty("options", this, "sorted"     );
  addProperty("options", this, "innerRadius");
  addProperty("options", this, "labelRadius");

  addLineProperties("stroke", "border");

  addProperty("segment", this, "segmentAlpha", "alpha");

  addProperty("arc", this, "arcAlpha", "alpha");

  addProperty("label", textBox_, "textVisible");
  addProperty("label", textBox_, "textFont"   );
  addProperty("label", textBox_, "textColor"  );

  QString labelBoxPath = id() + "/label/box";

  textBox_->CQChartsBoxObj::addProperties(propertyModel(), labelBoxPath);
}

void
CQChartsChordPlot::
calcRange()
{
  double r = 1.0;

  r = std::max(r, labelRadius());

  dataRange_.reset();

  dataRange_.updateRange(-r, -r);
  dataRange_.updateRange( r,  r);

  if (isEqualScale()) {
    double aspect = this->aspect();

    dataRange_.equalScale(aspect);
  }
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

  if (valueColumn().isValid())
    return initHierObjs();

  return initTableObjs();
}

bool
CQChartsChordPlot::
initTableObjs()
{
  using RowData = std::vector<QVariant>;

  struct IndRowData {
    QModelIndex ind;
    RowData     rowData;
  };

  using IndRowDatas = std::vector<IndRowData>;

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsChordPlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *, const VisitData &data) override {
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
    CQChartsChordPlot *plot_ { nullptr };
    IndRowDatas        indRowDatas_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  //---

  const IndRowDatas &indRowDatas = visitor.indRowDatas();

  int nr = indRowDatas.size   ();
  int nc = (nr > 0 ? indRowDatas[0].rowData.size() : 0);

  int numExtraColumns = 0;

  if (nameColumn ().isValid()) ++numExtraColumns;
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

    if (nameColumn().isValid() && nameColumn().column() < nv) {
      QModelIndex nameInd  = modelIndex(ind.row(), nameColumn(), ind.parent());
      QModelIndex nameInd1 = normalizeIndex(nameInd);

      QVariant var = indRowDatas[row].rowData[nameColumn().column()];

      QString name;

      CQChartsVariant::toString(var, name);

      data.setName(name);

      data.setInd(nameInd1);
    }

    //---

    if (groupColumn().isValid() && groupColumn().column() < nv) {
      QModelIndex groupInd  = modelIndex(ind.row(), groupColumn(), ind.parent());
      QModelIndex groupInd1 = normalizeIndex(groupInd);

      QVariant var = indRowDatas[row].rowData[groupColumn().column()];

      QString group;

      CQChartsVariant::toString(var, group);

      data.setGroup(CQChartsChordData::Group(group, groupValues.imap(row)));

      data.setInd(groupInd1);
    }

    //---

    int col1 = 0;

    for (int col = 0; col < nv; ++col) {
      if (col == nameColumn() || col == groupColumn())
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

    double total1 = data.total();

    if (! CMathUtil::isZero(total1))
      ++nv1;

    total += total1;
  }

  //---

  if (isSorted()) {
    std::sort(datas.begin(), datas.end(),
      [](const CQChartsChordData &lhs, const CQChartsChordData &rhs) {
        return lhs.total() < rhs.total();
      });

    for (auto &data : datas)
      data.sort();
  }

  //---

  // 360 degree circle, minus 2 degree (gap) per set
  double drange = 360 - nv1*2;

  // divide remaining degrees by total to get value->degrees factor
  valueToDegrees_ = drange/total;

  //---

  double angle1 = 90.0;

  for (int row = 0; row < nv; ++row) {
    CQChartsChordData &data = datas[row];

    double total1 = data.total();

    if (CMathUtil::isZero(total1))
      continue;

    double dangle = -valueToDegrees(total1);

    double angle2 = angle1 + dangle;

    data.setAngles(angle1, dangle);

    angle1 = angle2 - 2;
  }

  //---

  for (int row = 0; row < nv; ++row) {
    CQChartsChordData &data = datas[row];

    CQChartsGeom::BBox rect(-1, -1, 1, 1);

    CQChartsChordObj *obj = new CQChartsChordObj(this, rect, data, row, nv);

    addPlotObject(obj);
  }

  //---

  return true;
}

bool
CQChartsChordPlot::
initHierObjs()
{
  CQChartsValueSet groupValues(this);

  if (groupColumn().isValid())
    addColumnValues(groupColumn(), groupValues);

  //---

  using NameDataMap = std::map<QString,CQChartsChordData>;

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsChordPlot *plot, CQChartsValueSet &groupValues) :
     plot_(plot), groupValues_(groupValues) {
    }

    State visit(QAbstractItemModel *, const VisitData &data) override {
      bool ok1, ok2;

      QString linkStr = plot_->modelString(data.row, plot_->nameColumn (), data.parent , ok1);
      double  value   = plot_->modelReal  (data.row, plot_->valueColumn(), data.parent, ok2);

      if (! ok1 || ! ok2)
        return State::SKIP;

      //---

      // decode link into src and dest
      int pos = linkStr.indexOf("/");

      if (pos == -1)
        return State::SKIP;

      QModelIndex linkInd  = plot_->modelIndex(data.row, plot_->nameColumn(), data.parent);
      QModelIndex linkInd1 = plot_->normalizeIndex(linkInd);

      QString srcStr  = linkStr.mid(0, pos ).simplified();
      QString destStr = linkStr.mid(pos + 1).simplified();

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

      // create linek from src to dest for value
      (*ps).second.addValue((*pd).second.from(), value);

      //---

      // set group if specified
      if (plot_->groupColumn().isValid()) {
        bool ok;

        QString groupStr = plot_->modelString(data.row, plot_->groupColumn(), data.parent, ok);

        (*ps).second.setGroup(CQChartsChordData::Group(groupStr, groupValues_.imap(data.row)));
      }

      return State::OK;
    }

    const NameDataMap &nameDataMap() const { return nameDataMap_; }

   private:
    CQChartsChordPlot* plot_ { nullptr };
    CQChartsValueSet&  groupValues_;
    NameDataMap        nameDataMap_;
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
        if (lhs.group().value != rhs.group().value)
          return lhs.group().value < rhs.group().value;

        return lhs.total() < rhs.total();
      });

    for (auto &data : datas)
      data.sort();
  }
  else {
    std::sort(datas.begin(), datas.end(),
      [](const CQChartsChordData &lhs, const CQChartsChordData &rhs) {
        if (lhs.group().value != rhs.group().value)
          return lhs.group().value < rhs.group().value;

        return lhs.from() < rhs.from();
      });

    for (auto &data : datas)
      data.sort();
  }

  //---

  // 360 degree circle, minus 2 degree (gap) per set
  double drange = 360 - nv*2;

  // divide remaining degrees by total to get value->degrees factor
  valueToDegrees_ = drange/total;

  //---

  double angle1 = 90.0;

  for (int row = 0; row < nv; ++row) {
    CQChartsChordData &data = datas[row];

    double total1 = data.total();

    double dangle = -valueToDegrees(total1);

    double angle2 = angle1 + dangle;

    data.setAngles(angle1, dangle);

    angle1 = angle2 - 2;
  }

  //---

  for (int row = 0; row < nv; ++row) {
    CQChartsChordData &data = datas[row];

    CQChartsGeom::BBox rect(-1, -1, 1, 1);

    CQChartsChordObj *obj = new CQChartsChordObj(this, rect, data, row, nv);

    addPlotObject(obj);
  }

  //---

  return true;
}

void
CQChartsChordPlot::
handleResize()
{
  CQChartsPlot::handleResize();

  dataRange_.reset();
}

//------

CQChartsChordObj::
CQChartsChordObj(CQChartsChordPlot *plot, const CQChartsGeom::BBox &rect,
                 const CQChartsChordData &data, int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), data_(data), i_(i), n_(n)
{
}

QString
CQChartsChordObj::
calcId() const
{
  if (data_.group().str != "")
    return QString("chord:%1:%2:%3").arg(data_.name()).arg(data_.group().str).arg(data_.total());
  else
    return QString("chord:%1:%2").arg(data_.name()).arg(data_.total());
}

bool
CQChartsChordObj::
inside(const CQChartsGeom::Point &p) const
{
  double r = std::hypot(p.x, p.y);

  double ro = 1.0;
  double ri = plot_->innerRadius();

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
  addColumnSelectIndex(inds, plot_->nameColumn ());
  addColumnSelectIndex(inds, plot_->groupColumn());
}

void
CQChartsChordObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid()) {
    const QModelIndex &ind = data_.ind();

    addSelectIndex(inds, ind.row(), column, ind.parent());
  }
}

void
CQChartsChordObj::
draw(QPainter *painter)
{
  // calc inner outer arc rectangles
  double ro = 1.0;
  double ri = plot_->innerRadius();

  if (ri < 0.0 || ri > 1.0)
    ri = 0.9;

  CQChartsGeom::Point po1, po2, pi1, pi2;

  plot_->windowToPixel(CQChartsGeom::Point(-ro, -ro), po1);
  plot_->windowToPixel(CQChartsGeom::Point( ro,  ro), po2);
  plot_->windowToPixel(CQChartsGeom::Point(-ri, -ri), pi1);
  plot_->windowToPixel(CQChartsGeom::Point( ri,  ri), pi2);

  CQChartsGeom::Point pc;

  plot_->windowToPixel(CQChartsGeom::Point(0, 0), pc);

  QRectF orect(CQChartsUtil::toQPoint(po1), CQChartsUtil::toQPoint(po2));
  QRectF irect(CQChartsUtil::toQPoint(pi1), CQChartsUtil::toQPoint(pi2));

  //---

  double angle1 = data_.angle();
  double dangle = data_.dangle();
  double angle2 = angle1 + dangle;

  // draw value set segment arc
  QPainterPath path;

  path.arcMoveTo(orect, -angle1);

  path.arcTo(orect, -angle1, -dangle);
  path.arcTo(irect, -angle2,  dangle);

  path.closeSubpath();

  //---

  QPen pen;

  QColor segmentBorderColor = plot_->interpBorderColor(0, 1);

  plot_->setPen(pen, true, segmentBorderColor, plot_->borderAlpha(),
                plot_->borderWidth(), plot_->borderDash());

  double gval = data_.group().value;

  QColor fromColor;

  if (gval >= 0.0) {
    double r = CMathUtil::norm(i(), 0, n());

    fromColor = plot_->interpGroupPaletteColor(gval, r, 0.1);
  }
  else
    fromColor = plot_->interpPaletteColor(i(), n());

  double fromAlpha = 1.0;

  if (! isInside() && ! isSelected())
    fromAlpha = plot_->segmentAlpha();

  QBrush brush;

  plot_->setBrush(brush, true, fromColor, fromAlpha, CQChartsFillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  painter->setPen  (pen);
  painter->setBrush(brush);

  painter->drawPath(path);

  //---

  // draw arcs between value sets

  QColor arcBorderColor = plot_->interpBorderColor(0, 1);

  int from = data_.from();

  for (const auto &value : data_.values()) {
    double a1, da1;

    valueAngles(value.to, a1, da1);

    if (CMathUtil::isZero(da1))
      continue;

    CQChartsChordObj *toObj = dynamic_cast<CQChartsChordObj *>(plotObject(value.to));

    QColor toColor = plot_->interpPaletteColor(toObj->i(), toObj->n());

    double a2, da2;

    toObj->valueAngles(from, a2, da2);

    //if (CMathUtil::isZero(da2))
    //  continue;

    double a11 = a1 + da1;
    double a21 = a2 + da2;

    QPainterPath path;

    path.arcMoveTo(irect, -a1 ); QPointF p1 = path.currentPosition();
    path.arcMoveTo(irect, -a11); QPointF p2 = path.currentPosition();
    path.arcMoveTo(irect, -a2 ); //QPointF p3 = path.currentPosition();
    path.arcMoveTo(irect, -a21); QPointF p4 = path.currentPosition();

    //--

    if (from != value.to) {
      path.moveTo(p1);
      path.quadTo(CQChartsUtil::toQPoint(pc), p4);
      path.arcTo (irect, -a21, da2);
      path.quadTo(CQChartsUtil::toQPoint(pc), p2);
      path.arcTo (irect, -a11, da1);

      path.closeSubpath();
    }
    else {
      path.moveTo(p1);
      path.quadTo(CQChartsUtil::toQPoint(pc), p2);
      path.arcTo (irect, -a11, da1);

      path.closeSubpath();
    }

    //--

    QPen pen;

    plot_->setPen(pen, true, arcBorderColor, plot_->borderAlpha(),
                  plot_->borderWidth(), plot_->borderDash());

    painter->setPen(pen);

    QColor c = CQChartsUtil::blendColors(fromColor, toColor, 0.5);

    double alpha = 1.0;

    if (! isInside() && ! isSelected())
      alpha = plot_->arcAlpha();

    QBrush brush;

    plot_->setBrush(brush, true, c, alpha, CQChartsFillPattern());

    painter->setPen  (pen);
    painter->setBrush(brush);

    painter->drawPath(path);
  }
}

void
CQChartsChordObj::
drawFg(QPainter *painter)
{
  if (data_.name() == "")
    return;

  if (! plot_->textBox()->isTextVisible())
    return;

  double total = data_.total();

  if (CMathUtil::isZero(total))
    return;

  double ro = 1.0;
  double ri = plot_->innerRadius();

  if (ri < 0.0 || ri > 1.0)
    ri = 0.9;

  double angle1 = data_.angle();
  double dangle = data_.dangle();
  double angle2 = angle1 + dangle;

  // draw on arc center line
  double lr = plot_->labelRadius();

  double ta = CMathUtil::avg(angle1, angle2);

  QPointF center(0, 0);

  double lr1 = ri + lr*(ro - ri);

  if (lr1 < 0.01)
    lr1 = 0.01;

  //---

  QPen lpen;

  QColor bg = plot_->interpPaletteColor(i_, n_);

  plot_->setPen(lpen, true, bg, 1.0, CQChartsLength("0px"), CQChartsLineDash());

  plot_->textBox()->drawConnectedRadialText(painter, center, ro, lr1, ta, data_.name(),
                                            lpen, /*isRotated*/false);
}

CQChartsGeom::BBox
CQChartsChordObj::
textBBox() const
{
  double ro = 1.0;
  double ri = plot_->innerRadius();

  if (ri < 0.0 || ri > 1.0)
    ri = 0.9;

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

  if (lr1 < 0.01)
    lr1 = 0.01;

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
