#ifndef CQChartsBoxObj_H
#define CQChartsBoxObj_H

#include <QObject>
#include <QColor>

class CQPropertyTree;

class CQChartsBoxObj : public QObject {
  Q_OBJECT

  Q_PROPERTY(int    margin      READ margin      WRITE setMargin     )
  Q_PROPERTY(QColor background  READ background  WRITE setBackground )
  Q_PROPERTY(bool   border      READ isBorder    WRITE setBorder     )
  Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)
  Q_PROPERTY(double borderWidth READ borderWidth WRITE setBorderWidth)

 public:
  CQChartsBoxObj();

  virtual ~CQChartsBoxObj() { }

  int margin() const { return margin_; }
  void setMargin(int i) { margin_ = i; redraw(); }

  const QColor &background() const { return background_; }
  void setBackground(const QColor &v) { background_ = v; redraw(); }

  bool isBorder() const { return border_; }
  void setBorder(bool b) { border_ = b; redraw(); }

  const QColor &borderColor() const { return borderColor_; }
  void setBorderColor(const QColor &v) { borderColor_ = v; redraw(); }

  double borderWidth() const { return borderWidth_; }
  void setBorderWidth(double r) { borderWidth_ = r; redraw(); }

  virtual void addProperties(CQPropertyTree *tree, const QString &path);

  virtual void redraw() = 0;

 protected:
  int    margin_      { 2 };
  QColor background_  { 0, 0, 0, 0 };
  bool   border_      { false };
  QColor borderColor_ { 0, 0, 0 };
  double borderWidth_ { 0 };
};

#endif
