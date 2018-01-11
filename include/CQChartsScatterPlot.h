#ifndef CQChartsScatterPlot_H
#define CQChartsScatterPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsColorSet.h>
#include <CQChartsDataLabel.h>

class CQChartsScatterPlot;

class CQChartsScatterPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using OptReal  = boost::optional<double>;
  using OptColor = boost::optional<CQChartsPaletteColor>;

 public:
  CQChartsScatterPointObj(CQChartsScatterPlot *plot, const CQChartsGeom::BBox &rect,
                          const QPointF &p, double symbolSize, const OptReal &fontSize,
                          const OptColor &color, int is, int ns, int iv, int nv);

  const QString &name() const { return name_; }
  void setName(const QString &v) { name_ = v; }

  const QModelIndex &ind() const { return ind_; }
  void setInd(const QModelIndex &v) { ind_ = v; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void addSelectIndex() override;

  bool isIndex(const QModelIndex &) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

 private:
  CQChartsScatterPlot* plot_       { nullptr };
  QPointF              p_;
  double               symbolSize_;
  OptReal              fontSize_;
  OptColor             color_;
  int                  is_         { -1 };
  int                  ns_         { -1 };
  int                  iv_         { -1 };
  int                  nv_         { -1 };
  QString              name_;
  QModelIndex          ind_;
};

//---

#include <CQChartsKey.h>

class CQChartsScatterKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsScatterKeyColor(CQChartsScatterPlot *plot, int i, int n);

  bool mousePress(const CQChartsGeom::Point &p) override;

  QBrush fillBrush() const override;
};

//---

class CQChartsScatterPlotType : public CQChartsPlotType {
 public:
  CQChartsScatterPlotType();

  QString name() const override { return "scatter"; }
  QString desc() const override { return "Scatter"; }

  void addParameters() override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsScatterPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int     nameColumn           READ nameColumn             WRITE setNameColumn          )
  Q_PROPERTY(int     xColumn              READ xColumn                WRITE setXColumn             )
  Q_PROPERTY(int     yColumn              READ yColumn                WRITE setYColumn             )
  Q_PROPERTY(int     symbolSizeColumn     READ symbolSizeColumn       WRITE setSymbolSizeColumn    )
  Q_PROPERTY(int     fontSizeColumn       READ fontSizeColumn         WRITE setFontSizeColumn      )
  Q_PROPERTY(int     colorColumn          READ colorColumn            WRITE setColorColumn         )
  Q_PROPERTY(bool    symbolBorder         READ isSymbolBorder         WRITE setSymbolBorder        )
  Q_PROPERTY(QString symbolBorderColor    READ symbolBorderColorStr   WRITE setSymbolBorderColorStr)
  Q_PROPERTY(double  symbolBorderAlpha    READ symbolBorderAlpha      WRITE setSymbolBorderAlpha   )
  Q_PROPERTY(double  symbolBorderWidth    READ symbolBorderWidth      WRITE setSymbolBorderWidth   )
  Q_PROPERTY(bool    symbolFilled         READ isSymbolFilled         WRITE setSymbolFilled        )
  Q_PROPERTY(QString symbolFillColor      READ symbolFillColorStr     WRITE setSymbolFillColorStr  )
  Q_PROPERTY(double  symbolFillAlpha      READ symbolFillAlpha        WRITE setSymbolFillAlpha     )
  Q_PROPERTY(double  symbolSize           READ symbolSize             WRITE setSymbolSize          )
  Q_PROPERTY(bool    symbolSizeMapEnabled READ isSymbolSizeMapEnabled WRITE setSymbolSizeMapEnabled)
  Q_PROPERTY(double  symbolSizeMapMin     READ symbolSizeMapMin       WRITE setSymbolSizeMapMin    )
  Q_PROPERTY(double  symbolSizeMapMax     READ symbolSizeMapMax       WRITE setSymbolSizeMapMax    )
  Q_PROPERTY(bool    colorMapEnabled      READ isColorMapEnabled      WRITE setColorMapEnabled     )
  Q_PROPERTY(double  colorMapMin          READ colorMapMin            WRITE setColorMapMin         )
  Q_PROPERTY(double  colorMapMax          READ colorMapMax            WRITE setColorMapMax         )
  Q_PROPERTY(double  fontSize             READ fontSize               WRITE setFontSize            )
  Q_PROPERTY(bool    fontSizeMapEnabled   READ isFontSizeMapEnabled   WRITE setFontSizeMapEnabled  )
  Q_PROPERTY(double  fontSizeMapMin       READ fontSizeMapMin         WRITE setFontSizeMapMin      )
  Q_PROPERTY(double  fontSizeMapMax       READ fontSizeMapMax         WRITE setFontSizeMapMax      )
  Q_PROPERTY(bool    textLabels           READ isTextLabels           WRITE setTextLabels          )

 public:
  struct Point {
    QPointF     p;
    int         i;
    QModelIndex ind;
    QString     symbolSizeStr;
    QString     fontSizeStr;
    QString     colorStr;

    Point(double x, double y, int i, const QModelIndex &ind, const QString &symbolSizeStr="",
          const QString &fontSizeStr="", const QString &colorStr="") :
     p(x, y), i(i), ind(ind), symbolSizeStr(symbolSizeStr), fontSizeStr(fontSizeStr),
     colorStr(colorStr) {
    }
  };

  using Values     = std::vector<Point>;
  using NameValues = std::map<QString,Values>;
  using OptColor   = boost::optional<CQChartsPaletteColor>;

 public:
  CQChartsScatterPlot(CQChartsView *view, const ModelP &model);

  //---

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; updateRangeAndObjs(); }

  int xColumn() const { return xColumn_; }
  void setXColumn(int i) { xColumn_ = i; updateRangeAndObjs(); }

  int yColumn() const { return yColumn_; }
  void setYColumn(int i) { yColumn_ = i; updateRangeAndObjs(); }

  int symbolSizeColumn() const { return symbolSizeColumn_; }
  void setSymbolSizeColumn(int i) { symbolSizeColumn_ = i; updateRangeAndObjs(); }

  int fontSizeColumn() const { return fontSizeColumn_; }
  void setFontSizeColumn(int i) { fontSizeColumn_ = i; updateRangeAndObjs(); }

  int colorColumn() const { return colorColumn_; }
  void setColorColumn(int i) { colorColumn_ = i; updateRangeAndObjs(); }

  //---

  bool isSymbolBorder() const { return symbolBorder_; }
  void setSymbolBorder(bool b) { symbolBorder_ = b; update(); }

  QString symbolBorderColorStr() const { return symbolBorderColor_.colorStr(); }
  void setSymbolBorderColorStr(const QString &s) { symbolBorderColor_.setColorStr(s); update(); }

  QColor interpSymbolBorderColor(int i, int n) const {
    return symbolBorderColor_.interpColor(this, i, n); }

  double symbolBorderAlpha() const { return symbolBorderAlpha_; }
  void setSymbolBorderAlpha(double r) { symbolBorderAlpha_ = r; update(); }

  double symbolBorderWidth() const { return symbolBorderWidth_; }
  void setSymbolBorderWidth(double r) { symbolBorderWidth_ = r; update(); }

  //---

  bool isSymbolFilled() const { return symbolFilled_; }
  void setSymbolFilled(bool b) { symbolFilled_ = b; update(); }

  QString symbolFillColorStr() const { return symbolFillColor_.colorStr(); }
  void setSymbolFillColorStr(const QString &s) { symbolFillColor_.setColorStr(s); update(); }

  QColor interpSymbolFillColor(int i, int n) const {
    return symbolFillColor_.interpColor(this, i, n); }

  double symbolFillAlpha() const { return symbolFillAlpha_; }
  void setSymbolFillAlpha(double r) { symbolFillAlpha_ = r; update(); }

  //---

  double symbolSize() const { return symbolSize_; }
  void setSymbolSize(double s) { symbolSize_ = s; updateObjs(); }

  bool isSymbolSizeMapEnabled() const { return symbolSizeSet_.isMapEnabled(); }
  void setSymbolSizeMapEnabled(bool b) { symbolSizeSet_.setMapEnabled(b); updateObjs(); }

  double symbolSizeMapMin() const { return symbolSizeSet_.mapMin(); }
  void setSymbolSizeMapMin(double r) { symbolSizeSet_.setMapMin(r); updateObjs(); }

  double symbolSizeMapMax() const { return symbolSizeSet_.mapMax(); }
  void setSymbolSizeMapMax(double r) { symbolSizeSet_.setMapMax(r); updateObjs(); }

  //---

  bool isColorMapEnabled() const { return colorSet_.isMapEnabled(); }
  void setColorMapEnabled(bool b) { colorSet_.setMapEnabled(b); updateObjs(); }

  double colorMapMin() const { return colorSet_.mapMin(); }
  void setColorMapMin(double r) { colorSet_.setMapMin(r); updateObjs(); }

  double colorMapMax() const { return colorSet_.mapMax(); }
  void setColorMapMax(double r) { colorSet_.setMapMax(r); updateObjs(); }

  //---

  bool isTextLabels() const { return dataLabel_.isVisible(); }
  void setTextLabels(bool b) { dataLabel_.setVisible(b); }

  double fontSize() const { return fontSize_; }
  void setFontSize(double s) { fontSize_ = s; updateObjs(); }

  bool isFontSizeMapEnabled() const { return fontSizeSet_.isMapEnabled(); }
  void setFontSizeMapEnabled(bool b) { fontSizeSet_.setMapEnabled(b); updateObjs(); }

  double fontSizeMapMin() const { return fontSizeSet_.mapMin(); }
  void setFontSizeMapMin(double r) { fontSizeSet_.setMapMin(r); updateObjs(); }

  double fontSizeMapMax() const { return fontSizeSet_.mapMax(); }
  void setFontSizeMapMax(double r) { fontSizeSet_.setMapMax(r); updateObjs(); }

  //---

  const NameValues &nameValues() const { return nameValues_; }

  const QString &xname         () const { return xname_         ; }
  const QString &yname         () const { return yname_         ; }
  const QString &symbolSizeName() const { return symbolSizeName_; }
  const QString &fontSizeName  () const { return fontSizeName_  ; }
  const QString &colorName     () const { return colorName_     ; }

  //---

  const CQChartsDataLabel &dataLabel() const { return dataLabel_; }
  CQChartsDataLabel &dataLabel() { return dataLabel_; }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void initSymbolSizeSet();
  void initFontSizeSet();
  void initColorSet();

  bool colorSetColor(int i, OptColor &color);

  void updateObjs() override;

  bool initObjs() override;

  //---

  int numRows() const;

  int nameIndex(const QString &name) const;

  void addKeyItems(CQChartsKey *key) override;

  //---

  void draw(QPainter *) override;

 private:
  int                  nameColumn_        { -1 };
  int                  xColumn_           { 0 };
  int                  yColumn_           { 1 };
  int                  symbolSizeColumn_  { -1 };
  int                  fontSizeColumn_    { -1 };
  int                  colorColumn_       { -1 };
  bool                 symbolBorder_      { true };
  CQChartsPaletteColor symbolBorderColor_;
  double               symbolBorderAlpha_ { 1.0 };
  double               symbolBorderWidth_ { 0 };
  bool                 symbolFilled_      { true };
  CQChartsPaletteColor symbolFillColor_;
  double               symbolFillAlpha_   { 1.0 };
  double               symbolFillWidth_   { 0 };
  double               symbolSize_        { 4 };
  CQChartsValueSet     symbolSizeSet_;
  double               fontSize_          { 8 };
  CQChartsValueSet     fontSizeSet_;
  CQChartsColorSet     colorSet_;
  NameValues           nameValues_;
  CQChartsDataLabel    dataLabel_;
  QString              xname_;
  QString              yname_;
  QString              symbolSizeName_;
  QString              fontSizeName_;
  QString              colorName_;
};

#endif
