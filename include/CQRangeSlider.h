#ifndef CQRangeSlider_H
#define CQRangeSlider_H

#include <QFrame>
#include <QLinearGradient>

class CQRangeSlider : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool      showRangeLabels  READ showRangeLabels  WRITE setShowRangeLabels )
  Q_PROPERTY(bool      showSliderLabels READ showSliderLabels WRITE setShowSliderLabels)
  Q_PROPERTY(QFont     textFont         READ textFont         WRITE setTextFont        )
  Q_PROPERTY(SliderPos sliderPos        READ sliderPos        WRITE setSliderPos       )
  Q_PROPERTY(int       xBorder          READ xBorder          WRITE setXBorder         )

  Q_ENUMS(SliderPos)

 public:
  enum class SliderPos {
    CENTER,
    TOP,
    BOTTOM,
    BOTH
  };

 public:
  CQRangeSlider(QWidget *parent=nullptr);

  virtual ~CQRangeSlider() { }

  //---

  bool showRangeLabels() const { return showRangeLabels_; }
  void setShowRangeLabels(bool b) { showRangeLabels_ = b; update(); }

  bool showSliderLabels() const { return showSliderLabels_; }
  void setShowSliderLabels(bool b) { showSliderLabels_ = b; update(); }

  const QFont &textFont() const { return textFont_; }
  void setTextFont(const QFont &v) { textFont_ = v; }

  const SliderPos &sliderPos() const { return sliderPos_; }
  void setSliderPos(const SliderPos &v) { sliderPos_ = v; }

  int xBorder() const { return xBorder_; }
  void setXBorder(int i) { xBorder_ = i; }

  //---

  //! set/clear linear gradient
  void setLinearGradient(const QLinearGradient &lg);
  void clearLinearGradient();

 protected:
  virtual void drawSlider(QPainter *painter);

  virtual double valueToPixel(double x) const = 0;
  virtual double pixelToValue(double px) const = 0;

  virtual double getSliderMin() const = 0;
  virtual double getSliderMax() const = 0;

  virtual void pixelToSliderValue(int px, int &ind, bool force=false) = 0;
  virtual void pixelSetSliderValue(int px, int ind, bool force=false) = 0;

  virtual void saveRange() = 0;

  virtual void deltaSliderMin(int d) = 0;
  virtual void deltaSliderMax(int d) = 0;

  //--

  void mousePressEvent  (QMouseEvent *) override;
  void mouseMoveEvent   (QMouseEvent *) override;
  void mouseReleaseEvent(QMouseEvent *) override;

  void keyPressEvent(QKeyEvent *e) override;

  //---

  QColor colorAt(double x) const;

  static QColor interpColor(const QColor &c1, const QColor &c2, double f);
  static QColor blendColors(const QColor &c1, const QColor &c2, double f);

  static QColor bwColor(const QColor &c);

 protected:
  bool showRangeLabels_  { false };
  bool showSliderLabels_ { true };

  QFont textFont_;

  SliderPos sliderPos_ { SliderPos::CENTER };

  int xBorder_ { 2 };

  QLinearGradient lg_;
  bool            lgSet_ { false };

  bool pressed_  { false };
  int  pressInd_ { 0 };

  mutable int xs1_ { 0 };
  mutable int xs2_ { 0 };
};

#endif
