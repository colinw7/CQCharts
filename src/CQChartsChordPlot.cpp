#include <CQChartsChordPlot.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
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
  addColumnParameter("name" , "Name" , "nameColumn" , "optional");
  addColumnParameter("value", "Value", "valueColumn", "optional");
  addColumnParameter("group", "Group", "groupColumn", "optional");
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
 CQChartsPlot(view, view->charts()->plotType("chord"), model)
{
  textBox_ = new CQChartsRotatedTextBoxObj(this);

  borderColor_ = CQChartsPaletteColor(CQChartsPaletteColor::Type::THEME_VALUE, 1);

  setLayerActive(Layer::FG, true);

  addTitle();
}

CQChartsChordPlot::
~CQChartsChordPlot()
{
  delete textBox_;
}

//---

QColor
CQChartsChordPlot::
interpBorderColor(int i, int n) const
{
  return borderColor_.interpColor(this, i, n);
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

  addProperty("", this, "sorted"     );
  addProperty("", this, "innerRadius");
  addProperty("", this, "labelRadius");

  addProperty("border", this, "borderColor", "color");
  addProperty("border", this, "borderAlpha", "alpha");

  addProperty("segment", this, "segmentAlpha", "alpha");

  addProperty("arc", this, "arcAlpha", "alpha");

  addProperty("label", textBox_, "visible");
  addProperty("label", textBox_, "font"   );
  addProperty("label", textBox_, "color"  );

  QString labelBoxPath = id() + "/label/box";

  textBox_->CQChartsBoxObj::addProperties(propertyModel(), labelBoxPath);
}

void
CQChartsChordPlot::
updateRange(bool apply)
{
  double r = 1.0;

  r = std::max(r, labelRadius());

  double xr = r;
  double yr = r;

  if (isEqualScale()) {
    double aspect = this->aspect();

    if (aspect > 1.0)
      xr *= aspect;
    else
      yr *= 1.0/aspect;
  }

  dataRange_.reset();

  dataRange_.updateRange(-xr, -yr);
  dataRange_.updateRange( xr,  yr);

  if (apply)
    applyDataRange();
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

  if (valueColumn() >= 0)
    return initHierObjs();

  return initTableObjs();
}

bool
CQChartsChordPlot::
initTableObjs()
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return false;

  //---

  int num_rows = model->rowCount   ();
  int num_cols = model->columnCount();

  int numExtraColumns = 0;

  if (nameColumn_  >= 0) ++numExtraColumns;
  if (groupColumn_ >= 0) ++numExtraColumns;

  int nv = std::min(num_rows, num_cols - numExtraColumns);

  //---

  using Datas = std::vector<CQChartsChordData>;

  Datas datas;

  datas.resize(nv);

  //---

  CQChartsValueSet groupValues;

  if (groupColumn_ >= 0) {
    for (int r = 0; r < nv; ++r) {
      QModelIndex groupInd = model->index(r, groupColumn());

      bool ok;

      QVariant group = CQChartsUtil::modelValue(model, groupInd, ok);

      groupValues.addValue(group);
    }
  }

  //---

  int nv1 = 0;

  double total = 0.0;

  for (int r = 0; r < nv; ++r) {
    CQChartsChordData &data = datas[r];

    data.setFrom(r);

    //---

    if (nameColumn() >= 0) {
      QModelIndex nameInd  = model->index(r, nameColumn());
      QModelIndex nameInd1 = normalizeIndex(nameInd);

      bool ok;

      QString name = CQChartsUtil::modelString(model, nameInd, ok);

      if (ok) {
        data.setName(name);

        data.setInd(nameInd1);
      }
    }

    //---

    if (groupColumn() >= 0) {
      QModelIndex groupInd  = model->index(r, groupColumn());
      QModelIndex groupInd1 = normalizeIndex(groupInd);

      bool ok;

      QString group = CQChartsUtil::modelString(model, groupInd, ok);

      if (ok) {
        data.setGroup(CQChartsChordData::Group(group, groupValues.imap(r)));

        data.setInd(groupInd1);
      }
    }

    //---

    int c1 = 0;

    for (int c = 0; c < nv; ++c) {
      if (c == nameColumn() || c == groupColumn())
        continue;

      QModelIndex ind = model->index(r, c);

      //---

      bool ok;

      double value = CQChartsUtil::modelReal(model, ind, ok);

      //---

      if (CQChartsUtil::isZero(value))
        continue;

      data.addValue(c1, value);

      ++c1;
    }

    //---

    double total1 = data.total();

    if (! CQChartsUtil::isZero(total1))
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

  for (int r = 0; r < nv; ++r) {
    CQChartsChordData &data = datas[r];

    double total1 = data.total();

    if (CQChartsUtil::isZero(total1))
      continue;

    double dangle = -valueToDegrees(total1);

    double angle2 = angle1 + dangle;

    data.setAngles(angle1, dangle);

    angle1 = angle2 - 2;
  }

  //---

  for (int r = 0; r < nv; ++r) {
    CQChartsChordData &data = datas[r];

    CQChartsGeom::BBox rect(-1, -1, 1, 1);

    CQChartsChordObj *obj = new CQChartsChordObj(this, rect, data, r, nv);

    addPlotObject(obj);
  }

  //---

  return true;
}

bool
CQChartsChordPlot::
initHierObjs()
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return false;

  //---

  using NameDataMap = std::map<QString,CQChartsChordData>;

  NameDataMap nameDataMap;

  //---

  int nr = model->rowCount();

  CQChartsValueSet groupValues;

  if (groupColumn() >= 0) {
    for (int r = 0; r < nr; ++r) {
      QModelIndex groupInd = model->index(r, groupColumn());

      bool ok;

      QVariant group = CQChartsUtil::modelValue(model, groupInd, ok);

      groupValues.addValue(group);
    }
  }

  for (int r = 0; r < nr; ++r) {
    QModelIndex linkInd  = model->index(r, nameColumn ());
    QModelIndex valueInd = model->index(r, valueColumn());

    QModelIndex linkInd1 = normalizeIndex(linkInd);

    bool ok1, ok2;

    QString linkStr = CQChartsUtil::modelString (model, linkInd , ok1);
    double  value   = CQChartsUtil::modelReal   (model, valueInd, ok2);

    if (! ok1 || ! ok2)
      continue;

    //---

    int pos = linkStr.indexOf("/");

    if (pos == -1)
      continue;

    QString srcStr  = linkStr.mid(0, pos ).simplified();
    QString destStr = linkStr.mid(pos + 1).simplified();

    auto ps = nameDataMap.find(srcStr);

    if (ps == nameDataMap.end()) {
      ps = nameDataMap.insert(ps, NameDataMap::value_type(srcStr, CQChartsChordData()));

      (*ps).second.setFrom(nameDataMap.size() - 1);
      (*ps).second.setName(srcStr);
      (*ps).second.setInd (linkInd1);
    }

    auto pd = nameDataMap.find(destStr);

    if (pd == nameDataMap.end()) {
      pd = nameDataMap.insert(pd, NameDataMap::value_type(destStr, CQChartsChordData()));

      (*pd).second.setFrom(nameDataMap.size() - 1);
      (*pd).second.setName(destStr);
      (*pd).second.setInd (linkInd1);
    }

    (*ps).second.addValue((*pd).second.from(), value);

    //---

    if (groupColumn() >= 0) {
      QModelIndex groupInd = model->index(r, groupColumn());

      bool ok;

      QString groupStr = CQChartsUtil::modelString(model, groupInd, ok);

      (*ps).second.setGroup(CQChartsChordData::Group(groupStr, groupValues.imap(r)));
    }
  }

  //---

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

  for (int r = 0; r < nv; ++r) {
    CQChartsChordData &data = datas[r];

    double total1 = data.total();

    double dangle = -valueToDegrees(total1);

    double angle2 = angle1 + dangle;

    data.setAngles(angle1, dangle);

    angle1 = angle2 - 2;
  }

  //---

  for (int r = 0; r < nv; ++r) {
    CQChartsChordData &data = datas[r];

    CQChartsGeom::BBox rect(-1, -1, 1, 1);

    CQChartsChordObj *obj = new CQChartsChordObj(this, rect, data, r, nv);

    addPlotObject(obj);
  }

  //---

  return true;
}

void
CQChartsChordPlot::
handleResize()
{
  dataRange_.reset();

  CQChartsPlot::handleResize();
}

void
CQChartsChordPlot::
draw(QPainter *painter)
{
  initPlotObjs();

  //---

  drawParts(painter);
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
    return QString("%1:%2:%3").arg(data_.name()).arg(data_.group().str).arg(data_.total());
  else
    return QString("%1:%2").arg(data_.name()).arg(data_.total());
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
  double a = CQChartsUtil::Rad2Deg(atan2(p.y, p.x));
  a = CQChartsUtil::normalizeAngle(a);

  double a1 = CQChartsUtil::normalizeAngle(data_.angle());
  double a2 = CQChartsUtil::normalizeAngle(a1 + data_.dangle());

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
addSelectIndex()
{
  const QModelIndex &ind = data_.ind();

  plot_->addSelectIndex(ind.row(), plot_->nameColumn (), ind.parent());
  plot_->addSelectIndex(ind.row(), plot_->groupColumn(), ind.parent());
}

bool
CQChartsChordObj::
isIndex(const QModelIndex &ind) const
{
  return (ind == data_.ind());
}

void
CQChartsChordObj::
draw(QPainter *painter, const CQChartsPlot::Layer &layer)
{
  // calc inner outer arc rectangles
  CQChartsGeom::Point po1, po2, pi1, pi2;

  double ro = 1.0;
  double ri = plot_->innerRadius();

  if (ri < 0.0 || ri > 1.0)
    ri = 0.9;

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

  if (layer == CQChartsPlot::Layer::MID) {
    // draw value set segment arc
    QColor segmentBorderColor = plot_->interpBorderColor(0, 1);

    segmentBorderColor.setAlphaF(CQChartsUtil::clamp(plot_->borderAlpha(), 0, 1));

    QPainterPath path;

    path.arcMoveTo(orect, -angle1);

    path.arcTo(orect, -angle1, -dangle);
    path.arcTo(irect, -angle2,  dangle);

    path.closeSubpath();

    //---

    QPen pen(segmentBorderColor);

    double gval = data_.group().value;

    QColor fromColor;

    if (gval >= 0.0) {
      double r = CQChartsUtil::norm(i(), 0, n());

      fromColor = plot_->interpGroupPaletteColor(gval, r, 0.1);
    }
    else
      fromColor = plot_->interpPaletteColor(i(), n());

    if (! isInside() && ! isSelected())
      fromColor.setAlphaF(CQChartsUtil::clamp(plot_->segmentAlpha(), 0, 1));

    QBrush brush(fromColor);

    plot_->updateObjPenBrushState(this, pen, brush);

    //---

    painter->setPen  (pen);
    painter->setBrush(brush);

    painter->drawPath(path);

    //---

    // draw arcs between value sets

    QColor arcBorderColor = plot_->interpBorderColor(0, 1);

    arcBorderColor.setAlphaF(CQChartsUtil::clamp(plot_->borderAlpha(), 0, 1));

    int from = data_.from();

    for (const auto &value : data_.values()) {
      double a1, da1;

      valueAngles(value.to, a1, da1);

      if (CQChartsUtil::isZero(da1))
        continue;

      CQChartsChordObj *toObj = dynamic_cast<CQChartsChordObj *>(plotObject(value.to));

      QColor toColor = plot_->interpPaletteColor(toObj->i(), toObj->n());

      double a2, da2;

      toObj->valueAngles(from, a2, da2);

      //if (CQChartsUtil::isZero(da2))
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

      QPen pen(arcBorderColor);

      painter->setPen(pen);

      QColor c = CQChartsUtil::blendColors(fromColor, toColor, 0.5);

      if (! isInside() && ! isSelected())
        c.setAlphaF(CQChartsUtil::clamp(plot_->arcAlpha(), 0, 1));

      QBrush brush(c);

      painter->setPen  (pen);
      painter->setBrush(brush);

      painter->drawPath(path);

      //--

  #if 0
      path.arcMoveTo(orect, -a1 ); p3 = path.currentPosition();
      path.arcMoveTo(orect, -a11); p4 = path.currentPosition();

      painter->setPen(QColor(0,0,0));

      painter->drawLine(p1, p3);
      painter->drawLine(p2, p4);
  #endif
    }
  }

  //---

  if (layer == CQChartsPlot::Layer::FG && data_.name() != "") {
    if (! plot_->textBox()->isTextVisible())
      return;

    double total = data_.total();

    if (CQChartsUtil::isZero(total))
      return;

    // draw on arc center line
    double lr = plot_->labelRadius();

    double ta = CQChartsUtil::avg(angle1, angle2);

    QPointF center(0, 0);

    double lr1 = ri + lr*(ro - ri);

    if (lr1 < 0.01)
      lr1 = 0.01;

    QColor bg = plot_->interpPaletteColor(i_, n_);

    QPen lpen(bg);

    plot_->textBox()->drawConnectedRadialText(painter, center, ro, lr1, ta, data_.name(),
                                              lpen, /*isRotated*/false);
  }
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

  if (CQChartsUtil::isZero(total))
    return CQChartsGeom::BBox();

  //---

  // calc box of text on arc center line
  double lr = plot_->labelRadius();

  double ta = CQChartsUtil::avg(angle1, angle2);

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
