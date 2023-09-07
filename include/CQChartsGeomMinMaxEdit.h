#ifndef CQChartsGeomMinMaxEdit_H
#define CQChartsGeomMinMaxEdit_H

#include <CQChartsGeom.h>

#include <QPointer>

class CQChartsView;
class CQChartsPlot;
class CQChartsLineEdit;

/*!
 * \brief geometry range edit
 * \ingroup Charts
 */
class CQChartsGeomMinMaxEdit : public QFrame {
  Q_OBJECT

 public:
  using View   = CQChartsView;
  using Plot   = CQChartsPlot;
  using MinMax = CQChartsGeom::RMinMax;

 public:
  CQChartsGeomMinMaxEdit(QWidget *parent, const MinMax &value=MinMax(0, 0));
  CQChartsGeomMinMaxEdit(const MinMax &value=MinMax(0, 0));

 ~CQChartsGeomMinMaxEdit() = default;

  const MinMax &getValue() const { return range_; }
  void setValue(const MinMax &range);

  View *view() const;
  void setView(View *view);

  Plot *plot() const;
  void setPlot(Plot *p);

 private:
  void init(const MinMax &value);

 private Q_SLOTS:
  void editingFinishedI();

 Q_SIGNALS:
  void valueChanged();

 private:
  void updateRange();

  void rangeToWidget();
  bool widgetToRange();

 private:
  using ViewP = QPointer<View>;
  using PlotP = QPointer<Plot>;

  ViewP             view_;
  PlotP             plot_;
  MinMax            range_          { 0, 0 };
  CQChartsLineEdit* edit_           { nullptr };
  mutable bool      disableSignals_ { false };
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsGeomRange
 * \ingroup Charts
 */
class CQChartsGeomMinMaxPropertyViewType : public CQPropertyViewType {
 public:
  using PropertyItem = CQPropertyViewItem;
  using Delegate     = CQPropertyViewDelegate;
  using StyleOption  = QStyleOptionViewItem;

 public:
  CQChartsGeomMinMaxPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(PropertyItem *item, const QVariant &value) override;

  void draw(PropertyItem *item, const Delegate *delegate, QPainter *painter,
            const StyleOption &option, const QModelIndex &index,
            const QVariant &value, const ItemState &itemState) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "geom_range"; }

 private:
  QString valueString(PropertyItem *item, const QVariant &value, bool &ok) const;
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsGeomRange
 * \ingroup Charts
 */
class CQChartsGeomMinMaxPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsGeomMinMaxPropertyViewEditor();

  QWidget *createEdit(QWidget *parent) override;

  void connect(QWidget *w, QObject *obj, const char *method) override;

  QVariant getValue(QWidget *w) override;

  void setValue(QWidget *w, const QVariant &var) override;
};

#endif
