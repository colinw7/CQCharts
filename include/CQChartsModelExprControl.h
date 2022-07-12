#ifndef CQChartsModelExprControl_H
#define CQChartsModelExprControl_H

#include <CQChartsColumn.h>
#include <CQChartsColumnType.h>
#include <CQChartsWidgetIFace.h>

#include <QFrame>
#include <QSharedPointer>

class CQChartsModelData;
class CQChartsColumnCombo;
class CQChartsColumnTypeCombo;
class CQChartsColumnExprEdit;
class CQChartsLineEdit;

class QAbstractItemModel;
class QRadioButton;
class QLabel;

/*!
 * \brief Widget to allow User to Add/Remove/Modify Extra Columns in Model
 * \ingroup Charts
 */
class CQChartsModelExprControl : public QFrame, public CQChartsWidgetIFace  {
  Q_OBJECT

  Q_PROPERTY(Mode                 mode   READ mode   WRITE setMode  )
  Q_PROPERTY(QString              expr   READ expr   WRITE setExpr  )
  Q_PROPERTY(CQChartsColumn       column READ column WRITE setColumn)
  Q_PROPERTY(QString              name   READ name   WRITE setName  )
  Q_PROPERTY(CQChartsColumnTypeId type   READ type   WRITE setType  )

  Q_ENUMS(Mode)

 public:
  enum class Mode {
    ADD,
    REMOVE,
    MODIFY
  };

  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsModelExprControl(QWidget *parent=nullptr);

  const Mode &mode() const { return exprMode_; }
  void setMode(const Mode &m);

  const QString &expr() const { return expr_; }
  void setExpr(const QString &s);

  const CQChartsColumn &column() const { return column_; }
  void setColumn(const CQChartsColumn &c);

  const QString &name() const { return name_; }
  void setName(const QString &s);

  const CQChartsColumnTypeId &type() const { return type_; }
  void setType(const CQChartsColumnTypeId &t);

  CQChartsModelData *modelData() const { return modelData_; }
  void setModelData(CQChartsModelData *modelData) override;

  void init();

 private Q_SLOTS:
  void modeSlot();
  void exprSlot();
  void columnSlot();
  void nameSlot();
  void typeSlot();

  void modelColumnSlot(int);

  void applySlot();

 private:
  // data
  CQChartsModelData*   modelData_ { nullptr };
  Mode                 exprMode_  { Mode::ADD };
  QString              expr_;
  CQChartsColumn       column_;
  QString              name_;
  CQChartsColumnTypeId type_;

  // widgets
  QRadioButton*            addRadio_      { nullptr };
  QRadioButton*            removeRadio_   { nullptr };
  QRadioButton*            modifyRadio_   { nullptr };
  CQChartsColumnExprEdit*  exprEdit_      { nullptr };
  QLabel*                  columnLabel_   { nullptr };
  CQChartsColumnCombo*     columnNumEdit_ { nullptr };
  CQChartsLineEdit*        nameEdit_      { nullptr };
  QLabel*                  typeLabel_     { nullptr };
  CQChartsColumnTypeCombo* typeCombo_     { nullptr };
};

#endif
