#ifndef CQChartsNameValueData_H
#define CQChartsNameValueData_H

#include <QString>
#include <map>

struct CQChartsNameValueData {
  using NameValues = std::map<QString,QString>;
  using NameReals  = std::map<QString,double>;
  using NameBools  = std::map<QString,bool>;

  NameValues values;
  NameValues strings;
  NameReals  reals;
  NameBools  bools;
};

#endif
