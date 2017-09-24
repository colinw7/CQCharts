#ifndef CQChartsBoxObj_H
#define CQChartsBoxObj_H

#include <QObject>
#include <QColor>

class CQPropertyViewTree;
class QPainter;

class CQChartsBoxObj : public QObject {
  Q_OBJECT

  Q_PROPERTY(int    margin          READ margin          WRITE setMargin         )
  Q_PROPERTY(int    padding         READ padding         WRITE setPadding        )
  Q_PROPERTY(bool   background      READ isBackground    WRITE setBackground     )
  Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
  Q_PROPERTY(bool   border          READ isBorder        WRITE setBorder         )
  Q_PROPERTY(QColor borderColor     READ borderColor     WRITE setBorderColor    )
  Q_PROPERTY(double borderWidth     READ borderWidth     WRITE setBorderWidth    )
  Q_PROPERTY(double borderRadius    READ borderRadius    WRITE setBorderRadius   )

 public:
  CQChartsBoxObj();

  virtual ~CQChartsBoxObj() { }

  //---

  // inside margin
  int margin() const { return margin_; }
  virtual void setMargin(int i) { margin_ = i; redrawBoxObj(); }

  // outside padding
  int padding() const { return padding_; }
  virtual void setPadding(int i) { padding_ = i; redrawBoxObj(); }

  //---

  // background
  bool isBackground() const { return background_; }
  virtual void setBackground(bool b) { background_ = b; redrawBoxObj(); }

  const QColor &backgroundColor() const { return backgroundColor_; }
  virtual void setBackgroundColor(const QColor &c) { backgroundColor_ = c; redrawBoxObj(); }

  //---

  // border
  bool isBorder() const { return border_; }
  virtual void setBorder(bool b) { border_ = b; redrawBoxObj(); }

  const QColor &borderColor() const { return borderColor_; }
  virtual void setBorderColor(const QColor &c) { borderColor_ = c; redrawBoxObj(); }

  double borderWidth() const { return borderWidth_; }
  virtual void setBorderWidth(double r) { borderWidth_ = r; redrawBoxObj(); }

  double borderRadius() const { return borderRadius_; }
  virtual void setBorderRadius(double r) { borderRadius_ = r; redrawBoxObj(); }

  //---

  virtual void addProperties(CQPropertyViewTree *tree, const QString &path);

  void draw(QPainter *p, const QRectF &rect);
  void draw(QPainter *p, const QPolygonF &poly);

  virtual void redrawBoxObj() = 0;

 protected:
  int    margin_          { 4 }; // inside margin
  int    padding_         { 0 }; // outside margin
  bool   background_      { false };
  QColor backgroundColor_ { 255, 255, 255 };
  bool   border_          { false };
  QColor borderColor_     { 0, 0, 0 };
  double borderWidth_     { 0 };
  double borderRadius_    { 0 };
};

#endif
