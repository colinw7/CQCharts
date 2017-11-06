#ifndef CQChartsScatterPlot_H
#define CQChartsScatterPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsValueSet.h>
#include <CQChartsDataLabel.h>

class CQChartsScatterPlot;

class CQChartsScatterPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using OptReal = boost::optional<double>;

 public:
  CQChartsScatterPointObj(CQChartsScatterPlot *plot, const CBBox2D &rect, const QPointF &p,
                          double symbolSize, const OptReal &fontSize, const OptReal &color,
                          int i, int n);

  const QString &name() const { return name_; }
  void setName(const QString &v) { name_ = v; }

  bool inside(const CPoint2D &p) const override;

  void mousePress(const CPoint2D &);

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  CQChartsScatterPlot* plot_       { nullptr };
  QPointF              p_;
  double               symbolSize_;
  OptReal              fontSize_;
  OptReal              color_;
  int                  i_          { -1 };
  int                  n_          { -1 };
  QString              name_;
};

//---

#include <CQChartsKey.h>

class CQChartsScatterKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsScatterKeyColor(CQChartsScatterPlot *plot, int i, int n);

  bool mousePress(const CPoint2D &p) override;

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

  Q_PROPERTY(int    nameColumn           READ nameColumn             WRITE setNameColumn          )
  Q_PROPERTY(int    xColumn              READ xColumn                WRITE setXColumn             )
  Q_PROPERTY(int    yColumn              READ yColumn                WRITE setYColumn             )
  Q_PROPERTY(int    symbolSizeColumn     READ symbolSizeColumn       WRITE setSymbolSizeColumn    )
  Q_PROPERTY(int    fontSizeColumn       READ fontSizeColumn         WRITE setFontSizeColumn      )
  Q_PROPERTY(int    colorColumn          READ colorColumn            WRITE setColorColumn         )
  Q_PROPERTY(QColor symbolBorderColor    READ symbolBorderColor      WRITE setSymbolBorderColor   )
  Q_PROPERTY(double symbolSize           READ symbolSize             WRITE setSymbolSize          )
  Q_PROPERTY(bool   symbolSizeMapEnabled READ isSymbolSizeMapEnabled WRITE setSymbolSizeMapEnabled)
  Q_PROPERTY(double symbolSizeMapMin     READ symbolSizeMapMin       WRITE setSymbolSizeMapMin    )
  Q_PROPERTY(double symbolSizeMapMax     READ symbolSizeMapMax       WRITE setSymbolSizeMapMax    )
  Q_PROPERTY(bool   colorMapEnabled      READ isColorMapEnabled      WRITE setColorMapEnabled     )
  Q_PROPERTY(double colorMapMin          READ colorMapMin            WRITE setColorMapMin         )
  Q_PROPERTY(double colorMapMax          READ colorMapMax            WRITE setColorMapMax         )
  Q_PROPERTY(double fontSize             READ fontSize               WRITE setFontSize            )
  Q_PROPERTY(bool   fontSizeMapEnabled   READ isFontSizeMapEnabled   WRITE setFontSizeMapEnabled  )
  Q_PROPERTY(double fontSizeMapMin       READ fontSizeMapMin         WRITE setFontSizeMapMin      )
  Q_PROPERTY(double fontSizeMapMax       READ fontSizeMapMax         WRITE setFontSizeMapMax      )
  Q_PROPERTY(bool   textLabels           READ isTextLabels           WRITE setTextLabels          )

 public:
  struct Point {
    QPointF p;
    int     i;
    QString symbolSizeStr;
    QString fontSizeStr;
    QString colorStr;

    Point(double x, double y, int i, const QString &symbolSizeStr="",
          const QString &fontSizeStr="", const QString &colorStr="") :
     p(x, y), i(i), symbolSizeStr(symbolSizeStr), fontSizeStr(fontSizeStr), colorStr(colorStr) {
    }
  };

  using Values     = std::vector<Point>;
  using NameValues = std::map<QString,Values>;

 public:
  CQChartsScatterPlot(CQChartsView *view, const ModelP &model);

  //---

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; update(); }

  int xColumn() const { return xColumn_; }
  void setXColumn(int i) { xColumn_ = i; update(); }

  int yColumn() const { return yColumn_; }
  void setYColumn(int i) { yColumn_ = i; update(); }

  int symbolSizeColumn() const { return symbolSizeColumn_; }
  void setSymbolSizeColumn(int i) { symbolSizeColumn_ = i; update(); }

  int fontSizeColumn() const { return fontSizeColumn_; }
  void setFontSizeColumn(int i) { fontSizeColumn_ = i; update(); }

  int colorColumn() const { return colorColumn_; }
  void setColorColumn(int i) { colorColumn_ = i; update(); }

  //---

  const QColor &symbolBorderColor() const { return symbolBorderColor_; }
  void setSymbolBorderColor(const QColor &c) { symbolBorderColor_ = c; update(); }

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

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void initObjs() override;

  //---

  int numRows() const;

  int nameIndex(const QString &name) const;

  void addKeyItems(CQChartsKey *key) override;

  //---

  void draw(QPainter *) override;

  void drawDataLabel(QPainter *p, const QRectF &qrect, const QString &str, double fontSize=-1);

 private:
  int               nameColumn_        { -1 };
  int               xColumn_           { 0 };
  int               yColumn_           { 1 };
  int               symbolSizeColumn_  { -1 };
  int               fontSizeColumn_    { -1 };
  int               colorColumn_       { -1 };
  QColor            symbolBorderColor_ { Qt::black };
  double            symbolSize_        { 4 };
  CQChartsValueSet  symbolSizeSet_;
  double            fontSize_          { 4 };
  CQChartsValueSet  fontSizeSet_;
  CQChartsValueSet  colorSet_;
  NameValues        nameValues_;
  CQChartsDataLabel dataLabel_;
};

#endif
