#ifndef CQChartsFillUnderSideEdit_H
#define CQChartsFillUnderSideEdit_H

#include <CQChartsFillUnder.h>
#include <CQChartsLineEditBase.h>

class QComboBox;

class CQChartsFillUnderSideEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsFillUnderSideEdit(QWidget *parent=nullptr);

  const CQChartsFillUnderSide &fillUnderSide() const;
  void setFillUnderSide(const CQChartsFillUnderSide &side);

 signals:
  void fillUnderSideChanged();

 private slots:
  void comboChanged();

 private:
  CQChartsFillUnderSide fillUnderSide_;
  QComboBox*            combo_ { nullptr };
};

//------

#include <CQPropertyViewType.h>

// type for CQChartsFillUnderSide
class CQChartsFillUnderSidePropertyViewType : public CQPropertyViewType {
 public:
  CQChartsFillUnderSidePropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;
};

//---

#include <CQPropertyViewEditor.h>

// editor factory for CQChartsFillUnderSide
class CQChartsFillUnderSidePropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsFillUnderSidePropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

//------

class CQChartsFillUnderPosEdit;

class CQChartsFillUnderPosLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsFillUnderPos fillUnderPos READ fillUnderPos WRITE setFillUnderPos)

 public:
  CQChartsFillUnderPosLineEdit(QWidget *parent=nullptr);

  const CQChartsFillUnderPos &fillUnderPos() const;
  void setFillUnderPos(const CQChartsFillUnderPos &fillUnderPos);

 signals:
  void fillUnderPosChanged();

 private slots:
  void menuEditChanged();

 private:
  void textChanged() override;

  void updateFillUnderPos(const CQChartsFillUnderPos &fillUnderPos, bool updateText);

  void fillUnderPosToWidgets();

  void connectSlots(bool b) override;

 private:
  CQChartsFillUnderPosEdit *menuEdit_ { nullptr };
};

//------

class CQRealSpin;

class CQChartsFillUnderPosEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsFillUnderPos fillUnderPos READ fillUnderPos WRITE setFillUnderPos)

 public:
  CQChartsFillUnderPosEdit(QWidget *parent=nullptr);

  const CQChartsFillUnderPos &fillUnderPos() const;
  void setFillUnderPos(const CQChartsFillUnderPos &fillUnderPos);

 signals:
  void fillUnderPosChanged();

 private slots:
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

// type for CQChartsFillUnderPos
class CQChartsFillUnderPosPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsFillUnderPosPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;
};

//---

#include <CQPropertyViewEditor.h>

// editor factory for CQChartsFillUnderPos
class CQChartsFillUnderPosPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsFillUnderPosPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
