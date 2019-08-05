#ifndef CQChartsNameValueData_H
#define CQChartsNameValueData_H

#include <QString>
#include <map>

/*!
 * \brief Charts Name Value Data
 * \ingroup Charts
 */
struct CQChartsNameValueData {
  using NameValues = std::map<QString,QString>;
  using NameBools  = std::map<QString,bool>;
  using NameInts   = std::map<QString,int>;
  using NameReals  = std::map<QString,double>;

  NameValues columns;
  NameValues parameters;
  NameValues values;
  NameBools  bools;
  NameValues strings;
  NameInts   ints;
  NameReals  reals;
};

#endif
