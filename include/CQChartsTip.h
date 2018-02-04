#ifndef CQChartsTip_H
#define CQChartsTip_H

class CQChartsTableTip {
 public:
  CQChartsTableTip() {
    //str_ += "<b></b>\n";
  }

  void addBoldLine(const QString &name) {
    if (inTable_)
      str_ += "</table>\n";

    str_ += "<b>" + name + "</b>\n";
  }

  template<typename T>
  void addTableRow(const QString &name, const T &value) {
    if (! inTable_)
      str_ += "<table>\n";

    str_ += QString("<tr><th>%1</th><td>&nbsp;</td><td>%2</td></tr>\n").arg(name).arg(value);

    inTable_ = true;
  }

  template<typename T1, typename T2>
  void addTableRow(const QString &name, const T1 &value1, const T2 &value2) {
    if (! inTable_)
      str_ += "<table>\n";

    str_ += QString("<tr><th>%1</th><td>&nbsp;</td><td>%2</td><td>%3</td></tr>\n").
              arg(name).arg(value1).arg(value2);

    inTable_ = true;
  }

  QString str() {
    if (inTable_)
      str_ += "</table>\n";

    return str_;
  }

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
  QString str_;
  bool    inTable_ { false };
};

#endif
