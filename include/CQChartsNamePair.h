#ifndef CQChartsNamePair_H
#define CQChartsNamePair_H

#include <CQChartsUtil.h>
#include <CQUtilMeta.h>

/*!
 * \brief Name Pair class
 * \ingroup Charts
 */
class CQChartsNamePair {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsNamePair, metaTypeId)

 public:
  //! name pair
  struct Names {
    bool    valid { false };
    QString name1;
    QString name2;
  };

 public:
  CQChartsNamePair(const QString &str=QString(), const QString &separator="/") {
    setValue(str, separator);
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

  bool setValue(const QString &str, const QString &separator="/") {
    separator_ = separator;

    return stringToNames(str, names_, separator_);
  }

  const QString &separator() const { return separator_; }
  void setSeparator(const QString &c) { separator_ = c; }

  //---

  QString toString() const {
    return namesToString(names_, separator_);
  }

  bool fromString(const QString &s) {
    return setValue(s, separator_);
  }

  //---

  static bool stringToNames(const QString &str, Names &names, const QString &separator="/");
  static QString namesToString(const Names &names, const QString &separator="/");

 private:
  Names   names_;
  QString separator_ { "/" };
};

//---

CQUTIL_DCL_META_TYPE(CQChartsNamePair)

#endif
