#ifndef CQChartsPiePlot_H
#define CQChartsPiePlot_H

#include <CQChartsGroupPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsRotatedTextBoxObj.h>
#include <CQChartsGeom.h>

#include <boost/optional.hpp>
#include <string>

//---

/*!
 * \brief Pie Plot type
 * \ingroup Charts
 */
class CQChartsPiePlotType : public CQChartsGroupPlotType {
 public:
  CQChartsPiePlotType();

  QString name() const override { return "pie"; }
  QString desc() const override { return "Pie"; }

  Dimension dimension() const override { return Dimension::ONE_D; }

  void addParameters() override;

  bool hasAxes() const override { return false; }

  bool allowXLog() const override { return false; }
  bool allowYLog() const override { return false; }

  bool canProbe() const override { return false; }

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsPiePlot;

//---

/*!
 * \brief Pie Plot Text object
 * \ingroup Charts
 */
class CQChartsPieTextObj : public CQChartsRotatedTextBoxObj {
 public:
  CQChartsPieTextObj(const CQChartsPiePlot *plot);

  const CQChartsPiePlot *plot() const { return plot_; }

 private:
  const CQChartsPiePlot* plot_ { nullptr };
};

//---

class CQChartsPieGroupObj;

/*!
 * \brief Pie Plot object
 * \ingroup Charts
 */
class CQChartsPieObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(int           colorIndex  READ colorIndex  WRITE setColorIndex )
  Q_PROPERTY(double        angle1      READ angle1      WRITE setAngle1     )
  Q_PROPERTY(double        angle2      READ angle2      WRITE setAngle2     )
  Q_PROPERTY(double        innerRadius READ innerRadius WRITE setInnerRadius)
  Q_PROPERTY(double        outerRadius READ outerRadius WRITE setOuterRadius)
  Q_PROPERTY(QString       label       READ label       WRITE setLabel      )
  Q_PROPERTY(double        value       READ value       WRITE setValue      )
  Q_PROPERTY(bool          missing     READ isMissing   WRITE setMissing    )
//Q_PROPERTY(double        radius      READ radius      WRITE setRadius     )
  Q_PROPERTY(QString       keyLabel    READ keyLabel    WRITE setKeyLabel   )
//Q_PROPERTY(CQChartsColor color       READ color       WRITE setColor      )
  Q_PROPERTY(bool          exploded    READ isExploded  WRITE setExploded   )

 public:
  using OptReal = boost::optional<double>;

 public:
  CQChartsPieObj(const CQChartsPiePlot *plot, const CQChartsGeom::BBox &rect,
                 const QModelIndex &ind, const ColorInd &ig);

  //---

  QString typeName() const override { return "pie"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  //! get/set color index
  int colorIndex() const { return colorIndex_; }
  void setColorIndex(int i) { colorIndex_ = i; }

  //---

  double angle1() const { return angle1_; }
  void setAngle1(double a) { angle1_ = a; }

  double angle2() const { return angle2_; }
  void setAngle2(double a) { angle2_ = a; }

  double innerRadius() const { return ri_; }
  void setInnerRadius(double r) { ri_ = r; }

  double outerRadius() const { return ro_; }
  void setOuterRadius(double r) { ro_ = r; }

  double valueRadius() const { return rv_; }
  void setValueRadius(double r) { rv_ = r; }

  const QString &label() const { return label_; }
  void setLabel(const QString &s) { label_ = s; }

  double value() const { return value_; }
  void setValue(double r) { value_ = r; }

  bool isMissing() const { return missing_; }
  void setMissing(bool b) { missing_ = b; }

  const OptReal &radius() const { return radius_; }
  void setRadius(const OptReal &r) { radius_ = r; }

  const QString &keyLabel() const { return keyLabel_; }
  void setKeyLabel(const QString &s) { keyLabel_ = s; }

  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &c) { color_ = c; }

  CQChartsPieGroupObj *groupObj() const { return groupObj_; }
  void setGroupObj(CQChartsPieGroupObj *p) { groupObj_ = p; }

  //---

  bool isExploded() const { return exploded_; }
  void setExploded(bool b) { exploded_ = b; }

  bool calcExploded() const;

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  //---

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  CQChartsGeom::BBox annotationBBox() const;

  void draw(CQChartsPaintDevice *device) override;

  void drawFg(CQChartsPaintDevice *device) const override;

  void drawSegmentLabel(CQChartsPaintDevice *device, const CQChartsGeom::Point &c) const;

  CQChartsGeom::Point getCenter() const;

  //---

  QColor fillColor() const;

  double xColorValue(bool relative) const override;
  double yColorValue(bool relative) const override;

 protected:
  const CQChartsPiePlot* plot_       { nullptr }; //!< parent plot
  int                    colorIndex_ { -1 };      //!< color index
  double                 angle1_     { 0 };       //!< wedge start angle
  double                 angle2_     { 360 };     //!< wedge end angle
  double                 ri_         { 0.0 };     //!< inner radius
  double                 ro_         { 0.0 };     //!< outer radius
  double                 rv_         { 0.0 };     //!< value radius
  QString                label_      { "" };      //!< label
  double                 value_      { 0 };       //!< value
  bool                   missing_    { false };   //!< value missing
  OptReal                radius_;                 //!< radius
  QString                keyLabel_   { "" };      //!< key label
  CQChartsColor          color_;                  //!< calculated color
  CQChartsPieGroupObj*   groupObj_   { nullptr }; //!< parent group object
  bool                   exploded_   { false };   //!< exploded
};

//---

/*!
 * \brief Pie Plot Group object
 * \ingroup Charts
 */
class CQChartsPieGroupObj : public CQChartsGroupObj {
 public:
  using PieObjs = std::vector<CQChartsPieObj *>;

 public:
  CQChartsPieGroupObj(const CQChartsPiePlot *plot, const CQChartsGeom::BBox &bbox,
                      int groupInd, const QString &name, const ColorInd &ig);

  const CQChartsPiePlot *plot() const { return plot_; }

  //---

  int groupInd() const { return groupInd_; }
  void setGroupInd(int i) { groupInd_ = i; }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  //---

  //! get/set color index
  int colorIndex() const { return colorIndex_; }
  void setColorIndex(int i) { colorIndex_ = i; }

  //---

  double dataTotal() const { return dataTotal_; }
  void setDataTotal(double r) { dataTotal_ = r; }

  int numValues() const { return numValues_; }
  void setNumValues(int n) { numValues_ = n; }

  double radiusMax() const { return radiusMax_; }
  void setRadiusMax(double r) { radiusMax_ = r; }

  bool isRadiusScaled() const { return radiusScaled_; }
  void setRadiusScaled(bool b) { radiusScaled_ = b; }

  double innerRadius() const { return innerRadius_; }
  void setInnerRadius(double r) { innerRadius_ = r; }

  double outerRadius() const { return outerRadius_; }
  void setOuterRadius(double r) { outerRadius_ = r; }

  //---

  void addObject(CQChartsPieObj *obj);

  CQChartsPieObj *lookupObj(const QString &name) const;

  int numObjs() const { return objs_.size(); }

  const PieObjs &objs() const { return objs_; }

  void setAngles(double a1, double a2) { startAngle_ = a1; endAngle_ = a2; }

  //---

  QString typeName() const override { return "group"; }

  QString calcId() const override;

  QString calcTipId() const override;

  void getSelectIndices(Indices &) const override { }

  bool inside(const CQChartsGeom::Point &p) const override;

  void draw(CQChartsPaintDevice *device) override;

  void drawFg(CQChartsPaintDevice *device) const override;

  QColor bgColor() const;

 private:
  const CQChartsPiePlot* plot_         { nullptr }; //!< parent plot
  int                    groupInd_     { -1 };      //!< groupInd
  QString                name_;                     //!< group name
  int                    colorIndex_   { -1 };      //!< color index
  double                 dataTotal_    { 0.0 };     //!< value data total
  int                    numValues_    { 0 };       //!< num values
  double                 radiusMax_    { 0.0 };     //!< radius data max
  bool                   radiusScaled_ { false };   //!< radius scaled
  double                 innerRadius_  { 0.0 };     //!< inner radius
  double                 outerRadius_  { 0.0 };     //!< outer radius
  PieObjs                objs_;                     //!< objects
  double                 startAngle_   { 0.0 };     //!< start angle
  double                 endAngle_     { 0.0 };     //!< end angle
};

//---

#include <CQChartsKey.h>

/*!
 * \brief Pie Plot Key Color Box
 * \ingroup Charts
 */
class CQChartsPieKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsPieKeyColor(CQChartsPiePlot *plot, CQChartsPlotObj *obj);

  bool selectPress(const CQChartsGeom::Point &p, CQChartsSelMod selMod) override;

  QBrush fillBrush() const override;

  int setIndex() const;

 private:
  CQChartsPlotObj* obj_ { nullptr };
};

/*!
 * \brief Pie Plot Key Text
 * \ingroup Charts
 */
class CQChartsPieKeyText : public CQChartsKeyText {
  Q_OBJECT

 public:
  CQChartsPieKeyText(CQChartsPiePlot *plot, CQChartsPlotObj *obj);

  QColor interpTextColor(const ColorInd &ind) const override;

  int setIndex() const;

 private:
  CQChartsPlotObj* obj_ { nullptr };
};

//---

/*!
 * \brief Pie Chart Plot
 * \ingroup Charts
 */
class CQChartsPiePlot : public CQChartsGroupPlot,
 public CQChartsObjShapeData   <CQChartsPiePlot>,
 public CQChartsObjGridLineData<CQChartsPiePlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn  labelColumn    READ labelColumn    WRITE setLabelColumn   )
  Q_PROPERTY(CQChartsColumns valueColumns   READ valueColumns   WRITE setValueColumns  )
  Q_PROPERTY(CQChartsColumn  radiusColumn   READ radiusColumn   WRITE setRadiusColumn  )
  Q_PROPERTY(CQChartsColumn  keyLabelColumn READ keyLabelColumn WRITE setKeyLabelColumn)

  // options
  //  donut, count, inner radius, outer radius, label radius, start angle, end angle,
  //  explode/explode radius
  Q_PROPERTY(bool   donut       READ isDonut       WRITE setDonut      )
  Q_PROPERTY(bool   count       READ isCount       WRITE setCount      )
  Q_PROPERTY(double innerRadius READ innerRadius   WRITE setInnerRadius)
  Q_PROPERTY(double outerRadius READ outerRadius   WRITE setOuterRadius)
  Q_PROPERTY(double labelRadius READ labelRadius   WRITE setLabelRadius)
  Q_PROPERTY(double startAngle  READ startAngle    WRITE setStartAngle )
  Q_PROPERTY(double angleExtent READ angleExtent   WRITE setAngleExtent)
  Q_PROPERTY(double gapAngle    READ gapAngle      WRITE setGapAngle   )
  Q_PROPERTY(bool   rotatedText READ isRotatedText WRITE setRotatedText)

  // explode
  Q_PROPERTY(bool   explodeSelected READ isExplodeSelected WRITE setExplodeSelected)
  Q_PROPERTY(double explodeRadius   READ explodeRadius     WRITE setExplodeRadius  )

  // shape
  CQCHARTS_SHAPE_DATA_PROPERTIES

  // grid
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Grid,grid)

 public:
  CQChartsPiePlot(CQChartsView *view, const ModelP &model);
 ~CQChartsPiePlot();

  //---

  const CQChartsColumn &labelColumn() const { return labelColumn_; }
  void setLabelColumn(const CQChartsColumn &c);

  const CQChartsColumns &valueColumns() const { return valueColumns_; }
  void setValueColumns(const CQChartsColumns &c);

  //---

  const CQChartsColumn &radiusColumn() const { return radiusColumn_; }
  void setRadiusColumn(const CQChartsColumn &c);

  const CQChartsColumn &keyLabelColumn() const { return keyLabelColumn_; }
  void setKeyLabelColumn(const CQChartsColumn &c);

  //---

  bool isDonut() const { return donut_; }

  bool isCount() const { return count_; }

  //---

  double innerRadius() const { return innerRadius_; }
  void setInnerRadius(double r);

  double outerRadius() const { return outerRadius_; }
  void setOuterRadius(double r);

  double labelRadius() const { return labelRadius_; }
  void setLabelRadius(double r);

  double startAngle() const { return startAngle_; }
  void setStartAngle(double r);

  double angleExtent() const { return angleExtent_; }
  void setAngleExtent(double r);

  double gapAngle() const { return gapAngle_; }
  void setGapAngle(double r);

  //---

  bool isRotatedText() const { return rotatedText_; }
  void setRotatedText(bool b);

  bool isExplodeSelected() const { return explodeSelected_; }
  void setExplodeSelected(bool b);

  double explodeRadius() const { return explodeRadius_; }
  void setExplodeRadius(double r);

  //---

  CQChartsPieTextObj *textBox() const { return textBox_; }

  //---

  void addProperties() override;

  CQChartsGeom::Range calcRange() const override;

  CQChartsGeom::BBox annotationBBox() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  void adjustObjAngles() const;

  void addKeyItems(CQChartsPlotKey *key) override;

  bool addMenuItems(QMenu *menu) override;

  //---

  int numGroupObjs() const { return groupObjs_.size(); }

  //---

  void postResize() override;

  //---

  bool selectInvalidateObjs() const override { return true; }

  //---

  void write(std::ostream &os, const QString &varName, const QString &modelName) const override;

 public slots:
  void setDonut(bool b);

  void setCount(bool b);

 private:
  void addRow(const ModelVisitor::VisitData &data, PlotObjs &objs) const;

  void addRowColumn(const CQChartsModelIndex &ind, PlotObjs &objs) const;

  void calcDataTotal() const;

  void addRowDataTotal(const ModelVisitor::VisitData &data) const;

  void addRowColumnDataTotal(const CQChartsModelIndex &ind) const;

  bool getColumnSizeValue(const CQChartsModelIndex &ind, double &value, bool &missing) const;

 private:
  struct GroupData {
    GroupData(const QString &name) :
     name(name) {
    }

    QString              name;                     //!< name
    double               dataTotal    { 0.0 };     //!< data column value total
    int                  numValues    { 0 };       //!< data column num values
    double               radiusMax    { 0.0 };     //!< radius column value max
    bool                 radiusScaled { false };   //!< has radius column value max
    CQChartsPieGroupObj* groupObj     { nullptr }; //!< associated group obj
  };

  using GroupInd   = std::map<QString,int>;
  using GroupDatas = std::map<int,GroupData>;
  using GroupObjs  = std::vector<CQChartsPieGroupObj *>;

  CQChartsColumn      labelColumn_;                 //!< label column
  CQChartsColumns     valueColumns_;                //!< value columns
  CQChartsColumn      radiusColumn_;                //!< radius value column
  CQChartsColumn      keyLabelColumn_;              //!< key label column
  bool                donut_           { false };   //!< is donut
  bool                count_           { false };   //!< show value counts
  double              innerRadius_     { 0.6 };     //!< relative inner donut radius
  double              outerRadius_     { 0.9 };     //!< relative outer donut radius
  double              labelRadius_     { 1.1 };     //!< label radius
  double              startAngle_      { 90 };      //!< first pie start angle
  double              angleExtent_     { 360.0 };   //!< pie angle extent
  double              gapAngle_        { 0.0 };     //!< angle gap between segments
  bool                rotatedText_     { false };   //!< is label rotated
  bool                explodeSelected_ { true };    //!< explode selected pie
  double              explodeRadius_   { 0.05 };    //!< expose radius
  CQChartsPieTextObj* textBox_         { nullptr }; //!< text box
  CQChartsGeom::Point center_;                      //!< center point
  GroupDatas          groupDatas_;                  //!< data per group
  GroupObjs           groupObjs_;                   //!< group objects
};

#endif
