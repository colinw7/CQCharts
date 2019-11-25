#ifndef CQChartsAnalyzeModelData_H
#define CQChartsAnalyzeModelData_H

#include <CQChartsColumn.h>
#include <QString>
#include <map>

struct CQChartsAnalyzeModelData {
  using ParameterNameColumn  = std::map<QString,CQChartsColumn>;
  using ParameterNameColumns = std::map<QString,CQChartsColumns>;
  using ParameterNameBool    = std::map<QString,bool>;

  ParameterNameColumn  parameterNameColumn;
  ParameterNameColumns parameterNameColumns;
  ParameterNameBool    parameterNameBool;
};

#endif
