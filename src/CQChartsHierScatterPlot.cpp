#include <CQChartsHierScatterPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>

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
  addColumnParameter ("x"    , "X"    , "xColumn"       , 0).setRequired();
  addColumnParameter ("y"    , "Y"    , "yColumn"       , 1).setRequired();
  addColumnParameter ("name" , "Name" , "nameColumn"    );
  addColumnsParameter("group", "Group", "groupColumnStr");

  addBoolParameter("textLabels", "Text Labels", "textLabels");

  CQChartsPlotType::addParameters();
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
 CQChartsPlot(view, view->charts()->plotType("hierscatter"), model), dataLabel_(this)
{
  symbolData_.size = 4;

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
setGroupColumnStr(const QString &s)
{
  if (s != groupColumnStr_) {
    groupColumnStr_ = s;

    initGroupValueSets();

    updateRangeAndObjs();
  }
}

//------

void
CQChartsHierScatterPlot::
popCurrentGroup()
{
  if (! currentGroup_)
    return;

  CQChartsHierScatterPointGroup *group = currentGroup_->parent();

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

  //CQChartsPlot::updateObjs();
}

//------

void
CQChartsHierScatterPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "xColumn"    , "x"    );
  addProperty("columns", this, "yColumn"    , "y"    );
  addProperty("columns", this, "nameColumn" , "name" );
  addProperty("columns", this, "groupColumn", "group");

  addProperty("symbol", this, "symbolBorderColor", "borderColor");
  addProperty("symbol", this, "symbolSize"       , "size"       );

  addProperty("font", this, "fontSize", "font");

  dataLabel_.addProperties("dataLabel");
}

void
CQChartsHierScatterPlot::
updateRange(bool apply)
{
  QAbstractItemModel *model = this->model().data();

  if (! model)
    return;

  //---

  // calc data range (x, y values)
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsHierScatterPlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *model, const QModelIndex &parent, int row) override {
      if (! plot_->acceptsRow(row, parent))
        return State::SKIP;

      bool ok1, ok2;

      double x = plot_->modelReal(model, row, plot_->xColumn(), parent, ok1);
      double y = plot_->modelReal(model, row, plot_->yColumn(), parent, ok2);

      if (! ok1) x = row;
      if (! ok2) y = row;

      if (CQChartsUtil::isNaN(x) || CQChartsUtil::isNaN(y))
        return State::SKIP;

      range_.updateRange(x, y);

      return State::OK;
    }

    const CQChartsGeom::Range &range() const { return range_; }

   private:
    CQChartsHierScatterPlot *plot_ { nullptr };
    CQChartsGeom::Range      range_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  dataRange_ = visitor.range();

  //---

  if (CQChartsUtil::isZero(dataRange_.xsize())) {
    double x = dataRange_.xmid();
    double y = dataRange_.ymid();

    dataRange_.updateRange(x - 1, y);
    dataRange_.updateRange(x + 1, y);
  }

  if (CQChartsUtil::isZero(dataRange_.ysize())) {
    double x = dataRange_.xmid();
    double y = dataRange_.ymid();

    dataRange_.updateRange(x, y - 1);
    dataRange_.updateRange(x, y + 1);
  }

  //---

  xAxis_->setColumn(xColumn());
  yAxis_->setColumn(yColumn());

  bool ok;

  QString xname = modelHeaderString(model, xColumn(), Qt::Horizontal, Qt::DisplayRole, ok);
  QString yname = modelHeaderString(model, yColumn(), Qt::Horizontal, Qt::DisplayRole, ok);

  xAxis_->setLabel(xname);
  yAxis_->setLabel(yname);

  //---

  if (apply)
    applyDataRange();
}

int
CQChartsHierScatterPlot::
acceptsRow(int row, const QModelIndex &parent) const
{
  QAbstractItemModel *model = this->model().data();
  assert(model);

  int depth = filterNames_.size();

  for (int i = 0; i < depth; ++i) {
    const CQChartsColumn &column = groupValues_[i];

    bool ok;

    QString name = modelString(model, row, column, parent, ok);

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
  groupValues_.clear();

  (void) CQChartsUtil::fromString(groupColumnStr(), groupValues_);

  for (const auto &groupValueSet : groupValueSets_)
    delete groupValueSet.second;

  groupValueSets_.clear();

  if (groupValues_.empty())
    return;

  //---

  QAbstractItemModel *model = this->model().data();

  if (! model)
    return;

  //---

  for (const auto &groupColumn : groupValues_)
    groupValueSets_[groupColumn] = new CQChartsValueSet(this);

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsHierScatterPlot *plot) :
     plot_(plot) {
    }

    State visit(QAbstractItemModel *, const QModelIndex &parent, int row) override {
      plot_->addRowGroupValueSets(parent, row);

      return State::OK;
    }

   private:
    CQChartsHierScatterPlot *plot_ { nullptr };
  };

  RowVisitor visitor(this);

  visitModel(visitor);
}

void
CQChartsHierScatterPlot::
addRowGroupValueSets(const QModelIndex &parent, int row)
{
  QAbstractItemModel *model = this->model().data();
  assert(model);

  for (const auto &groupValueSet : groupValueSets_) {
    CQChartsColumn    groupColumn = groupValueSet.first;
    CQChartsValueSet *valueSet    = groupValueSet.second;

    bool ok;

    QVariant value = modelValue(model, row, groupColumn, parent, ok);

    if (! ok)
      continue;

    valueSet->addValue(value);
  }
}

//---

void
CQChartsHierScatterPlot::
updateObjs()
{
  delete rootGroup_;

  rootGroup_    = nullptr;
  currentGroup_ = nullptr;

  CQChartsPlot::updateObjs();
}

bool
CQChartsHierScatterPlot::
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

  // init name values
  if (! rootGroup_) {
    rootGroup_ = new CQChartsHierScatterPointGroup(nullptr, 0);

    currentGroup_ = rootGroup_;

    //---

    QAbstractItemModel *model = this->model().data();

    if (! model)
      return false;

    class RowVisitor : public ModelVisitor {
     public:
      RowVisitor(CQChartsHierScatterPlot *plot) :
       plot_(plot) {
      }

      State visit(QAbstractItemModel *model, const QModelIndex &parent, int row) override {
        if (! plot_->acceptsRow(row, parent))
          return State::SKIP;

        //---

        // get x, y value
        bool ok1, ok2;

        double x = plot_->modelReal(model, row, plot_->xColumn(), parent, ok1);
        double y = plot_->modelReal(model, row, plot_->yColumn(), parent, ok2);

        if (! ok1) x = row;
        if (! ok2) y = row;

        if (CQChartsUtil::isNaN(x) || CQChartsUtil::isNaN(y))
          return State::SKIP;

        //---

        // get optional name
        bool ok;

        QString name = plot_->modelString(model, row, plot_->nameColumn(), parent, ok);

        //---

        plot_->addGroupPoint(parent, row, x, y, name);

        return State::OK;
      }

     private:
      CQChartsHierScatterPlot *plot_ { nullptr };
    };

    RowVisitor visitor(this);

    visitModel(visitor);
  }

  //---

  QAbstractItemModel *model = this->model().data();

  //---

  if (model) {
    bool ok;

    xname_ = modelHeaderString(model, xColumn(), Qt::Horizontal, Qt::DisplayRole, ok);
    yname_ = modelHeaderString(model, yColumn(), Qt::Horizontal, Qt::DisplayRole, ok);
  }
  else {
    xname_ = "";
    yname_ = "";
  }

  if (! xname_.length()) xname_ = "x";
  if (! yname_.length()) yname_ = "y";

  //---

  for (const auto &name : filterNames_) {
    CQChartsHierScatterPointGroup *group1 = currentGroup_->lookupGroup(name);

    if (! group1)
      break;

    currentGroup_ = group1;
  }

  //---

  for (auto &igroup : currentGroup_->groups()) {
    CQChartsHierScatterPointGroup *group = igroup.second;

    addGroupPoints(group, group);
  }

  //---

  resetKeyItems();

  //---

  return true;
}

void
CQChartsHierScatterPlot::
addGroupPoint(const QModelIndex &parent, int row, double x, double y, const QString &name)
{
  // get point groups
  struct GroupData {
    CQChartsColumn    column;
    CQChartsValueSet* valueSet { nullptr };
    QString           str;
    int               ind      { -1 };
  };

  QAbstractItemModel *model = this->model().data();
  assert(model);

  //---

  std::vector<GroupData> groupDatas;

  for (const auto &groupValue : groupValues_) {
    GroupData groupData;

    groupData.column   = groupValue;
    groupData.valueSet = groupValueSets_[groupData.column];

    bool ok3;

    groupData.str = modelString(model, row, groupData.column, parent, ok3);

    groupData.ind = groupData.valueSet->sind(groupData.str);

    groupDatas.push_back(groupData);
  }

  //---

  // get parent group
  CQChartsHierScatterPointGroup *group = currentGroup_;

  for (const auto &groupData : groupDatas) {
    CQChartsHierScatterPointGroup *group1 = group->lookupGroup(groupData.ind);

    if (! group1) {
      group1 = group->addGroup(groupData.ind, groupData.str);
    }

    group = group1;
  }

  //---

  QModelIndex xInd  = model->index(row, xColumn().column(), parent);
  QModelIndex xInd1 = normalizeIndex(xInd);

  CQChartsHierScatterPoint point(group, x, y, name, row, xInd1);

  group->addPoint(point);
}

void
CQChartsHierScatterPlot::
addGroupPoints(CQChartsHierScatterPointGroup *baseGroup, CQChartsHierScatterPointGroup *group)
{
  for (auto &igroup : group->groups()) {
    CQChartsHierScatterPointGroup *group1 = igroup.second;

    addGroupPoints(baseGroup, group1);
  }

  //---

  double symbolSize = this->symbolSize();

  double sw = pixelToWindowWidth (symbolSize);
  double sh = pixelToWindowHeight(symbolSize);

  //---

  int n = baseGroup->parentCount();
  int i = baseGroup->i();

  for (const auto &point : group->points()) {
    const QPointF &p = point.p;

    CQChartsGeom::BBox bbox(p.x() - sw, p.y() - sh, p.x() + sw, p.y() + sh);

    CQChartsHierScatterPointObj *pointObj =
      new CQChartsHierScatterPointObj(this, bbox, p, i, n);

    //---

    pointObj->setName (point.name);
    pointObj->setInd  (point.ind);
    pointObj->setGroup(point.group);

    addPlotObject(pointObj);
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
    CQChartsKeyText             *text  = new CQChartsKeyText            (this, name);

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

  popAction  ->setEnabled(currentGroup_ != rootGroup_);
  resetAction->setEnabled(currentGroup_ != rootGroup_);

  menu->addSeparator();

  menu->addAction(popAction  );
  menu->addAction(resetAction);

  return true;
}

//------

void
CQChartsHierScatterPlot::
draw(QPainter *painter)
{
  initPlotObjs();

  //---

  drawParts(painter);
}

//------

CQChartsHierScatterPointObj::
CQChartsHierScatterPointObj(CQChartsHierScatterPlot *plot, const CQChartsGeom::BBox &rect,
                            const QPointF &p, int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), p_(p), i_(i), n_(n)
{
}

QString
CQChartsHierScatterPointObj::
calcId() const
{
  QString id = name_;

  id += QString(" %1=%2").arg(plot_->xname()).arg(p_.x());
  id += QString(" %1=%2").arg(plot_->yname()).arg(p_.y());

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
  double s = plot_->symbolSize(); // TODO: ensure not a crazy number

  double px, py;

  plot_->windowToPixel(p_.x(), p_.y(), px, py);

  CQChartsGeom::BBox pbbox(px - s, py - s, px + s, py + s);

  CQChartsGeom::Point pp;

  plot_->windowToPixel(p, pp);

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
draw(QPainter *painter, const CQChartsPlot::Layer &)
{
  double s = plot_->symbolSize(); // TODO: ensure not a crazy number

  QColor fillColor   = plot_->interpPaletteColor(i_, n_);
  QColor strokeColor = plot_->interpPaletteColor(i_, n_);

  QBrush brush(fillColor);
  QPen   pen  (strokeColor);

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  double px, py;

  plot_->windowToPixel(p_.x(), p_.y(), px, py);

  painter->setPen  (pen);
  painter->setBrush(brush);

  QRectF erect(px - s, py - s, 2*s, 2*s);

  painter->drawEllipse(erect);

  if (plot_->isTextLabels()) {
    CQChartsDataLabel &dataLabel = plot_->dataLabel();

    dataLabel.draw(painter, erect, name_);
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
selectPress(const CQChartsGeom::Point &)
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
  //  c = CQChartsUtil::blendColors(c, key_->interpBgColor(), 0.5);

  return c;
}
