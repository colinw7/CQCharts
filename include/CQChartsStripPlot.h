#ifndef CQChartsStripPlot_H
#define CQChartsStripPlot_H

#include <CQChartsGroupPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsUtil.h>
#include <CQChartsValueInd.h>
#include <CQChartsBoxWhisker.h>
#include <CQChartsGeom.h>

#include <map>

class CQChartsStripPlot;

//---

/*!
 * \brief Box plot type
 * \ingroup Charts
 */
class CQChartsStripPlotType : public CQChartsGroupPlotType {
 public:
  CQChartsStripPlotType();

  QString name() const override { return "strip"; }
  QString desc() const override { return "Strip Plot"; }

  Category category() const override { return Category::ONE_D; }

  void addParameters() override;

  QString yColumnName() const override { return "value"; }

  bool allowXAxisIntegral() const override { return false; }

  bool allowXLog() const override { return false; }

  bool canProbe() const override { return true; }

  QString description() const override;

  //---

  void analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData) override;

  //---

  Plot *create(View *view, const ModelP &model) const override;
};

//---

/*!
 * \brief Strip Plot Point object
 * \ingroup Charts
 */
class CQChartsStripPointObj : public CQChartsPlotPointObj {
  Q_OBJECT

 public:
  using Plot   = CQChartsStripPlot;
  using Length = CQChartsLength;

 public:
  CQChartsStripPointObj(const Plot *plot, const BBox &rect, int groupInd,
                        const Point &p, const QModelIndex &ind, const ColorInd &ig,
                        const ColorInd &iv);

  //---

  const Plot *plot() const { return plot_; }

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  void getObjSelectIndices(Indices &inds) const override;

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  Length calcSymbolSize() const override;

 protected:
  const Plot* plot_     { nullptr }; //!< parent plot
  int         groupInd_ { 0 };       //!< group index
};

//---

/*!
 * \brief Strip Plot
 * \ingroup Charts
 */
class CQChartsStripPlot : public CQChartsGroupPlot,
 public CQChartsObjPointData<CQChartsStripPlot> {
  Q_OBJECT

  // calc value columns
  Q_PROPERTY(CQChartsColumn valueColumn    READ valueColumn    WRITE setValueColumn   )
  Q_PROPERTY(CQChartsColumn nameColumn     READ nameColumn     WRITE setNameColumn    )
  Q_PROPERTY(CQChartsColumn positionColumn READ positionColumn WRITE setPositionColumn)

  // options
  Q_PROPERTY(double margin READ margin WRITE setMargin)

  // symbol data
  CQCHARTS_POINT_DATA_PROPERTIES

 public:
  using Symbol     = CQChartsSymbol;
  using SymbolType = CQChartsSymbolType;
  using Length     = CQChartsLength;
  using Color      = CQChartsColor;
  using ColorInd   = CQChartsUtil::ColorInd;

 public:
  CQChartsStripPlot(View *view, const ModelP &model);
 ~CQChartsStripPlot();

  //---

  void init() override;
  void term() override;

  //---

  const Column &valueColumn() const { return valueColumn_; }
  void setValueColumn(const Column &c);

  const Column &nameColumn() const { return nameColumn_; }
  void setNameColumn(const Column &c);

  const Column &positionColumn() const { return positionColumn_; }
  void setPositionColumn(const Column &c);

  //---

  double margin() const { return margin_; }
  void setMargin(double r) { margin_ = r; }

  //---

  void addProperties() override;

  //---

  Range calcRange() const override;

  //---

  bool createObjs(PlotObjs &objs) const override;

  //---

  bool addMenuItems(QMenu *menu, const Point &p) override;

  //---

  void addKeyItems(PlotKey *) override;

  //---

  bool hasForeground() const override;

  void execDrawForeground(PaintDevice *) const override;

 protected:
  using PointObj = CQChartsStripPointObj;

 protected:
  void calcRowRange(const ModelVisitor::VisitData &data, Range &range) const;

  void addRowObj(const ModelVisitor::VisitData &data, PlotObjs &objs) const;

  virtual PointObj *createPointObj(const BBox &rect, int groupInd, const Point &p,
                                   const QModelIndex &ind, const ColorInd &ig,
                                   const ColorInd &iv) const;

 protected:
  CQChartsPlotCustomControls *createCustomControls() override;

 private:
  using Values = std::vector<double>;

  struct ValuesData {
    int    n { 0 };
    Values values;
  };

  struct PosValuesData {
    int    maxN { 0 };
    Values values;
  };

  using NamePos    = std::map<QString, int>;
  using YValues    = std::map<int, ValuesData>;
  using PosYValues = std::map<int, YValues>;
  using PosValues  = std::map<int, PosValuesData>;

  Column     valueColumn_;           //!< value column
  Column     nameColumn_;            //!< name column
  Column     positionColumn_;        //!< position column
  double     margin_       { 0.25 }; //!< strip margin
  NamePos    namePos_;               //!< name position
  IMinMax    posRange_;              //!< position range
  PosValues  posValues_;             //!< position values
  PosYValues posYValues_;            //!< position values per mapped y
};

//---

#include <CQChartsGroupPlotCustomControls.h>

/*!
 * \brief Strip Plot plot custom controls
 * \ingroup Charts
 */
class CQChartsStripPlotCustomControls : public CQChartsGroupPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsStripPlotCustomControls(CQCharts *charts);

  void init() override;

  void addWidgets() override;

  void setPlot(CQChartsPlot *plot) override;

 protected:
  CQChartsColor getColorValue() override;
  void setColorValue(const CQChartsColor &c) override;

 protected:
  CQChartsStripPlot* plot_ { nullptr };
};

#endif
