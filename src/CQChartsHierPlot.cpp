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

//----

void
CQChartsHierPlot::
setNameColumns(const CQChartsColumns &c)
{
  CQChartsUtil::testAndSet(nameColumns_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsHierPlot::
setValueColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//----

void
CQChartsHierPlot::
setFollowViewExpand(bool b)
{
  if (followViewExpand_ != b) {
    followViewExpand_ = b;

    followViewExpandChanged();
  }
}

void
CQChartsHierPlot::
expandedModelIndices(std::set<QModelIndex> &indSet) const
{
  QModelIndexList inds;

  view()->expandedModelIndices(inds);

  for (const auto &ind : inds) {
    QModelIndex ind1 = normalizeIndex(ind);

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
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  addBaseProperties();

  addProp("columns", "nameColumns", "name" , "Name columns");
  addProp("columns", "valueColumn", "value", "Data value column");

  addProp("options", "separator", "", "Separator for hierarchical path in name column");

  addColorMapProperties();
}
