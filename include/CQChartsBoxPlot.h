#ifndef CQChartsBoxPlot_H
#define CQChartsBoxPlot_H

#include <CQChartsGroupPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsUtil.h>
#include <CQChartsValueInd.h>
#include <CQChartsBoxWhisker.h>
#include <CQChartsGeom.h>

#include <map>

//---

// box plot type
class CQChartsBoxPlotType : public CQChartsGroupPlotType {
 public:
  CQChartsBoxPlotType();

  QString name() const override { return "boxplot"; }
  QString desc() const override { return "BoxPlot"; }

  const char *yColumnName() const override { return "value"; }

  bool allowXAxisIntegral() const override { return false; }

  bool allowXLog() const override { return false; }

  Dimension dimension() const override { return Dimension::ONE_D; }

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
  double      x       { 0.0 };
  double      min     { 0.0 };
  double      lower   { 0.0 };
  double      median  { 0.5 };
  double      upper   { 0.0 };
  double      max     { 1.0 };
  Outliers    outliers;
  double      dataMin { 0.0 };
  double      dataMax { 1.0 };
};

//---

// box plot base object
class CQChartsBoxPlotObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsBoxPlotObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect);

  void drawHText(QPainter *painter, double xl, double xr, double y,
                 const QString &text, bool onLeft);
  void drawVText(QPainter *painter, double yt, double yb, double x,
                 const QString &text, bool onBottom);

  void addHBBox(CQChartsGeom::BBox &pbbox, double xl, double xr, double y,
                const QString &text, bool onLeft) const;
  void addVBBox(CQChartsGeom::BBox &pbbox, double yb, double yt, double x,
                const QString &text, bool onBottom) const;

 protected:
  CQChartsBoxPlot* plot_ { nullptr }; // parent plot
};

//---

// box plot whisker object
class CQChartsBoxPlotWhiskerObj : public CQChartsBoxPlotObj {
  Q_OBJECT

  Q_PROPERTY(double min    READ min   )
  Q_PROPERTY(double lower  READ lower )
  Q_PROPERTY(double median READ median)
  Q_PROPERTY(double upper  READ upper )
  Q_PROPERTY(double max    READ max   )

 public:
  CQChartsBoxPlotWhiskerObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect, int setId,
                            int groupInd, const CQChartsBoxPlotWhisker &whisker,
                            int ig, int ng, int is, int ns);

  double min   () const;
  double lower () const;
  double median() const;
  double upper () const;
  double max   () const;

  QString calcId   () const override;
  QString calcTipId() const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

  CQChartsGeom::BBox annotationBBox() const;

 private:
  double remapPos(double pos) const;

 private:
  int                    setId_    { 0 };  // set id
  int                    groupInd_ { 0 };  // set id
  CQChartsBoxPlotWhisker whisker_;         // whisker data
  int                    ig_       { -1 }; // group index
  int                    ng_       { 0 };  // group count
  int                    is_       { -1 }; // value set index
  int                    ns_       { 0 };  // value set count
};

//---

// box plot whisker object
class CQChartsBoxPlotDataObj : public CQChartsBoxPlotObj {
  Q_OBJECT

 public:
  CQChartsBoxPlotDataObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect,
                         const CQChartsBoxWhiskerData &data);

  QString calcId   () const override;
  QString calcTipId() const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

  CQChartsGeom::BBox annotationBBox() const;

 private:
  double remapPos(double pos) const;

 private:
  CQChartsBoxWhiskerData data_;             // whisker data
  double                 ymargin_ { 0.05 };
};

//---

// box plot connected objects
class CQChartsBoxPlotConnectedObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsBoxPlotConnectedObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect,
                              int groupInd, int i, int n);

  QString calcId   () const override;
  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &) const override { }

  void addColumnSelectIndex(Indices &, const CQChartsColumn &) const override { }

  void draw(QPainter *painter) override;

 private:
  using SetWhiskerMap = std::map<int,CQChartsBoxPlotWhisker>;

  const SetWhiskerMap &setWhiskerMap() const;

  void initPolygon();

 private:
  CQChartsBoxPlot* plot_     { nullptr }; // parent plot
  int              groupInd_ { -1 };      // group ind
  int              i_        { -1 };      // group index
  int              n_        { 0 };       // group count
  QPolygonF        line_;
  QPolygonF        poly_;
};

//---

// box plot connected objects
class CQChartsBoxPlotPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsBoxPlotPointObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect,
                         int setId, int groupInd, const QPointF &p, const QModelIndex &ind,
                         int ig, int ng, int is, int ns, int iv, int nv);

  QString calcId   () const override;
  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &) const override;

  void addColumnSelectIndex(Indices &, const CQChartsColumn &) const override;

  void draw(QPainter *painter) override;

 private:
  CQChartsBoxPlot* plot_     { nullptr }; // parent plot
  int              setId_    { -1 };      // set id
  int              groupInd_ { -1 };      // group id
  QPointF          p_;                    // point
  QModelIndex      ind_;                  // model index
  int              ig_       { -1 };      // group index
  int              ng_       { 0 };       // group count
  int              is_       { -1 };      // set index
  int              ns_       { 0 };       // set count
  int              iv_       { -1 };      // value index
  int              nv_       { 0 };       // value count
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
class CQChartsBoxPlot : public CQChartsGroupPlot {
  Q_OBJECT

  // data
  Q_PROPERTY(CQChartsColumn valueColumn  READ valueColumn     WRITE setValueColumn    )
  Q_PROPERTY(QString        valueColumns READ valueColumnsStr WRITE setValueColumnsStr)
  Q_PROPERTY(CQChartsColumn nameColumn   READ nameColumn      WRITE setNameColumn     )
  Q_PROPERTY(CQChartsColumn setColumn    READ setColumn       WRITE setSetColumn      )

  Q_PROPERTY(CQChartsColumn xColumn           READ xColumn           WRITE setXColumn          )
  Q_PROPERTY(CQChartsColumn minColumn         READ minColumn         WRITE setMinColumn        )
  Q_PROPERTY(CQChartsColumn lowerMedianColumn READ lowerMedianColumn WRITE setLowerMedianColumn)
  Q_PROPERTY(CQChartsColumn medianColumn      READ medianColumn      WRITE setMedianColumn     )
  Q_PROPERTY(CQChartsColumn upperMedianColumn READ upperMedianColumn WRITE setUpperMedianColumn)
  Q_PROPERTY(CQChartsColumn maxColumn         READ maxColumn         WRITE setMaxColumn        )
  Q_PROPERTY(CQChartsColumn outliersColumn    READ outliersColumn    WRITE setOutliersColumn   )

  // options
  Q_PROPERTY(bool   showOutliers READ isShowOutliers WRITE setShowOutliers)
  Q_PROPERTY(bool   connected    READ isConnected    WRITE setConnected   )
  Q_PROPERTY(double whiskerRange READ whiskerRange   WRITE setWhiskerRange)
  Q_PROPERTY(bool   horizontal   READ isHorizontal   WRITE setHorizontal  )
  Q_PROPERTY(bool   normalized   READ isNormalized   WRITE setNormalized  )
  Q_PROPERTY(bool   jitterPoints READ isJitterPoints WRITE setJitterPoints)

  // box
  Q_PROPERTY(CQChartsLength   boxWidth    READ boxWidth        WRITE setBoxWidth     )
  Q_PROPERTY(bool             boxFilled   READ isBoxFilled     WRITE setBoxFilled    )
  Q_PROPERTY(CQChartsColor    boxColor    READ boxColor        WRITE setBoxColor     )
  Q_PROPERTY(double           boxAlpha    READ boxAlpha        WRITE setBoxAlpha     )
  Q_PROPERTY(Pattern          boxPattern  READ boxPattern      WRITE setBoxPattern   )
  Q_PROPERTY(bool             boxStroked  READ isBorderStroked WRITE setBorderStroked)
  Q_PROPERTY(CQChartsColor    borderColor READ borderColor     WRITE setBorderColor  )
  Q_PROPERTY(double           borderAlpha READ borderAlpha     WRITE setBorderAlpha  )
  Q_PROPERTY(CQChartsLength   borderWidth READ borderWidth     WRITE setBorderWidth  )
  Q_PROPERTY(CQChartsLineDash borderDash  READ borderDash      WRITE setBorderDash   )
  Q_PROPERTY(CQChartsLength   cornerSize  READ cornerSize      WRITE setCornerSize   )

  // whisker
  Q_PROPERTY(CQChartsColor  whiskerColor     READ whiskerColor     WRITE setWhiskerColor    )
  Q_PROPERTY(double         whiskerAlpha     READ whiskerAlpha     WRITE setWhiskerAlpha    )
  Q_PROPERTY(CQChartsLength whiskerLineWidth READ whiskerLineWidth WRITE setWhiskerLineWidth)
  Q_PROPERTY(double         whiskerExtent    READ whiskerExtent    WRITE setWhiskerExtent   )

  // labels
  Q_PROPERTY(bool          textVisible READ isTextVisible WRITE setTextVisible)
  Q_PROPERTY(CQChartsColor textColor   READ textColor     WRITE setTextColor  )
  Q_PROPERTY(double        textAlpha   READ textAlpha     WRITE setTextAlpha  )
  Q_PROPERTY(QFont         textFont    READ textFont      WRITE setTextFont   )
  Q_PROPERTY(double        textMargin  READ textMargin    WRITE setTextMargin )

  // outliers
  Q_PROPERTY(CQChartsSymbol symbolType        READ symbolType        WRITE setSymbolType       )
  Q_PROPERTY(CQChartsLength symbolSize        READ symbolSize        WRITE setSymbolSize       )
  Q_PROPERTY(bool           symbolStroked     READ isSymbolStroked   WRITE setSymbolStroked    )
  Q_PROPERTY(CQChartsColor  symbolStrokeColor READ symbolStrokeColor WRITE setSymbolStrokeColor)
  Q_PROPERTY(double         symbolStrokeAlpha READ symbolStrokeAlpha WRITE setSymbolStrokeAlpha)
  Q_PROPERTY(CQChartsLength symbolStrokeWidth READ symbolStrokeWidth WRITE setSymbolStrokeWidth)
  Q_PROPERTY(bool           symbolFilled      READ isSymbolFilled    WRITE setSymbolFilled     )
  Q_PROPERTY(CQChartsColor  symbolFillColor   READ symbolFillColor   WRITE setSymbolFillColor  )
  Q_PROPERTY(double         symbolFillAlpha   READ symbolFillAlpha   WRITE setSymbolFillAlpha  )
  Q_PROPERTY(Pattern        symbolFillPattern READ symbolFillPattern WRITE setSymbolFillPattern)

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

  const CQChartsColumn &valueColumn() const { return valueColumns_.column(); }
  void setValueColumn(const CQChartsColumn &c);

  const Columns &valueColumns() const { return valueColumns_.columns(); }
  void setValueColumns(const Columns &valueColumns);

  QString valueColumnsStr() const;
  bool setValueColumnsStr(const QString &s);

  const CQChartsColumn &valueColumnAt(int i) const;

  int numValueColumns() const;

  //---

  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c);

  const CQChartsColumn &setColumn() const { return setColumn_; }
  void setSetColumn(const CQChartsColumn &c);

  //---

  const CQChartsColumn &xColumn() const { return xColumn_; }
  void setXColumn(const CQChartsColumn &c);

  const CQChartsColumn &minColumn() const { return minColumn_; }
  void setMinColumn(const CQChartsColumn &c);

  const CQChartsColumn &lowerMedianColumn() const { return lowerMedianColumn_; }
  void setLowerMedianColumn(const CQChartsColumn &c);

  const CQChartsColumn &medianColumn() const { return medianColumn_; }
  void setMedianColumn(const CQChartsColumn &c);

  const CQChartsColumn &upperMedianColumn() const { return upperMedianColumn_; }
  void setUpperMedianColumn(const CQChartsColumn &c);

  const CQChartsColumn &maxColumn() const { return maxColumn_; }
  void setMaxColumn(const CQChartsColumn &c);

  const CQChartsColumn &outliersColumn() const { return outliersColumn_; }
  void setOutliersColumn(const CQChartsColumn &c);

  //---

  bool isShowOutliers() const { return showOutliers_; }
  void setShowOutliers(bool b);

  bool isConnected() const { return connected_; }
  void setConnected(bool b);

  double whiskerRange() const { return whiskerRange_; }
  void setWhiskerRange(double r);

  const CQChartsLength &boxWidth() const { return boxWidth_; }
  void setBoxWidth(const CQChartsLength &l);

  //---

  bool isHorizontal() const { return horizontal_; }

  bool isNormalized() const { return normalized_; }

  bool isJitterPoints() const { return jitterPoints_; }

  //---

  // whisker box
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

  const CQChartsLineDash &borderDash() const;
  void setBorderDash(const CQChartsLineDash &l);

  const CQChartsLength &cornerSize() const;
  void setCornerSize(const CQChartsLength &r);

  QColor interpBorderColor(int i, int n) const;

  //---

  const CQChartsColor &whiskerColor() const;
  void setWhiskerColor(const CQChartsColor &c);

  double whiskerAlpha() const;
  void setWhiskerAlpha(double a);

  const CQChartsLength &whiskerLineWidth() const { return whiskerData_.width; }
  void setWhiskerLineWidth(const CQChartsLength &l);

  double whiskerExtent() const { return whiskerExtent_; }
  void setWhiskerExtent(double r);

  QColor interpWhiskerColor(int i, int n) const;

  //---

  // label
  bool isTextVisible() const;
  void setTextVisible(bool b);

  const CQChartsColor &textColor() const;
  void setTextColor(const CQChartsColor &c);

  double textAlpha() const;
  void setTextAlpha(double a);

  const QFont &textFont() const;
  void setTextFont(const QFont &f);

  double textMargin() const { return textMargin_; }
  void setTextMargin(double r);

  QColor interpTextColor(int i, int n) const;

  //---

  // symbol
  const CQChartsSymbol &symbolType() const { return symbolData_.type; }
  void setSymbolType(const CQChartsSymbol &t);

  const CQChartsLength &symbolSize() const { return symbolData_.size; }
  void setSymbolSize(const CQChartsLength &s);

  //--

  bool isSymbolStroked() const { return symbolData_.stroke.visible; }
  void setSymbolStroked(bool b);

  const CQChartsColor &symbolStrokeColor() const;
  void setSymbolStrokeColor(const CQChartsColor &c);

  QColor interpSymbolStrokeColor(int i, int n) const;

  double symbolStrokeAlpha() const;
  void setSymbolStrokeAlpha(double a);

  const CQChartsLength &symbolStrokeWidth() const { return symbolData_.stroke.width; }
  void setSymbolStrokeWidth(const CQChartsLength &l);

  //--

  bool isSymbolFilled() const { return symbolData_.fill.visible; }
  void setSymbolFilled(bool b);

  const CQChartsColor &symbolFillColor() const;
  void setSymbolFillColor(const CQChartsColor &c);

  QColor interpSymbolFillColor(int i, int n) const;

  double symbolFillAlpha() const;
  void setSymbolFillAlpha(double a);

  Pattern symbolFillPattern() const;
  void setSymbolFillPattern(const Pattern &p);

  //---

  bool isGrouped() const { return grouped_; }

  int numGroups() const { return groupWhiskers_.size(); }

  const GroupSetWhiskerMap &groupWhiskers() const { return groupWhiskers_; }

  QString setIdName(int setId) const { return setValueInd_.idName(setId); }

  //---

  void addProperties() override;

  //---

  bool isPreCalc() const;

  //---

  void updateRange(bool apply=true) override;

  void updateRawRange ();
  void updateCalcRange();

  bool hasSets  () const;
  bool hasGroups() const;

  void updateRawWhiskers();

  CQChartsGeom::BBox annotationBBox() const override;

  bool initObjs() override;

  bool initRawObjs ();
  bool initCalcObjs();

  void addKeyItems(CQChartsPlotKey *key) override;

  //---

  bool probe(ProbeData &probeData) const override;

  //---

  bool addMenuItems(QMenu *menu) override;

  //---

 public slots:
  // set horizontal
  void setHorizontal(bool b);

  // set normalized
  void setNormalized(bool b);

  // set jitter points
  void setJitterPoints(bool b);

 private:
  void addCalcRow(const QModelIndex &ind, int r, WhiskerDataList &dataList);

  void addRawWhiskerRow(const QModelIndex &parent, int r);

 private:
  CQChartsColumns       valueColumns_   { 1 };                // value columns
  CQChartsColumn        nameColumn_;                          // name column
  CQChartsColumn        setColumn_;                           // set column

  CQChartsColumn        xColumn_;                             // x column
  CQChartsColumn        minColumn_;                           // min column
  CQChartsColumn        lowerMedianColumn_;                   // lower median column
  CQChartsColumn        medianColumn_;                        // median column
  CQChartsColumn        upperMedianColumn_;                   // upper median column
  CQChartsColumn        maxColumn_;                           // max column
  CQChartsColumn        outliersColumn_;                      // outliers column

  bool                  showOutliers_   { true };             // show outliers
  bool                  connected_      { false };            // connect boxes
  double                whiskerRange_   { 1.5 };              // whisker range
  bool                  horizontal_     { false };            // horizontal bars
  bool                  normalized_     { false };            // normalized values
  bool                  jitterPoints_   { false };            // show jitter points
  CQChartsLength        boxWidth_       { 0.2 };              // box width
  CQChartsLineData      whiskerData_;                         // whisker stroke
  double                whiskerExtent_  { 0.2 };              // whisker extent
  CQChartsGeom::RMinMax xrange_;                              // x range
  CQChartsBoxData       boxData_;                             // shape fill/border style
  CQChartsTextData      textData_;                            // text style
  double                textMargin_     { 2 };                // text margin
  CQChartsSymbolData    symbolData_;                          // outlier symbol data
  ColumnType            setType_        { ColumnType::NONE }; // set column data type
  GroupSetWhiskerMap    groupWhiskers_;                       // grouped whisker data
  WhiskerDataList       whiskerDataList_;                     // whisker data
  CQChartsValueInd      setValueInd_;                         // set value inds
  bool                  grouped_        { false };            // is grouped values
};

#endif
