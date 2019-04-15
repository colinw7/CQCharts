#ifndef CQChartsPositionEdit_H
#define CQChartsPositionEdit_H

#include <CQChartsPosition.h>
#include <QFrame>

class CQChartsUnitsEdit;
class CQPoint2DEdit;

//! \brief position edit
class CQChartsPositionEdit : public QFrame {
  Q_OBJECT

 public:
  CQChartsPositionEdit(QWidget *parent=nullptr);

  const CQChartsPosition &position() const;
  void setPosition(const CQChartsPosition &pos);

 signals:
  void positionChanged();

 private slots:
  void editChanged();
  void unitsChanged();

 private:
  void widgetsToPosition();
  void positionToWidgets();

  void connectSlots(bool b);

 private:
  CQChartsPosition   position_;              //!< position data
  CQPoint2DEdit*     edit_      { nullptr }; //!< position edit
  CQChartsUnitsEdit* unitsEdit_ { nullptr }; //!< units edit
  bool               connected_ { false };   //!< is connected
};

//------

#include <CQPropertyViewType.h>

//! \brief type for CQChartsPosition
class CQChartsPositionPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsPositionPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;
};

//---

#include <CQPropertyViewEditor.h>

//! \brief editor factory for CQChartsPosition
class CQChartsPositionPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsPositionPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
