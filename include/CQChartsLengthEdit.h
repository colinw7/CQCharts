#ifndef CQChartsLengthEdit_H
#define CQChartsLengthEdit_H

#include <CQChartsLength.h>
#include <QFrame>

class CQChartsUnitsEdit;
class CQPropertyViewTree;
class CQRealSpin;

/*!
 * \brief length edit
 * \ingroup Charts
 */
class CQChartsLengthEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsLength length READ length WRITE setLength)

 public:
  using Length = CQChartsLength;
  using Units  = CQChartsUnits::Type;

 public:
  CQChartsLengthEdit(QWidget *parent=nullptr);

  const Length &length() const;
  void setLength(const Length &l);

 Q_SIGNALS:
  void lengthChanged();
  void editingFinished();

 private Q_SLOTS:
  void editChanged();
  void unitsChanged();

  void spinKeyPress(int, int);

 private:
  void connectSlots(bool b);

//void widgetsToLength();
  void lengthToWidgets();

 private:
  Length             length_;                //!< length data
  CQRealSpin*        edit_      { nullptr }; //!< length edit
  CQChartsUnitsEdit* unitsEdit_ { nullptr }; //!< units edit
  bool               connected_ { false };   //!< is connected
};

//------

#include <CQSwitchLineEdit.h>

class CQChartsSwitchLengthEdit : public CQSwitchLineEdit {
  Q_OBJECT

  Q_PROPERTY(CQChartsLength length READ length WRITE setLength)

 public:
  using Length = CQChartsLength;

 public:
  CQChartsSwitchLengthEdit(QWidget *parent=nullptr);

  CQChartsLengthEdit *edit() const { return edit_; }

  Length length() const;
  void setLength(const Length &l);

  void setPropertyView(CQPropertyViewTree *pv);

  void updatePlacement() override;

 private:
  void connectSlots(bool b);

 Q_SIGNALS:
  void lengthChanged();
  void altEditingFinished();

 private Q_SLOTS:
  void editSwitched(bool);
  void textChangedSlot();
  void lengthChangedSlot();

 private:
  static bool s_isAlt;

  CQChartsLengthEdit *edit_ { nullptr };
  CQPropertyViewTree *pv_   { nullptr };
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsLength
 * \ingroup Charts
 */
class CQChartsLengthPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsLengthPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(ViewItem *item, const QVariant &value) override;

  void draw(ViewItem *item, const ViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, const ItemState &itemState) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "length"; }
};

//------

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsLength
 * \ingroup Charts
 */
class CQChartsLengthPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsLengthPropertyViewEditor();

  QWidget *createEdit(QWidget *parent) override;

  void connect(QWidget *w, QObject *obj, const char *method) override;

  QVariant getValue(QWidget *w) override;

  void setValue(QWidget *w, const QVariant &var) override;
};

#endif
