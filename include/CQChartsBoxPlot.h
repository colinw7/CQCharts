#ifndef CQChartsBoxPlot_H
#define CQChartsBoxPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsUtil.h>

#include <CQChartsBoxWhisker.h>
#include <map>

class CQChartsBoxPlot;
class CQChartsTextBoxObj;

// box value
struct CQChartsBoxPlotValue {
  CQChartsBoxPlotValue() = default;

  CQChartsBoxPlotValue(double value, const QModelIndex &ind) :
   value(value), ind(ind) {
  }

  operator double() const { return value; }

  double      value { 0.0 };
  QModelIndex ind;
};

//---

using CQChartsBoxPlotWhisker = CQChartsBoxWhiskerT<CQChartsBoxPlotValue>;

//---

// box plot object
class CQChartsBoxPlotObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsBoxPlotObj(CQChartsBoxPlot *plot, const CQChartsGeom::BBox &rect, double pos,
                     const CQChartsBoxPlotWhisker &whisker, int i, int n);

  QString calcId() const override;

  void addSelectIndex() override;

  bool isIndex(const QModelIndex &) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

 private:
  CQChartsBoxPlot*       plot_ { nullptr }; // parent plot
  double                 pos_  { 0.0 };     // x position
  CQChartsBoxPlotWhisker whisker_;          // whisker data
  int                    i_    { -1 };      // value index
  int                    n_    { 0 };       // value count
};

//---

#include <CQChartsKey.h>

class CQChartsBoxKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsBoxKeyColor(CQChartsBoxPlot *plot, int i, int n);

  bool mousePress(const CQChartsGeom::Point &p) override;

  QBrush fillBrush() const override;
};

class CQChartsBoxKeyText : public CQChartsKeyText {
  Q_OBJECT

 public:
  CQChartsBoxKeyText(CQChartsBoxPlot *plot, int i, const QString &text);

  QColor interpTextColor(int i, int n) const override;

 private:
  int i_ { 0 };
};

//---

// box plot type
class CQChartsBoxPlotType : public CQChartsPlotType {
 public:
  CQChartsBoxPlotType();

  QString name() const override { return "box"; }
  QString desc() const override { return "BoxPlot"; }

  void addParameters() override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

// box plot
class CQChartsBoxPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int     xColumn      READ xColumn         WRITE setXColumn        )
  Q_PROPERTY(int     yColumn      READ yColumn         WRITE setYColumn        )
  Q_PROPERTY(QString boxColor     READ boxColorStr     WRITE setBoxColorStr    )
  Q_PROPERTY(QString borderColor  READ borderColorStr  WRITE setBorderColorStr )
  Q_PROPERTY(double  borderAlpha  READ borderAlpha     WRITE setBorderAlpha    )
  Q_PROPERTY(double  borderWidth  READ borderWidth     WRITE setBorderWidth    )
  Q_PROPERTY(double  cornerSize   READ cornerSize      WRITE setCornerSize     )
  Q_PROPERTY(QString whiskerColor READ whiskerColorStr WRITE setWhiskerColorStr)
  Q_PROPERTY(QString textColor    READ textColorStr    WRITE setTextColorStr   )
  Q_PROPERTY(QFont   font         READ font            WRITE setFont           )
  Q_PROPERTY(double  textMargin   READ textMargin      WRITE setTextMargin     )
  Q_PROPERTY(double  symbolSize   READ symbolSize      WRITE setSymbolSize     )

 public:
  CQChartsBoxPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsBoxPlot();

  int xColumn() const { return xColumn_; }
  void setXColumn(int i) { xColumn_ = i; updateRangeAndObjs(); }

  int yColumn() const { return yColumn_; }
  void setYColumn(int i) { yColumn_ = i; updateRangeAndObjs(); }

  //---

  QString boxColorStr() const;
  void setBoxColorStr(const QString &s);

  QColor interpBoxColor(int i, int n) const;

  QString borderColorStr() const;
  void setBorderColorStr(const QString &s);

  QColor interpBorderColor(int i, int n) const;

  double borderAlpha() const;
  void setBorderAlpha(double r);

  double borderWidth() const;
  void setBorderWidth(double r);

  double cornerSize() const;
  void setCornerSize(double r);

  QString whiskerColorStr() const;
  void setWhiskerColorStr(const QString &s);

  QColor interpWhiskerColor(int i, int n) const;

  QString textColorStr() const;
  void setTextColorStr(const QString &s);

  QColor interpTextColor(int i, int n) const;

  const QFont &font() const;
  void setFont(const QFont &f);

  double textMargin() const { return textMargin_; }
  void setTextMargin(double r) { textMargin_ = r; update(); }

  double symbolSize() const { return symbolSize_; }
  void setSymbolSize(double r) { symbolSize_ = r; update(); }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void updateWhiskers();

  bool initObjs() override;

  void addKeyItems(CQChartsKey *key) override;

  //---

  bool probe(ProbeData &probeData) const override;

  void draw(QPainter *) override;

 private:
  struct RealCmp {
    bool operator()(const double &lhs, const double &rhs) const {
      if (CQChartsUtil::realEq(lhs, rhs))
        return false;

      return lhs < rhs;
    }
  };

  using Whiskers = std::map<int,CQChartsBoxPlotWhisker>;
  using ValueSet = std::map<double,int,RealCmp>;
  using SetValue = std::map<int,double>;
  using NameSet  = std::map<QString,int>;
  using SetName  = std::map<int,QString>;

  int                  xColumn_      { 0 };       // x column
  int                  yColumn_      { 1 };       // y column
  CQChartsBoxObj*      boxObj_       { nullptr }; // box data
  CQChartsPaletteColor whiskerColor_;             // whisker color
  CQChartsTextBoxObj*  textObj_      { nullptr }; // text data
  double               textMargin_   { 2 };       // text margin
  double               symbolSize_   { 4 };       // symbol size
  Whiskers             whiskers_;                 // whisker data
  ValueSet             valueSet_;                 // value set map
  SetValue             setValue_;                 // set value map
  NameSet              nameSet_;                  // name set map
  SetName              setName_;                  // set name map
};

#endif
