#ifndef CQChartsModelColumnEdit_H
#define CQChartsModelColumnEdit_H

#include <CQChartsModelColumn.h>
#include <CQChartsLineEditBase.h>

class CQChartsModelColumnEdit;
class CQChartsModelColumnCombo;
class CQChartsModelData;

/*!
 * \brief Column line edit
 * \ingroup Charts
 */
class CQChartsModelColumnLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsModelColumn modelColumn READ modelColumn WRITE setModelColumn)

 public:
  using Plot        = CQChartsPlot;
  using ModelData   = CQChartsModelData;
  using ModelColumn = CQChartsModelColumn;

 public:
  CQChartsModelColumnLineEdit(QWidget *parent=nullptr);

  void setView(View *view) override;
  void setPlot(Plot *plot) override;

  //! get/set column
  const ModelColumn &modelColumn() const;
  void setModelColumn(const ModelColumn &c);

  void setNoFocus();

  void drawPreview(QPainter *painter, const QRect &rect) override;

 Q_SIGNALS:
  void modelColumnChanged();

 private Q_SLOTS:
  void menuEditChanged();

 private:
  void updateMenu() override;

  void textChanged() override;

  void updateModelColumn(const ModelColumn &c, bool updateText);

  void modelColumnToWidgets();

  void connectSlots(bool b);

 private:
  CQChartsModelColumnEdit* dataEdit_ { nullptr };
};

//---

#include <CQChartsEditBase.h>

class CQChartsLineEdit;
class QComboBox;

/*!
 * \brief Column edit
 * \ingroup Charts
 */
class CQChartsModelColumnEdit : public CQChartsEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsModelColumn modelColumn READ modelColumn WRITE setModelColumn)

 public:
  using ModelData   = CQChartsModelData;
  using ModelColumn = CQChartsModelColumn;

 public:
  CQChartsModelColumnEdit(QWidget *parent=nullptr);

  //! get/set model data
  CQCharts *charts() const { return charts_; }
  void setCharts( CQCharts *charts);

  //! get/set column
  const ModelColumn &modelColumn() const { return modelColumn_; }
  void setModelColumn(const ModelColumn &c);

  void updateMenu();

  void setNoFocus();

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

 Q_SIGNALS:
  void modelColumnChanged();

 private Q_SLOTS:
  void widgetsToModelColumn();

  void updateState();

  void typeSlot();

  void widgetsToModelColumnSlot();

 private:
  void connectSlots(bool b);

  void modelColumnToWidgets();

  void updateColumnsFromModel();

 private:
  CQChartsModelColumnLineEdit *lineEdit_ { nullptr };

  CQCharts* charts_ { nullptr }; //!< charts;

  ModelColumn modelColumn_; //!< model column

  int  widgetHeight_ { 100 };   //!< widget height
  bool connected_    { false }; //!< is connected
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsModelColumn
 * \ingroup Charts
 */
class CQChartsModelColumnPropertyViewType : public CQPropertyViewType {
 public:
  using ModelColumn = CQChartsModelColumn;

 public:
  CQChartsModelColumnPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, const ItemState &itemState) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "modelColumn"; }

 private:
  QString valueString(CQPropertyViewItem *item, const QVariant &value, bool &ok) const;
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsModelColumn
 * \ingroup Charts
 */
class CQChartsModelColumnPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  using ModelColumn = CQChartsModelColumn;

 public:
  CQChartsModelColumnPropertyViewEditor();

  QWidget *createEdit(QWidget *parent) override;

  void connect(QWidget *w, QObject *obj, const char *method) override;

  QVariant getValue(QWidget *w) override;

  void setValue(QWidget *w, const QVariant &var) override;
};

#endif
