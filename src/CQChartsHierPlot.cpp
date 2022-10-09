#include <CQChartsHierPlot.h>

#include <CQPropertyViewItem.h>

CQChartsHierPlot::
CQChartsHierPlot(CQChartsView *view, CQChartsPlotType *type, const ModelP &model) :
 CQChartsPlot(view, type, model)
{
}

CQChartsHierPlot::
~CQChartsHierPlot()
{
}

//---

void
CQChartsHierPlot::
init()
{
  CQChartsPlot::init();
}

void
CQChartsHierPlot::
term()
{
}

//---

void
CQChartsHierPlot::
setNameColumns(const Columns &c)
{
  CQChartsUtil::testAndSet(nameColumns_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsHierPlot::
setValueColumn(const Column &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsHierPlot::
setGroupColumn(const Column &c)
{
  CQChartsUtil::testAndSet(groupColumn_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

//---

CQChartsColumn
CQChartsHierPlot::
getNamedColumn(const QString &name) const
{
  Column c;
  if      (name == "value") c = this->valueColumn();
  else if (name == "group") c = this->groupColumn();
  else                      c = CQChartsPlot::getNamedColumn(name);

  return c;
}

void
CQChartsHierPlot::
setNamedColumn(const QString &name, const Column &c)
{
  if      (name == "value") this->setValueColumn(c);
  else if (name == "group") this->setGroupColumn(c);
  else                      CQChartsPlot::setNamedColumn(name, c);
}

CQChartsColumns
CQChartsHierPlot::
getNamedColumns(const QString &name) const
{
  Columns c;
  if (name == "name") c = this->nameColumns();
  else                c = CQChartsPlot::getNamedColumns(name);

  return c;
}

void
CQChartsHierPlot::
setNamedColumns(const QString &name, const Columns &c)
{
  if (name == "name") this->setNameColumns(c);
  else                CQChartsPlot::setNamedColumns(name, c);
}

//---

void
CQChartsHierPlot::
setFollowViewExpand(bool b)
{
  CQChartsUtil::testAndSet(followViewExpand_, b, [&]() {
    followViewExpandChanged(); Q_EMIT customDataChanged();
  } );
}

bool
CQChartsHierPlot::
isExpandModelIndex(const QModelIndex &ind) const
{
  return view()->isExpandModelIndex(ind);
}

void
CQChartsHierPlot::
expandModelIndex(const QModelIndex &ind, bool b)
{
  auto ind1 = unnormalizeIndex(ind);

  view()->expandModelIndex(ind1, b);

  updateRangeAndObjs();
}

void
CQChartsHierPlot::
expandedModelIndices(std::set<QModelIndex> &indSet) const
{
  QModelIndexList inds;

  view()->expandedModelIndices(inds);

  for (const auto &ind : inds) {
    auto ind1 = normalizeIndex(ind);

    indSet.insert(ind1);
  }
}

//---

void
CQChartsHierPlot::
addProperties()
{
  addHierProperties();
}

void
CQChartsHierPlot::
addHierProperties()
{
  addBaseProperties();

  addProp("columns", "nameColumns", "name" , "Name columns");
  addProp("columns", "valueColumn", "value", "Data value column");
  addProp("columns", "groupColumn", "group", "Group column");

  addProp("options", "separator", "", "Separator for hierarchical path in name column");
}

//------

CQChartsHierPlotCustomControls::
CQChartsHierPlotCustomControls(CQCharts *charts, const QString &plotType) :
 CQChartsPlotCustomControls(charts, plotType)
{
}

void
CQChartsHierPlotCustomControls::
addHierColumnWidgets()
{
  // hier group
  auto hierFrame = createGroupFrame("Hier", "hierFrame");

  static auto columnNames = QStringList() << "name" << "value" << "group";

  addNamedColumnWidgets(columnNames, hierFrame);
}

void
CQChartsHierPlotCustomControls::
connectSlots(bool b)
{
  CQChartsPlotCustomControls::connectSlots(b);
}

void
CQChartsHierPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  if (plot_)
    disconnect(plot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));

  plot_ = dynamic_cast<CQChartsHierPlot *>(plot);

  CQChartsPlotCustomControls::setPlot(plot);

  if (plot_)
    connect(plot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));
}

void
CQChartsHierPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  CQChartsPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}
