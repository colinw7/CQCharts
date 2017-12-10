#ifndef CQChartsRegExp_H
#define CQChartsRegExp_H

#include <QRegExp>

class CQChartsRegExp {
 public:
  CQChartsRegExp(const QString &pattern="") :
   pattern_(pattern), regexp_(adjustPattern(pattern), Qt::CaseSensitive, QRegExp::Wildcard) {
  }

  bool match(const QString &str) const {
    return regexp_.exactMatch(str);
  }

 private:
  static QString adjustPattern(const QString &pattern) {
    QString pattern1 = pattern;

    if (pattern1.length() && pattern1[pattern1.length() - 1] != '*')
      pattern1 += "*";

    if (pattern1.length() && pattern1[0] != '*')
      pattern1 = "*" + pattern1;

    return pattern1;
  }

 private:
  QString pattern_;
  QRegExp regexp_;
};

#endif
