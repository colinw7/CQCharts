#ifndef CQChartsBoxObj_H
#define CQChartsBoxObj_H

#include <CQChartsPaletteColor.h>
#include <QObject>
#include <QRectF>
#include <QPolygonF>

class CQPropertyViewModel;
class QPainter;

class CQChartsBoxObj : public QObject {
  Q_OBJECT

  Q_PROPERTY(double  margin            READ margin             WRITE setMargin            )
  Q_PROPERTY(double  padding           READ padding            WRITE setPadding           )
  Q_PROPERTY(bool    background        READ isBackground       WRITE setBackground        )
  Q_PROPERTY(QString backgroundColor   READ backgroundColorStr WRITE setBackgroundColorStr)
  Q_PROPERTY(double  backgroundAlpha   READ backgroundAlpha    WRITE setBackgroundAlpha   )
  Q_PROPERTY(Pattern backgroundPattern READ backgroundPattern  WRITE setBackgroundPattern )
  Q_PROPERTY(bool    border            READ isBorder           WRITE setBorder            )
  Q_PROPERTY(QString borderColor       READ borderColorStr     WRITE setBorderColorStr    )
  Q_PROPERTY(double  borderAlpha       READ borderAlpha        WRITE setBorderAlpha       )
  Q_PROPERTY(double  borderWidth       READ borderWidth        WRITE setBorderWidth       )
  Q_PROPERTY(double  borderCornerSize  READ borderCornerSize   WRITE setBorderCornerSize  )
  Q_PROPERTY(QString borderSides       READ borderSides        WRITE setBorderSides       )

  Q_ENUMS(Pattern);

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
  CQChartsBoxObj(CQChartsPlot *plot);

  virtual ~CQChartsBoxObj() { }

  //---

  // inside margin
  double margin() const { return margin_; }
  virtual void setMargin(double r) { margin_ = r; redrawBoxObj(); }

  // outside padding
  double padding() const { return padding_; }
  virtual void setPadding(double r) { padding_ = r; redrawBoxObj(); }

  //---

  // background
  bool isBackground() const { return background_; }
  void setBackground(bool b) { background_ = b; redrawBoxObj(); }

  const CQChartsPaletteColor &backgroundColor() const { return backgroundColor_; }
  void setBackgroundColor(const CQChartsPaletteColor &c) { backgroundColor_ = c; redrawBoxObj(); }

  QString backgroundColorStr() const { return backgroundColor_.colorStr(); }
  void setBackgroundColorStr(const QString &str) {
    backgroundColor_.setColorStr(str); redrawBoxObj(); }

  QColor interpBackgroundColor(int i, int n) const;

  double backgroundAlpha() const { return backgroundAlpha_; }
  void setBackgroundAlpha(double a) { backgroundAlpha_ = a; redrawBoxObj(); }

  const Pattern &backgroundPattern() const { return backgroundPattern_; }
  void setBackgroundPattern(const Pattern &v) { backgroundPattern_ = v; redrawBoxObj(); }

  //---

  // border
  bool isBorder() const { return border_; }
  virtual void setBorder(bool b) { border_ = b; redrawBoxObj(); }

  const CQChartsPaletteColor &borderColor() const { return borderColor_; }
  void setBorderColor(const CQChartsPaletteColor &c) { borderColor_ = c; redrawBoxObj(); }

  QString borderColorStr() const { return borderColor_.colorStr(); }
  void setBorderColorStr(const QString &str) { borderColor_.setColorStr(str); redrawBoxObj(); }

  QColor interpBorderColor(int i, int n) const;

  double borderAlpha() const { return borderAlpha_; }
  virtual void setBorderAlpha(double a) { borderAlpha_ = a; redrawBoxObj(); }

  double borderWidth() const { return borderWidth_; }
  virtual void setBorderWidth(double r) { borderWidth_ = r; redrawBoxObj(); }

  double borderCornerSize() const { return borderCornerSize_; }
  virtual void setBorderCornerSize(double r) { borderCornerSize_ = r; redrawBoxObj(); }

  const QString &borderSides() const { return borderSides_; }
  void setBorderSides(const QString &s) { borderSides_ = s; redrawBoxObj(); }

  //---

  virtual void addProperties(CQPropertyViewModel *model, const QString &path);

  virtual void redrawBoxObj();

  void draw(QPainter *painter, const QRectF &rect) const;
  void draw(QPainter *painter, const QPolygonF &poly) const;

 protected:
  CQChartsPlot*        plot_              { nullptr };        // parent plot
  double               margin_            { 4 };              // inside margin (pixels)
  double               padding_           { 0 };              // outside margin (pixels)
  bool                 background_        { false };          // draw background
  CQChartsPaletteColor backgroundColor_;                      // background fill color
  double               backgroundAlpha_   { 1.0 };            // background fill color alpha
  Pattern              backgroundPattern_ { Pattern::SOLID }; // background fill pattern
  bool                 border_            { false };          // draw border
  CQChartsPaletteColor borderColor_;                          // border color
  double               borderWidth_       { 0.0 };            // border width
  double               borderAlpha_       { 1.0 };            // border color alpha
  double               borderCornerSize_  { 0.0 };            // border rounded corner size
  QString              borderSides_       { "tlbr" };         // border sides to draw
};

#endif
