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
  CQChartsNamePair(const QString &str=QString(), const QChar &separator='/') {
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

  bool setValue(const QString &str, const QChar &separator='/') {
    separator_ = separator;

    return stringToNames(str, names_, separator_);
  }

  const QChar &separator() const { return separator_; }
  void setSeparator(const QChar &c) { separator_ = c; }

  //---

  QString toString() const {
    return namesToString(names_, separator_);
  }

  bool fromString(const QString &s) {
    return setValue(s, separator_);
  }

  //---

  static bool stringToNames(const QString &str, Names &names, const QChar &separator='/');
  static QString namesToString(const Names &names, const QChar &separator='/');

 private:
  Names names_;
  QChar separator_ { '/' };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsNamePair)

#endif
