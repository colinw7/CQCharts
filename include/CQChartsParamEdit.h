#ifndef CQChartsParamEdit_H
#define CQChartsParamEdit_H

#include <CQBaseModelTypes.h>

#include <QFrame>

class CQChartsColorLineEdit;
class CQChartsLineEdit;
class CQIntegerSpin;
class CQCheckBox;
class CQIconButton;

class QComboBox;
class QHBoxLayout;

/*!
 * \brief Widget to edit Plot Parameter
 * \ingroup Charts
 */
class CQChartsParamEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsParamEdit(QWidget *parent=nullptr);

  CQBaseModelType type() const { return type_; }

  void setString(const QString &str="");

  void setBool(bool b=false);

  void setInteger(int i=0);

  void setEnum(const QString &str, const QStringList &values);

  void setColor(const QString &str="");

  QString getString() const;

  bool getBool() const;

  int getInteger() const;

  QString getEnum() const;

  QString getColor() const;

  void reset();

 private Q_SLOTS:
  void clearSlot();

 private:
  CQBaseModelType        type_        { CQBaseModelType::NONE };
  QHBoxLayout*           layout_      { nullptr };
  QFrame*                editFrame_   { nullptr };
  QHBoxLayout*           editLayout_  { nullptr };
  CQChartsLineEdit*      edit_        { nullptr };
  CQIntegerSpin*         ispin_       { nullptr };
  CQCheckBox*            check_       { nullptr };
  QComboBox*             combo_       { nullptr };
  CQChartsColorLineEdit* color_       { nullptr };
  CQIconButton*          clearButton_ { nullptr };
};

#endif
