#ifndef CQChartsRectEdit_H
#define CQChartsRectEdit_H

#include <CQChartsRect.h>
#include <QFrame>

class CQRectEdit;
class QComboBox;

class CQChartsRectEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsRectEdit(QWidget *parent=nullptr);

  const CQChartsRect &rect() const;
  void setRect(const CQChartsRect &pos);

 signals:
  void rectChanged();

 private slots:
  void editChanged();
  void unitsChanged();

 private:
  void rectToWidgets();
  void widgetsToRect();

  void connectSlots(bool b);

  void updateUnits();

 private:
  CQChartsRect rect_;
  CQRectEdit*  edit_       { nullptr };
  QComboBox*   unitsCombo_ { nullptr };
};

//------

#include <CQPropertyViewType.h>

// type for CQChartsRect
class CQChartsRectPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsRectPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;
};

//---

#include <CQPropertyViewEditor.h>

// editor factory for CQChartsRect
class CQChartsRectPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsRectPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
