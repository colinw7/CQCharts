#ifndef CQRangeSlider_H
#define CQRangeSlider_H

#include <QFrame>

class CQRangeSlider : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool  showRangeLabels  READ showRangeLabels  WRITE setShowRangeLabels )
  Q_PROPERTY(bool  showSliderLabels READ showSliderLabels WRITE setShowSliderLabels)
  Q_PROPERTY(QFont textFont         READ textFont         WRITE setTextFont        )

 public:
  CQRangeSlider(QWidget *parent=nullptr);

  bool showRangeLabels() const { return showRangeLabels_; }
  void setShowRangeLabels(bool b) { showRangeLabels_ = b; update(); }

  bool showSliderLabels() const { return showSliderLabels_; }
  void setShowSliderLabels(bool b) { showSliderLabels_ = b; update(); }

  const QFont &textFont() const { return textFont_; }
  void setTextFont(const QFont &v) { textFont_ = v; }

 protected:
  static QColor blendColors(const QColor &c1, const QColor &c2, double f);

  bool showRangeLabels_  { false };
  bool showSliderLabels_ { true };

  QFont textFont_;

  bool pressed_  { false };
  int  pressInd_ { 0 };

  mutable int xs1_ { 0 };
  mutable int xs2_ { 0 };
};

#endif
