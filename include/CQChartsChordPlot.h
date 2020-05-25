#ifndef CQChartsChordPlot_H
#define CQChartsChordPlot_H

#include <CQChartsConnectionPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsValueSet.h>
#include <QModelIndex>

class CQChartsRotatedTextBoxObj;

//---

/*!
 * \brief Chord Plot Type
 * \ingroup Charts
 */
class CQChartsChordPlotType : public CQChartsConnectionPlotType {
 public:
  using ColumnType = CQBaseModelType;

 public:
  CQChartsChordPlotType();

  QString name() const override { return "chord"; }
  QString desc() const override { return "Chord"; }

  void addParameters() override;

  bool customXRange() const override { return false; }
  bool customYRange() const override { return false; }

  bool hasAxes() const override { return false; }
  bool hasKey () const override { return false; }

  bool canProbe() const override { return false; }

  QString description() const override;

  //---

  bool isColumnForParameter(CQChartsModelColumnDetails *columnDetails,
                            CQChartsPlotParameter *parameter) const override;

  void analyzeModel(CQChartsModelData *modelData,
                    CQChartsAnalyzeModelData &analyzeModelData) override;

  //---

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsChordPlot;

//---

class CQChartsChordData {
 public:
  using Group       = CQChartsConnectionPlot::TableConnectionData::Group;
  using Values      = CQChartsConnectionPlot::TableConnectionData::Values;
  using PrimaryType = CQChartsConnectionPlot::TableConnectionData::PrimaryType;

 public:
  CQChartsChordData() { }

  const CQChartsConnectionPlot::TableConnectionData &data() const { return data_; }
  void setData(CQChartsConnectionPlot::TableConnectionData &data) { data_ = data; }

  int from() const { return data_.from(); }
  void setFrom(int i) { data_.setFrom(i); }

  const QString &name() const { return data_.name(); }
  void setName(const QString &name) { data_.setName(name); }

  const Group &group() const { return data_.group(); }
  void setGroup(const Group &group) { data_.setGroup(group); }

  const Values &values() const { return data_.values(); }

  void addValue(int to, double fromValue, double toValue, bool primary=true) {
    data_.addValue(to, fromValue, toValue, primary);
  }

  const QModelIndex &linkInd() const { return data_.linkInd(); }
  void setLinkInd(const QModelIndex &i) { data_.setLinkInd(i); }

  double fromTotal(bool primaryOnly=false) const { return data_.fromTotal(primaryOnly); }
  double toTotal  (bool primaryOnly=false) const { return data_.toTotal  (primaryOnly); }

  double maxTotal() const {
    double totalFrom = fromTotal(/*primaryOnly*/true);
    double totalTo   = fromTotal() - totalFrom;

    return std::max(totalFrom, totalTo);
  }

  void sort() { data_.sort(); }

  //---

  void setAngles(const CQChartsAngle &a, const CQChartsAngle &da) {
    a_  = a;
    da_ = da;
  }

  const CQChartsAngle &angle() const { return a_; }

  const CQChartsAngle &dangle() const { return da_; }

 private:
  CQChartsConnectionPlot::TableConnectionData data_; //!< connection data
  CQChartsAngle                               a_;    //!< start angle
  CQChartsAngle                               da_;   //!< delta angle
};

//---

/*!
 * \brief Chord Plot object
 * \ingroup Charts
 */
class CQChartsChordObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using ChordData = CQChartsChordData;

 public:
  CQChartsChordObj(const CQChartsChordPlot *plot, const CQChartsGeom::BBox &rect,
                   const ChordData &data, const ColorInd &ig, const ColorInd &iv);

  const ChordData &data() { return data_; }

  //---

  QString typeName() const override { return "chord"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool isArc() const override { return true; }
  CQChartsArcData arcData() const override;

  //---

  double innerRadius() const;
  double outerRadius() const;

  //---

  bool inside(const CQChartsGeom::Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(CQChartsPaintDevice *device) override;

  void drawFg(CQChartsPaintDevice *device) const override;

  //---

  void calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const;

  void calcArcPenBrush(CQChartsChordObj *toObj, CQChartsPenBrush &penBrush) const;

  QColor calcFromColor() const;

  //---

  void writeScriptData(CQChartsScriptPaintDevice *device) const override;

  //---

  CQChartsGeom::BBox textBBox() const;

 private:
  CQChartsChordObj *plotObject(int ind) const;

  void valueAngles(int ind, double &a, double &da,
                   ChordData::PrimaryType primaryType=ChordData::PrimaryType::ANY) const;

 private:
  const CQChartsChordPlot* plot_ { nullptr }; //!< parent plot
  ChordData                data_;             //!< chord data
};

//---

/*!
 * \brief Chord Plot
 * \ingroup Charts
 *
 * columns:
 *   + link  : link
 *   + value : value
 *   + group : group
 *
 * Plot Type
 *   + \ref CQChartsChordPlotType
 *
 * Example
 *   + \image html chord_plot.png
 */
class CQChartsChordPlot : public CQChartsConnectionPlot,
 public CQChartsObjStrokeData<CQChartsChordPlot> {
  Q_OBJECT

  // options
  Q_PROPERTY(double innerRadius READ innerRadius   WRITE setInnerRadius)
  Q_PROPERTY(double labelRadius READ labelRadius   WRITE setLabelRadius)
  Q_PROPERTY(bool   rotatedText READ isRotatedText WRITE setRotatedText)

  // stroke
  CQCHARTS_STROKE_DATA_PROPERTIES

  // style
  Q_PROPERTY(CQChartsAlpha segmentAlpha READ segmentAlpha WRITE setSegmentAlpha)
  Q_PROPERTY(CQChartsAlpha arcAlpha     READ arcAlpha     WRITE setArcAlpha    )
  Q_PROPERTY(CQChartsAngle gapAngle     READ gapAngle     WRITE setGapAngle    )
  Q_PROPERTY(CQChartsAngle startAngle   READ startAngle   WRITE setStartAngle  )

 public:
  CQChartsChordPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsChordPlot();

  //---

  //! get/set inner radius
  double innerRadius() const { return innerRadius_; }
  void setInnerRadius(double r);

  //! get/set label radius
  double labelRadius() const { return labelRadius_; }
  void setLabelRadius(double r);

  //! get/set rotated text
  bool isRotatedText() const { return rotatedText_; }
  void setRotatedText(bool b);

  //---

  const CQChartsAlpha &segmentAlpha() const { return segmentAlpha_; }
  void setSegmentAlpha(const CQChartsAlpha &a);

  const CQChartsAlpha &arcAlpha() const { return arcAlpha_; }
  void setArcAlpha(const CQChartsAlpha &a);

  const CQChartsAngle &gapAngle() const { return gapAngle_; }
  void setGapAngle(const CQChartsAngle &a);

  const CQChartsAngle &startAngle() const { return startAngle_; }
  void setStartAngle(const CQChartsAngle &a);

  //---

  CQChartsRotatedTextBoxObj *textBox() const { return textBox_; }

  //---

  double valueToDegrees(double v) const { return v*valueToDegrees_; }

  //---

  void addProperties() override;

  CQChartsGeom::Range calcRange() const override;

  CQChartsGeom::BBox calcAnnotationBBox() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  void postResize() override;

  //---

  void write(std::ostream &os, const QString &plotVarName, const QString &modelVarName,
             const QString &viewVarName) const override;

 private:
  using ChordData   = CQChartsChordData;
  using NameDataMap = std::map<QString,ChordData>;

  //---

  bool initHierObjs(PlotObjs &objs) const;

  void initHierObjsAddHierConnection(const HierConnectionData &srcHierData,
                                     const HierConnectionData &destHierData) const override;
  void initHierObjsAddLeafConnection(const HierConnectionData &srcHierData,
                                     const HierConnectionData &destHierData) const override;

  void initHierObjsConnection(const QString &srcStr, const CQChartsModelIndex &srcLinkInd,
                              double srcValue,
                              const QString &destStr, const CQChartsModelIndex &destLinkInd,
                              double destValue) const;

  //---

  bool initLinkObjs      (PlotObjs &objs) const;
  bool initConnectionObjs(PlotObjs &objs) const;
  bool initTableObjs     (PlotObjs &objs) const;

  ChordData &findNameData(const QString &name, const QModelIndex &linkInd) const;
  ChordData &findNameData(NameDataMap &nameDataMap, const QString &name,
                          const QModelIndex &linkInd) const;

  void addNameDataMap(const NameDataMap &nameDataMap, PlotObjs &objs);

  ChordData::Group getChordGroup(const QVariant &groupVar) const;

 private:
  // options
  double        innerRadius_  { 0.9 };   //!< inner radius
  double        labelRadius_  { 1.1 };   //!< label radius
  bool          rotatedText_  { false }; //!< is text rotated
  CQChartsAlpha segmentAlpha_ { 0.7 };   //!< segment alpha
  CQChartsAlpha arcAlpha_     { 0.3 };   //!< arc alpha
  CQChartsAngle gapAngle_     { 2.0 };   //!< gap angle
  CQChartsAngle startAngle_   { 90.0 };  //!< start angle

  CQChartsRotatedTextBoxObj* textBox_        { nullptr }; //!< text box
  double                     valueToDegrees_ { 1.0 };     //!< value to degrees scale
  CQChartsValueSet           groupValues_;                //!< group values
  NameDataMap                nameDataMap_;                //!< name data map
};

#endif
