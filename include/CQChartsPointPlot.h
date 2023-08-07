#ifndef CQChartsPointPlot_H
#define CQChartsPointPlot_H

#include <CQChartsGroupPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsAxisRug.h>
#include <CQStatData.h>

class CQChartsPointPlot;
class CQChartsDataLabel;
class CQChartsGrahamHull;
class CQChartsSymbolTypeMapKey;
class CQChartsSymbolSizeMapKey;
class CQChartsFitData;
class CQChartsTextPlacer;

/*!
 * \brief Point plot type (Base class for XY and Symbol Plot Types)
 * \ingroup Charts
 */
class CQChartsPointPlotType : public CQChartsGroupPlotType {
 public:
  CQChartsPointPlotType();

  Category category() const override { return Category::TWO_D; }

  QString xColumnName() const override { return "x"; }
  QString yColumnName() const override { return "y"; }

  void addMappingParameters();
  void addMiscParameters();
};

//---

/*!
 * \brief Point Plot Best Fit object
 * \ingroup Charts
 */
class CQChartsPointBestFitObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using PointPlot = CQChartsPointPlot;
  using BestFit   = CQChartsFitData;

 public:
  CQChartsPointBestFitObj(const PointPlot *plot, int groupInd, const QString &name,
                          const ColorInd &ig, const ColorInd &is, const BBox &rect);

  int groupInd() const { return groupInd_; }

  const QString &name() const { return name_; }

  //---

  QString typeName() const override { return "best_fit"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  //---

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  bool drawMouseOver() const override { return false; }

 private:
  BestFit *getBestFit() const;

 private:
  const PointPlot* pointPlot_ { nullptr }; //!< point plot
  int              groupInd_  { -1 };      //!< plot group index
  QString          name_;                  //!< plot set name
};

//---

/*!
 * \brief Point Plot Hull object
 * \ingroup Charts
 */
class CQChartsPointHullObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using PointPlot = CQChartsPointPlot;
  using Hull      = CQChartsGrahamHull;

 public:
  CQChartsPointHullObj(const PointPlot *plot, int groupInd, const QString &name,
                       const ColorInd &ig, const ColorInd &is, const BBox &rect);

  int groupInd() const { return groupInd_; }

  const QString &name() const { return name_; }

  //---

  QString typeName() const override { return "hull"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  //---

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  bool drawMouseOver() const override { return false; }

 private:
  Hull *getHull() const;

 private:
  const PointPlot* pointPlot_ { nullptr }; //!< point plot
  int              groupInd_  { -1 };      //!< plot group index
  QString          name_;                  //!< plot set name
};

//---

CQCHARTS_NAMED_SHAPE_DATA(Hull, hull)

/*!
 * \brief Point Plot Base Class for Scatter/XY Plots
 * \ingroup Charts
 */
class CQChartsPointPlot : public CQChartsGroupPlot,
 public CQChartsObjBestFitShapeData<CQChartsPointPlot>,
 public CQChartsObjHullShapeData   <CQChartsPointPlot>,
 public CQChartsObjStatsShapeData  <CQChartsPointPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn symbolTypeColumn READ symbolTypeColumn WRITE setSymbolTypeColumn)
  Q_PROPERTY(CQChartsColumn symbolSizeColumn READ symbolSizeColumn WRITE setSymbolSizeColumn)
  Q_PROPERTY(CQChartsColumn fontSizeColumn   READ fontSizeColumn   WRITE setFontSizeColumn  )

  // symbol type map
  Q_PROPERTY(bool    symbolTypeMapped  READ isSymbolTypeMapped WRITE setSymbolTypeMapped )
  Q_PROPERTY(long    symbolTypeMapMin  READ symbolTypeMapMin   WRITE setSymbolTypeMapMin )
  Q_PROPERTY(long    symbolTypeMapMax  READ symbolTypeMapMax   WRITE setSymbolTypeMapMax )
  Q_PROPERTY(QString symbolTypeSetName READ symbolTypeSetName  WRITE setSymbolTypeSetName)

  Q_PROPERTY(CQChartsSymbolTypeMap symbolTypeMap READ symbolTypeMap WRITE setSymbolTypeMap)

  // symbol size map
  Q_PROPERTY(bool          symbolSizeMapped   READ isSymbolSizeMapped WRITE setSymbolSizeMapped)
  Q_PROPERTY(double        symbolSizeMapMin   READ symbolSizeMapMin   WRITE setSymbolSizeMapMin)
  Q_PROPERTY(double        symbolSizeMapMax   READ symbolSizeMapMax   WRITE setSymbolSizeMapMax)
  Q_PROPERTY(CQChartsUnits symbolSizeMapUnits READ symbolSizeUnits    WRITE setSymbolSizeUnits )

  Q_PROPERTY(CQChartsSymbolSizeMap symbolSizeMap READ symbolSizeMap WRITE setSymbolSizeMap)

  // font size map
  Q_PROPERTY(bool          fontSizeMapped   READ isFontSizeMapped WRITE setFontSizeMapped)
  Q_PROPERTY(double        fontSizeMapMin   READ fontSizeMapMin   WRITE setFontSizeMapMin)
  Q_PROPERTY(double        fontSizeMapMax   READ fontSizeMapMax   WRITE setFontSizeMapMax)
  Q_PROPERTY(CQChartsUnits fontSizeMapUnits READ fontSizeUnits    WRITE setFontSizeUnits )

  // text labels
  Q_PROPERTY(bool pointLabels READ isPointLabels WRITE setPointLabels)
  Q_PROPERTY(bool adjustText  READ isAdjustText  WRITE setAdjustText)

  // best fit
  Q_PROPERTY(bool      bestFit          READ isBestFit          WRITE setBestFit         )
  Q_PROPERTY(bool      bestFitOutliers  READ isBestFitOutliers  WRITE setBestFitOutliers )
  Q_PROPERTY(int       bestFitOrder     READ bestFitOrder       WRITE setBestFitOrder    )
  Q_PROPERTY(bool      bestFitDeviation READ isBestFitDeviation WRITE setBestFitDeviation)
  Q_PROPERTY(DrawLayer bestFitLayer     READ bestFitLayer       WRITE setBestFitLayer    )

  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(BestFit, bestFit)

  // convex hull
  Q_PROPERTY(bool      hull      READ isHull    WRITE setHull     )
  Q_PROPERTY(DrawLayer hullLayer READ hullLayer WRITE setHullLayer)

  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Hull, hull)

  // stats
  Q_PROPERTY(bool statsLines READ isStatsLines WRITE setStatsLines)

  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Stats, stats)

  // split grouping
  Q_PROPERTY(bool   splitGroups  READ isSplitGroups  WRITE setSplitGroups )
  Q_PROPERTY(bool   splitSharedY READ isSplitSharedY WRITE setSplitSharedY)
  Q_PROPERTY(double splitMargin  READ splitMargin    WRITE setSplitMargin )

  // axis rug
  Q_PROPERTY(bool                  xRug     READ isXRug   WRITE setXRug    )
  Q_PROPERTY(CQChartsAxisRug::Side xRugSide READ xRugSide WRITE setXRugSide)
  Q_PROPERTY(bool                  yRug     READ isYRug   WRITE setYRug    )
  Q_PROPERTY(CQChartsAxisRug::Side yRugSide READ yRugSide WRITE setYRugSide)

  // symbol size map key
  Q_PROPERTY(bool          symbolSizeMapKey
             READ isSymbolSizeMapKey     WRITE setSymbolSizeMapKey      )
  Q_PROPERTY(CQChartsAlpha symbolSizeMapKeyAlpha
             READ symbolSizeMapKeyAlpha  WRITE setSymbolSizeMapKeyAlpha )
  Q_PROPERTY(double        symbolSizeMapKeyMargin
             READ symbolSizeMapKeyMargin WRITE setSymbolSizeMapKeyMargin)

  // symbol type map key
  Q_PROPERTY(bool symbolTypeMapKey READ isSymbolTypeMapKey WRITE setSymbolTypeMapKey)

  // options
  Q_PROPERTY(bool outlineSelected READ isOutlineSelected WRITE setOutlineSelected)

  Q_PROPERTY(CQChartsLength minSymbolSize READ minSymbolSize WRITE setMinSymbolSize)
  Q_PROPERTY(CQChartsLength minLabelSize  READ minLabelSize  WRITE setMinLabelSize )

  Q_ENUMS(DrawLayer)

 public:
  enum class DrawLayer {
    NONE,
    BACKGROUND,
    MIDDLE,
    FOREGROUND
  };

 protected:
  using DataLabel  = CQChartsDataLabel;
  using Hull       = CQChartsGrahamHull;
  using HullObj    = CQChartsPointHullObj;
  using BestFit    = CQChartsFitData;
  using BestFitObj = CQChartsPointBestFitObj;
  using Length     = CQChartsLength;
  using Color      = CQChartsColor;
  using Alpha      = CQChartsAlpha;
  using ColorInd   = CQChartsUtil::ColorInd;
  using PenBrush   = CQChartsPenBrush;
  using PenData    = CQChartsPenData;
  using BrushData  = CQChartsBrushData;
  using Symbol     = CQChartsSymbol;
  using LineDash   = CQChartsLineDash;
  using SymbolType = CQChartsSymbolType;

 public:
  CQChartsPointPlot(View *view, PlotType *plotType, const ModelP &model);
 ~CQChartsPointPlot();

  //---

  void init() override;
  void term() override;

  //---

  // data label
  const DataLabel *dataLabel() const { return dataLabel_.get(); }
  DataLabel *dataLabel() { return dataLabel_.get(); }

  //---

  // symbol type column and map
  const SymbolTypeData &symbolTypeData() const { return symbolTypeData_; }

  const Column &symbolTypeColumn() const { return symbolTypeData_.column(); }
  void setSymbolTypeColumn(const Column &c);

  bool isSymbolTypeMapped() const { return symbolTypeData_.isMapped(); }
  void setSymbolTypeMapped(bool b);

  long symbolTypeDataMin() const { return symbolTypeData_.dataMin(); }
  long symbolTypeDataMax() const { return symbolTypeData_.dataMax(); }

  long symbolTypeMapMin() const { return symbolTypeData_.mapMin(); }
  void setSymbolTypeMapMin(long i);

  long symbolTypeMapMax() const { return symbolTypeData_.mapMax(); }
  void setSymbolTypeMapMax(long i);

  const QString &symbolTypeSetName() const { return symbolTypeData_.setName(); }
  void setSymbolTypeSetName(const QString &s);

  const CQChartsSymbolTypeMap &symbolTypeMap() const { return symbolTypeData_.typeMap(); }
  void setSymbolTypeMap(const CQChartsSymbolTypeMap &s);

  //---

  // symbol type map key
  void addSymbolTypeMapKey();

  bool canDrawSymbolTypeMapKey() const;
  void drawSymbolTypeMapKey(PaintDevice *device) const;

  void updateSymbolTypeMapKey() const;

  bool isSymbolTypeMapKey() const;

  void addSymbolTypeMapKeyProperties();

  CQChartsSymbolTypeMapKey *symbolTypeMapKey() const { return symbolTypeMapKey_.get(); }

  bool symbolTypeVisible(const Symbol &symbol) const override;

  QStringList symbolTypeFilterNames() const;
  void setSymbolTypeFilterNames(const QStringList &names);

  //---

  // symbol size column and map
  const SymbolSizeData &symbolSizeData() const { return symbolSizeData_; }

  const Column &symbolSizeColumn() const { return symbolSizeData_.column(); }
  void setSymbolSizeColumn(const Column &c);

  bool isSymbolSizeMapped() const { return symbolSizeData_.isMapped(); }
  void setSymbolSizeMapped(bool b);

  double symbolSizeDataMin() const { return symbolSizeData_.dataMin(); }
  double symbolSizeDataMax() const { return symbolSizeData_.dataMax(); }

  double symbolSizeMapMin() const { return symbolSizeData_.mapMin(); }
  void setSymbolSizeMapMin(double r);

  double symbolSizeMapMax() const { return symbolSizeData_.mapMax(); }
  void setSymbolSizeMapMax(double r);

  double symbolSizeUserMapMin() const { return symbolSizeData_.userMapMin(); }
  void setSymbolSizeUserMapMin(double r);

  double symbolSizeUserMapMax() const { return symbolSizeData_.userMapMax(); }
  void setSymbolSizeUserMapMax(double r);

  const CQChartsUnits &symbolSizeUnits() const { return symbolSizeData_.units(); }
  void setSymbolSizeUnits(const CQChartsUnits &u);

  const CQChartsSymbolSizeMap &symbolSizeMap() const { return symbolSizeData_.sizeMap(); }
  void setSymbolSizeMap(const CQChartsSymbolSizeMap &s);

  //---

  // symbol size map key
  void addSymbolSizeMapKey();

  bool canDrawSymbolSizeMapKey() const;
  void drawSymbolSizeMapKey(PaintDevice *device) const;

  void updateSymbolSizeMapKey() const;

  bool isSymbolSizeMapKey() const;

  void addSymbolSizeMapKeyProperties();

  const Alpha &symbolSizeMapKeyAlpha() const;
  void setSymbolSizeMapKeyAlpha(const Alpha &a);

  double symbolSizeMapKeyMargin() const;
  void setSymbolSizeMapKeyMargin(double r);

  CQChartsSymbolSizeMapKey *symbolSizeMapKey() const { return symbolSizeMapKey_.get(); }

  bool symbolSizeVisible(const Length &size) const override;

  QStringList symbolSizeFilterNames() const;
  void setSymbolSizeFilterNames(const QStringList &names);

  //----

  virtual void setFixedSymbolSize(const Length &s) = 0;
  virtual const Length &fixedSymbolSize() const = 0;

  virtual void setFixedSymbol(const Symbol &s) = 0;
  virtual const Symbol &fixedSymbol() const = 0;

  //---

  void updateMapKey(CQChartsMapKey *key) const override;

  //---

  // label font size column and map
  const Column &fontSizeColumn() const { return fontSizeData_.column(); }
  void setFontSizeColumn(const Column &c);

  bool isFontSizeMapped() const { return fontSizeData_.isMapped(); }

  void setFontSizeMapped(bool b);

  double fontSizeMapMin() const { return fontSizeData_.mapMin(); }
  void setFontSizeMapMin(double r);
  double fontSizeMapMax() const { return fontSizeData_.mapMax(); }
  void setFontSizeMapMax(double r);

  double fontSizeUserMapMin() const { return fontSizeData_.userMapMin(); }
  void setFontSizeUserMapMin(double r);
  double fontSizeUserMapMax() const { return fontSizeData_.userMapMax(); }
  void setFontSizeUserMapMax(double r);

  const CQChartsUnits &fontSizeUnits() const { return fontSizeData_.units(); }
  void setFontSizeUnits(const CQChartsUnits &u);

  //---

  // best fit
  bool isBestFit() const { return bestFitData_.visible; }

  bool isBestFitOutliers() const { return bestFitData_.includeOutliers; }
  void setBestFitOutliers(bool b);

  int bestFitOrder() const { return bestFitData_.order; }
  void setBestFitOrder(int o);

  bool isBestFitDeviation() const { return bestFitData_.showDeviation; }
  void setBestFitDeviation(bool b);

  const DrawLayer &bestFitLayer() const { return bestFitData_.layer; }
  void setBestFitLayer(const DrawLayer &layer);

  //---

  // convex hull
  bool isHull() const { return hullData_.visible; }

  const DrawLayer &hullLayer() const { return hullData_.layer; }
  void setHullLayer(const DrawLayer &layer);

  //---

  // stats lines
  bool isStatsLines() const { return statsLines_; }

  //---

  // split groups
  bool isSplitGroups() const { return splitGroupData_.enabled; }
  void setSplitGroups(bool b);

  bool isSplitSharedY() const { return splitGroupData_.sharedY; }
  void setSplitSharedY(bool b);

  double splitMargin() const { return splitGroupData_.margin; }
  void setSplitMargin(double r);

  Point   adjustGroupPoint(int groupInd, const Point   &p   ) const;
  BBox    adjustGroupBBox (int groupInd, const BBox    &bbox) const;
  Polygon adjustGroupPoly (int groupInd, const Polygon &poly) const;

  const Range &getGroupRange(int groupInd) const;

  double mapGroupX(const Range &range, int groupInd, double x) const;
  double mapGroupY(const Range &range, double y) const;

  double unmapGroupX(const Range &range, int groupInd, double x) const;
  double unmapGroupY(const Range &range, double y) const;

  virtual int numVisibleGroups() const = 0;

  virtual int mapVisibleGroup(int groupInd) const = 0;
  virtual int unmapVisibleGroup(int groupInd) const = 0;

  //---

  // axis x rug
  bool isXRug() const;

  const CQChartsAxisRug::Side &xRugSide() const;
  void setXRugSide(const CQChartsAxisRug::Side &s);

  const Symbol &xRugSymbol() const;
  void setXRugSymbol(const Symbol &s);

  const Length &xRugSymbolSize() const;
  void setXRugSymbolSize(const Length &l);

  //--

  // axis y rug
  bool isYRug() const;

  const CQChartsAxisRug::Side &yRugSide() const;
  void setYRugSide(const CQChartsAxisRug::Side &s);

  const Symbol &yRugSymbol() const;
  void setYRugSymbol(const Symbol &s);

  const Length &yRugSymbolSize() const;
  void setYRugSymbolSize(const Length &l);

  //---

  void clearFitData ();

  BestFit *getBestFit(int ind, bool &created) const;

  //---

  void clearHullData();

  Hull *getHull(int ind, bool &created) const;

  //---

  virtual QString singleGroupName(ColorInd &) const { return ""; }

  //---

  void addPointProperties();

  void addBestFitProperties(bool hasLayer);
  void addHullProperties   (bool hasLayer);

  void addStatsProperties();
  void addSplitGroupsProperties();
  void addRugProperties(const QString &path);

  void getPropertyNames(QStringList &names, bool hidden) const override;

  //---

  // data labels
  bool isPointLabels() const;

  CQChartsLabelPosition dataLabelPosition() const;
  void setDataLabelPosition(CQChartsLabelPosition position);

  Font dataLabelFont() const;
  void setDataLabelFont(const Font &font, bool notify=true);

  Length dataLabelFontSize() const;
  void setDataLabelFontSize(const Length &l);

  //---

  //! get/set adjust text
  bool isAdjustText() const { return adjustText_; }
  void setAdjustText(bool b);

  //---

  //! get/set outline selected points
  bool isOutlineSelected() const { return outlineSelected_; }
  void setOutlineSelected(bool b) { outlineSelected_ = b; }

  //---

  //! get/set min symbol size
  const Length &minSymbolSize() const { return minSymbolSize_; }
  void setMinSymbolSize(const Length &l);

  //! get/set min label size
  const Length &minLabelSize() const { return minLabelSize_; }
  void setMinLabelSize(const Length &l);

  //---

  CQChartsGeom::BBox fitBBox() const override;

  //---

  void write(std::ostream &os, const QString &plotVarName, const QString &modelVarName,
             const QString &viewVarName) const override;

  //---

  bool executeSlotFn(const QString &name, const QVariantList &args, QVariant &res) override;

 protected:
  virtual void resetBestFit() { }

  //---

  void initSymbolTypeData() const;
  bool columnSymbolType(int row, const QModelIndex &parent, Symbol &symbolType) const;

  //---

  void initSymbolSizeData() const;
  bool columnSymbolSize(int row, const QModelIndex &parent, Length &symbolSize,
                        Qt::Orientation &sizeDir) const;

  //---

  void initFontSizeData() const;
  bool columnFontSize(int row, const QModelIndex &parent, Length &fontSize,
                      Qt::Orientation &sizeDir) const;

  //---

  virtual BestFitObj *createBestFitObj(int groupInd, const QString &name, const ColorInd &ig,
                                       const ColorInd &is, const BBox &rect) const;

  virtual HullObj *createHullObj(int groupInd, const QString &name, const ColorInd &ig,
                                 const ColorInd &is, const BBox &rect) const;

  //---

 public:
  using Points = std::vector<Point>;

  Points indPoints(const QVariant &var, int isGroup) const;

  //---

 public:
  void execDrawForeground(PaintDevice *device) const override;

  //---

  void drawBestFit(PaintDevice *device, const BestFit *fitData, const PenBrush &penBrush) const;

  void initGroupBestFit(BestFit *fitData, int ind, const QVariant &var, bool isGroup) const;

 protected:
  void initGroupStats(int ind, const QVariant &var, bool isGroup) const;

  //---

 public:
  //! point value data
  struct ValueData {
    Point       p;
    int         row { -1 };
    QModelIndex ind;
    Color       color;
    Alpha       alpha;

    ValueData(const Point &p=Point(), int row=-1, const QModelIndex &ind=QModelIndex(),
              const Color &color=Color(), const Alpha &alpha=Alpha()) :
     p(p), row(row), ind(ind), color(color), alpha(alpha) {
    }
  };

  using Values = std::vector<ValueData>;

  //! real values data
  struct ValuesData {
    Values  values;
    RMinMax xrange;
    RMinMax yrange;
  };

  using NameValues      = std::map<QString, ValuesData>;
  using GroupNameValues = std::map<int, NameValues>;

 public:
  const GroupNameValues &groupNameValues() const { return groupNameValues_; }

 public Q_SLOTS:
  void setPointLabels(bool b);

  // symbol keys
  void setSymbolSizeMapKey(bool b);
  void setSymbolTypeMapKey(bool b);

  void symbolSizeSelected(const CQChartsLength &size, bool visible);
  void symbolTypeSelected(const CQChartsSymbol &symbol, bool visible);

  // overlays
  void setBestFit   (bool b);
  void setHull      (bool b);
  void setStatsLines(bool b);

  // rug
  void setXRug(bool b);
  void setYRug(bool b);

 protected Q_SLOTS:
  void dataLabelChanged();

 Q_SIGNALS:
  // emitted when symbol size details changed (symbol size column, symbol size range)
  void symbolSizeDetailsChanged();

  // emitted when symbol type details changed (symbol type column, symbol type range)
  void symbolTypeDetailsChanged();

  // emitted when font size details changed (font size column, font size range)
  void fontSizeDetailsChanged();

 protected:
  //! best fit data
  struct BestFitData {
    bool      visible         { false };                 //!< show fit
    bool      showDeviation   { false };                 //!< show fit deviation
    int       order           { 3 };                     //!< fit order
    bool      includeOutliers { true };                  //!< include outliers
    DrawLayer layer           { DrawLayer::BACKGROUND }; //!< draw layer
  };

  //! hull data
  struct HullData {
    bool      visible { false };                 //!< show convex hull
    DrawLayer layer   { DrawLayer::BACKGROUND }; //!< draw layer
  };

  //! stat data
  struct StatData {
    CQStatData xstat;
    CQStatData ystat;
  };

  //! symbol size map key data
  struct SymbolSizeMapKeyData {
    bool displayed { false }; //!< is symbol size map key displayed
  };

  //! symbol type map key data
  struct SymbolTypeMapKeyData {
    bool displayed { false }; //!< is symbol type map key displayed
  };

 protected:
  using DataLabelP       = std::shared_ptr<DataLabel>;
  using GroupPoints      = std::map<int, Points>;
  using GroupStatData    = std::map<int, StatData>;
  using GroupFitData     = std::map<int, BestFit *>;
  using GroupHull        = std::map<int, Hull *>;
  using RugP             = std::unique_ptr<CQChartsAxisRug>;
  using SymbolSizeFilter = std::set<Length>;
  using SymbolTypeFilter = std::set<Symbol>;
  using GroupRange       = std::map<int, Range>;

  // label data
  DataLabelP dataLabel_;                               //!< data label style
  Length     dataLabelFontSize_ { Length::pixel(-1) }; //!< data label font size
  bool       adjustText_        { false };             //!< adjust text position

  // custom column data
  SymbolTypeData symbolTypeData_; //!< symbol type column data
  SymbolSizeData symbolSizeData_; //!< symbol size column data
  FontSizeData   fontSizeData_;   //!< font size column data

  // symbol size and type map keys
  using SymbolTypeMapKeyP = std::unique_ptr<CQChartsSymbolTypeMapKey>;
  using SymbolSizeMapKeyP = std::unique_ptr<CQChartsSymbolSizeMapKey>;

  SymbolTypeMapKeyP symbolTypeMapKey_; //!< symbol type map key
  SymbolSizeMapKeyP symbolSizeMapKey_; //!< symbol size map key

  SymbolSizeFilter symbolSizeFilter_; //!< symbol size map filter
  SymbolTypeFilter symbolTypeFilter_; //!< symbol type map filter

  // plot overlay data
  BestFitData bestFitData_;          //!< best fit data
  HullData    hullData_;             //!< hull data
  bool        statsLines_ { false }; //!< stats lines data

  // split groups
  struct SplitGroupData {
    bool   enabled { false }; //!< enabled
    bool   sharedY { true };  //!< shared y
    double margin  { 0.05 };  //!< margin
  };

  SplitGroupData splitGroupData_; //!< split group data

  // group data
  GroupPoints     groupPoints_;     //!< group fit points
  GroupStatData   groupStatData_;   //!< group stat data
  GroupNameValues groupNameValues_; //!< group name values (individual points)
  GroupFitData    groupFitData_;    //!< group fit data
  GroupHull       groupHull_;       //!< group hull

  RugP xRug_; //!< x rug
  RugP yRug_; //!< y rug

  // options
  bool outlineSelected_ { true }; //!< outline selected point

  Length minSymbolSize_; //!< min symbol size
  Length minLabelSize_;  //!< min label size

  mutable CQChartsTextPlacer *placer_;

  // combined and group ranges
  Range      range_;
  GroupRange groupRange_;

  using Axes = std::vector<CQChartsAxis *>;

  Axes xaxes_;
  Axes yaxes_;
};

#endif
