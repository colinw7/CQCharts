#include <CQChartsColorStops.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsColorStops, toString, fromString)

int CQChartsColorStops::metaTypeId;

void
CQChartsColorStops::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsColorStops);

  CQPropertyViewMgrInst->setUserName("CQChartsColorStops", "color_stops");
}

QString
CQChartsColorStops::
toString() const
{
  QString s;

  for (const auto &v : values_) {
    if (s.length())
      s += " ";

    s += QString("%1").arg(v);
  }

  if (units() == Units::PERCENT)
    s += " %";

  return s;
}

bool
CQChartsColorStops::
fromString(const QString &s)
{
  QStringList strs = s.split(" ", QString::SkipEmptyParts);

  Units  units = Units::ABSOLUTE;
  Values values;

  bool ok = true;

  for (int i = 0; i < strs.length(); ++i) {
    const QString &s = strs[i];

    if (s == "%")
      units = Units::PERCENT;

    bool ok1;

    double v = s.toDouble(&ok1);

    if (ok1)
      values.push_back(v);
    else
      ok = false;
  }

  units_  = units;
  values_ = values;

  return ok;
}

int
CQChartsColorStops::
cmp(const CQChartsColorStops &lhs, const CQChartsColorStops &rhs)
{
  if (lhs.units_ != rhs.units_) {
    return (lhs.units_ == Units::ABSOLUTE ? -1 : 1);
  }

  int n1 = lhs.values_.size();
  int n2 = rhs.values_.size();

  if (n1 != n2)
    return n1 - n2;

  for (int i = 0; i < n1; ++i) {
    if (lhs.values_[i] == rhs.values_[i])
      continue;

    return (lhs.values_[i] < rhs.values_[i] ? -1 : 1);
  }

  return 0;
}
