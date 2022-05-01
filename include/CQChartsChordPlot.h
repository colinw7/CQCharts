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

  bool canEqualScale() const override { return true; }

  QString description() const override;

  //---

  bool isColumnForParameter(ColumnDetails *columnDetails, Parameter *parameter) const override;

  void analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData) override;

  //---

  Plot *create(View *view, const ModelP &model) const override;
};

//---

class CQChartsChordPlot;

//---

/*!
 * \brief Chord Data class
 * \ingroup Charts
 */
class CQChartsChordData {
 public:
  using TableConnectionData = CQChartsConnectionPlot::TableConnectionData;
  using Group               = CQChartsConnectionPlot::GroupData;
  using Values              = TableConnectionData::Values;
  using Value               = TableConnectionData::Value;
  using PrimaryType         = TableConnectionData::PrimaryType;
  using OptReal             = CQChartsOptReal;
  using Angle               = CQChartsAngle;

 public:
  CQChartsChordData() { }

  const TableConnectionData &data() const { return data_; }
  void setData(TableConnectionData &data) { data_ = data; }

  //! get/set from node
  int from() const { return data_.from(); }
  void setFrom(int i) { data_.setFrom(i); }

  //! get/set name
  const QString &name() const { return data_.name(); }
  void setName(const QString &name) { data_.setName(name); }

  //! get/set label
  const QString &label() const { return data_.label(); }
  void setLabel(const QString &label) { data_.setLabel(label); }

  //! get/set group
  const Group &group() const { return data_.group(); }
  void setGroup(const Group &group) { data_.setGroup(group); }

  //! get/set hier path
  const QStringList &path() const { return path_; }
  void setPath(const QStringList &v) { path_ = v; }

  //! get/set depth
  int depth() const { return data_.depth(); }
  void setDepth(int depth) { data_.setDepth(depth); }

  //! get/set visible
  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  //! get/set value
  bool hasValue() const { return value_.isSet(); }
  const OptReal &value() const { return value_; }
  void setValue(const OptReal &v) { value_ = v; }

  //! get/set values
  const Values &values() const { return data_.values(); }

  const Value &ivalue(int i) const { return data_.ivalue(i); }

  //! add value
  void addValue(int to, double value, const QModelIndex &ind, bool primary=true) {
    data_.addValue(to, OptReal(value), ind, primary);
  }

  void setToValue(int to, double value) {
    data_.setToValue(to, OptReal(value));
  }

  //! has to chord
  bool hasTo(int to) const {
    return data_.hasTo(to);
  }

  //! get/set linked model index
  const QModelIndex &nameInd() const { return data_.nameInd(); }
  void setNameInd(const QModelIndex &i) { data_.setNameInd(i); }

  //---

  //! get/set hierarchical parent
  int parent() const { return parent_; }
  void setParent(int i) { parent_ = i; }

  //---

  //! get total
  double total(bool primaryOnly=false) const { return data_.total(primaryOnly); }

  double maxTotal() const {
    double t1 = data_.total(/*primaryOnly*/false);
    double t2 = data_.total(/*primaryOnly*/true);
    double t3 = t1 - t2;

    return std::max(t2, t3);
  }

  //---

  //! sort data values
  void sort() { data_.sort(); }

  //---

  //! set arc angles
  void setAngles(const Angle &a, const Angle &da) {
    a_  = a;
    da_ = da;
  }

  //! get start angle and delta angle
  const Angle &angle () const { return a_; }
  const Angle &dangle() const { return da_; }

 private:
  TableConnectionData data_;             //!< connection data
  OptReal             value_;            //!< value
  int                 parent_ { -1 };    //!< hierarchical parent
  QStringList         path_;             //!< hier path
  bool                visible_ { true }; //!< is visible
  Angle               a_;                //!< start angle
  Angle               da_;               //!< delta angle
};

//---

class CQChartsChordEdgeObj;
class CQChartsChordHierObj;

/*!
 * \brief Chord Plot Segment object
 * \ingroup Charts
 */
class CQChartsChordSegmentObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(double total READ calcTotal)

 public:
  using ChordPlot   = CQChartsChordPlot;
  using ChordData   = CQChartsChordData;
  using PrimaryType = ChordData::PrimaryType;
  using SegmentObj  = CQChartsChordSegmentObj;
  using EdgeObj     = CQChartsChordEdgeObj;
  using EdgeObjs    = std::vector<EdgeObj *>;
  using HierObj     = CQChartsChordHierObj;
  using Angle       = CQChartsAngle;

 public:
  CQChartsChordSegmentObj(const ChordPlot *plot, const BBox &rect, const ChordData &data,
                          const ColorInd &ig, const ColorInd &iv);

  const ChordData &data() const { return data_; }

  int from() const { return data().from(); }

  //---

  bool hasEdgeObj(EdgeObj *obj) const {
    for (auto *obj1 : edgeObjs_)
      if (obj1 == obj)
        return true;

    return false;
  }

  void addEdgeObj(EdgeObj *obj) {
    assert(! hasEdgeObj(obj));

    edgeObjs_.push_back(obj);
  }

  //---

  const HierObj *hierObj() const { return hierObj_; }
  void setHierObj(HierObj *p) { hierObj_ = p; }

  void addChildObj(CQChartsPlotObj *obj) { childObjs_.push_back(obj); }

  bool hasChildObj(CQChartsPlotObj *obj) const {
    for (auto *obj1 : childObjs_)
      if (obj1 == obj)
        return true;

    return false;
  }

  //---

  QString typeName() const override { return "segment"; }

  //---

  bool isArc() const override { return true; }

  CQChartsArcData arcData() const override;

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  double calcInnerRadius() const;
  double calcOuterRadius() const;

  double labelRadius() const;

  QString dataName() const { return data_.name(); }

  double calcTotal() const;
  double calcAltTotal() const;

  int calcNumValues() const;

  //---

  bool inside(const Point &p) const override;

  //---

  void getObjSelectIndices(Indices &inds) const override;

  PlotObjs getConnected() const override;

  //---

  void draw(PaintDevice *device) const override;

  void drawFg(PaintDevice *device) const override;

  //---

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  QColor calcFromColor() const;

  //---

  BBox textBBox() const;

  void dataAngles(Angle &a, Angle &da) const;

  void valueAngles(int ind, Angle &a, Angle &da, PrimaryType primaryType=PrimaryType::ANY) const;

 private:
  const ChordPlot* plot_    { nullptr }; //!< parent plot
  ChordData        data_;                //!< chord data
  EdgeObjs         edgeObjs_;            //!< edge objects
  HierObj*         hierObj_ { nullptr }; //!< parent hier obj
  PlotObjs         childObjs_;           //!< child objs
};

//---

/*!
 * \brief Chord Plot Edge object
 * \ingroup Charts
 */
class CQChartsChordEdgeObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using ChordPlot   = CQChartsChordPlot;
  using ChordData   = CQChartsChordData;
  using PrimaryType = ChordData::PrimaryType;
  using EdgeObj     = CQChartsChordEdgeObj;
  using OptReal     = CQChartsOptReal;
  using Angle       = CQChartsAngle;

 public:
  CQChartsChordEdgeObj(const ChordPlot *plot, const BBox &rect, const ChordData &data,
                       int to, const OptReal &value, const QModelIndex &ind);

  const ChordData &data() const { return data_; }

  int from() const { return data().from(); }
  int to  () const { return to_; }

  const OptReal &value() const { return value_; }

  //---

  QString typeName() const override { return "edge"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool inside(const Point &p) const override;

  //---

  void getObjSelectIndices(Indices &inds) const override;

  PlotObjs getConnected() const override;

  //---

  void draw(PaintDevice *device) const override;

  //---

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  //---

  CQChartsChordSegmentObj *fromObj() const;
  CQChartsChordSegmentObj *toObj  () const;

 private:
  const ChordPlot*     plot_ { nullptr }; //!< parent plot
  ChordData            data_;             //!< chord data (from)
  int                  to_ { -1 };        //!< to index
  OptReal              value_;            //!< edge value
  mutable QPainterPath path_;             //!< drawn path
};

//---

/*!
 * \brief Chord Plot Hier object
 * \ingroup Charts
 */
class CQChartsChordHierObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using ChordPlot = CQChartsChordPlot;
  using ChordData = CQChartsChordData;
  using HierObj   = CQChartsChordHierObj;
  using Angle     = CQChartsAngle;

 public:
  CQChartsChordHierObj(const ChordPlot *plot, const QString &name, const BBox &rect);

  const QString &name() const { return name_; }

  //---

  const HierObj *hierObj() const { return hierObj_; }
  void setHierObj(HierObj *p) { hierObj_ = p; }

  void addChildObj(CQChartsPlotObj *obj) { childObjs_.push_back(obj); }

  bool hasChildObj(CQChartsPlotObj *obj) const {
    for (auto *obj1 : childObjs_)
      if (obj1 == obj)
        return true;

    return false;
  }

  //---

  QString typeName() const override { return "hier"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool inside(const Point &p) const override;

  //---

  void draw(PaintDevice *device) const override;

  void drawFg(PaintDevice *device) const override;

  //---

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  QColor fillColor() const;

  //---

  double calcInnerRadius() const;
  double calcOuterRadius() const;

  double labelRadius() const;

  void dataAngles(Angle &angle1, Angle &angle2) const;

  double calcTotal() const;
  double calcAltTotal() const;

  int calcNumValues() const;

  int depth() const;

  int childDepth() const;

 private:
  const ChordPlot*     plot_    { nullptr }; //!< parent plot
  QString              name_;                //!< hier name
  HierObj*             hierObj_ { nullptr }; //!< parent hier obj
  PlotObjs             childObjs_;           //!< child objs
  mutable QPainterPath path_;                //!< drawn path
};

//---

CQCHARTS_NAMED_SHAPE_DATA(Segment, segment)
CQCHARTS_NAMED_SHAPE_DATA(Arc, arc)

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
 public CQChartsObjSegmentShapeData<CQChartsChordPlot>,
 public CQChartsObjArcShapeData    <CQChartsChordPlot> {
  Q_OBJECT

  // options
  Q_PROPERTY(double innerRadius READ innerRadius   WRITE setInnerRadius)
  Q_PROPERTY(double labelRadius READ labelRadius   WRITE setLabelRadius)
  Q_PROPERTY(bool   rotatedText READ isRotatedText WRITE setRotatedText)

  // segment/arc shape data
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Segment, segment)
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Arc    , arc    )

  // style
  Q_PROPERTY(CQChartsAngle startAngle READ startAngle WRITE setStartAngle)
  Q_PROPERTY(CQChartsAngle gapAngle   READ gapAngle   WRITE setGapAngle  )

 public:
  using SegmentObj        = CQChartsChordSegmentObj;
  using EdgeObj           = CQChartsChordEdgeObj;
  using HierObj           = CQChartsChordHierObj;
  using RotatedTextBoxObj = CQChartsRotatedTextBoxObj;
  using Color             = CQChartsColor;
  using Alpha             = CQChartsAlpha;
  using ColorInd          = CQChartsUtil::ColorInd;

 public:
  CQChartsChordPlot(View *view, const ModelP &model);
 ~CQChartsChordPlot();

  //---

  void init() override;
  void term() override;

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

  const Angle &startAngle() const { return startAngle_; }
  void setStartAngle(const Angle &a);

  const Angle &gapAngle() const { return gapAngle_; }
  void setGapAngle(const Angle &a);

  //---

  RotatedTextBoxObj *textBox() const { return textBox_.get(); }

  //---

  double valueToDegrees(double v) const { return v*valueToDegrees_; }

  //---

  void addProperties() override;

  Range calcRange() const override;

  BBox calcExtraFitBBox() const override;

  bool createObjs(PlotObjs &objs) const override;

  void preDrawObjs(PaintDevice *) const override;

  SegmentObj *segmentObject(int ind) const;
  EdgeObj    *edgeObject   (int from, int to) const;

  //---

  double calcInnerRadius() const;
  double calcOuterRadius() const;

  double maxTotal() const { return maxTotal_; }

  //---

  void postResize() override;

  //---

  void write(std::ostream &os, const QString &plotVarName, const QString &modelVarName,
             const QString &viewVarName) const override;

 protected:
  using ChordData   = CQChartsChordData;
  using NameDataMap = std::map<QString, ChordData>;

  //---

  bool initHierObjs() const;

  void initHierObjsAddHierConnection(const HierConnectionData &srcHierData,
                                     const HierConnectionData &destHierData) const override;
  void initHierObjsAddLeafConnection(const HierConnectionData &srcHierData,
                                     const HierConnectionData &destHierData) const override;

  void initHierObjsConnection(const QString &srcStr, const ModelIndex &srcLinkInd,
                              double srcValue,
                              const QString &destStr, const ModelIndex &destLinkInd,
                              double destValue) const;

  //---

  bool initLinkObjs() const;

  void addLinkConnection(const LinkConnectionData &linkConnectionData) const override;

  //---

  bool initPathObjs() const;

  void addPathValue(const PathData &pathData) const override;

  void propagatePathValues();

  //---

  bool initFromToObjs() const;

  void addFromToValue(const FromToData &fromToData) const override;

  //---

  bool initConnectionObjs() const;

  void addConnectionObj(int id, const ConnectionsData &connectionsData,
                        const NodeIndex &nodeIndex) const override;

  //---

  bool initTableObjs(PlotObjs &objs) const;

  void filterObjs();

  //---

  ChordData &findNameData(const QString &name, const QModelIndex &nameInd) const;
  ChordData &findNameData(NameDataMap &nameDataMap, const QString &name,
                          const QModelIndex &nameInd, bool global=false) const;

  void addEdge(ChordData &srcData, const QModelIndex &srcInd, ChordData &destData,
               const QModelIndex &destInd, double value, bool symmetric) const;

  ChordData &getIndData(int ind);

  void addNameDataMap(const NameDataMap &nameDataMap, PlotObjs &objs);

  //---

  bool addMenuItems(QMenu *menu) override;

  //---

  virtual SegmentObj* createSegmentObj(const BBox &rect, const ChordData &data,
                                       const ColorInd &ig, const ColorInd &iv) const;
  virtual EdgeObj*    createEdgeObj(const BBox &rect, const ChordData &data,
                                    int to, const OptReal &value, const QModelIndex &ind) const;
  virtual HierObj*    createHierObj(const QString &name, const BBox &rect) const;

 protected:
  CQChartsPlotCustomControls *createCustomControls() override;

 private:
  using SegmentObjs        = std::vector<SegmentObj *>;
  using EdgeObjs           = std::vector<EdgeObj *>;
  using HierObjs           = std::vector<HierObj *>;
  using IndName            = std::map<int, QString>;
  using RotatedTextBoxObjP = std::unique_ptr<RotatedTextBoxObj>;

  // options
  double innerRadius_ { 0.9 };   //!< inner radius
  double labelRadius_ { 1.1 };   //!< label radius
  bool   rotatedText_ { false }; //!< is text rotated
  Angle  startAngle_  { 90.0 };  //!< start angle
  Angle  gapAngle_    { 2.0 };   //!< gap angle

  RotatedTextBoxObjP textBox_;                //!< text box
  double             valueToDegrees_ { 1.0 }; //!< value to degrees scale
  NameDataMap        nameDataMap_;            //!< name data map
  IndName            indName_;                //!< ind name
  SegmentObjs        segmentObjs_;            //!< segment objects
  EdgeObjs           edgeObjs_;               //!< edge objects
  HierObjs           hierObjs_;               //!< hier objects
  int                maxNodeDepth_   { -1 };  //!< max node depth
  int                maxHierDepth_   { 0 };   //!< max hier obj depth
  mutable double     maxTotal_       { 0.0 }; //!< max total
};

//---

/*!
 * \brief Chord Plot plot custom controls
 * \ingroup Charts
 */
class CQChartsChordPlotCustomControls : public CQChartsConnectionPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsChordPlotCustomControls(CQCharts *charts);

  void init() override;

  void addWidgets() override;

  void setPlot(CQChartsPlot *plot) override;

 public slots:
  void updateWidgets() override;

 protected:
  void connectSlots(bool b) override;

  //---

  CQChartsColor getColorValue() override;
  void setColorValue(const CQChartsColor &c) override;

 protected:
  CQChartsChordPlot* plot_ { nullptr };
};

#endif
