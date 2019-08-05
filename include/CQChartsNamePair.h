#ifndef CQChartsNamePair_H
#define CQChartsNamePair_H

#include <CQChartsUtil.h>

/*!
 * \brief Name Pair class
 * \ingroup Charts
 */
class CQChartsNamePair {
 public:
  static void registerMetaType();

  static int metaTypeId;

  struct Names {
    bool    valid { false };
    QString name1;
    QString name2;
  };

 public:
  CQChartsNamePair(const QString &s=QString()) {
    setValue(s);
  }

  CQChartsNamePair(const CQChartsNamePair &rhs) :
   names_(rhs.names_) {
  }

  CQChartsNamePair &operator=(const CQChartsNamePair &rhs) {
    names_ = rhs.names_;

    return *this;
  }

  bool isValid() const { return names_.valid; }

  const QString &name1() const { return names_.name1; }
  const QString &name2() const { return names_.name2; }

  bool setValue(const QString &str) {
    return stringToNames(str, names_);
  }

  //---

  QString toString() const {
    return namesToString(names_);
  }

  bool fromString(const QString &s) {
    return setValue(s);
  }

  //---

  static bool stringToNames(const QString &str, Names &names);
  static QString namesToString(const Names &names);

 private:
  Names names_;
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsNamePair)

#endif
