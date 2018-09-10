#ifndef CQChartsAnalyzeModel_H
#define CQChartsAnalyzeModel_H

#include <CQChartsColumn.h>
#include <map>

class CQCharts;
class CQChartsModelData;

class CQChartsAnalyzeModel {
 public:
  using NameColumns     = std::map<QString,CQChartsColumn>;
  using TypeNameColumns = std::map<QString,NameColumns>;

 public:
  CQChartsAnalyzeModel(CQCharts *charts, CQChartsModelData *modelData);

  void analyze();

  const TypeNameColumns &typeNameColumns() { return typeNameColumns_; }

  void print() const;

 private:
  CQCharts*          charts_    { nullptr };
  CQChartsModelData* modelData_ { nullptr };
  TypeNameColumns    typeNameColumns_;
};

#endif
