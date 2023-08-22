#include <CQChartsColorStops.h>
#include <CQChartsUtil.h>

#include <CQPropertyView.h>
#include <CQTclUtil.h>

CQUTIL_DEF_META_TYPE(CQChartsColorStops, toString, fromString)

int CQChartsColorStops::metaTypeId;

void
CQChartsColorStops::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsColorStops);

  CQPropertyViewMgrInst->setUserName("CQChartsColorStops", "color_stops");
}

//---

int
CQChartsColorStops::
ind(double v) const
{
  int n = size();

  if (n == 0)
    return -1;

  for (int i = 0; i < n; ++i) {
    if (v < values_[size_t(i)].value)
      return i;
  }

  return n;
}

double
CQChartsColorStops::
interp(double v) const
{
  uint n = size();
  if (n == 0) return 0.0;

  if (v < values_[0].value)
    return 0.0;

  for (uint i = 1; i < n; ++i) {
    if (v >= values_[i - 1].value && v < values_[i].value) {
      auto r = CMathUtil::map(v, values_[i - 1].value, values_[i].value,
                              values_[i - 1].percent, values_[i].percent);

      return r;
    }
  }

  return 1.0;
}

//---

QString
CQChartsColorStops::
toString() const
{
  QStringList strs;

  for (const auto &v : values_) {
    if (v.percent >= 0.0) {
      QStringList strs1;

      strs1 << QString::number(v.value) << QString::number(v.percent);

      strs << CQTcl::mergeList(strs1);
    }
    else
      strs << QString::number(v.value);
  }

  if (units() == Units::PERCENT)
    strs << "%";

  return CQTcl::mergeList(strs);
}

bool
CQChartsColorStops::
fromString(const QString &str)
{
  QStringList strs;

  if (! CQTcl::splitList(str, strs))
    return false;

  bool   isDiscreet = true;
  Units  units      = Units::ABSOLUTE;
  Values values;

  bool ok = true;

  for (int i = 0; i < strs.length(); ++i) {
    const auto &s = strs[i];

    QString s1 = s, s2;

    QStringList strs;

    if (CQTcl::splitList(s, strs) && strs.size() == 2) {
      s1 = strs[0];
      s2 = strs[1];
    }

    if (s1 == "%")
      units = Units::PERCENT;

    bool ok1;
    double v1 = CQChartsUtil::toReal(s1, ok1);

    if (ok1) {
      if (s2 != "") {
        bool ok2;
        double v2 = CQChartsUtil::toReal(s2, ok2);

        if (ok2) {
          values.emplace_back(v1, v2);

          isDiscreet = false;
        }
        else
          ok = false;
      }
      else
        values.emplace_back(v1, -1.0);
    }
    else
      ok = false;
  }

  isDiscreet_ = isDiscreet;
  units_      = units;
  values_     = values;

  return ok;
}

int
CQChartsColorStops::
cmp(const CQChartsColorStops &s) const
{
  if (units_ != s.units_) {
    return (units_ == Units::ABSOLUTE ? -1 : 1);
  }

  auto n1 =   values_.size();
  auto n2 = s.values_.size();

  if (n1 != n2)
    return int(n1 - n2);

  for (size_t i = 0; i < n1; ++i) {
    if (values_[i].cmp(s.values_[i]) == 0)
      continue;

    return values_[i].cmp(s.values_[i]);
  }

  return 0;
}
