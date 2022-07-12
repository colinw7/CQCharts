#ifndef CQChartsOptEdit_H
#define CQChartsOptEdit_H

#include <QFrame>

class CQChartsLineEdit;

/*!
 * \brief optional real edit
 * \ingroup Charts
 */
class CQChartsOptEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsOptEdit(QWidget *parent, const char *type);

  void setPlaceholderText(const QString &text);

 Q_SIGNALS:
  void valueChanged();

 protected:
  friend class CQChartsOptPropertyViewEditor;

  virtual void setTextValue(const QString &str) = 0;
  virtual void resetValue() = 0;

  virtual bool isValueSet() const = 0;

  virtual QString valueToText() = 0;

  virtual QVariant toVariant() const = 0;
  virtual void fromVariant(const QVariant &var) = 0;

  void valueToWidgets();

 private Q_SLOTS:
  void editChanged();

 private:
  void widgetsToValue();

  void connectSlots(bool b);

 private:
  CQChartsLineEdit* edit_      { nullptr }; //!< line edit
  bool              connected_ { false };   //!< is connected
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for optional value
 * \ingroup Charts
 */
class CQChartsOptPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsOptPropertyViewType() { }

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, const ItemState &itemState) override;

  QString tip(const QVariant &value) const override;

 protected:
  virtual QString valueString(CQPropertyViewItem *item, const QVariant &value, bool &ok) const = 0;
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for optional value
 * \ingroup Charts
 */
class CQChartsOptPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsOptPropertyViewEditor() { }

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
