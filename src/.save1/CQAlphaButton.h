#ifndef CQAlphaButton_H
#define CQAlphaButton_H

#include <QWidget>

class CQAlphaButton : public QWidget {
  Q_OBJECT

 public:
  CQAlphaButton(QWidget *parent=0);

  double getAlpha() const;

  void setAlpha(double alpha);

  void setColor(QColor color);

 private:
  void paintEvent(QPaintEvent *);

  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);

  void posToAlpha(int pos);
  int alphaToPos();

 signals:
  void valueChanged();

 private:
  double alpha_;
  QColor fg_, bg_;
};

#endif
