#ifndef CQBBox2DEdit_H
#define CQBBox2DEdit_H

#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <CBBox2D.h>

class CQBBox2DEdit : public QFrame {
  Q_OBJECT

 public:
  CQBBox2DEdit(QWidget *parent, const CBBox2D &value=CBBox2D(0,0,1,1));
  CQBBox2DEdit(const CBBox2D &value=CBBox2D(0,0,1,1));

  CQBBox2DEdit(QWidget *parent, const QRectF &value);
  CQBBox2DEdit(const QRectF &value);

  virtual ~CQBBox2DEdit() { }

  void setValue(const CBBox2D &rect);
  void setValue(const QRectF &rect);

  const CBBox2D &getValue() const;
  QRectF getQValue() const;

 private:
  void init(const CBBox2D &value);

 private slots:
  void editingFinishedI();

 signals:
  void valueChanged();

 private:
  void updateRange();

  void rectToWidget();
  bool widgetToPoint();

 private:
  CBBox2D      rect_           { 0, 0, 1, 1 };
  QLineEdit*   edit_           { nullptr };
  QHBoxLayout* layout_         { nullptr };
  mutable bool disableSignals_ { false };
};

#endif
