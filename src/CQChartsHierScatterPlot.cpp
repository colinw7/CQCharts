#include <CQChartsHierScatterPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsValueSet.h>
#include <CQChartsUtil.h>
#include <CQChartsDataLabel.h>
#include <CQCharts.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

#include <QPainter>
#include <QMenu>

CQChartsHierScatterPlotType::
CQChartsHierScatterPlotType()
{
}

void
CQChartsHierScatterPlotType::
addParameters()
{
  startParameterGroup("Hier Scatter");

  addColumnParameter("x", "X", "xColumn").
    setTip("X Value").setRequired().setNumeric();
  addColumnParameter("y", "Y", "yColumn").
    setTip("Y Value").setRequired().setNumeric();

  addColumnParameter("name", "Name", "nameColumn").
    setTip("Value Name").setString();

  addColumnsParameter("group", "Group", "groupColumns").setTip("Group Name(s)");

  addBoolParameter("textLabels", "Text Labels", "showTextLabels");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsHierScatterPlotType::
description() const
{
  return "<h2>Summary</h2>\n"
         "<p>Draws scatter plot x, y points with support for customization of"
         "point size, color and label font.\n";
}

CQChartsPlot *
CQChartsHierScatterPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsHierScatterPlot(view, model);
}

//---

CQChartsHierScatterPlot::
CQChartsHierScatterPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("hierscatter"), model),
 CQChartsObjPointData<CQChartsHierScatterPlot>(this)
{
  NoUpdate noUpdate(this);

  dataLabel_ = new CQChartsDataLabel(this);

  setSymbolSize(CQChartsLength("4px"));

  addAxes();

  addKey();

  addTitle();
}

CQChartsHierScatterPlot::
~CQChartsHierScatterPlot()
{
  for (const auto &groupValueSet : groupValueSets_)
    delete groupValueSet.second;

  delete rootGroup_;

  delete dataLabel_;
}

//------

void
CQChartsHierScatterPlot::
setXColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(xColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsHierScatterPlot::
setYColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(yColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsHierScatterPlot::
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsHierScatterPlot::
setGroupColumns(const CQChartsColumns &c)
{
  CQChartsUtil::testAndSet(groupColumns_, c, [&]() {
    initGroupValueSets();

    updateRangeAndObjs();
  } );
}

//---

bool
CQChartsHierScatterPlot::
isTextLabels() const
{
  return dataLabel_->isVisible();
}

void
CQChartsHierScatterPlot::
setTextLabels(bool b)
{
  if (b != isTextLabels()) { dataLabel_->setVisible(b); drawObjs(); }
}

//---

void
CQChartsHierScatterPlot::
setFontSize(double s)
{
  CQChartsUtil::testAndSet(fontSize_, s, [&]() { updateObjs(); } );
}

//---

void
CQChartsHierScatterPlot::
popCurrentGroup()
{
  if (! currentGroup())
    return;

  CQChartsHierScatterPointGroup *group = currentGroup()->parent();

  if (group)
    setCurrentGroup(group);
}

void
CQChartsHierScatterPlot::
resetCurrentGroup()
{
  setCurrentGroup(rootGroup_);
}

void
CQChartsHierScatterPlot::
setCurrentGroup(CQChartsHierScatterPointGroup *group)
{
  filterNames_.clear();

  CQChartsHierScatterPointGroup *group1 = group;

  while (group1 && group1 != rootGroup_) {
    filterNames_.push_front(group1->name());

    group1 = group1->parent();
  }

  currentGroup_ = group;

  updateRangeAndObjs();

  //updateObjs();
}

//------

void
CQChartsHierScatterPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "xColumn"     , "x"     )->setDesc("X column");
  addProperty("columns", this, "yColumn"     , "y"     )->setDesc("Y column");
  addProperty("columns", this, "nameColumn"  , "name"  )->setDesc("Name column");
  addProperty("columns", this, "groupColumns", "groups")->setDesc("Group columns");

  addSymbolProperties("symbol");

  addProperty("font", this, "fontSize", "font")->setDesc("Font size");

  // point data labels
  dataLabel_->addPathProperties("dataLabel");
}

CQChartsGeom::Range
CQChartsHierScatterPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsHierScatterPlot::calcRange");

  // calc data range (x, y values)
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsHierScatterPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      if (! plot_->acceptsRow(data.row, data.parent))
        return State::SKIP;

      bool ok1, ok2;

      double x = plot_->modelReal(data.row, plot_->xColumn(), data.parent, ok1);
      double y = plot_->modelReal(data.row, plot_->yColumn(), data.parent, ok2);

      if (! ok1) x = data.row;
      if (! ok2) y = data.row;

      if (CMathUtil::isNaN(x) || CMathUtil::isNaN(y))
        return State::SKIP;

      range_.updateRange(x, y);

      return State::OK;
    }

    const CQChartsGeom::Range &range() const { return range_; }

   private:
    const CQChartsHierScatterPlot* plot_ { nullptr };
    CQChartsGeom::Range            range_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  CQChartsGeom::Range dataRange = visitor.range();

  //---

  dataRange.makeNonZero();

  //---

  xAxis_->setColumn(xColumn());
  yAxis_->setColumn(yColumn());

  bool ok;

  QString xname = modelHeaderString(xColumn(), ok);
  QString yname = modelHeaderString(yColumn(), ok);

  xAxis_->setLabel(xname);
  yAxis_->setLabel(yname);

  //---

  return dataRange;
}

int
CQChartsHierScatterPlot::
acceptsRow(int row, const QModelIndex &parent) const
{
  int depth = filterNames_.size();

  for (int i = 0; i < depth; ++i) {
    const CQChartsColumn &column = groupValues_.getColumn(i);

    bool ok;

    QString name = modelString(row, column, parent, ok);

    if (! ok)
      return false;

    if (name != filterNames_[i])
      return false;
  }

  return true;
}

//------

void
CQChartsHierScatterPlot::
initGroupValueSets()
{
  // init and populate group value sets
  groupValues_ = groupColumns();

  for (const auto &groupValueSet : groupValueSets_)
    delete groupValueSet.second;

  groupValueSets_.clear();

  if (groupValues_.count() == 0)
    return;

  //---

  for (const auto &groupColumn : groupValues_)
    groupValueSets_[groupColumn] = new CQChartsValueSet(this);

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsHierScatterPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      plot_->addRowGroupValueSets(data);

      return State::OK;
    }

   private:
    const CQChartsHierScatterPlot* plot_ { nullptr };
  };

  RowVisitor visitor(this);

  visitModel(visitor);
}

void
CQChartsHierScatterPlot::
addRowGroupValueSets(const ModelVisitor::VisitData &data) const
{
  for (const auto &groupValueSet : groupValueSets_) {
    CQChartsColumn    groupColumn = groupValueSet.first;
    CQChartsValueSet *valueSet    = groupValueSet.second;

    bool ok;

    QVariant value = modelValue(data.row, groupColumn, data.parent, ok);

    if (! ok)
      continue;

    valueSet->addValue(value);
  }
}

//---

void
CQChartsHierScatterPlot::
clearPlotObjects()
{
  delete rootGroup_;

  rootGroup_    = nullptr;
  currentGroup_ = nullptr;

  CQChartsPlot::clearPlotObjects();
}

bool
CQChartsHierScatterPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsHierScatterPlot::createObjs");

  CQChartsHierScatterPlot *th = const_cast<CQChartsHierScatterPlot *>(this);

  NoUpdate noUpdate(th);

  //---

  // init name values
  if (! rootGroup_) {
    th->rootGroup_    = new CQChartsHierScatterPointGroup(nullptr, 0);
    th->currentGroup_ = rootGroup_;

    //---

    class RowVisitor : public ModelVisitor {
     public:
      RowVisitor(const CQChartsHierScatterPlot *plot) :
       plot_(plot) {
      }

      State visit(const QAbstractItemModel *, const VisitData &data) override {
        if (! plot_->acceptsRow(data.row, data.parent))
          return State::SKIP;

        //---

        // get x, y value
        bool ok1, ok2;

        double x = plot_->modelReal(data.row, plot_->xColumn(), data.parent, ok1);
        double y = plot_->modelReal(data.row, plot_->yColumn(), data.parent, ok2);

        if (! ok1) x = data.row;
        if (! ok2) y = data.row;

        if (CMathUtil::isNaN(x) || CMathUtil::isNaN(y))
          return State::SKIP;

        //---

        // get optional name
        bool ok;

        QString name = plot_->modelString(data.row, plot_->nameColumn(), data.parent, ok);

        //---

        plot_->addGroupPoint(data, x, y, name);

        return State::OK;
      }

     private:
      const CQChartsHierScatterPlot* plot_ { nullptr };
    };

    RowVisitor visitor(this);

    visitModel(visitor);
  }

  //---

  bool ok1, ok2;

  th->xname_ = modelHeaderString(xColumn(), ok1);
  th->yname_ = modelHeaderString(yColumn(), ok2);

  if (! xname_.length()) th->xname_ = "x";
  if (! yname_.length()) th->yname_ = "y";

  //---

  for (const auto &name : filterNames_) {
    CQChartsHierScatterPointGroup *group1 = currentGroup()->lookupGroup(name);

    if (! group1)
      break;

    th->currentGroup_ = group1;
  }

  //---

  if (! currentGroup()->groups().empty()) {
    for (auto &igroup : currentGroup()->groups()) {
      CQChartsHierScatterPointGroup *group = igroup.second;

      addGroupPoints(group, group, objs);
    }
  }
  else {
    addGroupPoints(currentGroup(), currentGroup(), objs);
  }

  //---

  return true;
}

void
CQChartsHierScatterPlot::
addGroupPoint(const ModelVisitor::VisitData &data, double x, double y, const QString &name) const
{
  // get point groups
  struct GroupData {
    CQChartsColumn    column;
    CQChartsValueSet* valueSet { nullptr };
    QString           str;
    int               ind      { -1 };
  };

  //---

  std::vector<GroupData> groupDatas;

  for (const auto &groupValue : groupValues_) {
    GroupData groupData;

    groupData.column = groupValue;

    auto pv = groupValueSets_.find(groupData.column);

    if (pv != groupValueSets_.end())
      groupData.valueSet = (*pv).second;

    bool ok3;

    groupData.str = modelString(data.row, groupData.column, data.parent, ok3);

    groupData.ind = groupData.valueSet->sind(groupData.str);

    groupDatas.push_back(groupData);
  }

  //---

  // get parent group
  CQChartsHierScatterPointGroup *group = currentGroup();

  for (const auto &groupData : groupDatas) {
    CQChartsHierScatterPointGroup *group1 = group->lookupGroup(groupData.ind);

    if (! group1) {
      group1 = group->addGroup(groupData.ind, groupData.str);
    }

    group = group1;
  }

  //---

  QModelIndex xInd  = modelIndex(data.row, xColumn(), data.parent);
  QModelIndex xInd1 = normalizeIndex(xInd);

  CQChartsHierScatterPoint point(group, x, y, name, data.row, xInd1);

  group->addPoint(point);
}

void
CQChartsHierScatterPlot::
addGroupPoints(CQChartsHierScatterPointGroup *baseGroup,
               CQChartsHierScatterPointGroup *group, PlotObjs &objs) const
{
  for (auto &igroup : group->groups()) {
    CQChartsHierScatterPointGroup *group1 = igroup.second;

    addGroupPoints(baseGroup, group1, objs);
  }

  //---

  double sx, sy;

  pixelSymbolSize(symbolSize(), sx, sy);

  //---

  int n = baseGroup->parentCount();
  int i = baseGroup->i();

  for (const auto &point : group->points()) {
    const QPointF &p = point.p;

    CQChartsGeom::BBox bbox(p.x() - sx, p.y() - sy, p.x() + sx, p.y() + sy);

    CQChartsHierScatterPointObj *pointObj =
      new CQChartsHierScatterPointObj(this, bbox, p, i, n);

    //---

    pointObj->setName (point.name);
    pointObj->setInd  (point.ind);
    pointObj->setGroup(point.group);

    objs.push_back(pointObj);
  }
}

void
CQChartsHierScatterPlot::
addKeyItems(CQChartsPlotKey *key)
{
  CQChartsHierScatterPointGroup *group = currentGroup();

  int n = group->numGroups();

  for (const auto &igroup : group->groups()) {
    CQChartsHierScatterPointGroup *group = igroup.second;

    int i = group->i();

    const QString &name = group->name();

    CQChartsHierScatterKeyColor *color = new CQChartsHierScatterKeyColor(this, group, i, n);
    CQChartsKeyText             *text  = new CQChartsKeyText            (this, name , i, n);

    key->addItem(color, i, 0);
    key->addItem(text , i, 1);
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

//------

bool
CQChartsHierScatterPlot::
addMenuItems(QMenu *menu)
{
  QAction *popAction   = new QAction("Pop Filter"  , menu);
  QAction *resetAction = new QAction("Reset Filter", menu);

  connect(popAction  , SIGNAL(triggered()), this, SLOT(popCurrentGroup()));
  connect(resetAction, SIGNAL(triggered()), this, SLOT(resetCurrentGroup()));

  popAction  ->setEnabled(currentGroup() != rootGroup_);
  resetAction->setEnabled(currentGroup() != rootGroup_);

  menu->addSeparator();

  menu->addAction(popAction  );
  menu->addAction(resetAction);

  return true;
}

//------

CQChartsHierScatterPointObj::
CQChartsHierScatterPointObj(const CQChartsHierScatterPlot *plot, const CQChartsGeom::BBox &rect,
                            const QPointF &p, int i, int n) :
 CQChartsPlotObj(const_cast<CQChartsHierScatterPlot *>(plot), rect), plot_(plot),
 p_(p), i_(i), n_(n)
{
}

QString
CQChartsHierScatterPointObj::
calcId() const
{
  QString id = QString("%1:%2:").arg(typeName()).arg(name_);

  id += QString("%1:%2").arg(plot_->xname()).arg(p_.x());
  id += QString("%1:%2").arg(plot_->yname()).arg(p_.y());

  return id;
}

QString
CQChartsHierScatterPointObj::
calcTipId() const
{
  QString tip = QString("%1:%2").arg(group()->name()).arg(name());

  return tip;
}

bool
CQChartsHierScatterPointObj::
inside(const CQChartsGeom::Point &p) const
{
  double sx, sy;

  plot_->pixelSymbolSize(plot_->symbolSize(), sx, sy);

  QPointF p1 = plot_->windowToPixel(p_);

  CQChartsGeom::BBox pbbox(p1.x() - sx, p1.y() - sy, p1.x() + sx, p1.y() + sy);

  CQChartsGeom::Point pp = plot_->windowToPixel(p);

  return pbbox.inside(pp);
}

void
CQChartsHierScatterPointObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->xColumn());
  addColumnSelectIndex(inds, plot_->yColumn());
}

void
CQChartsHierScatterPointObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid())
    addSelectIndex(inds, ind_.row(), column, ind_.parent());
}

void
CQChartsHierScatterPointObj::
draw(QPainter *painter)
{
  // get symbol size
  double sx, sy;

  plot_->pixelSymbolSize(plot_->symbolSize(), sx, sy);

  //---

  // set pen and brush
  // TODO: allow full control of symbol fill and stroke
  QPen   pen;
  QBrush brush;

  QColor fillColor   = plot_->interpPaletteColor(i_, n_);
  QColor strokeColor = plot_->interpPaletteColor(i_, n_);

  plot_->setPen  (pen  , true, strokeColor, 1.0);
  plot_->setBrush(brush, true, fillColor, 1.0);

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setPen  (pen);
  painter->setBrush(brush);

  //---

  // draw symbol
  QPointF p1 = plot_->windowToPixel(p_);

  QRectF erect(p1.x() - sx, p1.y() - sy, 2*sx, 2*sy);

  painter->drawEllipse(erect);

  //---

  // draw label
  if (plot_->isTextLabels()) {
    const CQChartsDataLabel *dataLabel = plot_->dataLabel();

    dataLabel->draw(painter, erect, name_);
  }
}

//------

CQChartsHierScatterKeyColor::
CQChartsHierScatterKeyColor(CQChartsHierScatterPlot *plot, CQChartsHierScatterPointGroup *group,
                            int i, int n) :
 CQChartsKeyColorBox(plot, i, n), group_(group)
{
}

bool
CQChartsHierScatterKeyColor::
selectPress(const CQChartsGeom::Point &, CQChartsSelMod)
{
  CQChartsHierScatterPlot *plot = qobject_cast<CQChartsHierScatterPlot *>(plot_);

  //plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  //plot->updateObjs();

  if (group_->numGroups())
    plot->setCurrentGroup(group_);

  return true;
}

QBrush
CQChartsHierScatterKeyColor::
fillBrush() const
{
  QColor c = CQChartsKeyColorBox::fillBrush().color();

  //CQChartsHierScatterPlot *plot = qobject_cast<CQChartsHierScatterPlot *>(plot_);

  //if (plot->isSetHidden(i_))
  //  c = CQChartsUtil::blendColors(c, key_->interpBgColor(), key_->hiddenAlpha());

  return c;
}
