#ifndef CQChartsModelIndEdit_H
#define CQChartsModelIndEdit_H

#include <CQChartsModelInd.h>
#include <CQChartsLineEditBase.h>

class CQChartsModelIndEdit;
class CQChartsModelIndCombo;
class CQChartsModelData;

/*!
 * \brief Model Ind line edit
 * \ingroup Charts
 */
class CQChartsModelIndLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsModelInd modelInd READ modelInd WRITE setModelInd)

 public:
  using Plot        = CQChartsPlot;
  using ModelData   = CQChartsModelData;
  using ModelInd = CQChartsModelInd;

 public:
  CQChartsModelIndLineEdit(QWidget *parent=nullptr);

  void setView(View *view) override;
  void setPlot(Plot *plot) override;

  //! get/set model ind
  const ModelInd &modelInd() const;
  void setModelInd(const ModelInd &c);

  void setNoFocus();

  void drawPreview(QPainter *painter, const QRect &rect) override;

 Q_SIGNALS:
  void modelIndChanged();

 private Q_SLOTS:
  void menuEditChanged();

 private:
  void updateMenu() override;

  void textChanged() override;

  void updateModelInd(const ModelInd &c, bool updateText);

  void modelIndToWidgets();

  void connectSlots(bool b);

 private:
  CQChartsModelIndEdit* dataEdit_ { nullptr };
};

//---

#include <CQChartsEditBase.h>

class CQChartsLineEdit;
class QComboBox;

/*!
 * \brief Model Ind edit
 * \ingroup Charts
 */
class CQChartsModelIndEdit : public CQChartsEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsModelInd modelInd READ modelInd WRITE setModelInd)

 public:
  using ModelData   = CQChartsModelData;
  using ModelInd = CQChartsModelInd;

 public:
  CQChartsModelIndEdit(QWidget *parent=nullptr);

  //! get/set model data
  CQCharts *charts() const { return charts_; }
  void setCharts( CQCharts *charts);

  //! get/set model ind
  const ModelInd &modelInd() const { return modelInd_; }
  void setModelInd(const ModelInd &c);

  void updateMenu();

  void setNoFocus();

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

 Q_SIGNALS:
  void modelIndChanged();

 private Q_SLOTS:
  void widgetsToModelInd();

  void updateState();

  void typeSlot();

  void widgetsToModelIndSlot();

 private:
  void connectSlots(bool b);

  void modelIndToWidgets();

 private:
  CQChartsModelIndLineEdit *lineEdit_ { nullptr };

  CQCharts* charts_ { nullptr }; //!< charts;

  ModelInd modelInd_; //!< model ind

  int  widgetHeight_ { 100 };   //!< widget height
  bool connected_    { false }; //!< is connected
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsModelInd
 * \ingroup Charts
 */
class CQChartsModelIndPropertyViewType : public CQPropertyViewType {
 public:
  using ModelInd = CQChartsModelInd;

 public:
  CQChartsModelIndPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, const ItemState &itemState) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "modelInd"; }

 private:
  QString valueString(CQPropertyViewItem *item, const QVariant &value, bool &ok) const;
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsModelInd
 * \ingroup Charts
 */
class CQChartsModelIndPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  using ModelInd = CQChartsModelInd;

 public:
  CQChartsModelIndPropertyViewEditor();

  QWidget *createEdit(QWidget *parent) override;

  void connect(QWidget *w, QObject *obj, const char *method) override;

  QVariant getValue(QWidget *w) override;

  void setValue(QWidget *w, const QVariant &var) override;
};

#endif
