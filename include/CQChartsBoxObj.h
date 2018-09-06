#ifndef CQChartsBoxObj_H
#define CQChartsBoxObj_H

#include <CQChartsObj.h>
#include <CQChartsData.h>
#include <QRectF>
#include <QPolygonF>

class CQChartsView;
class CQPropertyViewModel;
class QPainter;

class CQChartsBoxObj : public CQChartsObj {
  Q_OBJECT

  Q_PROPERTY(bool   visible READ isVisible WRITE setVisible)
  Q_PROPERTY(double margin  READ margin    WRITE setMargin )
  Q_PROPERTY(double padding READ padding   WRITE setPadding)

  Q_PROPERTY(bool                background
             READ isBackground      WRITE setBackground       )
  Q_PROPERTY(CQChartsColor       backgroundColor
             READ backgroundColor   WRITE setBackgroundColor  )
  Q_PROPERTY(double              backgroundAlpha
             READ backgroundAlpha   WRITE setBackgroundAlpha  )
  Q_PROPERTY(CQChartsFillPattern backgroundPattern
             READ backgroundPattern WRITE setBackgroundPattern)

  Q_PROPERTY(bool             border      READ isBorder    WRITE setBorder     )
  Q_PROPERTY(CQChartsColor    borderColor READ borderColor WRITE setBorderColor)
  Q_PROPERTY(double           borderAlpha READ borderAlpha WRITE setBorderAlpha)
  Q_PROPERTY(CQChartsLength   borderWidth READ borderWidth WRITE setBorderWidth)
  Q_PROPERTY(CQChartsLineDash borderDash  READ borderDash  WRITE setBorderDash )
  Q_PROPERTY(QString          borderSides READ borderSides WRITE setBorderSides)
  Q_PROPERTY(CQChartsLength   cornerSize  READ cornerSize  WRITE setCornerSize )

 public:
  CQChartsBoxObj(CQChartsView *view);
  CQChartsBoxObj(CQChartsPlot *plot);

  virtual ~CQChartsBoxObj() { }

  //---

  CQChartsView *view() const;
  CQChartsPlot *plot() const { return plot_; }

  //---

  // get/set visible
  bool isVisible() const { return boxData_.visible; }
  void setVisible(bool b) { boxData_.visible = b; redrawBoxObj(); }

  //---

  // inside margin
  double margin() const { return boxData_.margin; }
  void setMargin(double r) { boxData_.margin = r; redrawBoxObj(); }

  // outside padding
  double padding() const { return boxData_.padding; }
  void setPadding(double r) { boxData_.padding = r; redrawBoxObj(); }

  //---

  // background
  bool isBackground() const { return boxData_.shape.background.visible; }
  void setBackground(bool b) { boxData_.shape.background.visible = b; redrawBoxObj(); }

  const CQChartsColor &backgroundColor() const { return boxData_.shape.background.color; }
  void setBackgroundColor(const CQChartsColor &c) {
    boxData_.shape.background.color = c; redrawBoxObj(); }

  double backgroundAlpha() const { return boxData_.shape.background.alpha; }
  void setBackgroundAlpha(double a) { boxData_.shape.background.alpha = a; redrawBoxObj(); }

  const CQChartsFillPattern &backgroundPattern() const {
    return boxData_.shape.background.pattern; }
  void setBackgroundPattern(const CQChartsFillPattern &p) {
    boxData_.shape.background.pattern = p; redrawBoxObj(); }

  //---

  // border
  bool isBorder() const { return boxData_.shape.border.visible; }
  void setBorder(bool b) { boxData_.shape.border.visible = b; redrawBoxObj(); }

  const CQChartsColor &borderColor() const { return boxData_.shape.border.color; }
  void setBorderColor(const CQChartsColor &c) { boxData_.shape.border.color = c; redrawBoxObj(); }

  double borderAlpha() const { return boxData_.shape.border.alpha; }
  void setBorderAlpha(double a) { boxData_.shape.border.alpha = a; redrawBoxObj(); }

  const CQChartsLength &borderWidth() const { return boxData_.shape.border.width; }
  void setBorderWidth(const CQChartsLength &l) { boxData_.shape.border.width = l; redrawBoxObj(); }

  const CQChartsLineDash &borderDash() const { return boxData_.shape.border.dash; }
  void setBorderDash(const CQChartsLineDash &l) { boxData_.shape.border.dash = l; redrawBoxObj(); }

  const CQChartsLength &cornerSize() const { return boxData_.shape.border.cornerSize; }
  void setCornerSize(const CQChartsLength &l) {
    boxData_.shape.border.cornerSize = l; redrawBoxObj(); }

  const QString &borderSides() const { return boxData_.borderSides; }
  void setBorderSides(const QString &s) { boxData_.borderSides = s; redrawBoxObj(); }

  //---

  const CQChartsBoxData &boxData() const { return boxData_; }
  void setBoxData(const CQChartsBoxData &data) { boxData_ = data; }

  const CQChartsShapeData &shapeData() const { return boxData_.shape; }
  void setShapeData(const CQChartsShapeData &data) { boxData_.shape = data; }

  //---

  QColor interpBackgroundColor(int i, int n) const;
  QColor interpBorderColor    (int i, int n) const;

  //---

  virtual void redrawBoxObj(); // TODO: signal

  //---

  virtual void addProperties(CQPropertyViewModel *model, const QString &path);

  //---

  void draw(QPainter *painter, const QRectF &rect) const;
  void draw(QPainter *painter, const QPolygonF &poly) const;

 protected:
  CQChartsView*   view_     { nullptr }; // parent view
  CQChartsPlot*   plot_     { nullptr }; // parent plot
  CQChartsBoxData boxData_;              // box data
};

#endif
