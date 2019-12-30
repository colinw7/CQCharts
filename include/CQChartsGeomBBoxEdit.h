#ifndef CQChartsGeomBBoxEdit_H
#define CQChartsGeomBBoxEdit_H

#include <CQChartsGeom.h>

class CQChartsLineEdit;

/*!
 * \brief geometry bbox edit
 * \ingroup Charts
 */
class CQChartsGeomBBoxEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsGeom::BBox bbox READ getValue WRITE setValue)

 public:
  using BBox = CQChartsGeom::BBox;

 public:
  CQChartsGeomBBoxEdit(QWidget *parent, const BBox &value=BBox(0,0,1,1));
  CQChartsGeomBBoxEdit(const BBox &value=BBox(0,0,1,1));

 ~CQChartsGeomBBoxEdit() { }

  void setValue(const BBox &bbox);
  const BBox &getValue() const;

 private:
  void init(const BBox &value);

 private slots:
  void editingFinishedI();

 signals:
  void valueChanged();

 private:
  void updateRange();

  void bboxToWidget();
  bool widgetToBBox();

 private:
  BBox              bbox_           { 0, 0, 1, 1 };
  CQChartsLineEdit* edit_           { nullptr };
  mutable bool      disableSignals_ { false };
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsGeomBBox
 * \ingroup Charts
 */
class CQChartsGeomBBoxPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsGeomBBoxPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "geom_bbox"; }
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

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
