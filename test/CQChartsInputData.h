#ifndef CQChartsInputData_H
#define CQChartsInputData_H

#include <QString>

struct CQChartsInputData {
  using Vars = std::vector<QString>;

  bool    commentHeader     { false };
  bool    firstLineHeader   { false };
  bool    firstColumnHeader { false };
  int     numRows           { 100 };
  QString filter;
  QString fold;
  QString sort;
  Vars    vars;
};

#endif
