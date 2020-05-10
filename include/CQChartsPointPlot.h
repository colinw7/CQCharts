#ifndef CQChartsPointPlot_H
#define CQChartsPointPlot_H

#include <CQChartsGroupPlot.h>

class CQChartsDataLabel;
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

CQCHARTS_NAMED_SHAPE_DATA(Hull,hull)

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
  Q_PROPERTY(bool bestFit          READ isBestFit          WRITE setBestFit         )
  Q_PROPERTY(bool bestFitOutliers  READ isBestFitOutliers  WRITE setBestFitOutliers )
  Q_PROPERTY(int  bestFitOrder     READ bestFitOrder       WRITE setBestFitOrder    )
  Q_PROPERTY(bool bestFitDeviation READ isBestFitDeviation WRITE setBestFitDeviation)

  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(BestFit,bestFit)

  // convex hull
  Q_PROPERTY(bool hull READ isHull WRITE setHull)

  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Hull,hull)

  // stats
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Stats, stats)

 public:
  CQChartsPointPlot(CQChartsView *view, CQChartsPlotType *plotType, const ModelP &model);

 ~CQChartsPointPlot();

  //---

  // data label
  const CQChartsDataLabel *dataLabel() const { return dataLabel_; }
  CQChartsDataLabel *dataLabel() { return dataLabel_; }

  //---

  // symbol type column and map
  const CQChartsColumn &symbolTypeColumn() const;
  void setSymbolTypeColumn(const CQChartsColumn &c);

  bool isSymbolTypeMapped() const;
  void setSymbolTypeMapped(bool b);

  int symbolTypeMapMin() const;
  void setSymbolTypeMapMin(int i);

  int symbolTypeMapMax() const;
  void setSymbolTypeMapMax(int i);

  //---

  // symbol size column and map
  const CQChartsColumn &symbolSizeColumn() const;
  void setSymbolSizeColumn(const CQChartsColumn &c);

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
  const CQChartsColumn &fontSizeColumn() const;
  void setFontSizeColumn(const CQChartsColumn &c);

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

 //---

  // convex hull
  bool isHull() const { return hullData_.visible; }

  //---

  void addPointProperties();

  void addBestFitProperties();
  void addHullProperties();
  void addStatsProperties();

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
  void initSymbolTypeData() const;
  bool columnSymbolType(int row, const QModelIndex &parent, CQChartsSymbol &symbolType) const;

  //---

  void initSymbolSizeData() const;
  bool columnSymbolSize(int row, const QModelIndex &parent, CQChartsLength &symbolSize) const;

  //---

  void initFontSizeData() const;
  bool columnFontSize(int row, const QModelIndex &parent, CQChartsLength &fontSize) const;

  //---

 public:
  void drawBestFit(CQChartsPaintDevice *device, const CQChartsFitData &fitData,
                   const ColorInd &ic) const;

 public slots:
  // overlays
  void setBestFit       (bool b);
  void setHull          (bool b);
  void setStatsLinesSlot(bool b);

 protected slots:
  void dataLabelChanged();

 protected:
  struct BestFitData {
    bool visible         { false }; //!< show fit
    bool showDeviation   { false }; //!< show fit deviation
    int  order           { 3 };     //!< fit order
    bool includeOutliers { true };  //!< include outliers
  };

  struct HullData {
    bool visible { false }; //!< show convex hull
  };

 protected:
  CQChartsDataLabel* dataLabel_ { nullptr }; //!< data label style

  // custom column data
  SymbolTypeData symbolTypeData_; //!< symbol type column data
  SymbolSizeData symbolSizeData_; //!< symbol size column data
  FontSizeData   fontSizeData_;   //!< font size column data

  // plot overlay data
  BestFitData    bestFitData_; //!< best fit data
  HullData       hullData_;    //!< hull data
};

#endif
