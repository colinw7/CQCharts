#ifndef CQChartsColumnEdit_H
#define CQChartsColumnEdit_H

#include <CQChartsColumn.h>
#include <CQChartsLineEditBase.h>

class CQChartsColumnEdit;
class CQChartsColumnCombo;
class CQChartsModelData;

/*!
 * \brief Column line edit
 * \ingroup Charts
 */
class CQChartsColumnLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsColumn column      READ column        WRITE setColumn     )
  Q_PROPERTY(bool           numericOnly READ isNumericOnly WRITE setNumericOnly)
  Q_PROPERTY(bool           proxy       READ isProxy       WRITE setProxy      )

 public:
  using Plot      = CQChartsPlot;
  using ModelData = CQChartsModelData;
  using Column    = CQChartsColumn;

 public:
  CQChartsColumnLineEdit(QWidget *parent=nullptr);

  void setPlot(Plot *plot) override;

  //! get/set is model data
  ModelData *modelData() const;
  void setModelData(ModelData *model);

  //! get/set is column
  const Column &column() const;
  void setColumn(const Column &c);

  //! get/set is numeric only
  bool isNumericOnly() const { return numericOnly_; }
  void setNumericOnly(bool b);

  //! get/set is proxy
  bool isProxy() const { return proxy_; }
  void setProxy(bool b);

  void setNoFocus();

  void drawPreview(QPainter *painter, const QRect &rect) override;

 Q_SIGNALS:
  void columnChanged();

 private Q_SLOTS:
  void menuEditChanged();

 private:
  void updateMenu() override;

  void textChanged() override;

  void updateColumn(const Column &c, bool updateText);

  void columnToWidgets();

  void connectSlots(bool b);

 private:
  CQChartsColumnEdit* dataEdit_    { nullptr };
  bool                numericOnly_ { false };
  bool                proxy_       { true };
};

//---

#include <CQChartsEditBase.h>

class CQChartsLineEdit;
class QComboBox;

/*!
 * \brief Column edit
 * \ingroup Charts
 */
class CQChartsColumnEdit : public CQChartsEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsColumn column READ column WRITE setColumn)

 public:
  using ModelData = CQChartsModelData;
  using Column    = CQChartsColumn;

 public:
  CQChartsColumnEdit(QWidget *parent=nullptr);

  //! get/set model data
  ModelData *modelData() const { return modelData_; }
  void setModelData(ModelData *model);

  //! get/set column
  const Column &column() const { return column_; }
  void setColumn(const Column &c);

  void updateMenu();

  void setNoFocus();

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

 Q_SIGNALS:
  void columnChanged();

 private Q_SLOTS:
  void resetModelData();

  void widgetsToColumn();

  void updateState();

  void typeSlot();

  void widgetsToColumnSlot();

 private:
  void connectSlots(bool b);

  void columnToWidgets();

  void updateColumnsFromModel();

 private:
  using WidgetLabels = std::map<QWidget*, QWidget*>;

  CQChartsColumnLineEdit *lineEdit_ { nullptr };

  ModelData* modelData_ { nullptr }; //!< model data
  Column     column_;                //!< column

  CQChartsLineEdit*    nameEdit_       { nullptr }; //!< name edit
  QComboBox*           typeCombo_      { nullptr }; //!< type combo
  CQChartsColumnCombo* columnCombo_    { nullptr }; //!< column combo
  CQChartsLineEdit*    roleEdit_       { nullptr }; //!< role edit
  CQChartsLineEdit*    indexEdit_      { nullptr }; //!< index edit
  CQChartsLineEdit*    expressionEdit_ { nullptr }; //!< expression edit
  WidgetLabels         widgetLabels_;               //!< widget labels
  int                  widgetHeight_   { 100 };     //!< widget height
  bool                 connected_      { false };   //!< is connected
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsColumn
 * \ingroup Charts
 */
class CQChartsColumnPropertyViewType : public CQPropertyViewType {
 public:
  using Column = CQChartsColumn;

 public:
  CQChartsColumnPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, const ItemState &itemState) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "column"; }

 private:
  QString valueString(CQPropertyViewItem *item, const QVariant &value, bool &ok) const;
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsColumn
 * \ingroup Charts
 */
class CQChartsColumnPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  using Column = CQChartsColumn;

 public:
  CQChartsColumnPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
