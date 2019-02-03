#ifndef CQChartsLengthEdit_H
#define CQChartsLengthEdit_H

#include <CQChartsLength.h>
#include <QFrame>

class CQChartsUnitsEdit;
class CQRealSpin;

class CQChartsLengthEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsLengthEdit(QWidget *parent=nullptr);

  const CQChartsLength &length() const;
  void setLength(const CQChartsLength &pos);

 signals:
  void lengthChanged();

 private slots:
  void editChanged();
  void unitsChanged();

 private:
  void widgetsToLength();
  void lengthToWidgets();

  void connectSlots(bool b);

 private:
  CQChartsLength     length_;
  CQRealSpin*        edit_      { nullptr };
  CQChartsUnitsEdit* unitsEdit_ { nullptr };
};

//------

#include <CQPropertyViewType.h>

// type for CQChartsLength
class CQChartsLengthPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsLengthPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;
};

//---

#include <CQPropertyViewEditor.h>

// editor factory for CQChartsLength
class CQChartsLengthPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsLengthPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
