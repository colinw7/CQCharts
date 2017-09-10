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
  CQChartsColumnTypeMgrInst->addType("real"   , new CQChartsColumnRealType   );
  CQChartsColumnTypeMgrInst->addType("integer", new CQChartsColumnIntegerType);
  CQChartsColumnTypeMgrInst->addType("string" , new CQChartsColumnStringType );
  CQChartsColumnTypeMgrInst->addType("time"   , new CQChartsColumnTimeType   );
}
