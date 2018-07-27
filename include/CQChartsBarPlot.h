#ifndef CQChartsBarPlot_H
#define CQChartsBarPlot_H

#include <CQChartsGroupPlot.h>

//---

// bar plot
class CQChartsBarPlot : public CQChartsGroupPlot {
  Q_OBJECT

  // data
  Q_PROPERTY(CQChartsColumn valueColumn  READ valueColumn     WRITE setValueColumn    )
  Q_PROPERTY(QString        valueColumns READ valueColumnsStr WRITE setValueColumnsStr)
  Q_PROPERTY(CQChartsColumn colorColumn  READ colorColumn     WRITE setColorColumn    )

  // options
  Q_PROPERTY(bool           horizontal  READ isHorizontal WRITE setHorizontal )
  Q_PROPERTY(CQChartsLength margin      READ margin       WRITE setMargin     )
  Q_PROPERTY(CQChartsLength groupMargin READ groupMargin  WRITE setGroupMargin)

  // bar border
  Q_PROPERTY(bool             border      READ isBorder      WRITE setBorder     )
  Q_PROPERTY(CQChartsColor    borderColor READ borderColor   WRITE setBorderColor)
  Q_PROPERTY(double           borderAlpha READ borderAlpha   WRITE setBorderAlpha)
  Q_PROPERTY(CQChartsLength   borderWidth READ borderWidth   WRITE setBorderWidth)
  Q_PROPERTY(CQChartsLineDash borderDash  READ borderDash    WRITE setBorderDash )
  Q_PROPERTY(CQChartsLength   cornerSize  READ cornerSize    WRITE setCornerSize )

  // bar fill
  Q_PROPERTY(bool          barFill    READ isBarFill  WRITE setBarFill   )
  Q_PROPERTY(CQChartsColor barColor   READ barColor   WRITE setBarColor  )
  Q_PROPERTY(double        barAlpha   READ barAlpha   WRITE setBarAlpha  )
  Q_PROPERTY(Pattern       barPattern READ barPattern WRITE setBarPattern)

  // color map
  Q_PROPERTY(bool   colorMapped READ isColorMapped WRITE setColorMapped)
  Q_PROPERTY(double colorMapMin READ colorMapMin   WRITE setColorMapMin)
  Q_PROPERTY(double colorMapMax READ colorMapMax   WRITE setColorMapMax)

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
  CQChartsBarPlot(CQChartsView *view, CQChartsPlotType *plotType, const ModelP &model);

  virtual ~CQChartsBarPlot();

  //---

  const CQChartsColumn &valueColumn() const { return valueColumns_.column(); }
  void setValueColumn(const CQChartsColumn &c);

  const Columns &valueColumns() const { return valueColumns_.columns(); }
  void setValueColumns(const Columns &valueColumns);

  QString valueColumnsStr() const;
  bool setValueColumnsStr(const QString &s);

  const CQChartsColumn &valueColumnAt(int i);

  int numValueColumns() const;

  //---

  bool isHorizontal() const { return horizontal_; }

  //---

  // bar margin
  const CQChartsLength &margin() const { return margin_; }
  void setMargin(const CQChartsLength &l);

  // group margin
  const CQChartsLength &groupMargin() const { return groupMargin_; }
  void setGroupMargin(const CQChartsLength &l);

  //---

  // bar stroke
  bool isBorder() const;
  void setBorder(bool b);

  const CQChartsColor &borderColor() const;
  void setBorderColor(const CQChartsColor &c);

  QColor interpBorderColor(int i, int n) const;

  double borderAlpha() const;
  void setBorderAlpha(double r);

  const CQChartsLength &borderWidth() const;
  void setBorderWidth(const CQChartsLength &l);

  const CQChartsLineDash &borderDash() const;
  void setBorderDash(const CQChartsLineDash &v);

  const CQChartsLength &cornerSize() const;
  void setCornerSize(const CQChartsLength &r);

  //---

  // bar fill
  bool isBarFill() const;
  void setBarFill(bool b);

  const CQChartsColor &barColor() const;
  void setBarColor(const CQChartsColor &c);

  QColor interpBarColor(int i, int n) const;

  double barAlpha() const;
  void setBarAlpha(double a);

  Pattern barPattern() const;
  void setBarPattern(Pattern pattern);

  //---

  const CQChartsColumn &colorColumn() const { return valueSetColumn("color"); }
  void setColorColumn(const CQChartsColumn &c) {
    (void) setValueSetColumn("color", c); updateRangeAndObjs(); }

  bool isColorMapped() const { return isValueSetMapped("color"); }
  void setColorMapped(bool b) { setValueSetMapped("color", b); updateObjs(); }

  double colorMapMin() const { return valueSetMapMin("color"); }
  void setColorMapMin(double r) { setValueSetMapMin("color", r); updateObjs(); }

  double colorMapMax() const { return valueSetMapMax("color"); }
  void setColorMapMax(double r) { setValueSetMapMax("color", r); updateObjs(); }

  //---

  bool allowZoomX() const override { return ! isHorizontal(); }
  bool allowZoomY() const override { return   isHorizontal(); }

  bool allowPanX() const override { return ! isHorizontal(); }
  bool allowPanY() const override { return   isHorizontal(); }

  //---

  void addProperties() override;

  //---

  bool probe(ProbeData &probeData) const override;

 public slots:
  // set horizontal
  virtual void setHorizontal(bool b);

 protected:
  CQChartsColumns valueColumns_ { 0 };     // value columns
  bool            horizontal_   { false }; // horizontal bars
  CQChartsLength  margin_       { "2px" }; // bar margin
  CQChartsLength  groupMargin_  { "4px" }; // bar group margin
  CQChartsBoxData boxData_;                // box style data
};

#endif
