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
  Q_PROPERTY(CQChartsColumn  groupColumn READ groupColumn WRITE setGroupColumn)

  //! hier separator
  Q_PROPERTY(QString separator READ separator WRITE setSeparator)

  //! follow view
  Q_PROPERTY(bool followViewExpand READ isFollowViewExpand WRITE setFollowViewExpand)

 public:
  CQChartsHierPlot(View *view, CQChartsPlotType *type, const ModelP &model);
 ~CQChartsHierPlot();

  //---

  void init() override;
  void term() override;

  //---

  //! get/set name columns
  const Columns &nameColumns() const { return nameColumns_; }
  void setNameColumns(const Columns &c);

  //! get/set value column
  const Column &valueColumn() const { return valueColumn_; }
  void setValueColumn(const Column &c);

  //! get/set group column
  const Column &groupColumn() const { return groupColumn_; }
  void setGroupColumn(const Column &c);

  //---

  Column getNamedColumn(const QString &name) const override;
  void setNamedColumn(const QString &name, const Column &c) override;

  Columns getNamedColumns(const QString &name) const override;
  void setNamedColumns(const QString &name, const Columns &c) override;

  //---

  ColumnType valueColumnType() const { return valueColumnType_; }

  //---

  //! get/set separator
  const QString &separator() const { return separator_; }
  void setSeparator(const QString &s) { separator_ = s; }

  QString calcSeparator() const { return (separator().length() ? separator() : QString("/")); }

  //---

  //! get/set follow view expand
  bool isFollowViewExpand() const { return followViewExpand_; }
  void setFollowViewExpand(bool b);

  virtual void followViewExpandChanged() { }

  bool isExpandModelIndex(const QModelIndex &ind) const;
  void expandModelIndex(const QModelIndex &ind, bool b);

  void expandedModelIndices(std::set<QModelIndex> &indSet) const;

  //---

  void addProperties() override;

  void addHierProperties();

 protected:
  // columns
  Columns nameColumns_; //!< multiple name columns
  Column  valueColumn_; //!< value column
  Column  groupColumn_; //!< group column

  ColumnType valueColumnType_ { ColumnType::NONE }; //!< value column type

  // options
  QString separator_        { "/" };   //!< hierarchical name separator
  bool    followViewExpand_ { false }; //!< follow view expand
};

//------

#include <CQChartsPlotCustomControls.h>

/*!
 * \brief Hier Plot plot custom controls
 * \ingroup Charts
 */
class CQChartsHierPlotCustomControls : public CQChartsPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsHierPlotCustomControls(CQCharts *charts, const QString &plotType);

  void setPlot(Plot *plot) override;

  void addHierColumnWidgets();

 public Q_SLOTS:
  void updateWidgets() override;

 protected:
  void connectSlots(bool b) override;

 protected:
  CQChartsHierPlot* hierPlot_ { nullptr };
};

#endif
