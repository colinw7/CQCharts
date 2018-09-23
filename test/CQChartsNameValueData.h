#ifndef CQChartsNameValueData_H
#define CQChartsNameValueData_H

#include <QString>
#include <map>

struct CQChartsNameValueData {
  using NameValues = std::map<QString,QString>;
  using NameBools  = std::map<QString,bool>;
  using NameInts   = std::map<QString,int>;
  using NameReals  = std::map<QString,double>;

  NameValues values;
  NameBools  bools;
  NameValues strings;
  NameInts   ints;
  NameReals  reals;
  NameValues enums;
};

#endif
