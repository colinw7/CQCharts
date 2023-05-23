#ifndef CQChartsTip_H
#define CQChartsTip_H

#include <CQChartsColumn.h>
#include <CQChartsHtml.h>
#include <set>

/*!
 * \brief class to build tip text as an HTML table
 * \ingroup Charts
 */
class CQChartsTableTip {
 public:
  using Columns = CQChartsColumns;
  using Column  = CQChartsColumn;

 public:
  CQChartsTableTip() {
    //str_ += "<b></b>\n";
    //str_ += "<style>p { margin: 0 0 0 0 }</style><p style='white-space:pre'>";
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

    str_ += tableRow(tableHeader(escapeText(name)) + tableData() + tableDataT<T>(value));

    inTable_ = true;
  }

  void addTableRow(const QString &name, const QString &value) {
    if (! inTable_)
      str_ += "<table>\n";

    str_ += tableRow(tableHeader(escapeText(name)) + tableData() + tableData(escapeText(value)));

    inTable_ = true;
  }

  void addTableRow(const QString &name, const QString &value, const QColor &c) {
    if (! inTable_)
      str_ += "<table>\n";

    str_ += tableRow(tableHeader(escapeText(name)) + tableData() +
                     tableData(escapeText(value)) + tableData(c));

    inTable_ = true;
  }

  void addTableRow(const QString &name, const QString &value1, const QString &value2) {
    if (! inTable_)
      str_ += "<table>\n";

    str_ += tableRow(tableHeader(escapeText(name)) + tableData() +
                     tableData(escapeText(value1)) + tableData(escapeText(value2)));

    inTable_ = true;
  }

  void addTableRow(const QString &name, const QVariant &value) {
    // TODO: custom tip for color ....
    addTableRow(name, value.toString());
  }

  QString str() {
    if (str_ == "")
      return str_;

    if (inTable_)
      str_ += "</table>";

    return "<style>p { margin: 0 0 0 0 }</style><p style='white-space:pre'>" + str_;
  }

  void addColumn(const Column &c) {
    columnSet_.insert(c);
  }

  bool hasColumn(const Column &c) const {
    return (columnSet_.find(c) != columnSet_.end());
  }

  void addColumns(const Columns &c) {
    if (c.count() == 1)
      addColumn(c.column());

    columnsSet_.insert(c);
  }

  bool hasColumns(const Columns &c) const {
    if (c.count() == 1 && hasColumn(c.column()))
      return true;

    return (columnsSet_.find(c) != columnsSet_.end());
  }

 private:
  QString tableHeader(const QString &str   ) const { return CQChartsHtml::tableHeader(str); }
  QString tableRow   (const QString &str   ) const { return CQChartsHtml::tableRow   (str); }
  QString tableData  (const QString &str="") const { return CQChartsHtml::tableData  (str); }

  QString tableData(const QColor &c) const { return CQChartsHtml::tableData(c); }

  template<typename T>
  QString tableDataT(const T &value) const { return CQChartsHtml::tableDataT<T>(value); }

  QString escapeText(const QString &str) const { return CQChartsHtml::escapeText(str); }

 private:
  using ColumnsSet = std::set<Columns>;
  using ColumnSet  = std::set<Column>;

  QString    str_;
  bool       inTable_ { false };
  ColumnSet  columnSet_;
  ColumnsSet columnsSet_;
};

#endif
