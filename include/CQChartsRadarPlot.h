#ifndef CQChartsRadarPlot_H
#define CQChartsRadarPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDataLabel.h>

// TODO:
//  Stacked
//  mouse feedback depend on angle (actual value)
//  axes on spokes

class CQChartsRadarPlot;
class CQChartsBoxObj;

class CQChartsRadarObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsRadarObj(CQChartsRadarPlot *plot, const CQChartsGeom::BBox &rect, const QString &name,
                   const QPolygonF &poly, const QModelIndex &ind, int i, int n);

  QString calcId() const;

  bool inside(const CQChartsGeom::Point &p) const;

  void addSelectIndex() override;

  bool isIndex(const QModelIndex &) const override;

  void draw(CQChartsRenderer *renderer, const CQChartsPlot::Layer &) override;

 private:
  CQChartsRadarPlot *plot_  { nullptr }; // parent plot
  QString            name_;              // name
  QPolygonF          poly_;              // polygon
  QModelIndex        ind_;               // data index
  int                i_     { 0 };       // value ind
  int                n_     { 1 };       // value count
};

//---

class CQChartsRadarPlotType : public CQChartsPlotType {
 public:
  CQChartsRadarPlotType();

  QString name() const override { return "radar"; }
  QString desc() const override { return "Radar"; }

  void addParameters() override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsRadarPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int     nameColumn   READ nameColumn      WRITE setNameColumn     )
  Q_PROPERTY(int     valueColumn  READ valueColumn     WRITE setValueColumn    )
  Q_PROPERTY(QString valueColumns READ valueColumnsStr WRITE setValueColumnsStr)
  Q_PROPERTY(double  angleStart   READ angleStart      WRITE setAngleStart     )
  Q_PROPERTY(QString gridColor    READ gridColorStr    WRITE setGridColorStr   )
  Q_PROPERTY(double  gridAlpha    READ gridAlpha       WRITE setGridAlpha      )
  Q_PROPERTY(QString fillColor    READ fillColorStr    WRITE setFillColorStr   )
  Q_PROPERTY(double  fillAlpha    READ fillAlpha       WRITE setFillAlpha      )
  Q_PROPERTY(bool    border       READ isBorder        WRITE setBorder         )
  Q_PROPERTY(QString borderColor  READ borderColorStr  WRITE setBorderColorStr )
  Q_PROPERTY(double  borderAlpha  READ borderAlpha     WRITE setBorderAlpha    )
  Q_PROPERTY(double  borderWidth  READ borderWidth     WRITE setBorderWidth    )
  Q_PROPERTY(QFont   textFont     READ textFont        WRITE setTextFont       )
  Q_PROPERTY(QString textColor    READ textColorStr    WRITE setTextColorStr   )

 public:
  CQChartsRadarPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsRadarPlot();

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; updateRangeAndObjs(); }

  int valueColumn() const { return valueColumn_; }

  void setValueColumn(int i) {
    valueColumn_ = i;

    valueColumns_.clear();

    if (valueColumn_ >= 0)
      valueColumns_.push_back(valueColumn_);

    updateRangeAndObjs();
  }

  const Columns &valueColumns() const { return valueColumns_; }

  void setValueColumns(const Columns &valueColumns) {
    valueColumns_ = valueColumns;

    if (! valueColumns_.empty())
      valueColumn_ = valueColumns_[0];
    else
      valueColumn_ = -1;

    updateRangeAndObjs();
  }

  QString valueColumnsStr() const;
  bool setValueColumnsStr(const QString &s);

  //---

  double angleStart() const { return angleStart_; }
  void setAngleStart(double r) { angleStart_ = r; update(); }

  QString gridColorStr() const { return gridColor_.colorStr(); }
  void setGridColorStr(const QString &s) { gridColor_.setColorStr(s); update(); }

  QColor interpGridColor(int i, int n) { return gridColor_.interpColor(this, i, n); }

  double gridAlpha() const { return gridAlpha_; }
  void setGridAlpha(double r) { gridAlpha_ = r; update(); }

  QString fillColorStr() const;
  void setFillColorStr(const QString &s);

  QColor interpFillColor(int i, int n);

  double fillAlpha() const;
  void setFillAlpha(double r);

  bool isBorder() const;
  void setBorder(bool b);

  QString borderColorStr() const;
  void setBorderColorStr(const QString &str);

  QColor interpBorderColor(int i, int n) const;

  double borderAlpha() const;
  void setBorderAlpha(double a);

  double borderWidth() const;
  void setBorderWidth(double r);

  const QFont &textFont() const { return textFont_; }
  void setTextFont(const QFont &f) { textFont_ = f; update(); }

  QString textColorStr() const { return textColor_.colorStr(); }
  void setTextColorStr(const QString &s) { textColor_.setColorStr(s); update(); }

  QColor interpTextColor(int i, int n) const { return textColor_.interpColor(this, i, n); }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  bool initObjs() override;

  void addKeyItems(CQChartsKey *key) override;

  //---

  void drawBackground(CQChartsRenderer *) override;

  void draw(CQChartsRenderer *) override;

 private:
  class ValueData {
   public:
    ValueData() { }

    void add(double v) {
      if (values_.empty()) {
        min_ = v;
        max_ = v;
        sum_ = v;
      }
      else {
        min_  = std::min(min_, v);
        max_  = std::max(max_, v);
        sum_ += v;
      }

      values_.push_back(v);
    }

    double min() const { return min_; }
    double max() const { return max_; }
    double sum() const { return sum_; }

   private:
    using Values = std::vector<double>;

    Values values_;
    double min_ { 0.0 };
    double max_ { 0.0 };
    double sum_ { 0.0 };
  };

  using ValueDatas = std::map<int,ValueData>;

  int                  nameColumn_    { -1 };      // name column
  int                  valueColumn_   { 1 };       // value column
  Columns              valueColumns_;              // values column
  double               angleStart_    { 90 };      // angle start
  CQChartsPaletteColor gridColor_;                 // grid color
  double               gridAlpha_     { 0.5 };     // grid alpha
  CQChartsBoxObj*      boxObj_        { nullptr }; // box object for fill and border
  QFont                textFont_;                  // text font
  CQChartsPaletteColor textColor_;                 // text color
  ValueDatas           valueDatas_;                // value
  double               valueRadius_   { 1.0 };     // max value (radius)
};

#endif
