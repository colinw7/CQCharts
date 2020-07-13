#ifndef CQChartsModelIndex_H
#define CQChartsModelIndex_H

#include <CQChartsColumn.h>

#include <QModelIndex>

class CQChartsPlot;

class CQChartsModelIndex {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsModelIndex() = default;

  //CQChartsModelIndex(int row, const CQChartsColumn &column,
  //                   const QModelIndex &parent=QModelIndex());

  CQChartsModelIndex(CQChartsPlot *plot, int row, const CQChartsColumn &column,
                     const QModelIndex &parent=QModelIndex());

  //---

  const CQChartsPlot *plot() const { return plot_; }
  void setPlot(CQChartsPlot *p) { plot_ = p; }

  int row() const { return row_; }
  void setRow(int i) { row_ = i; }

  const CQChartsColumn &column() const { return column_; }
  void setColumn(const CQChartsColumn &v) { column_ = v; }

  const QModelIndex &parent() const { return parent_; }
  void setParent(const QModelIndex &v) { parent_ = v; }

  int cellCol() const { return cellCol_; }
  void setCellCol(int i) { cellCol_ = i; }

  bool isNormalized() const { return normalized_; }
  void setNormalized(bool b) { normalized_ = b; }

  //---

  bool isValid() const;

  QString toString() const;

  bool fromString(const QString &s);

 private:
  CQChartsPlot*  plot_       { nullptr };
  int            row_        { -1 };
  CQChartsColumn column_;
  QModelIndex    parent_;
  int            cellCol_    { -1 };
  bool           normalized_ { false };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsModelIndex)

#endif
