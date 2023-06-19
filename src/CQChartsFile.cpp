#include <CQChartsFile.h>
#include <CQCharts.h>

#include <QFileInfo>

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
resolve(bool exists) const
{
  if (! charts_) {
    if (exists && ! QFileInfo(name_).exists())
      return "";

    return name_;
  }

  return charts_->lookupFile(name_, exists);
}
