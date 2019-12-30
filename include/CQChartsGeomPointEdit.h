#ifndef CQChartsGeomPointEdit_H
#define CQChartsGeomPointEdit_H

#include <CQChartsGeom.h>

class CQChartsLineEdit;
class CQRealSpin;

class QHBoxLayout;
class QDoubleValidator;

/*!
 * \brief geometry point edit
 * \ingroup Charts
 */
class CQChartsGeomPointEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsGeom::Point point    READ getValue WRITE setValue   )
  Q_PROPERTY(bool                spin     READ isSpin   WRITE setSpin    )
  Q_PROPERTY(int                 decimals READ decimals WRITE setDecimals)

 public:
  using Point = CQChartsGeom::Point;

 public:
  CQChartsGeomPointEdit(QWidget *parent, const Point &value=Point(0,0), bool spin=false);
  CQChartsGeomPointEdit(const Point &value=Point(0, 0), bool spin=false);

 ~CQChartsGeomPointEdit() { }

  bool isSpin() const { return spin_; }
  void setSpin(bool b);

  int decimals() const { return decimals_; }
  void setDecimals(int i);

  const Point &getValue() const;
  void setValue(const Point &point);

  void setMinimum(const Point &point);
  void setMaximum(const Point &point);

  void setStep(const Point &point);

 private:
  void init(const Point &value);

 private slots:
  void editingFinishedI();

 signals:
  void valueChanged();

 private:
  void updateRange();

  void pointToWidget();
  bool widgetToPoint();

 private:
  Point             point_          { 0, 0 };
  bool              spin_           { false };
  Point             min_            { -1E50, -1E50 };
  Point             max_            { 1E50, 1E50 };
  Point             step_           { 1, 1 };
  int               decimals_       { 3 };
  QDoubleValidator *x_validator_    { nullptr };
  QDoubleValidator *y_validator_    { nullptr };
  CQChartsLineEdit *x_edit_         { nullptr };
  CQChartsLineEdit *y_edit_         { nullptr };
  CQRealSpin       *x_spin_         { nullptr };
  CQRealSpin       *y_spin_         { nullptr };
  QHBoxLayout      *layout_         { nullptr };
  mutable bool      disableSignals_ { false };
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsGeomPoint
 * \ingroup Charts
 */
class CQChartsGeomPointPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsGeomPointPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "geom_point"; }
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsGeomPoint
 * \ingroup Charts
 */
class CQChartsGeomPointPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsGeomPointPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
