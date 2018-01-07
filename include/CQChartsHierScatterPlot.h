#ifndef CQChartsHierScatterPlot_H
#define CQChartsHierScatterPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsColorSet.h>
#include <CQChartsDataLabel.h>

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
  void setName(const QString &v) { name_ = v; }

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

class CQChartsHierScatterPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsHierScatterPointObj(CQChartsHierScatterPlot *plot, const CQChartsGeom::BBox &rect,
                              const QPointF &p, int i, int n);

  const QString &name() const { return name_; }
  void setName(const QString &v) { name_ = v; }

  const QModelIndex &ind() const { return ind_; }
  void setInd(const QModelIndex &v) { ind_ = v; }

  CQChartsHierScatterPointGroup *group() const { return group_; }
  void setGroup(CQChartsHierScatterPointGroup *p) { group_ = p; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void addSelectIndex() override;

  bool isIndex(const QModelIndex &) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

 private:
  CQChartsHierScatterPlot*       plot_ { nullptr };
  QPointF                        p_;
  int                            i_    { -1 };
  int                            n_    { -1 };
  QString                        name_;
  QModelIndex                    ind_;
  CQChartsHierScatterPointGroup* group_ { nullptr };
};

//---

#include <CQChartsKey.h>

class CQChartsHierScatterKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsHierScatterKeyColor(CQChartsHierScatterPlot *plot, CQChartsHierScatterPointGroup *group,
                              int i, int n);

  bool mousePress(const CQChartsGeom::Point &p) override;

  QBrush fillBrush() const override;

 private:
  CQChartsHierScatterPointGroup *group_ { nullptr };
};

//---

class CQChartsHierScatterPlotType : public CQChartsPlotType {
 public:
  CQChartsHierScatterPlotType();

  QString name() const override { return "hierscatter"; }
  QString desc() const override { return "HierScatter"; }

  void addParameters() override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsHierScatterPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int     xColumn           READ xColumn              WRITE setXColumn             )
  Q_PROPERTY(int     yColumn           READ yColumn              WRITE setYColumn             )
  Q_PROPERTY(int     nameColumn        READ nameColumn           WRITE setNameColumn          )
  Q_PROPERTY(QString groupColumnStr    READ groupColumnStr       WRITE setGroupColumnStr      )
  Q_PROPERTY(QString symbolBorderColor READ symbolBorderColorStr WRITE setSymbolBorderColorStr)
  Q_PROPERTY(double  symbolSize        READ symbolSize           WRITE setSymbolSize          )
  Q_PROPERTY(double  fontSize          READ fontSize             WRITE setFontSize            )
  Q_PROPERTY(bool    textLabels        READ isTextLabels         WRITE setTextLabels          )

 public:
  CQChartsHierScatterPlot(CQChartsView *view, const ModelP &model);
 ~CQChartsHierScatterPlot();

  //---

  int xColumn() const { return xColumn_; }
  void setXColumn(int i);

  int yColumn() const { return yColumn_; }
  void setYColumn(int i);

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i);

  const QString &groupColumnStr() const { return groupColumnStr_; }
  void setGroupColumnStr(const QString &s);

  //---

  QString symbolBorderColorStr() const { return symbolBorderColor_.colorStr(); }
  void setSymbolBorderColorStr(const QString &s) { symbolBorderColor_.setColorStr(s); update(); }

  QColor interpSymbolBorderColor(int i, int n) const {
    return symbolBorderColor_.interpColor(this, i, n); }

  double symbolSize() const { return symbolSize_; }
  void setSymbolSize(double s) { symbolSize_ = s; updateObjs(); }

  //---

  bool isTextLabels() const { return dataLabel_.isVisible(); }
  void setTextLabels(bool b) { dataLabel_.setVisible(b); }

  double fontSize() const { return fontSize_; }
  void setFontSize(double s) { fontSize_ = s; updateObjs(); }

  //---

  CQChartsHierScatterPointGroup *currentGroup() const { return currentGroup_; }
  void setCurrentGroup(CQChartsHierScatterPointGroup *group);

  //---

  const QString &xname() const { return xname_; }
  const QString &yname() const { return yname_; }

  //---

  const CQChartsDataLabel &dataLabel() const { return dataLabel_; }
  CQChartsDataLabel &dataLabel() { return dataLabel_; }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void initGroupValueSets();

  void updateObjs() override;

  bool initObjs() override;

  void addGroupPoints(CQChartsHierScatterPointGroup *baseGroup,
                      CQChartsHierScatterPointGroup *group);

  //---

  int numRows() const;

  int acceptsRow(int r) const;

  void addKeyItems(CQChartsKey *key) override;

  //---

  bool addMenuItems(QMenu *menu) override;

  void draw(QPainter *) override;

 private slots:
  void popCurrentGroup();

  void resetCurrentGroup();

 private:
  using GroupValues    = std::vector<int>;
  using GroupValueSets = std::map<int,CQChartsValueSet *>;

  int                            xColumn_           { 0 };
  int                            yColumn_           { 1 };
  int                            nameColumn_        { -1 };
  QString                        groupColumnStr_;
  CQChartsPaletteColor           symbolBorderColor_;
  double                         symbolSize_        { 4 };
  double                         fontSize_          { 8 };
  CQChartsHierScatterPointGroup* rootGroup_         { nullptr };
  CQChartsHierScatterPointGroup* currentGroup_      { nullptr };
  QStringList                    filterNames_;
  CQChartsDataLabel              dataLabel_;
  QString                        xname_;
  QString                        yname_;
  GroupValues                    groupValues_;
  GroupValueSets                 groupValueSets_;
};

#endif
