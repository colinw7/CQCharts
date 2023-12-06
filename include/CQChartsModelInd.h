#ifndef CQChartsModelInd_H
#define CQChartsModelInd_H

#include <CQChartsTmpl.h>
#include <CQUtilMeta.h>

#include <QString>

class CQCharts;
class CQChartsModelData;

/*!
 * \brief class to contain model id
 * \ingroup Charts
 *
 * TODO: support postfix for type of degrees/radians
 */
class CQChartsModelInd :
  public CQChartsComparatorBase<CQChartsModelInd>,
  public CQChartsToStringBase<CQChartsModelInd> {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsModelInd, metaTypeId)

 public:
  using ModelData = CQChartsModelData;

 public:
  //! default constructor
  CQChartsModelInd() = default;

  explicit CQChartsModelInd(const QString &str);

  explicit CQChartsModelInd(int modelInd) :
   modelInd_(modelInd) {
  }

  CQChartsModelInd(CQCharts *charts, int modelInd) :
   charts_(charts), modelInd_(modelInd) {
  }

  //---

  const CQCharts *charts() const { return charts_; }
  void setCharts(CQCharts *charts) { charts_ = charts; }

  int modelInd() const { return modelInd_; }
  void setModelInd(int modelInd) { modelInd_ = modelInd; }

  //---

  const ModelData *modelData() const;

  //---

  bool isValid() const { return modelInd_ > 0; }

  //---

  //! compare for (==, !=, <, >, <=, >=)
  friend int cmp(const CQChartsModelInd &lhs, const CQChartsModelInd &rhs) {
    if (lhs.modelInd_ > rhs.modelInd_ ) return  1;
    if (lhs.modelInd_ < rhs.modelInd_ ) return -1;
    return 0;
  }

  //---

  QString toString() const;
  bool fromString(const QString &s);

 private:
  CQCharts* charts_   { nullptr }; //!< charts
  int       modelInd_ { -1 };      //!< model index
};

//---

CQUTIL_DCL_META_TYPE(CQChartsModelInd)

#endif
