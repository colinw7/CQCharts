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
  Q_PROPERTY(bool symbolTypeMapped READ isSymbolTypeMapped WRITE setSymbolTypeMapped)
  Q_PROPERTY(int  symbolTypeMapMin READ symbolTypeMapMin   WRITE setSymbolTypeMapMin)
  Q_PROPERTY(int  symbolTypeMapMax READ symbolTypeMapMax   WRITE setSymbolTypeMapMax)

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
  const Column &symbolTypeColumn() const;
  void setSymbolTypeColumn(const Column &c);

  bool isSymbolTypeMapped() const;
  void setSymbolTypeMapped(bool b);

  int symbolTypeMapMin() const;
  void setSymbolTypeMapMin(int i);

  int symbolTypeMapMax() const;
  void setSymbolTypeMapMax(int i);

  //---

  // symbol size column and map
  const Column &symbolSizeColumn() const;
  void setSymbolSizeColumn(const Column &c);

  bool isSymbolSizeMapped() const;
  void setSymbolSizeMapped(bool b);

  double symbolSizeMapMin() const;
  void setSymbolSizeMapMin(double r);

  double symbolSizeMapMax() const;
  void setSymbolSizeMapMax(double r);

  const QString &symbolSizeMapUnits() const;
  void setSymbolSizeMapUnits(const QString &s);

  //---

  // label font size column and map
  const Column &fontSizeColumn() const;
  void setFontSizeColumn(const Column &c);

  bool isFontSizeMapped() const;
  void setFontSizeMapped(bool b);

  double fontSizeMapMin() const;
  void setFontSizeMapMin(double r);

  double fontSizeMapMax() const;
  void setFontSizeMapMax(double r);

  const QString &fontSizeMapUnits() const;
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

  const CQChartsSymbol &xRugSymbolType() const;
  void setXRugSymbolType(const CQChartsSymbol &s);

  const Length &xRugSymbolSize() const;
  void setXRugSymbolSize(const Length &l);

  //--

  // axis y rug
  bool isYRug() const;

  const CQChartsAxisRug::Side &yRugSide() const;
  void setYRugSide(const CQChartsAxisRug::Side &s);

  const CQChartsSymbol &yRugSymbolType() const;
  void setYRugSymbolType(const CQChartsSymbol &s);

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
  void setPointLabels(bool b);

  void setDataLabelFont(const CQChartsFont &font);

  //---

  void write(std::ostream &os, const QString &plotVarName, const QString &modelVarName,
             const QString &viewVarName) const override;

 protected:
  virtual void resetBestFit() { }

  //---

  void initSymbolTypeData() const;
  bool columnSymbolType(int row, const QModelIndex &parent, CQChartsSymbol &symbolType) const;

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
  // overlays
  void setBestFit       (bool b);
  void setHull          (bool b);
  void setStatsLinesSlot(bool b);

  // rug
  void setXRug(bool b);
  void setYRug(bool b);

 protected slots:
  void dataLabelChanged();

 protected:
  struct BestFitData {
    bool      visible         { false };                 //!< show fit
    bool      showDeviation   { false };                 //!< show fit deviation
    int       order           { 3 };                     //!< fit order
    bool      includeOutliers { true };                  //!< include outliers
    DrawLayer layer           { DrawLayer::BACKGROUND }; //!< draw layer
  };

  struct HullData {
    bool      visible { false };                 //!< show convex hull
    DrawLayer layer   { DrawLayer::BACKGROUND }; //!< draw layer
  };

  struct StatData {
    CQStatData xstat;
    CQStatData ystat;
  };

 protected:
  using GroupPoints   = std::map<int, Points>;
  using GroupStatData = std::map<int, StatData>;
  using GroupFitData  = std::map<int, BestFit *>;
  using GroupHull     = std::map<int, Hull *>;
  using RugP          = std::unique_ptr<CQChartsAxisRug>;

  CQChartsDataLabel* dataLabel_ { nullptr }; //!< data label style

  // custom column data
  SymbolTypeData symbolTypeData_; //!< symbol type column data
  SymbolSizeData symbolSizeData_; //!< symbol size column data
  FontSizeData   fontSizeData_;   //!< font size column data

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

#endif
