#ifndef CQChartsRegExp_H
#define CQChartsRegExp_H

#include <QRegExp>

/*!
 * \brief Regular Expression class
 * \ingroup Charts
 */
class CQChartsRegExp {
 public:
  CQChartsRegExp(const QString &pattern="", QRegExp::PatternSyntax syntax=QRegExp::Wildcard) :
   pattern_(pattern), syntax_(syntax),
   regexp_(adjustPattern(pattern, syntax), Qt::CaseSensitive, syntax) {
  }

  void setSyntax(QRegExp::PatternSyntax syntax) {
    syntax_ = syntax;
    regexp_ = QRegExp(adjustPattern(pattern_, syntax_), Qt::CaseSensitive, syntax_);
  }

  bool match(const QString &str) const {
    return regexp_.exactMatch(str);
  }

 private:
  static QString adjustPattern(const QString &pattern, QRegExp::PatternSyntax syntax) {
    if (syntax == QRegExp::Wildcard) {
      QString pattern1 = pattern;

      if (pattern1.length() && pattern1[pattern1.length() - 1] != '*')
        pattern1 += "*";

      if (pattern1.length() && pattern1[0] != '*')
        pattern1 = "*" + pattern1;

      return pattern1;
    }
    else {
      return pattern;
    }
  }

 private:
  QString                pattern_;
  QRegExp::PatternSyntax syntax_ { QRegExp::Wildcard };
  QRegExp                regexp_;
};

#endif
