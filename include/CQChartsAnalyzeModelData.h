#ifndef CQChartsAnalyzeModelData_H
#define CQChartsAnalyzeModelData_H

#include <CQChartsColumn.h>
#include <QString>
#include <map>

struct CQChartsAnalyzeModelData {
  using ParameterNameColumn  = std::map<QString,CQChartsColumn>;
  using ParameterNameColumns = std::map<QString,CQChartsColumns>;

  ParameterNameColumn  parameterNameColumn;
  ParameterNameColumns parameterNameColumns;
};

#endif
