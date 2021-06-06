#ifndef CQChartsModelColumn_H
#define CQChartsModelColumn_H

#include <CQChartsTmpl.h>
#include <CQChartsColumn.h>
#include <QString>

class CQCharts;

/*!
 * \brief class to contain charts angle (degrees)
 * \ingroup Charts
 *
 * TODO: support postfix for type of degrees/radians
 */
class CQChartsModelColumn :
  public CQChartsComparatorBase<CQChartsModelColumn>,
  public CQChartsToStringBase<CQChartsModelColumn> {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  using Column = CQChartsColumn;

 public:
  //! default constructor
  CQChartsModelColumn() = default;

  explicit CQChartsModelColumn(const QString &str);

  explicit CQChartsModelColumn(int modelInd, const Column &column) :
   modelInd_(modelInd), column_(column) {
  }

  //---

  const CQCharts *charts() const { return charts_; }
  void setCharts(CQCharts *charts) { charts_ = charts; }

  int modelInd() const { return modelInd_; }
  void setModelInd(int modelInd) { modelInd_ = modelInd; }

  const Column &column() const { return column_; }
  void setColumn(const Column &v) { column_ = v; }

  //---

  bool isValid() const { return charts_ && modelInd_ > 0 && column_.isValid(); }

  //---

  //! compare for (==, !=, <, >, <=, >=)
  friend int cmp(const CQChartsModelColumn &lhs, const CQChartsModelColumn &rhs) {
    if (lhs.modelInd_ > rhs.modelInd_ ) return  1;
    if (lhs.modelInd_ < rhs.modelInd_ ) return -1;
    if (lhs.column_   > rhs.column_   ) return  1;
    if (lhs.column_   < rhs.column_   ) return -1;
    return 0;
  }

  //---

  QString toString() const;
  bool fromString(const QString &s);

 private:
  CQCharts* charts_   { nullptr }; //! charts
  int       modelInd_ { -1 };      //!< model index
  Column    column_;               //!< column
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsModelColumn)

#endif
