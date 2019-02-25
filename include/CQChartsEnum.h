#ifndef CQChartsEnum_H
#define CQChartsEnum_H

#include <QString>
#include <QStringList>
#include <iostream>

class CQChartsEnum {
 public:
  CQChartsEnum() { }

  virtual ~CQChartsEnum() { }

  //---

  virtual QString toString() const = 0;
  virtual bool fromString(const QString &s) = 0;

  //---

  virtual QStringList enumNames() const = 0;

  void print(std::ostream &os) const {
    os << toString().toStdString();
  }
};

//---

#include <CQUtilMeta.h>

#endif
