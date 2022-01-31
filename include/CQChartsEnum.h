#ifndef CQChartsEnum_H
#define CQChartsEnum_H

#include <CQUtilMeta.h>
#include <QString>
#include <QStringList>
#include <iostream>

/*!
 * \brief Enum base class
 * \ingroup Charts
 */
template<typename TYPE>
class CQChartsEnum {
 public:
  CQChartsEnum() { }

  virtual ~CQChartsEnum() { }

  //---

  virtual QString toString() const = 0;
  virtual bool fromString(const QString &s) = 0;

  //---

  virtual QStringList enumNames() const = 0;

  //---

  friend bool operator<(const TYPE &lhs, const TYPE &rhs) {
    return (lhs.type() < rhs.type());
  }

  friend bool operator==(const TYPE &lhs, const TYPE &rhs) {
    return (lhs.type() == rhs.type());
  }

  friend bool operator!=(const TYPE &lhs, const TYPE &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  friend std::ostream &operator<<(std::ostream &os, const TYPE &t) {
    t.print(os);
    return os;
  }

  void print(std::ostream &os) const {
    os << toString().toStdString();
  }
};

//---

#endif
