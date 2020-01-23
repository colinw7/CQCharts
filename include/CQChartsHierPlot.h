#ifndef CQChartsHierPlot_H
#define CQChartsHierPlot_H

#include <CQChartsHierPlotType.h>
#include <CQChartsPlot.h>

//----

/*!
 * \brief Hierarchical base plot
 * \ingroup Charts
 */
class CQChartsHierPlot : public CQChartsPlot {
  Q_OBJECT

  //! columns
  Q_PROPERTY(CQChartsColumns nameColumns READ nameColumns WRITE setNameColumns)
  Q_PROPERTY(CQChartsColumn  valueColumn READ valueColumn WRITE setValueColumn)

  //! hier separator
  Q_PROPERTY(QString separator READ separator WRITE setSeparator)

  //! follow view
  Q_PROPERTY(bool followViewExpand READ isFollowViewExpand WRITE setFollowViewExpand)

 public:
  CQChartsHierPlot(CQChartsView *view, CQChartsPlotType *type, const ModelP &model);

 ~CQChartsHierPlot();

  //---

  //! get/set name columns
  const CQChartsColumns &nameColumns() const { return nameColumns_; }
  void setNameColumns(const CQChartsColumns &c);

  //! get/set value column
  const CQChartsColumn &valueColumn() const { return valueColumn_; }
  void setValueColumn(const CQChartsColumn &c);

  //---

  //! get/set separator
  const QString &separator() const { return separator_; }
  void setSeparator(const QString &s) { separator_ = s; }

  //---

  //! get/set folow view expand
  bool isFollowViewExpand() const { return followViewExpand_; }
  void setFollowViewExpand(bool b);

  virtual void followViewExpandChanged() { }

  void expandedModelIndices(std::set<QModelIndex> &indSet) const;

  //---

  void addProperties() override;

  void addHierProperties();

 protected:
  CQChartsColumns nameColumns_;                //!< multiple name columns
  CQChartsColumn  valueColumn_;                //!< value column
  QString         separator_        { "/" };   //!< hierarchical name separator
  bool            followViewExpand_ { false }; //!< follow view expand
};

#endif
