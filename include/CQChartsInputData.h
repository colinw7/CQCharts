#ifndef CQChartsInputData_H
#define CQChartsInputData_H

#include <QVariant>
#include <QString>
#include <vector>

struct CQChartsInputData {
  using Vars = std::vector<QVariant>;

  bool    commentHeader     { false };
  bool    firstLineHeader   { false };
  bool    firstColumnHeader { false };
  bool    transpose         { false };
  QString separator;
  int     numRows           { 100 };
  QString filter;
  QString fold;
  QString sort;
  Vars    vars;
};

#endif
