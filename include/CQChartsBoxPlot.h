#ifndef CQChartsBoxPlot_H
#define CQChartsBoxPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsUtil.h>
#include <CQChartsValueInd.h>
#include <CQChartsBoxWhisker.h>

#include <map>

//---

// box plot type
class CQChartsBoxPlotType : public CQChartsPlotType {
 public:
  CQChartsBoxPlotType();

  QString name() const override { return "box"; }
  QString desc() const override { return "BoxPlot"; }

  const char *yColumnName() const override { return "y"; }

  bool allowXAxisIntegral() const override { return false; }

  bool allowXLog() const override { return false; }

  void addParameters() override;

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsBoxPlot;

// box value
struct CQChartsBoxPlotValue {
  CQChartsBoxPlotValue() = default;

  CQChartsBoxPlotValue(double value, const QModelIndex &ind) :
   value(value), ind(ind) {
  }

  operator double() const { return value; }

  double      value { 0.0 };
  QModelIndex ind;
};

//---

using CQChartsBoxPlotWhisker = CQChartsBoxWhiskerT<CQChartsBoxPlotValue>;

struct CQChartsBoxWhiskerData {
  using Outliers = std::vector<double>;

  QModelIndex ind;
  QString     name;
  double      x      { 0.0 };
  double      min    { 0.0 };
  double      lower  { 0.0 };
  double      median { 0.0 };
  double      upper  { 0.0 };
  double      max    { 0.0 };
  Outliers    outliers;
};

//---

// box plot whisker object
class CQChartsBoxPlotWhiskerObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsBoxPlotWhiskerObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect, int setId,
                            const CQChartsBoxPlotWhisker &whisker, int ig, int ng, int is, int ns);

  QString calcId() const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

  CQChartsGeom::BBox annotationBBox() const;

 private:
  CQChartsBoxPlot*       plot_    { nullptr }; // parent plot
  int                    setId_   { 0 };       // set id
  CQChartsBoxPlotWhisker whisker_;             // whisker data
  int                    ig_      { -1 };      // group index
  int                    ng_      { 0 };       // group count
  int                    is_      { -1 };      // value set index
  int                    ns_      { 0 };       // value set count
};

//---

// box plot whisker object
class CQChartsBoxPlotDataObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsBoxPlotDataObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect,
                         const CQChartsBoxWhiskerData &data);

  QString calcId() const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

  CQChartsGeom::BBox annotationBBox() const;

  double remapY(double y) const;

 private:
  CQChartsBoxPlot*       plot_    { nullptr }; // parent plot
  CQChartsBoxWhiskerData data_;              // whisker data
  mutable double         yrange_  { 1.0 };
  mutable double         ymargin_ { 0.05 };
};

//---

// box plot connected objects
class CQChartsBoxPlotConnectedObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsBoxPlotConnectedObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect,
                              int groupId, int i, int n);

  QString calcId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &) const override { }

  void addColumnSelectIndex(Indices &, const CQChartsColumn &) const override { }

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

 private:
  void initPolygon();

 private:
  CQChartsBoxPlot* plot_    { nullptr }; // parent plot
  int              groupId_ { -1 };      // group index
  int              i_       { -1 };      // group index
  int              n_       { 0 };       // group count
  QPolygonF        line_;
  QPolygonF        poly_;
};

//---

#include <CQChartsKey.h>

class CQChartsBoxKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsBoxKeyColor(CQChartsBoxPlot *plot, int i, int n);

  bool selectPress(const CQChartsGeom::Point &p) override;

  QBrush fillBrush() const override;
};

class CQChartsBoxKeyText : public CQChartsKeyText {
  Q_OBJECT

 public:
  CQChartsBoxKeyText(CQChartsBoxPlot *plot, int i, const QString &text);

  QColor interpTextColor(int i, int n) const override;

 private:
  int i_ { 0 };
};

//---

// box plot
class CQChartsBoxPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(CQChartsColumn xColumn           READ xColumn           WRITE setXColumn          )
  Q_PROPERTY(CQChartsColumn yColumn           READ yColumn           WRITE setYColumn          )
  Q_PROPERTY(CQChartsColumn groupColumn       READ groupColumn       WRITE setGroupColumn      )
  Q_PROPERTY(CQChartsColumn minColumn         READ minColumn         WRITE setMinColumn        )
  Q_PROPERTY(CQChartsColumn lowerMedianColumn READ lowerMedianColumn WRITE setLowerMedianColumn)
  Q_PROPERTY(CQChartsColumn medianColumn      READ medianColumn      WRITE setMedianColumn     )
  Q_PROPERTY(CQChartsColumn upperMedianColumn READ upperMedianColumn WRITE setUpperMedianColumn)
  Q_PROPERTY(CQChartsColumn maxColumn         READ maxColumn         WRITE setMaxColumn        )
  Q_PROPERTY(CQChartsColumn outliersColumn    READ outliersColumn    WRITE setOutliersColumn   )

  Q_PROPERTY(bool           skipOutliers     READ isSkipOutliers   WRITE setSkipOutliers    )
  Q_PROPERTY(bool           connected        READ isConnected      WRITE setConnected       )
  Q_PROPERTY(double         whiskerRange     READ whiskerRange     WRITE setWhiskerRange    )
  Q_PROPERTY(CQChartsLength boxWidth         READ boxWidth         WRITE setBoxWidth        )
  Q_PROPERTY(bool           boxFilled        READ isBoxFilled      WRITE setBoxFilled       )
  Q_PROPERTY(CQChartsColor  boxColor         READ boxColor         WRITE setBoxColor        )
  Q_PROPERTY(double         boxAlpha         READ boxAlpha         WRITE setBoxAlpha        )
  Q_PROPERTY(Pattern        boxPattern       READ boxPattern       WRITE setBoxPattern      )
  Q_PROPERTY(bool           boxStroked       READ isBorderStroked  WRITE setBorderStroked   )
  Q_PROPERTY(CQChartsColor  borderColor      READ borderColor      WRITE setBorderColor     )
  Q_PROPERTY(double         borderAlpha      READ borderAlpha      WRITE setBorderAlpha     )
  Q_PROPERTY(CQChartsLength borderWidth      READ borderWidth      WRITE setBorderWidth     )
  Q_PROPERTY(CQChartsLength cornerSize       READ cornerSize       WRITE setCornerSize      )
  Q_PROPERTY(CQChartsColor  whiskerColor     READ whiskerColor     WRITE setWhiskerColor    )
  Q_PROPERTY(double         whiskerAlpha     READ whiskerAlpha     WRITE setWhiskerAlpha    )
  Q_PROPERTY(CQChartsLength whiskerLineWidth READ whiskerLineWidth WRITE setWhiskerLineWidth)
  Q_PROPERTY(double         whiskerExtent    READ whiskerExtent    WRITE setWhiskerExtent   )
  Q_PROPERTY(bool           textVisible      READ isTextVisible    WRITE setTextVisible     )
  Q_PROPERTY(CQChartsColor  textColor        READ textColor        WRITE setTextColor       )
  Q_PROPERTY(double         textAlpha        READ textAlpha        WRITE setTextAlpha       )
  Q_PROPERTY(QFont          textFont         READ textFont         WRITE setTextFont        )
  Q_PROPERTY(double         textMargin       READ textMargin       WRITE setTextMargin      )
  Q_PROPERTY(double         symbolSize       READ symbolSize       WRITE setSymbolSize      )

  Q_ENUMS(Pattern)

 public:
  enum class Pattern {
    SOLID,
    HATCH,
    DENSE,
    HORIZ,
    VERT,
    FDIAG,
    BDIAG
  };

  using SetWhiskerMap      = std::map<int,CQChartsBoxPlotWhisker>;
  using GroupSetWhiskerMap = std::map<int,SetWhiskerMap>;
  using WhiskerDataList    = std::vector<CQChartsBoxWhiskerData>;

 public:
  CQChartsBoxPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsBoxPlot();

  //---

  const CQChartsColumn &xColumn() const { return xColumn_; }
  void setXColumn(const CQChartsColumn &c) { xColumn_ = c; updateRangeAndObjs(); }

  const CQChartsColumn &yColumn() const { return yColumn_; }
  void setYColumn(const CQChartsColumn &c) { yColumn_ = c; updateRangeAndObjs(); }

  const CQChartsColumn &groupColumn() const { return groupColumn_; }
  void setGroupColumn(const CQChartsColumn &c) { groupColumn_ = c; updateRangeAndObjs(); }

  //---

  const CQChartsColumn &minColumn() const { return minColumn_; }
  void setMinColumn(const CQChartsColumn &c) { minColumn_ = c; updateRangeAndObjs(); }

  const CQChartsColumn &lowerMedianColumn() const { return lowerMedianColumn_; }
  void setLowerMedianColumn(const CQChartsColumn &c) { lowerMedianColumn_ = c;
    updateRangeAndObjs(); }

  const CQChartsColumn &medianColumn() const { return medianColumn_; }
  void setMedianColumn(const CQChartsColumn &c) { medianColumn_ = c; updateRangeAndObjs(); }

  const CQChartsColumn &upperMedianColumn() const { return upperMedianColumn_; }
  void setUpperMedianColumn(const CQChartsColumn &c) { upperMedianColumn_ = c;
    updateRangeAndObjs(); }

  const CQChartsColumn &maxColumn() const { return maxColumn_; }
  void setMaxColumn(const CQChartsColumn &c) { maxColumn_ = c; updateRangeAndObjs(); }

  const CQChartsColumn &outliersColumn() const { return outliersColumn_; }
  void setOutliersColumn(const CQChartsColumn &c) { outliersColumn_ = c; updateRangeAndObjs(); }

  //---

  bool isSkipOutliers() const { return skipOutliers_; }
  void setSkipOutliers(bool b) { skipOutliers_ = b; updateRangeAndObjs(); }

  bool isConnected() const { return connected_; }
  void setConnected(bool b) { connected_ = b; updateRangeAndObjs(); }

  double whiskerRange() const { return whiskerRange_; }
  void setWhiskerRange(double r) { whiskerRange_ = r; updateRangeAndObjs(); }

  const CQChartsLength &boxWidth() const { return boxWidth_; }
  void setBoxWidth(const CQChartsLength &l) { boxWidth_ = l; update(); }

  //---

  bool isBoxFilled() const;
  void setBoxFilled(bool b);

  const CQChartsColor &boxColor() const;
  void setBoxColor(const CQChartsColor &c);

  double boxAlpha() const;
  void setBoxAlpha(double r);

  Pattern boxPattern() const;
  void setBoxPattern(Pattern pattern);

  QColor interpBoxColor(int i, int n) const;

  //---

  bool isBorderStroked() const;
  void setBorderStroked(bool b);

  const CQChartsColor &borderColor() const;
  void setBorderColor(const CQChartsColor &c);

  double borderAlpha() const;
  void setBorderAlpha(double r);

  const CQChartsLength &borderWidth() const;
  void setBorderWidth(const CQChartsLength &l);

  const CQChartsLength &cornerSize() const;
  void setCornerSize(const CQChartsLength &r);

  QColor interpBorderColor(int i, int n) const;

  //---

  const CQChartsColor &whiskerColor() const;
  void setWhiskerColor(const CQChartsColor &c);

  double whiskerAlpha() const;
  void setWhiskerAlpha(double a);

  const CQChartsLength &whiskerLineWidth() const { return whiskerData_.width; }
  void setWhiskerLineWidth(const CQChartsLength &l) { whiskerData_.width = l; update(); }

  double whiskerExtent() const { return whiskerExtent_; }
  void setWhiskerExtent(double r) { whiskerExtent_ = r; update(); }

  QColor interpWhiskerColor(int i, int n) const;

  //---

  bool isTextVisible() const;
  void setTextVisible(bool b);

  const CQChartsColor &textColor() const;
  void setTextColor(const CQChartsColor &c);

  double textAlpha() const;
  void setTextAlpha(double a);

  const QFont &textFont() const;
  void setTextFont(const QFont &f);

  double textMargin() const { return textMargin_; }
  void setTextMargin(double r) { textMargin_ = r; update(); }

  QColor interpTextColor(int i, int n) const;

  //---

  double symbolSize() const { return symbolSize_; }
  void setSymbolSize(double r) { symbolSize_ = r; update(); }

  //---

  bool isGrouped() const { return grouped_; }

  int numGroups() const { return groupWhiskers_.size(); }

  const GroupSetWhiskerMap &groupWhiskers() const { return groupWhiskers_; }

  QString setIdName(int setId) const { return xValueInd_.idName(setId); }

  QString groupIdName(int groupId) const { return groupValueInd_.idName(groupId); }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void updateRawRange ();
  void updateCalcRange();

  void updateRawWhiskers();

  CQChartsGeom::BBox annotationBBox() const override;

  bool initObjs() override;

  bool initRawObjs ();
  bool initCalcObjs();

  void addKeyItems(CQChartsPlotKey *key) override;

  //---

  bool probe(ProbeData &probeData) const override;

  void draw(QPainter *) override;

 private:
  void addRawWhiskerRow(const QModelIndex &parent, int r);

 private:
  CQChartsColumn     xColumn_;                             // x column
  CQChartsColumn     yColumn_;                             // y column
  CQChartsColumn     groupColumn_;                         // grouping column
  CQChartsColumn     minColumn_;                           // min column
  CQChartsColumn     lowerMedianColumn_;                   // lower median column
  CQChartsColumn     medianColumn_;                        // median column
  CQChartsColumn     upperMedianColumn_;                   // upper median column
  CQChartsColumn     maxColumn_;                           // max column
  CQChartsColumn     outliersColumn_;                      // outliers column
  bool               skipOutliers_   { false };            // skip outliers
  bool               connected_      { false };            // connect boxes
  double             whiskerRange_   { 1.5 };              // whisker range
  CQChartsLength     boxWidth_       { 0.2 };              // box width
  CQChartsLineData   whiskerData_;                         // whisker stroke
  double             whiskerExtent_  { 0.2 };              // whisker extent
  CQChartsBoxData    boxData_;                             // shape fill/border style
  CQChartsTextData   textData_;                            // text style
  double             textMargin_     { 2 };                // text margin
  double             symbolSize_     { 4 };                // symbol size
  ColumnType         groupType_      { ColumnType::NONE }; // group column data type
  ColumnType         xType_          { ColumnType::NONE }; // x column data type
  GroupSetWhiskerMap groupWhiskers_;                       // whisker data
  CQChartsValueInd   groupValueInd_;                       // group value inds
  WhiskerDataList    whiskerDataList_;
  CQChartsValueInd   xValueInd_;                           // x value inds
  bool               grouped_        { false };            // is grouped values
};

#endif
