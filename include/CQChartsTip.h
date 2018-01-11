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

  QString str() {
    if (inTable_)
      str_ += "</table>\n";

    return str_;
  }

 private:
  QString str_;
  bool    inTable_ { false };
};

#endif
