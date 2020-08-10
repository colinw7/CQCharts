#ifndef CQChartsColumnEdit_H
#define CQChartsColumnEdit_H

#include <CQChartsColumn.h>
#include <CQChartsLineEditBase.h>

class CQChartsColumnEdit;
class CQChartsColumnCombo;
class QAbstractItemModel;

/*!
 * \brief Column line edit
 * \ingroup Charts
 */
class CQChartsColumnLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsColumn column READ column WRITE setColumn)

 public:
  CQChartsColumnLineEdit(QWidget *parent=nullptr);

  void setPlot(CQChartsPlot *plot) override;

  QAbstractItemModel *model() const;
  void setModel(QAbstractItemModel *model);

  const CQChartsColumn &column() const;
  void setColumn(const CQChartsColumn &c);

  void drawPreview(QPainter *painter, const QRect &rect) override;

 signals:
  void columnChanged();

 private slots:
  void menuEditChanged();

 private:
  void textChanged() override;

  void updateColumn(const CQChartsColumn &c, bool updateText);

  void columnToWidgets();

  void connectSlots(bool b);

 private:
  CQChartsColumnEdit* dataEdit_ { nullptr };
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
  CQChartsColumnEdit(QWidget *parent=nullptr);

  QAbstractItemModel *model() const { return model_; }
  void setModel(QAbstractItemModel *model);

  const CQChartsColumn &column() const;
  void setColumn(const CQChartsColumn &c);

  QSize sizeHint() const override;

 signals:
  void columnChanged();

 private slots:
  void widgetsToColumn();

  void updateState();

  void widgetsToColumnSlot();

 private:
  void connectSlots(bool b);

  void columnToWidgets();

  void updateColumnsFromModel();

 private:
  using WidgetLabels = std::map<QWidget*, QWidget*>;

  QAbstractItemModel*  model_          { nullptr }; //!< model
  CQChartsColumn       column_;                     //!< column
  CQChartsLineEdit*    nameEdit_       { nullptr }; //!< name edit
  QComboBox*           typeCombo_      { nullptr }; //!< type combo
  CQChartsColumnCombo* columnCombo_    { nullptr }; //!< column combo
  CQChartsLineEdit*    roleEdit_       { nullptr }; //!< role edit
  CQChartsLineEdit*    indexEdit_      { nullptr }; //!< index edit
  CQChartsLineEdit*    expressionEdit_ { nullptr }; //!< expression edit
  WidgetLabels         widgetLabels_;               //!< widget labels
  int                  widgetHeight_   { 100 };
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
  CQChartsColumnPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
