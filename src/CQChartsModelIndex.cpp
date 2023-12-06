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
CQChartsModelIndex(const Plot *plot, int row, const CQChartsColumn &column,
                   const QModelIndex &parent, bool normalized) :
 plot_(const_cast<Plot *>(plot)), row_(row), column_(column),
 parent_(parent), normalized_(normalized)
{
  assert(plot_->isNormalizedIndex(*this) == normalized_);
}

CQChartsModelIndex::Plot *
CQChartsModelIndex::
plot() const
{
  return plot_.data();
}

void
CQChartsModelIndex::
setPlot(const Plot *plot)
{
  plot_ = const_cast<Plot *>(plot);
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
  if (! plot_ && row_ < 0 && ! column_.isValid())
    return "";

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
  if (str.trimmed() == "") {
    *this = CQChartsModelIndex();
    return true;
  }

  QStringList strs;

  if (! CQTcl::splitList(str, strs))
    return false;

  // <plot> <row> <column>
  if (strs.size() < 3)
    return false;

  // TODO: plot

  bool ok;

  row_ = int(CQChartsUtil::toInt(strs[1], ok));
  if (! ok) return false;

  CQChartsColumn c(strs[2]);
  if (! c.isValid()) return false;

  column_ = c;

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
