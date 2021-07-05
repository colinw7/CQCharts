#include <CQChartsFile.h>
#include <CQCharts.h>

CQChartsFile::
CQChartsFile(CQCharts *charts, const QString &name) :
 charts_(charts), name_(name)
{
}

bool
CQChartsFile::
isValid() const
{
  return (charts_ && name_ != "");
}

QString
CQChartsFile::
resolve() const
{
  if (! charts_)
    return name_;

  return charts_->lookupFile(name_);
}
