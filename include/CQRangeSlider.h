#ifndef CQRangeSlider_H
#define CQRangeSlider_H

#include <QFrame>

class CQRangeSlider : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool      showRangeLabels  READ showRangeLabels  WRITE setShowRangeLabels )
  Q_PROPERTY(bool      showSliderLabels READ showSliderLabels WRITE setShowSliderLabels)
  Q_PROPERTY(QFont     textFont         READ textFont         WRITE setTextFont        )
  Q_PROPERTY(SliderPos sliderPos        READ sliderPos        WRITE setSliderPos       )

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

  bool showRangeLabels() const { return showRangeLabels_; }
  void setShowRangeLabels(bool b) { showRangeLabels_ = b; update(); }

  bool showSliderLabels() const { return showSliderLabels_; }
  void setShowSliderLabels(bool b) { showSliderLabels_ = b; update(); }

  const QFont &textFont() const { return textFont_; }
  void setTextFont(const QFont &v) { textFont_ = v; }

  const SliderPos &sliderPos() const { return sliderPos_; }
  void setSliderPos(const SliderPos &v) { sliderPos_ = v; }

 protected:
  virtual void drawSlider(QPainter *painter);

  virtual double valueToPixel(double x) const = 0;
  virtual double pixelToValue(double px) const = 0;

  virtual double getSliderMin() const = 0;
  virtual double getSliderMax() const = 0;

  //--

  static QColor blendColors(const QColor &c1, const QColor &c2, double f);

  bool showRangeLabels_  { false };
  bool showSliderLabels_ { true };

  QFont textFont_;

  SliderPos sliderPos_ { SliderPos::CENTER };

  bool pressed_  { false };
  int  pressInd_ { 0 };

  mutable int xs1_ { 0 };
  mutable int xs2_ { 0 };
};

#endif
