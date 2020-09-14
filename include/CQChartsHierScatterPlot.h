#ifndef CQChartsHierScatterPlot_H
#define CQChartsHierScatterPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>

class CQChartsDataLabel;

//---

/*!
 * \brief Hierarchical Scatter plot type
 * \ingroup Charts
 */
class CQChartsHierScatterPlotType : public CQChartsPlotType {
 public:
  CQChartsHierScatterPlotType();

  QString name() const override { return "hierscatter"; }
  QString desc() const override { return "HierScatter"; }

  bool isHierarchical() const override { return true; }

  Dimension dimension() const override { return Dimension::TWO_D; }

  void addParameters() override;

  bool canProbe() const override { return true; }

  QString description() const override;

  Plot *create(View *view, const ModelP &model) const override;
};

//---

class CQChartsHierScatterPlot;

class CQChartsHierScatterPointGroup;

using CQChartsHierScatterPointGroupMap = std::map<int, CQChartsHierScatterPointGroup *>;

//! hier scatter plot point
struct CQChartsHierScatterPoint {
  using Point = CQChartsGeom::Point;

  CQChartsHierScatterPointGroup* group { nullptr };
  Point                          p;
  QString                        name;
  int                            i     { 0 };
  QModelIndex                    ind;

  CQChartsHierScatterPoint(CQChartsHierScatterPointGroup *group, double x, double y,
                           const QString &name, int i, const QModelIndex &ind) :
   group(group), p(x, y), name(name), i(i), ind(ind){
  }
};

using CQChartsHierScatterPoints = std::vector<CQChartsHierScatterPoint>;

/*!
 * \brief Hierarchical Scatter plot point group
 * \ingroup Charts
 */
class CQChartsHierScatterPointGroup {
 public:
  CQChartsHierScatterPointGroup(CQChartsHierScatterPointGroup *parent, int ind) :
   parent_(parent), ind_(ind) {
  }

 ~CQChartsHierScatterPointGroup() {
    for (auto &group : groups_)
      delete group.second;
  }

  CQChartsHierScatterPointGroup *parent() const { return parent_; }

  int ind() const { return ind_; }

  int i() const { return i_; }
  void setI(int i) { i_ = i; }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  int numGroups() const { return groups_.size(); }

  const CQChartsHierScatterPointGroupMap &groups() const { return groups_; }

  int numPoints() const { return points_.size(); }

  const CQChartsHierScatterPoints &points() const { return points_; }

  int parentCount() const {
    if (! parent())
      return 0;

    return parent()->numGroups();
  }

  int depth() const {
    int d = 0;

    auto *p = parent();

    while (p) {
      ++d;

      p = p->parent();
    }

    return d;
  }

  CQChartsHierScatterPointGroup *lookupGroup(int ind) const {
    auto p = groups_.find(ind);

    if (p == groups_.end())
      return nullptr;

    return (*p).second;
  }

  CQChartsHierScatterPointGroup *lookupGroup(const QString &name) const {
    for (const auto &igroup : groups_) {
      auto *group = igroup.second;

      if (group->name() == name)
        return group;
    }

    return nullptr;
  }

  CQChartsHierScatterPointGroup *addGroup(int ind, const QString &name) {
    auto p = groups_.find(ind);
    assert(p == groups_.end());

    int n = groups_.size();

    auto *group1 = new CQChartsHierScatterPointGroup(this, ind);

    group1->setI(n);

    p = groups_.insert(p, CQChartsHierScatterPointGroupMap::value_type(ind, group1));

    auto *group = (*p).second;

    group->setName(name);

    return group;
  }

  void addPoint(const CQChartsHierScatterPoint &p) {
    points_.push_back(p);
  }

 private:
  CQChartsHierScatterPointGroup*   parent_ { nullptr };
  int                              ind_    { -1 };
  int                              i_      { -1 };
  QString                          name_;
  CQChartsHierScatterPointGroupMap groups_;
  CQChartsHierScatterPoints        points_;
};

//---

/*!
 * \brief Hierarchical Scatter Plot Point object
 * \ingroup Charts
 */
class CQChartsHierScatterPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Plot   = CQChartsHierScatterPlot;
  using Length = CQChartsLength;
  using Symbol = CQChartsSymbol;

 public:
  CQChartsHierScatterPointObj(const Plot *plot, const BBox &rect, const Point &p,
                              const ColorInd &iv);

  QString typeName() const override { return "point"; }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  CQChartsHierScatterPointGroup *group() const { return group_; }
  void setGroup(CQChartsHierScatterPointGroup *p) { group_ = p; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  void draw(PaintDevice *device) override;

 private:
  const Plot*                    plot_ { nullptr };
  Point                          p_;
  QString                        name_;
  CQChartsHierScatterPointGroup* group_ { nullptr };
};

//---

#include <CQChartsKey.h>

/*!
 * \brief Hierarchical Scatter Plot Key Color Box
 * \ingroup Charts
 */
class CQChartsHierScatterKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  using Plot = CQChartsHierScatterPlot;

 public:
  CQChartsHierScatterKeyColor(Plot *plot, CQChartsHierScatterPointGroup *group,
                              const ColorInd &ic);

  bool selectPress(const Point &p, CQChartsSelMod selMod) override;

  QBrush fillBrush() const override;

 private:
  CQChartsHierScatterPointGroup* group_ { nullptr };
};

//---

/*!
 * \brief Hierarchical Scatter Plot
 * \ingroup Charts
 */
class CQChartsHierScatterPlot : public CQChartsPlot,
 public CQChartsObjPointData<CQChartsHierScatterPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn  xColumn      READ xColumn      WRITE setXColumn     )
  Q_PROPERTY(CQChartsColumn  yColumn      READ yColumn      WRITE setYColumn     )
  Q_PROPERTY(CQChartsColumn  nameColumn   READ nameColumn   WRITE setNameColumn  )
  Q_PROPERTY(CQChartsColumns groupColumns READ groupColumns WRITE setGroupColumns)

  // symbol
  CQCHARTS_POINT_DATA_PROPERTIES

  // options
  Q_PROPERTY(bool textLabels READ isTextLabels WRITE setTextLabels)

 public:
  using GroupValueSets = std::map<Column, CQChartsValueSet *>;

 public:
  CQChartsHierScatterPlot(View *view, const ModelP &model);
 ~CQChartsHierScatterPlot();

  //---

  void init() override;
  void term() override;

  //---

  // columns
  const Column &xColumn() const { return xColumn_; }
  void setXColumn(const Column &c);

  const Column &yColumn() const { return yColumn_; }
  void setYColumn(const Column &c);

  const Column &nameColumn() const { return nameColumn_; }
  void setNameColumn(const Column &c);

  const Columns &groupColumns() const { return groupColumns_; }
  void setGroupColumns(const Columns &c);

  //---

  //! get/set text labels
  bool isTextLabels() const;
  void setTextLabels(bool b);

  //---

  //! get/set current group
  CQChartsHierScatterPointGroup *currentGroup() const { return currentGroup_; }
  void setCurrentGroup(CQChartsHierScatterPointGroup *group);

  //---

  const QString &xname() const { return xname_; }
  const QString &yname() const { return yname_; }

  //---

  const CQChartsDataLabel *dataLabel() const { return dataLabel_; }
  CQChartsDataLabel *dataLabel() { return dataLabel_; }

  //---

  void addProperties() override;

  void getPropertyNames(QStringList &names, bool hidden) const override;

  //---

  Range calcRange() const override;

  void initGroupValueSets();

  void addRowGroupValueSets(const ModelVisitor::VisitData &data) const;

  void clearPlotObjects() override;

  bool createObjs(PlotObjs &objs) const override;

  void addGroupPoint(const ModelVisitor::VisitData &data,
                     double x, double y, const QString &name) const;

  void addGroupPoints(CQChartsHierScatterPointGroup *baseGroup,
                      CQChartsHierScatterPointGroup *group, PlotObjs &objs) const;

  //---

  int acceptsRow(int row, const QModelIndex &parent) const;

  void addKeyItems(CQChartsPlotKey *key) override;

  //---

  bool addMenuItems(QMenu *menu) override;

  //---

  void write(std::ostream &os, const QString &plotVarName, const QString &modelVarName,
             const QString &viewVarName) const override;

 protected:
  void resetAxes();

  void initAxes();

  //---

  bool probe(ProbeData &probeData) const override;

  //---

  using PointObj = CQChartsHierScatterPointObj;

  virtual PointObj *createPointObj(const BBox &rect, const Point &p, const ColorInd &iv) const;

 public slots:
  void popCurrentGroup();

  void resetCurrentGroup();

 private:
  using PointGroup = CQChartsHierScatterPointGroup;

  Column             xColumn_;                     //!< x column
  Column             yColumn_;                     //!< y column
  Column             nameColumn_;                  //!< name column
  Columns            groupColumns_;                //!< group columns
  CQChartsDataLabel* dataLabel_       { nullptr }; //!< data label style
  QStringList        filterNames_;                 //!< filter names
  Columns            groupValues_;                 //!< group values
  PointGroup*        rootGroup_       { nullptr }; //!< root group
  PointGroup*        currentGroup_    { nullptr }; //!< current group
  QString            xname_;                       //!< x name
  QString            yname_;                       //!< y name
  GroupValueSets     groupValueSets_;              //!< group value sets
};

#endif
