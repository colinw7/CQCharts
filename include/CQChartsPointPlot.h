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

  Dimension dimension() const override { return Dimension::TWO_D; }

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
  Q_PROPERTY(int     symbolTypeMapMin  READ symbolTypeMapMin   WRITE setSymbolTypeMapMin )
  Q_PROPERTY(int     symbolTypeMapMax  READ symbolTypeMapMax   WRITE setSymbolTypeMapMax )
  Q_PROPERTY(QString symbolTypeSetName READ symbolTypeSetName  WRITE setSymbolTypeSetName)

  // symbol size map
  Q_PROPERTY(bool    symbolSizeMapped   READ isSymbolSizeMapped WRITE setSymbolSizeMapped  )
  Q_PROPERTY(double  symbolSizeMapMin   READ symbolSizeMapMin   WRITE setSymbolSizeMapMin  )
  Q_PROPERTY(double  symbolSizeMapMax   READ symbolSizeMapMax   WRITE setSymbolSizeMapMax  )
  Q_PROPERTY(QString symbolSizeMapUnits READ symbolSizeMapUnits WRITE setSymbolSizeMapUnits)

  // font size map
  Q_PROPERTY(bool    fontSizeMapped   READ isFontSizeMapped WRITE setFontSizeMapped  )
  Q_PROPERTY(double  fontSizeMapMin   READ fontSizeMapMin   WRITE setFontSizeMapMin  )
  Q_PROPERTY(double  fontSizeMapMax   READ fontSizeMapMax   WRITE setFontSizeMapMax  )
  Q_PROPERTY(QString fontSizeMapUnits READ fontSizeMapUnits WRITE setFontSizeMapUnits)

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
  using Symbol     = CQChartsSymbol;

 public:
  CQChartsPointPlot(View *view, PlotType *plotType, const ModelP &model);
 ~CQChartsPointPlot();

  //---

  void init() override;
  void term() override;

  //---

  // data label
  const DataLabel *dataLabel() const { return dataLabel_; }
  DataLabel *dataLabel() { return dataLabel_; }

  //---

  // symbol type column and map
  const Column &symbolTypeColumn() const { return symbolTypeData_.column; }
  void setSymbolTypeColumn(const Column &c);

  bool isSymbolTypeMapped() const { return symbolTypeData_.mapped; }
  void setSymbolTypeMapped(bool b);

  int symbolTypeDataMin() const { return symbolTypeData_.data_min; }
  int symbolTypeDataMax() const { return symbolTypeData_.data_max; }

  int symbolTypeMapMin() const { return symbolTypeData_.map_min; }
  void setSymbolTypeMapMin(int i);

  int symbolTypeMapMax() const { return symbolTypeData_.map_max; }
  void setSymbolTypeMapMax(int i);

  const QString &symbolTypeSetName() const { return symbolTypeData_.setName; }
  void setSymbolTypeSetName(const QString &s);

  //---

  // symbol type map key
  void addSymbolTypeMapKey();

  void addSymbolTypeMapKeyProperties();

  bool isSymbolTypeMapKey() const;

  //---

  bool canDrawSymbolTypeMapKey() const;
  void drawSymbolTypeMapKey(PaintDevice *device) const;

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

  const QString &symbolSizeMapUnits() const { return symbolSizeData_.units; }
  void setSymbolSizeMapUnits(const QString &s);

  virtual void setFixedSymbolSize(const Length &s) = 0;
  virtual const Length &fixedSymbolSize() const = 0;

  virtual void setFixedSymbolType(const Symbol &s) = 0;
  virtual const Symbol &fixedSymbolType() const = 0;

  //---

  // symbol size map key
  void addSymbolSizeMapKey();

  void addSymbolSizeMapKeyProperties();

  bool isSymbolSizeMapKey() const;

  const Alpha &symbolSizeMapKeyAlpha() const;
  void setSymbolSizeMapKeyAlpha(const Alpha &a);

  double symbolSizeMapKeyMargin() const;
  void setSymbolSizeMapKeyMargin(double r);

  //---

  bool canDrawSymbolSizeMapKey() const;
  void drawSymbolSizeMapKey(PaintDevice *device) const;

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

  const QString &fontSizeMapUnits() const { return fontSizeData_.units; }
  void setFontSizeMapUnits(const QString &s);

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

  const Symbol &xRugSymbolType() const;
  void setXRugSymbolType(const Symbol &s);

  const Length &xRugSymbolSize() const;
  void setXRugSymbolSize(const Length &l);

  //--

  // axis y rug
  bool isYRug() const;

  const CQChartsAxisRug::Side &yRugSide() const;
  void setYRugSide(const CQChartsAxisRug::Side &s);

  const Symbol &yRugSymbolType() const;
  void setYRugSymbolType(const Symbol &s);

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
  void addRugProperties();

  void getPropertyNames(QStringList &names, bool hidden) const override;

  //---

  // data labels
  bool isPointLabels() const;

  CQChartsLabelPosition dataLabelPosition() const;
  void setDataLabelPosition(CQChartsLabelPosition position);

  Font dataLabelFont() const;
  void setDataLabelFont(const Font &font, bool notify=true);

  //---

  void write(std::ostream &os, const QString &plotVarName, const QString &modelVarName,
             const QString &viewVarName) const override;

 protected:
  virtual void resetBestFit() { }

  //---

  void initSymbolTypeData() const;
  bool columnSymbolType(int row, const QModelIndex &parent,
                        Symbol &symbolType, OptBool &symbolFilled) const;

  //---

  void initSymbolSizeData() const;
  bool columnSymbolSize(int row, const QModelIndex &parent, Length &symbolSize) const;

  //---

  void initFontSizeData() const;
  bool columnFontSize(int row, const QModelIndex &parent, Length &fontSize) const;

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

  // emitted when symbol size details changed (symbol size column, symbol size range)
  void symbolTypeDetailsChanged();

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
  using GroupPoints   = std::map<int, Points>;
  using GroupStatData = std::map<int, StatData>;
  using GroupFitData  = std::map<int, BestFit *>;
  using GroupHull     = std::map<int, Hull *>;
  using RugP          = std::unique_ptr<CQChartsAxisRug>;

  DataLabel* dataLabel_ { nullptr }; //!< data label style

  // custom column data
  SymbolTypeData symbolTypeData_; //!< symbol type column data
  SymbolSizeData symbolSizeData_; //!< symbol size column data
  FontSizeData   fontSizeData_;   //!< font size column data

  // symbol size and type map keys
  using SymbolTypeMapKeyP = std::unique_ptr<CQChartsSymbolTypeMapKey>;
  using SymbolSizeMapKeyP = std::unique_ptr<CQChartsSymbolSizeMapKey>;

  SymbolTypeMapKeyP symbolTypeMapKey_; //!< symbol type map key
  SymbolSizeMapKeyP symbolSizeMapKey_; //!< symbol size map key

  // plot overlay data
  BestFitData bestFitData_; //!< best fit data
  HullData    hullData_;    //!< hull data

  // group data
  GroupPoints     groupPoints_;     //!< group fit points
  GroupStatData   groupStatData_;   //!< group stat data
  GroupNameValues groupNameValues_; //!< group name values (individual points)
  GroupFitData    groupFitData_;    //!< group fit data
  GroupHull       groupHull_;       //!< group hull

  RugP xRug_; //! x rug
  RugP yRug_; //! y rug
};

//---

class CQChartsLengthEdit;
class CQChartsSymbolEdit;
class CQChartsSymbolSetEdit;
class CQIntRangeSlider;

class CQChartsPointPlotCustomControls : public CQChartsGroupPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsPointPlotCustomControls(CQCharts *charts, const QString &plotType);

  void setPlot(CQChartsPlot *plot) override;

  void addSymbolSizeWidgets();

 protected slots:
  void symbolSizeDetailsSlot();
  void symbolTypeDetailsSlot();

  void symbolSizeLengthSlot();
  void symbolSizeColumnSlot();
  void symbolSizeRangeSlot(double min, double max);

  void symbolTypeSlot();
  void symbolTypeColumnSlot();
  void symbolTypeRangeSlot(int min, int max);
  void symbolTypeSetSlot(const QString &name);

 public slots:
  void updateWidgets() override;

 protected:
  void connectSlots(bool b);

 private:
  using ColumnEdits = std::vector<CQChartsColumnParameterEdit *>;

  CQChartsPointPlot*     plot_                  { nullptr };
  CQChartsLengthEdit*    symbolSizeLengthEdit_  { nullptr };
  CQChartsColumnCombo*   symbolSizeColumnCombo_ { nullptr };
  CQDoubleRangeSlider*   symbolSizeRange_       { nullptr };
  CQChartsSymbolEdit*    symbolTypeEdit_        { nullptr };
  CQChartsColumnCombo*   symbolTypeColumnCombo_ { nullptr };
  CQIntRangeSlider*      symbolTypeRange_       { nullptr };
  CQChartsSymbolSetEdit* symbolTypeSetEdit_     { nullptr };
};

#endif
