#ifndef CQChartsBoxObj_H
#define CQChartsBoxObj_H

#include <CQChartsData.h>
#include <QObject>
#include <QRectF>
#include <QPolygonF>

class CQChartsView;
class CQPropertyViewModel;
class QPainter;

class CQChartsBoxObj : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool           visible           READ isVisible         WRITE setVisible          )
  Q_PROPERTY(bool           selected          READ isSelected        WRITE setSelected         )
  Q_PROPERTY(double         margin            READ margin            WRITE setMargin           )
  Q_PROPERTY(double         padding           READ padding           WRITE setPadding          )
  Q_PROPERTY(bool           background        READ isBackground      WRITE setBackground       )
  Q_PROPERTY(CQChartsColor  backgroundColor   READ backgroundColor   WRITE setBackgroundColor  )
  Q_PROPERTY(double         backgroundAlpha   READ backgroundAlpha   WRITE setBackgroundAlpha  )
  Q_PROPERTY(Pattern        backgroundPattern READ backgroundPattern WRITE setBackgroundPattern)
  Q_PROPERTY(bool           border            READ isBorder          WRITE setBorder           )
  Q_PROPERTY(CQChartsColor  borderColor       READ borderColor       WRITE setBorderColor      )
  Q_PROPERTY(double         borderAlpha       READ borderAlpha       WRITE setBorderAlpha      )
  Q_PROPERTY(CQChartsLength borderWidth       READ borderWidth       WRITE setBorderWidth      )
  Q_PROPERTY(double         cornerSize        READ cornerSize        WRITE setCornerSize       )
  Q_PROPERTY(QString        borderSides       READ borderSides       WRITE setBorderSides      )

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
  CQChartsBoxObj(CQChartsView *view);
  CQChartsBoxObj(CQChartsPlot *plot);


  virtual ~CQChartsBoxObj() { }

  //---

  // get/set visible
  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; redrawBoxObj(); }

  // get/set selected
  bool isSelected() const { return selected_; }
  void setSelected(bool b) { selected_ = b; redrawBoxObj(); }

  //---

  // inside margin
  double margin() const { return boxData_.margin; }
  virtual void setMargin(double r) { boxData_.margin = r; redrawBoxObj(); }

  // outside padding
  double padding() const { return boxData_.padding; }
  virtual void setPadding(double r) { boxData_.padding = r; redrawBoxObj(); }

  //---

  // background
  bool isBackground() const { return boxData_.background.visible; }
  void setBackground(bool b) { boxData_.background.visible = b; redrawBoxObj(); }

  const CQChartsColor &backgroundColor() const { return boxData_.background.color; }
  void setBackgroundColor(const CQChartsColor &c) { boxData_.background.color = c; redrawBoxObj(); }

  double backgroundAlpha() const { return boxData_.background.alpha; }
  void setBackgroundAlpha(double a) { boxData_.background.alpha = a; redrawBoxObj(); }

  Pattern backgroundPattern() const { return (Pattern ) boxData_.background.pattern; }
  void setBackgroundPattern(const Pattern &p) {
    boxData_.background.pattern = (CQChartsFillPattern::Type) p; redrawBoxObj(); }

  //---

  // border
  bool isBorder() const { return boxData_.border.visible; }
  void setBorder(bool b) { boxData_.border.visible = b; redrawBoxObj(); }

  const CQChartsColor &borderColor() const { return boxData_.border.color; }
  void setBorderColor(const CQChartsColor &c) { boxData_.border.color = c; redrawBoxObj(); }

  double borderAlpha() const { return boxData_.border.alpha; }
  void setBorderAlpha(double a) { boxData_.border.alpha = a; redrawBoxObj(); }

  const CQChartsLength &borderWidth() const { return boxData_.border.width; }
  void setBorderWidth(const CQChartsLength &l) { boxData_.border.width = l; redrawBoxObj(); }

  double cornerSize() const { return boxData_.cornerSize; }
  void setCornerSize(double r) { boxData_.cornerSize = r; redrawBoxObj(); }

  const QString &borderSides() const { return boxData_.borderSides; }
  void setBorderSides(const QString &s) { boxData_.borderSides = s; redrawBoxObj(); }

  //---

  const CQChartsBoxData &data() const { return boxData_; }
  void setData(const CQChartsBoxData &data) { boxData_ = data; }

  //---

  QColor interpBackgroundColor(int i, int n) const;
  QColor interpBorderColor    (int i, int n) const;

  //---

  virtual void addProperties(CQPropertyViewModel *model, const QString &path);

  virtual void redrawBoxObj(); // TODO: signal

  void draw(QPainter *painter, const QRectF &rect) const;
  void draw(QPainter *painter, const QPolygonF &poly) const;

 protected:
  CQChartsView*   view_     { nullptr }; // parent view
  CQChartsPlot*   plot_     { nullptr }; // parent plot
  bool            visible_  { true };    // is visible
  bool            selected_ { false };   // is selected
  CQChartsBoxData boxData_;              // box data
};

#endif
