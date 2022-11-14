#ifndef CQChartsModelIndex_H
#define CQChartsModelIndex_H

#include <CQChartsColumn.h>
#include <CQUtilMeta.h>

#include <QModelIndex>
#include <QPointer>

class CQChartsPlot;

/*!
 * \brief Class for Model Index (wrapper aroung QModelIndex using CQChartsColumn)
 * \ingroup Charts
 */
class CQChartsModelIndex :
  public CQChartsComparatorBase<CQChartsModelIndex>,
  public CQChartsToStringBase<CQChartsModelIndex> {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsModelIndex, metaTypeId)

 public:
  using Plot   = CQChartsPlot;
  using PlotP  = QPointer<Plot>;
  using Column = CQChartsColumn;

 public:
  CQChartsModelIndex() = default;

//CQChartsModelIndex(int row, const Column &column,
//                   const QModelIndex &parent=QModelIndex());

  CQChartsModelIndex(const Plot *plot, int row, const Column &column,
                     const QModelIndex &parent=QModelIndex(), bool normalized=false);

  //---

  Plot *plot() const;
  void setPlot(const Plot *p);

  int row() const { return row_; }
  void setRow(int i) { row_ = i; }

  const Column &column() const { return column_; }
  void setColumn(const Column &v) { column_ = v; }

  const QModelIndex &parent() const { return parent_; }
  void setParent(const QModelIndex &v) { parent_ = v; }

  int cellCol() const { return cellCol_; }
  void setCellCol(int i) { cellCol_ = i; }

  bool isNormalized() const { return normalized_; }
  void setNormalized(bool b) { normalized_ = b; }

  //---

  bool isValid() const;

  //---

  //! compare for (==, !=, <, >, <=, >=)
  friend int cmp(const CQChartsModelIndex &lhs, const CQChartsModelIndex &rhs) {
    if (lhs.plot() > rhs.plot()) return  1;
    if (lhs.plot() < rhs.plot()) return  1;

    int c1 = cmpInd(lhs.parent_, rhs.parent_);
    if (c1 != 0) return c1;

    if (lhs.row_ > rhs.row_) return  1;
    if (lhs.row_ < rhs.row_) return -1;

    if (lhs.column_ > rhs.column_) return  1;
    if (lhs.column_ < rhs.column_) return -1;

    return 0;
  }

  //---

  QString toString() const;
  bool fromString(const QString &s);

  //---

  QString id() const;

 private:
  static int cmpInd(const QModelIndex &i1, const QModelIndex &i2) {
    if      (i1.parent().isValid() && i2.parent().isValid()) {
      int c1 = cmpInd(i1.parent(), i2.parent());
      if (c1 != 0) return c1;
    }
    else if (i1.parent().isValid()) {
      return 1;
    }
    else if (i2.parent().isValid()) {
      return -1;
    }

    if (i1.row() > i2.row()) return  1;
    if (i1.row() < i2.row()) return -1;

    if (i1.column() > i2.column()) return  1;
    if (i1.column() < i2.column()) return -1;

    return 0;
  }

 private:
  PlotP       plot_;
  int         row_        { -1 };
  Column      column_;
  QModelIndex parent_;
  int         cellCol_    { -1 };
  bool        normalized_ { false };
};

//---

CQUTIL_DCL_META_TYPE(CQChartsModelIndex)

#endif
