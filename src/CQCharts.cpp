#include <CQCharts.h>
#include <CQChartsColumn.h>

CQCharts::
CQCharts()
{
}

void
CQCharts::
init()
{
  CQChartsColumnTypeMgrInst->addType("time", new CQChartsColumnTimeType);
}
