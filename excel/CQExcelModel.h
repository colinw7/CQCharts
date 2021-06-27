#ifndef CQExcelModel_H
#define CQExcelModel_H

#include <CQDataModel.h>

#include <QPen>
#include <QBrush>
#include <QFont>
#include <set>

namespace CQExcel {

class Tcl;

class Model : public CQDataModel {
  Q_OBJECT

 public:
  struct Style {
    QBrush pen;
    QBrush brush;
    QFont  font;
    bool   fontSet { false };

    QString toString() const {
      QStringList strs;

      if (pen.color().isValid())
        strs << "stroke.color=" + pen.color().name();

      if (brush.color().isValid())
        strs << "fill.color=" + brush.color().name();

      if (fontSet)
        strs << "font=" + font.rawName();

      return strs.join(";");
    }

    static Style fromString(const QString &str) {
      Style style;

      auto strs = str.split(";");

      for (const auto &s : strs) {
        auto ss = s.split("=");

        if (ss.length() != 2)
          continue;

        auto name  = ss[0].trimmed().toLower();
        auto value = ss[1].trimmed();

        if      (name == "stroke.color")
          style.pen.setColor(QColor(value));
        else if (name == "fill.color")
          style.brush.setColor(QColor(value));
        else if (name == "font")
          style.font = QFont(value);
      }

      return style;
    }
  };

  using CellExpr  = std::map<QModelIndex, QString>;
  using CellStyle = std::map<QModelIndex, Style>;

 public:
  Model(QObject *parent, int nc=100, int nr=100);
  Model(int nc=100, int nr=100);

  Tcl *tcl() const { return tcl_; }

  void addRow   (int n=1) override;
  void addColumn(int n=1) override;

  //---

  bool hasCellExpression(const QModelIndex &ind) const;

  QString cellExpression(const QModelIndex &ind) const;
  void setCellExpression(const QModelIndex &ind, const QString &expr);

  void clearCellExpression(const QModelIndex &ind);

  void getCellExpressions(CellExpr &cellExpr) const;

  //---

  bool hasCellStyle(const QModelIndex &ind) const;

  const Style &cellStyle(const QModelIndex &ind) const;

  void setCellStyle(const QModelIndex &ind, const Style &style);

  void clearCellStyle(const QModelIndex &ind);

  void getCellStyles(CellStyle &cellStyle) const;

  //---

  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  QVariant data(const QModelIndex &index, int role) const override;

  bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::DisplayRole) override;

  //---

  QString rowName   (int r) const;
  QString columnName(int c) const;
  QString cellName  (int r, int c) const;

  bool decodeCellName(const QString &name, int &row, int &col) const;

  bool decodeColumnName(const QString &name, int &col) const;

  struct IndicesData {
    using Cols    = std::set<int>;
    using Rows    = std::set<int>;
    using RowCols = std::map<int,Cols>;
    using ColRows = std::map<int,Rows>;

    RowCols rowCols;
    ColRows colRows;
  };

  void setIndicesData(const QModelIndexList &indices, IndicesData &indicesData);

 private:
  void init(int nc, int nr);

  void updateTraceVars();

  void updateCellExpression(const QModelIndex &index, const QString &expr);

 private:
  using ColExpr     = std::map<int, QString>;
  using RowColExpr  = std::map<int, ColExpr>;
  using RowNames    = std::map<int, QString>;
  using ColumnNames = std::map<int, QString>;
  using TraceNames  = std::set<QString>;

  Tcl*        tcl_  { nullptr };
  RowColExpr  expr_;
  RowNames    rowNames_;
  ColumnNames columnNames_;
  TraceNames  traceNames_;
  CellStyle   cellStyle_;
};

}

#endif
