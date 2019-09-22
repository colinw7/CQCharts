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

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsHierScatterPlot;

class CQChartsHierScatterPointGroup;

using CQChartsHierScatterPointGroupMap = std::map<int,CQChartsHierScatterPointGroup *>;

struct CQChartsHierScatterPoint {
  CQChartsHierScatterPointGroup* group { nullptr };
  QPointF                        p;
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

    CQChartsHierScatterPointGroup *p = parent();

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
      CQChartsHierScatterPointGroup *group = igroup.second;

      if (group->name() == name)
        return group;
    }

    return nullptr;
  }

  CQChartsHierScatterPointGroup *addGroup(int ind, const QString &name) {
    auto p = groups_.find(ind);
    assert(p == groups_.end());

    int n = groups_.size();

    CQChartsHierScatterPointGroup *group1 = new CQChartsHierScatterPointGroup(this, ind);

    group1->setI(n);

    p = groups_.insert(p, CQChartsHierScatterPointGroupMap::value_type(ind, group1));

    CQChartsHierScatterPointGroup *group = (*p).second;

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
  CQChartsHierScatterPointObj(const CQChartsHierScatterPlot *plot, const CQChartsGeom::BBox &rect,
                              const QPointF &p, const ColorInd &iv);

  QString typeName() const override { return "point"; }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  CQChartsHierScatterPointGroup *group() const { return group_; }
  void setGroup(CQChartsHierScatterPointGroup *p) { group_ = p; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

 private:
  const CQChartsHierScatterPlot* plot_ { nullptr };
  QPointF                        p_;
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
  CQChartsHierScatterKeyColor(CQChartsHierScatterPlot *plot, CQChartsHierScatterPointGroup *group,
                              const ColorInd &ic);

  bool selectPress(const CQChartsGeom::Point &p, CQChartsSelMod selMod) override;

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
  Q_PROPERTY(double fontSize   READ fontSize     WRITE setFontSize  )
  Q_PROPERTY(bool   textLabels READ isTextLabels WRITE setTextLabels)

 public:
  using GroupValueSets = std::map<CQChartsColumn,CQChartsValueSet *>;

 public:
  CQChartsHierScatterPlot(CQChartsView *view, const ModelP &model);
 ~CQChartsHierScatterPlot();

  //---

  const CQChartsColumn &xColumn() const { return xColumn_; }
  void setXColumn(const CQChartsColumn &c);

  const CQChartsColumn &yColumn() const { return yColumn_; }
  void setYColumn(const CQChartsColumn &c);

  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c);

  const CQChartsColumns &groupColumns() const { return groupColumns_; }
  void setGroupColumns(const CQChartsColumns &c);

  //---

  bool isTextLabels() const;
  void setTextLabels(bool b);

  double fontSize() const { return fontSize_; }
  void setFontSize(double s);

  //---

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

  CQChartsGeom::Range calcRange() const override;

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

  void write(std::ostream &os, const QString &varName="",
             const QString &modelName="") const override;

 private:
  void resetAxes();

  void initAxes();

  //---

  bool probe(ProbeData &probeData) const override;

 public slots:
  void popCurrentGroup();

  void resetCurrentGroup();

 private:
  using PointGroup = CQChartsHierScatterPointGroup;

  CQChartsColumn     xColumn_;                     //!< x column
  CQChartsColumn     yColumn_;                     //!< y column
  CQChartsColumn     nameColumn_;                  //!< name column
  CQChartsColumns    groupColumns_;                //!< group columns
  double             fontSize_        { 8 };       //!< font size
  CQChartsDataLabel* dataLabel_       { nullptr }; //!< data label style
  QStringList        filterNames_;                 //!< filter names
  CQChartsColumns    groupValues_;                 //!< group values
  PointGroup*        rootGroup_       { nullptr }; //!< root group
  PointGroup*        currentGroup_    { nullptr }; //!< current group
  QString            xname_;                       //!< x name
  QString            yname_;                       //!< y name
  GroupValueSets     groupValueSets_;              //!< group value sets
};

#endif
