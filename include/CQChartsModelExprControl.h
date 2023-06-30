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
  Q_PROPERTY(CQChartsColumn       column READ column WRITE setColumn)
  Q_PROPERTY(QString              header READ header WRITE setHeader)
  Q_PROPERTY(QString              expr   READ expr   WRITE setExpr  )
  Q_PROPERTY(CQChartsColumnTypeId type   READ type   WRITE setType  )

  Q_ENUMS(Mode)

 public:
  enum class Mode {
    ADD,
    REMOVE,
    MODIFY,
    REPLACE
  };

  using Column = CQChartsColumn;
  using ModelP = QSharedPointer<QAbstractItemModel>;

 public:
  CQChartsModelExprControl(QWidget *parent=nullptr);

  //---

  Mode mode() const;
  void setMode(const Mode &m);

  Column column() const;
  void setColumn(const Column &c);

  QString header() const;
  void setHeader(const QString &s);

  QString expr() const;
  void setExpr(const QString &s);

  QString var() const;
  void setVar(const QString &s);

  CQChartsColumnTypeId type() const;
  void setType(const CQChartsColumnTypeId &t);

  //---

  CQChartsModelData *modelData() const { return modelData_; }
  void setModelData(CQChartsModelData *modelData) override;

  //---

  void init();

 private Q_SLOTS:
  void modeSlot();
  void columnSlot();

  void modelColumnSlot(int);

  void applySlot();

 private:
  // data
  CQChartsModelData* modelData_ { nullptr };

  // widgets
  QRadioButton*            addRadio_      { nullptr };
  QRadioButton*            removeRadio_   { nullptr };
  QRadioButton*            modifyRadio_   { nullptr };
  QRadioButton*            replaceRadio_  { nullptr };
  QLabel*                  columnLabel_   { nullptr };
  CQChartsColumnCombo*     columnNumEdit_ { nullptr };
  QLabel*                  headerLabel_   { nullptr };
  CQChartsLineEdit*        headerEdit_    { nullptr };
  QLabel*                  exprLabel_     { nullptr };
  CQChartsColumnExprEdit*  exprEdit_      { nullptr };
  QLabel*                  varLabel_      { nullptr };
  CQChartsLineEdit*        varEdit_       { nullptr };
  QLabel*                  typeLabel_     { nullptr };
  CQChartsColumnTypeCombo* typeCombo_     { nullptr };
  QLabel*                  oldValueLabel_ { nullptr };
  CQChartsLineEdit*        oldValueEdit_  { nullptr };
  QLabel*                  newValueLabel_ { nullptr };
  CQChartsLineEdit*        newValueEdit_  { nullptr };
};

#endif
