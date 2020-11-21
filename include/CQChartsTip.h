#ifndef CQChartsTip_H
#define CQChartsTip_H

#include <CQChartsColumn.h>
#include <set>

/*!
 * \brief class to build tip text as an HTML table
 * \ingroup Charts
 */
class CQChartsTableTip {
 public:
  CQChartsTableTip() {
    //str_ += "<b></b>\n";
    str_ += "<style>p { margin: 0 0 0 0 }</style><p style='white-space:pre'>";
  }

  void addBoldLine(const QString &name) {
    if (inTable_)
      str_ += "</table>\n";

    str_ += "<b>" + escapeText(name) + "</b>\n";
  }

  template<typename T>
  void addTableRow(const QString &name, const T &value) {
    if (! inTable_)
      str_ += "<table>\n";

    str_ += QString("<tr><th style=\"text-align:right\">%1</th>"
                    "<td>&nbsp;</td><td>%2</td></tr>\n").
              arg(escapeText(name)).arg(value);

    inTable_ = true;
  }

  void addTableRow(const QString &name, const QString &value) {
    if (! inTable_)
      str_ += "<table>\n";

    str_ += QString("<tr><th style=\"text-align:right\">%1</th>"
                    "<td>&nbsp;</td><td>%2</td></tr>\n").
              arg(escapeText(name)).arg(escapeText(value));

    inTable_ = true;
  }

  void addTableRow(const QString &name, const QString &value1, const QString &value2) {
    if (! inTable_)
      str_ += "<table>\n";

    str_ += QString("<tr><th style=\"text-align:right\">%1</th>"
                    "<td>&nbsp;</td><td>%2</td><td>%3</td></tr>\n").
              arg(escapeText(name)).arg(escapeText(value1)).arg(escapeText(value2));

    inTable_ = true;
  }

  QString str() {
    if (inTable_)
      str_ += "</table>";

    return str_;
  }

  void addColumn(const CQChartsColumn &c) {
    columnSet_.insert(c);
  }

  bool hasColumn(const CQChartsColumn &c) const {
    return (columnSet_.find(c) != columnSet_.end());
  }

 private:
  QString escapeText(const QString &str) const {
    QString str1;

    int i   = 0;
    int len = str.length();

    while (i < len) {
      if      (str[i] == '<') {
        str1 += "&lt;"; ++i;
      }
      else if (str[i] == '>') {
        str1 += "&gt;"; ++i;
      }
      else if (str[i] == '"') {
        str1 += "&quot;"; ++i;
      }
      else if (str[i] == '&') {
        str1 += "&amp;"; ++i;
      }
      else
        str1 += str[i++];
    }

    return str1;
  }

 private:
  using ColumnSet = std::set<CQChartsColumn>;

  QString   str_;
  bool      inTable_ { false };
  ColumnSet columnSet_;
};

#endif
