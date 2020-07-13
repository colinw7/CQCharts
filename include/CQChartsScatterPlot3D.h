#ifndef CQChartsScatterPlot3D_H
#define CQChartsScatterPlot3D_H

#include <CQChartsPlot3D.h>
#include <CQChartsPlotObj.h>
#include <CQChartsFitData.h>
#include <CQChartsImage.h>
#include <CQStatData.h>
#include <CInterval.h>

class CQChartsScatterPlot3D;
class CQChartsDataLabel;

//---

/*!
 * \brief Scatter plot type
 * \ingroup Charts
 */
class CQChartsScatterPlot3DType : public CQChartsPlot3DType {
 public:
  CQChartsScatterPlot3DType();

  QString name() const override { return "scatter"; }
  QString desc() const override { return "Scatter"; }

  void addParameters() override;

  bool canProbe() const override { return true; }

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

/*!
 * \brief Scatter Plot Point object
 * \ingroup Charts
 */
class CQChartsScatterPoint3DObj : public CQChartsPlot3DObj {
  Q_OBJECT

  Q_PROPERTY(int                   groupInd READ groupInd)
  Q_PROPERTY(CQChartsGeom::Point3D point    READ point   )
  Q_PROPERTY(QString               name     READ name    )

  Q_PROPERTY(CQChartsSymbol symbolType READ symbolType WRITE setSymbolType)
  Q_PROPERTY(CQChartsLength symbolSize READ symbolSize WRITE setSymbolSize)
  Q_PROPERTY(CQChartsLength fontSize   READ fontSize   WRITE setFontSize  )
  Q_PROPERTY(CQChartsColor  color      READ color      WRITE setColor     )

 public:
  using Plot    = CQChartsScatterPlot3D;
  using Point3D = CQChartsGeom::Point3D;

 public:
  CQChartsScatterPoint3DObj(const Plot *plot, int groupInd, const BBox &rect,
                            const Point3D &pos, const ColorInd &is, const ColorInd &ig,
                            const ColorInd &iv);

  const Plot *scatterPlot() const;

  int groupInd() const { return groupInd_; }

  //---

  // position
  const Point3D &point() const { return pos_; }

  //---

  // name
  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  //---

  QString typeName() const override { return "point"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  // symbol type
  CQChartsSymbol symbolType() const;
  void setSymbolType(const CQChartsSymbol &s) { extraData().symbolType = s; }

  // symbol size
  CQChartsLength symbolSize() const;
  void setSymbolSize(const CQChartsLength &s) { extraData().symbolSize = s; }

  // font size
  CQChartsLength fontSize() const;
  void setFontSize(const CQChartsLength &s) { extraData().fontSize = s; }

  // color
  CQChartsColor color() const;
  void setColor(const CQChartsColor &c) { extraData().color = c; }

  //---

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void postDraw(CQChartsPaintDevice *device) override;

  //---

  void calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const;

  //---

 private:
  struct ExtraData {
    CQChartsSymbol symbolType { CQChartsSymbol::Type::NONE }; //!< symbol type
    CQChartsLength symbolSize { CQChartsUnits::NONE, 0.0 };   //!< symbol size
    CQChartsLength fontSize   { CQChartsUnits::NONE, 0.0 };   //!< font size
    CQChartsColor  color;                                     //!< symbol fill color
  };

 private:
  const ExtraData &extraData() const { return edata_; };
  ExtraData &extraData() { return edata_; };

 private:
  int       groupInd_ { -1 }; //!< plot group index
  Point3D   pos_;             //!< point position
  ExtraData edata_;           //!< extra data
  QString   name_;            //!< label name
};

//---

#include <CQChartsKey.h>

/*!
 * \brief Scatter Plot Key Color Box
 * \ingroup Charts
 */
class CQChartsScatterKeyColor3D : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  using Plot = CQChartsScatterPlot3D;

 public:
  CQChartsScatterKeyColor3D(Plot *plot, int groupInd, const ColorInd &ic);

  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &c) { color_ = c; }

  bool selectPress(const Point &p, CQChartsSelMod selMod) override;

  QBrush fillBrush() const override;

 private:
  int hideIndex() const;

 private:
  int           groupInd_ { -1 };
  CQChartsColor color_;
};

//---

/*!
 * \brief Scatter Plot
 * \ingroup Charts
 */
class CQChartsScatterPlot3D : public CQChartsPlot3D,
 public CQChartsObjPointData<CQChartsScatterPlot3D> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn xColumn     READ xColumn     WRITE setXColumn    )
  Q_PROPERTY(CQChartsColumn yColumn     READ yColumn     WRITE setYColumn    )
  Q_PROPERTY(CQChartsColumn zColumn     READ zColumn     WRITE setZColumn    )
  Q_PROPERTY(CQChartsColumn nameColumn  READ nameColumn  WRITE setNameColumn )
  Q_PROPERTY(CQChartsColumn labelColumn READ labelColumn WRITE setLabelColumn)

  Q_PROPERTY(CQChartsColumn symbolTypeColumn READ symbolTypeColumn WRITE setSymbolTypeColumn)
  Q_PROPERTY(CQChartsColumn symbolSizeColumn READ symbolSizeColumn WRITE setSymbolSizeColumn)
  Q_PROPERTY(CQChartsColumn fontSizeColumn   READ fontSizeColumn   WRITE setFontSizeColumn  )

  // options
  Q_PROPERTY(bool drawSymbols READ isDrawSymbols WRITE setDrawSymbols)
  Q_PROPERTY(bool drawLines   READ isDrawLines   WRITE setDrawLines  )
  Q_PROPERTY(bool drawBars    READ isDrawBars    WRITE setDrawBars   )
  Q_PROPERTY(bool fillUnder   READ isFillUnder   WRITE setFillUnder  )

  Q_PROPERTY(double barSize READ barSize WRITE setBarSize)

  // symbol data
  CQCHARTS_POINT_DATA_PROPERTIES

 public:
  using Point3D = CQChartsGeom::Point3D;

  struct ValueData {
    Point3D       p;
    int           row { -1 };
    QModelIndex   ind;
    CQChartsColor color;

    ValueData(const Point3D &p=Point3D(), int row=-1, const QModelIndex &ind=QModelIndex(),
              const CQChartsColor &color=CQChartsColor()) :
     p(p), row(row), ind(ind), color(color) {
    }
  };

  using Values = std::vector<ValueData>;

  struct ValuesData {
    Values  values;
    RMinMax xrange;
    RMinMax yrange;
    RMinMax zrange;
  };

  using NameValues      = std::map<QString,ValuesData>;
  using GroupNameValues = std::map<int,NameValues>;

  //--

 public:
  CQChartsScatterPlot3D(CQChartsView *view, const ModelP &model);
 ~CQChartsScatterPlot3D();

  //---

  // data label
  const CQChartsDataLabel *dataLabel() const { return dataLabel_; }
  CQChartsDataLabel *dataLabel() { return dataLabel_; }

  //---

  // basic columns
  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c);

  const CQChartsColumn &labelColumn() const { return labelColumn_; }
  void setLabelColumn(const CQChartsColumn &c);

  const CQChartsColumn &xColumn() const { return xColumn_; }
  void setXColumn(const CQChartsColumn &c);

  const CQChartsColumn &yColumn() const { return yColumn_; }
  void setYColumn(const CQChartsColumn &c);

  const CQChartsColumn &zColumn() const { return zColumn_; }
  void setZColumn(const CQChartsColumn &c);

  //---

  // customization columns
  const CQChartsColumn &symbolTypeColumn() const;
  void setSymbolTypeColumn(const CQChartsColumn &c);

  const CQChartsColumn &symbolSizeColumn() const;
  void setSymbolSizeColumn(const CQChartsColumn &c);

  const CQChartsColumn &fontSizeColumn() const;
  void setFontSizeColumn(const CQChartsColumn &c);

  //---

  bool isDrawSymbols() const { return drawSymbols_; }
  void setDrawSymbols(bool b);

  bool isDrawLines() const { return drawLines_; }
  void setDrawLines(bool b);

  bool isDrawBars() const { return drawBars_; }
  void setDrawBars(bool b);

  bool isFillUnder() const { return fillUnder_; }
  void setFillUnder(bool b);

  double barSize() const { return barSize_; }
  void setBarSize(double s);

  //---

  ColumnType xColumnType() const { return xColumnType_; }
  ColumnType yColumnType() const { return yColumnType_; }
  ColumnType zColumnType() const { return zColumnType_; }

  //---

  void addNameValue(int groupInd, const QString &name, const Point3D &p, int row,
                    const QModelIndex &xind, const CQChartsColor &color=CQChartsColor());

  const GroupNameValues &groupNameValues() const { return groupNameValues_; }

  //---

  void addProperties() override;

  //---

  Range calcRange() const override;

  void postUpdateRange() override;

  bool createObjs(PlotObjs &obj) const override;

  void initGroups();

  void addPointObjects() const;

  void addNameValues() const;

  //---

  QString xHeaderName() const { return columnHeaderName(xColumn()); }
  QString yHeaderName() const { return columnHeaderName(yColumn()); }
  QString zHeaderName() const { return columnHeaderName(zColumn()); }

  void updateColumnNames() override;

  //---

  int numRows() const;

  void addKeyItems(CQChartsPlotKey *key) override;

  //---

  bool addMenuItems(QMenu *menu) override;

  BBox calcAnnotationBBox() const override;

  //---

  bool hasBackground() const override;

  void execDrawBackground(CQChartsPaintDevice *device) const override;

  void preDrawObjs (CQChartsPaintDevice *) const override;
  void postDrawObjs(CQChartsPaintDevice *) const override;

  bool hasForeground() const override;

  void execDrawForeground(CQChartsPaintDevice *device) const override;

 private:
  void addObjs() const;

  void addBarPolygons(const Point3D &p, const ColorInd &ig);

  //---

  void addPointKeyItems(CQChartsPlotKey *key);

  //---

  bool probe(ProbeData &probeData) const override;

  //---

  void initGroupBestFit(int groupInd) const;

  void drawBestFit(CQChartsPaintDevice *device) const;

  //---

  void initGroupStats(int groupInd) const;

  void drawStatsLines(CQChartsPaintDevice *device) const;

  //----

  struct GroupData {
    CQChartsPolygon3DObj  *polygon  { nullptr };
    CQChartsPolyline3DObj *polyline { nullptr };
  };

  GroupData &getGroupData(int groupId);

  //---

  virtual CQChartsScatterPoint3DObj *createPointObj(int groupInd, const BBox &rect,
                                                    const Point3D &pos, const ColorInd &is,
                                                    const ColorInd &ig, const ColorInd &iv) const;

 protected slots:
  void dataLabelChanged();

 private:
  using Points      = std::vector<Point3D>;
  using GroupPoints = std::map<int,Points>;

 protected:
  struct SymbolTypeData {
    CQChartsColumn column;             //!< symbol type column
    bool           valid    { false }; //!< symbol type valid
    bool           mapped   { false }; //!< symbol type values mapped
    int            data_min { 0 };     //!< model data min
    int            data_max { 1 };     //!< model data max
    int            map_min  { 0 };     //!< mapped size min
    int            map_max  { 1 };     //!< mapped size max
  };

  struct SymbolSizeData {
    CQChartsColumn column;              //!< symbol size column
    bool           valid     { false }; //!< symbol size valid
    bool           mapped    { false }; //!< symbol size values mapped
    double         data_min  { 0.0 };   //!< model data min
    double         data_max  { 1.0 };   //!< model data max
    double         data_mean { 0.0 };   //!< model data mean
    double         map_min   { 0.0 };   //!< mapped size min
    double         map_max   { 1.0 };   //!< mapped size max
    QString        units     { "px" };  //!< mapped size units
  };

  CQChartsDataLabel* dataLabel_ { nullptr }; //!< data label style

  // columns
  CQChartsColumn xColumn_;     //!< x column
  CQChartsColumn yColumn_;     //!< y column
  CQChartsColumn zColumn_;     //!< z column
  CQChartsColumn nameColumn_;  //!< name column
  CQChartsColumn labelColumn_; //!< label column

  ColumnType xColumnType_ { ColumnType::NONE }; //!< x column type
  ColumnType yColumnType_ { ColumnType::NONE }; //!< y column type
  ColumnType zColumnType_ { ColumnType::NONE }; //!< z column type

  // custom column data
  SymbolTypeData symbolTypeData_; //!< symbol type column data
  SymbolSizeData symbolSizeData_; //!< symbol size column data
  FontSizeData   fontSizeData_;   //!< font size column data

  // options
  bool drawSymbols_ { true  }; //! draw symbols
  bool drawLines_   { false }; //! draw lines
  bool drawBars_    { false }; //! draw bars
  bool fillUnder_   { false }; //! fill under

  double barSize_ { 0.01 }; //! bar size

  // group data
  GroupNameValues groupNameValues_; //!< group name values
  GroupPoints     groupPoints_;     //!< group fit points

  // draw data
  using GroupObj = std::map<int,GroupData>;

  mutable GroupObj groupObj_;
};

#endif
