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
CQChartsModelIndex(const CQChartsPlot *plot, int row, const CQChartsColumn &column,
                   const QModelIndex &parent, bool normalized) :
 plot_(plot), row_(row), column_(column), parent_(parent), normalized_(normalized)
{
  assert(plot_->isNormalizedIndex(*this) == normalized_);
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
  strs += QString::number(row());
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

  // TODO: plot

  bool ok;

  row_ = strs[1].toInt(&ok);
  if (! ok) return false;

  CQChartsColumn c(strs[2]);
  if (! c.isValid()) return false;

  return true;
}

QString
CQChartsModelIndex::
id() const
{
  QString id;

  auto parent = parent_;

  while (parent.isValid()) {
    if (id != "")
      id += ":";

    id += QString::number(parent.row());

    parent = parent.parent();
  }

  if (id != "")
    id += ":";

  id += QString("%1:%2").arg(row()).arg(column().column());

  return id;
}
