#ifndef CQChartsFillUnderSideEdit_H
#define CQChartsFillUnderSideEdit_H

#include <CQChartsFillUnder.h>
#include <CQChartsLineEditBase.h>

class QComboBox;

/*!
 * \brief fill under side edit
 * \ingroup Charts
 */
class CQChartsFillUnderSideEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsFillUnderSide fillUnderSide READ fillUnderSide WRITE setFillUnderSide)

 public:
  CQChartsFillUnderSideEdit(QWidget *parent=nullptr);

  const CQChartsFillUnderSide &fillUnderSide() const;
  void setFillUnderSide(const CQChartsFillUnderSide &side);

 private:
  void connectSlots(bool b);

 Q_SIGNALS:
  void fillUnderSideChanged();

 private Q_SLOTS:
  void comboChanged();

 private:
  CQChartsFillUnderSide fillUnderSide_;
  QComboBox*            combo_ { nullptr };
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsFillUnderSide
 * \ingroup Charts
 */
class CQChartsFillUnderSidePropertyViewType : public CQPropertyViewType {
 public:
  CQChartsFillUnderSidePropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, const ItemState &itemState) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "fill_under_side"; }
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsFillUnderSide
 * \ingroup Charts
 */
class CQChartsFillUnderSidePropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsFillUnderSidePropertyViewEditor();

  QWidget *createEdit(QWidget *parent) override;

  void connect(QWidget *w, QObject *obj, const char *method) override;

  QVariant getValue(QWidget *w) override;

  void setValue(QWidget *w, const QVariant &var) override;
};

//------

class CQChartsFillUnderPosEdit;

/*!
 * \brief Fill Under Position line edit
 * \ingroup Charts
 */
class CQChartsFillUnderPosLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsFillUnderPos fillUnderPos READ fillUnderPos WRITE setFillUnderPos)

 public:
  CQChartsFillUnderPosLineEdit(QWidget *parent=nullptr);

  const CQChartsFillUnderPos &fillUnderPos() const;
  void setFillUnderPos(const CQChartsFillUnderPos &fillUnderPos);

  void drawPreview(QPainter *painter, const QRect &rect) override;

 Q_SIGNALS:
  void fillUnderPosChanged();

 private Q_SLOTS:
  void menuEditChanged();

 private:
  void textChanged() override;

  void updateFillUnderPos(const CQChartsFillUnderPos &fillUnderPos, bool updateText);

  void fillUnderPosToWidgets();

  void connectSlots(bool b);

 private:
  CQChartsFillUnderPosEdit *dataEdit_ { nullptr };
};

//------

#include <CQChartsEditBase.h>

class CQRealSpin;

/*!
 * \brief Fill Under Position edit
 * \ingroup Charts
 */
class CQChartsFillUnderPosEdit : public CQChartsEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsFillUnderPos fillUnderPos READ fillUnderPos WRITE setFillUnderPos)

 public:
  CQChartsFillUnderPosEdit(QWidget *parent=nullptr);

  const CQChartsFillUnderPos &fillUnderPos() const;
  void setFillUnderPos(const CQChartsFillUnderPos &fillUnderPos);

  void drawPreview(QPainter *painter, const QRect &rect);

 Q_SIGNALS:
  void fillUnderPosChanged();

 private Q_SLOTS:
  void widgetsToFillUnderPos();

 private:
  void fillUnderPosToWidgets();

  void connectSlots(bool b);

 private:
  CQChartsFillUnderPos fillUnderPos_;
  QComboBox*           xtypeCombo_ { nullptr };
  CQRealSpin*          xposEdit_   { nullptr };
  QComboBox*           ytypeCombo_ { nullptr };
  CQRealSpin*          yposEdit_   { nullptr };
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsFillUnderPos
 * \ingroup Charts
 */
class CQChartsFillUnderPosPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsFillUnderPosPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, const ItemState &itemState) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "fill_under_position"; }
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsFillUnderPos
 * \ingroup Charts
 */
class CQChartsFillUnderPosPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsFillUnderPosPropertyViewEditor();

  QWidget *createEdit(QWidget *parent) override;

  void connect(QWidget *w, QObject *obj, const char *method) override;

  QVariant getValue(QWidget *w) override;

  void setValue(QWidget *w, const QVariant &var) override;
};

#endif
