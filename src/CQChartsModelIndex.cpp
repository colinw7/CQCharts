#include <CQChartsModelIndex.h>
#include <CQChartsPlot.h>
#include <CQTclUtil.h>

CQUTIL_DEF_META_TYPE(CQChartsModelIndex, toString, fromString)

int CQChartsModelIndex::metaTypeId;

void
CQChartsModelIndex::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsModelIndex);
}

#if 0
CQChartsModelIndex::
CQChartsModelIndex(int row, const CQChartsColumn &column, const QModelIndex &parent) :
 row_(row), column_(column), parent_(parent)
{
}
#endif

CQChartsModelIndex::
CQChartsModelIndex(CQChartsPlot *plot, int row, const CQChartsColumn &column,
                   const QModelIndex &parent) :
 plot_(plot), row_(row), column_(column), parent_(parent)
{
}

bool
CQChartsModelIndex::
isValid() const
{
  return (row_ >= 0);
}

QString
CQChartsModelIndex::
toString() const
{
  QStringList strs;

  strs += (plot_ ? plot_->id() : "");
  strs += QString("%1").arg(row());
  strs += column_.toString();

  if (parent_.isValid())
    strs += QVariant(parent_).toString();

  return CQTcl::mergeList(strs);
}

bool
CQChartsModelIndex::
fromString(const QString &str)
{
  QStringList strs;

  if (! CQTcl::splitList(str, strs))
    return false;

  if (strs.size() < 3)
    return false;

  // TODO

  return false;
}
