#ifndef CQChartsPointPlot_H
#define CQChartsPointPlot_H

#include <CQChartsGroupPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsAxisRug.h>
#include <CQChartsFitData.h>
#include <CQStatData.h>

class CQChartsPointPlot;
class CQChartsDataLabel;
class CQChartsGrahamHull;
class CQChartsSymbolTypeMapKey;
class CQChartsSymbolSizeMapKey;
class CQChartsFitData;

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
};

//---

/*!
 * \brief Point Plot Best Fit object
 * \ingroup Charts
 */
class CQChartsPointBestFitObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Plot    = CQChartsPointPlot;
  using BestFit = CQChartsFitData;

 public:
  CQChartsPointBestFitObj(const Plot *plot, int groupInd, const QString &name,
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

  bool drawMouseOver() const override { return false; }

 private:
  BestFit *getBestFit() const;

 private:
  const Plot* plot_     { nullptr }; //!< scatter plot
  int         groupInd_ { -1 };      //!< plot group index
  QString     name_;                 //!< plot set name
};

//---

/*!
 * \brief Point Plot Hull object
 * \ingroup Charts
 */
class CQChartsPointHullObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Plot = CQChartsPointPlot;
  using Hull = CQChartsGrahamHull;

 public:
  CQChartsPointHullObj(const Plot *plot, int groupInd, const QString &name,
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

  bool drawMouseOver() const override { return false; }

 private:
  Hull *getHull() const;

 private:
  const Plot* plot_     { nullptr }; //!< scatter plot
  int         groupInd_ { -1 };      //!< plot group index
  QString     name_;                 //!< plot set name
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
 public CQChartsObjStatsLineData   <CQChartsPointPlot> {
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

  // symbol size map
  Q_PROPERTY(bool          symbolSizeMapped   READ isSymbolSizeMapped WRITE setSymbolSizeMapped  )
  Q_PROPERTY(double        symbolSizeMapMin   READ symbolSizeMapMin   WRITE setSymbolSizeMapMin  )
  Q_PROPERTY(double        symbolSizeMapMax   READ symbolSizeMapMax   WRITE setSymbolSizeMapMax  )
  Q_PROPERTY(CQChartsUnits symbolSizeMapUnits READ symbolSizeMapUnits WRITE setSymbolSizeMapUnits)

  // font size map
  Q_PROPERTY(bool          fontSizeMapped   READ isFontSizeMapped WRITE setFontSizeMapped  )
  Q_PROPERTY(double        fontSizeMapMin   READ fontSizeMapMin   WRITE setFontSizeMapMin  )
  Q_PROPERTY(double        fontSizeMapMax   READ fontSizeMapMax   WRITE setFontSizeMapMax  )
  Q_PROPERTY(CQChartsUnits fontSizeMapUnits READ fontSizeMapUnits WRITE setFontSizeMapUnits)

  // text labels
  Q_PROPERTY(bool pointLabels READ isPointLabels WRITE setPointLabels)

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
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Stats, stats)

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
  const Column &symbolTypeColumn() const { return symbolTypeData_.column; }
  void setSymbolTypeColumn(const Column &c);

  bool isSymbolTypeMapped() const { return symbolTypeData_.mapped; }
  void setSymbolTypeMapped(bool b);

  long symbolTypeDataMin() const { return symbolTypeData_.data_min; }
  long symbolTypeDataMax() const { return symbolTypeData_.data_max; }

  long symbolTypeMapMin() const { return symbolTypeData_.map_min; }
  void setSymbolTypeMapMin(long i);

  long symbolTypeMapMax() const { return symbolTypeData_.map_max; }
  void setSymbolTypeMapMax(long i);

  const QString &symbolTypeSetName() const { return symbolTypeData_.setName; }
  void setSymbolTypeSetName(const QString &s);

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

  //---

  // symbol size column and map
  const Column &symbolSizeColumn() const { return symbolSizeData_.column; }
  void setSymbolSizeColumn(const Column &c);

  bool isSymbolSizeMapped() const { return symbolSizeData_.mapped; }
  void setSymbolSizeMapped(bool b);

  double symbolSizeDataMin() const { return symbolSizeData_.data_min; }
  double symbolSizeDataMax() const { return symbolSizeData_.data_max; }

  double symbolSizeMapMin() const { return symbolSizeData_.map_min; }
  void setSymbolSizeMapMin(double r);
  double symbolSizeMapMax() const { return symbolSizeData_.map_max; }
  void setSymbolSizeMapMax(double r);

  double symbolSizeUserMapMin() const { return symbolSizeData_.user_map_min; }
  void setSymbolSizeUserMapMin(double r);
  double symbolSizeUserMapMax() const { return symbolSizeData_.user_map_max; }
  void setSymbolSizeUserMapMax(double r);

  const CQChartsUnits &symbolSizeMapUnits() const { return symbolSizeData_.units; }
  void setSymbolSizeMapUnits(const CQChartsUnits &u);

  virtual void setFixedSymbolSize(const Length &s) = 0;
  virtual const Length &fixedSymbolSize() const = 0;

  virtual void setFixedSymbol(const Symbol &s) = 0;
  virtual const Symbol &fixedSymbol() const = 0;

  CQChartsSymbolSizeMapKey *symbolSizeMapKey() const { return symbolSizeMapKey_.get(); }

  bool symbolSizeVisible(const Length &size) const override;

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

  //---

  void updateMapKey(CQChartsMapKey *key) const override;

  //---

  // label font size column and map
  const Column &fontSizeColumn() const { return fontSizeData_.column; }
  void setFontSizeColumn(const Column &c);

  bool isFontSizeMapped() const { return fontSizeData_.mapped; }

  void setFontSizeMapped(bool b);

  double fontSizeMapMin() const { return fontSizeData_.map_min; }
  void setFontSizeMapMin(double r);
  double fontSizeMapMax() const { return fontSizeData_.map_max; }
  void setFontSizeMapMax(double r);

  double fontSizeUserMapMin() const { return fontSizeData_.user_map_min; }
  void setFontSizeUserMapMin(double r);
  double fontSizeUserMapMax() const { return fontSizeData_.user_map_max; }
  void setFontSizeUserMapMax(double r);

  const CQChartsUnits &fontSizeMapUnits() const { return fontSizeData_.units; }
  void setFontSizeMapUnits(const CQChartsUnits &u);

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
  void drawBestFit(PaintDevice *device, const BestFit *fitData, const ColorInd &ic) const;

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

 public slots:
  void setPointLabels(bool b);

  // symbol keys
  void setSymbolSizeMapKey(bool b);
  void setSymbolTypeMapKey(bool b);

  void symbolSizeSelected(const CQChartsLength &size, bool visible);
  void symbolTypeSelected(const CQChartsSymbol &symbol, bool visible);

  // overlays
  void setBestFit       (bool b);
  void setHull          (bool b);
  void setStatsLinesSlot(bool b);

  // rug
  void setXRug(bool b);
  void setYRug(bool b);

 protected slots:
  void dataLabelChanged();

 signals:
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
  using Real             = CQChartsGeom::Real;
  using SymbolSizeFilter = std::set<Real>;
  using SymbolTypeFilter = std::set<Symbol>;

  DataLabelP dataLabel_; //!< data label style

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
  BestFitData bestFitData_; //!< best fit data
  HullData    hullData_;    //!< hull data

  // group data
  GroupPoints     groupPoints_;     //!< group fit points
  GroupStatData   groupStatData_;   //!< group stat data
  GroupNameValues groupNameValues_; //!< group name values (individual points)
  GroupFitData    groupFitData_;    //!< group fit data
  GroupHull       groupHull_;       //!< group hull

  RugP xRug_; //!< x rug
  RugP yRug_; //!< y rug

  // options
  Length minSymbolSize_; //!< min symbol size
  Length minLabelSize_;  //!< min label size
};

#endif
