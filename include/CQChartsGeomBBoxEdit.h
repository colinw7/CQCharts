#ifndef CQChartsGeomBBoxEdit_H
#define CQChartsGeomBBoxEdit_H

#include <CQChartsGeom.h>

#include <QPointer>

class CQChartsView;
class CQChartsPlot;
class CQChartsLineEdit;
class CQChartsRegionButton;

/*!
 * \brief geometry bbox edit
 * \ingroup Charts
 */
class CQChartsGeomBBoxEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsGeom::BBox bbox READ getValue WRITE setValue)

 public:
  using View = CQChartsView;
  using Plot = CQChartsPlot;
  using BBox = CQChartsGeom::BBox;

 public:
  CQChartsGeomBBoxEdit(QWidget *parent, const BBox &value=BBox(0, 0, 1, 1));
  CQChartsGeomBBoxEdit(const BBox &value=BBox(0, 0, 1, 1));

 ~CQChartsGeomBBoxEdit() = default;

  const BBox &getValue() const { return bbox_; }
  void setValue(const BBox &bbox);

  View *view() const;
  void setView(View *view);

  Plot *plot() const;
  void setPlot(Plot *plot);

  void setRegion(const BBox &bbox);

 private:
  void init(const BBox &value);

 private Q_SLOTS:
  void editingFinishedI();

  void regionSlot(const CQChartsGeom::BBox &bbox);

 Q_SIGNALS:
  void valueChanged();

  void regionChanged();

 private:
  void updateRange();

  void bboxToWidget();
  bool widgetToBBox();

 private:
  using ViewP = QPointer<View>;
  using PlotP = QPointer<Plot>;

  BBox                  bbox_           { 0, 0, 1, 1 };
  ViewP                 view_;
  PlotP                 plot_;
  CQChartsLineEdit*     edit_           { nullptr };
  CQChartsRegionButton* regionButton_   { nullptr };
  mutable bool          disableSignals_ { false };
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsGeomBBox
 * \ingroup Charts
 */
class CQChartsGeomBBoxPropertyViewType : public CQPropertyViewType {
 public:
  using PropertyItem = CQPropertyViewItem;
  using Delegate     = CQPropertyViewDelegate;
  using StyleOption  = QStyleOptionViewItem;

 public:
  CQChartsGeomBBoxPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(PropertyItem *item, const QVariant &value) override;

  void draw(PropertyItem *item, const Delegate *delegate, QPainter *painter,
            const StyleOption &option, const QModelIndex &index,
            const QVariant &value, const ItemState &itemState) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "geom_bbox"; }

 private:
  QString valueString(PropertyItem *item, const QVariant &value, bool &ok) const;
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsGeomBBox
 * \ingroup Charts
 */
class CQChartsGeomBBoxPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsGeomBBoxPropertyViewEditor();

  QWidget *createEdit(QWidget *parent) override;

  void connect(QWidget *w, QObject *obj, const char *method) override;

  QVariant getValue(QWidget *w) override;

  void setValue(QWidget *w, const QVariant &var) override;
};

#endif
