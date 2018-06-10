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

  QString calcId() const override;

  bool inside(const CQChartsGeom::Point &p) const;

  void getSelectIndices(Indices &inds) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

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

  Q_PROPERTY(CQChartsColumn nameColumn   READ nameColumn      WRITE setNameColumn     )
  Q_PROPERTY(CQChartsColumn valueColumn  READ valueColumn     WRITE setValueColumn    )
  Q_PROPERTY(QString        valueColumns READ valueColumnsStr WRITE setValueColumnsStr)
  Q_PROPERTY(double         angleStart   READ angleStart      WRITE setAngleStart     )
  Q_PROPERTY(double         angleExtent  READ angleExtent     WRITE setAngleExtent    )
  Q_PROPERTY(bool           grid         READ isGrid          WRITE setGrid           )
  Q_PROPERTY(CQChartsColor  gridColor    READ gridColor       WRITE setGridColor      )
  Q_PROPERTY(double         gridAlpha    READ gridAlpha       WRITE setGridAlpha      )
  Q_PROPERTY(bool           border       READ isBorder        WRITE setBorder         )
  Q_PROPERTY(CQChartsColor  borderColor  READ borderColor     WRITE setBorderColor    )
  Q_PROPERTY(double         borderAlpha  READ borderAlpha     WRITE setBorderAlpha    )
  Q_PROPERTY(CQChartsLength borderWidth  READ borderWidth     WRITE setBorderWidth    )
  Q_PROPERTY(bool           filled       READ isFilled        WRITE setFilled         )
  Q_PROPERTY(CQChartsColor  fillColor    READ fillColor       WRITE setFillColor      )
  Q_PROPERTY(double         fillAlpha    READ fillAlpha       WRITE setFillAlpha      )
  Q_PROPERTY(Pattern        fillPattern  READ fillPattern     WRITE setFillPattern    )
  Q_PROPERTY(QFont          textFont     READ textFont        WRITE setTextFont       )
  Q_PROPERTY(CQChartsColor  textColor    READ textColor       WRITE setTextColor      )
  Q_PROPERTY(double         textAlpha    READ textAlpha       WRITE setTextAlpha      )

  Q_ENUMS(Pattern)

 public:
  enum class Pattern {
    SOLID,
    HATCH,
    DENSE,
    HORIZ,
    VERT,
    FDIAG,
    BDIAG
  };

 public:
  CQChartsRadarPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsRadarPlot();

  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c) { nameColumn_ = c; updateRangeAndObjs(); }

  const CQChartsColumn &valueColumn() const { return valueColumn_; }

  void setValueColumn(const CQChartsColumn &c) {
    valueColumn_ = c;

    valueColumns_.clear();

    if (valueColumn_.isValid())
      valueColumns_.push_back(valueColumn_);

    updateRangeAndObjs();
  }

  int numValueColumns() const { return valueColumns_.size(); }

  const CQChartsColumn &valueColumn(int i) const {
    assert(i >= 0 && i < numValueColumns());

    return valueColumns_[i];
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
  void setAngleStart(double r) { angleStart_ = r; updateRangeAndObjs(); }

  double angleExtent() const { return angleExtent_; }
  void setAngleExtent(double r) { angleExtent_ = r; updateRangeAndObjs(); }

  //----

  bool isGrid() const { return gridData_.visible; }
  void setGrid(bool b) { gridData_.visible = b; update(); }

  const CQChartsColor &gridColor() const { return gridData_.color; }
  void setGridColor(const CQChartsColor &c) { gridData_.color = c; update(); }

  QColor interpGridColor(int i, int n) { return gridColor().interpColor(this, i, n); }

  double gridAlpha() const { return gridData_.alpha; }
  void setGridAlpha(double r) { gridData_.alpha = r; update(); }

  //---

  bool isBorder() const;
  void setBorder(bool b);

  const CQChartsColor &borderColor() const;
  void setBorderColor(const CQChartsColor &c);

  double borderAlpha() const;
  void setBorderAlpha(double a);

  const CQChartsLength &borderWidth() const;
  void setBorderWidth(const CQChartsLength &l);

  QColor interpBorderColor(int i, int n) const;

  //---

  bool isFilled() const;
  void setFilled(bool b);

  const CQChartsColor &fillColor() const;
  void setFillColor(const CQChartsColor &c);

  double fillAlpha() const;
  void setFillAlpha(double r);

  Pattern fillPattern() const;
  void setFillPattern(Pattern pattern);

  QColor interpFillColor(int i, int n);

  //---

  const QFont &textFont() const;
  void setTextFont(const QFont &f);

  const CQChartsColor &textColor() const;
  void setTextColor(const CQChartsColor &c);

  double textAlpha() const;
  void setTextAlpha(double a);

  QColor interpTextColor(int i, int n) const;

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  bool initObjs() override;

  void addKeyItems(CQChartsPlotKey *key) override;

  //---

  void handleResize() override;

  //---

  void drawBackground(QPainter *) override;

  void draw(QPainter *) override;

 private:
  void addRow(QAbstractItemModel *model, const QModelIndex &parent, int r, int nr);

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

  CQChartsColumn    nameColumn_;              // name column
  CQChartsColumn    valueColumn_   { 1 };     // value column
  Columns           valueColumns_;            // values column
  double            angleStart_    { 90.0 };  // angle start
  double            angleExtent_   { 360.0 }; // angle extent
  CQChartsLineData  gridData_;                // grid line data
  CQChartsShapeData shapeData_;               // fill/border data
  CQChartsTextData  textData_;                // text data
  ValueDatas        valueDatas_;              // value
  double            valueRadius_   { 1.0 };   // max value (radius)
};

#endif
