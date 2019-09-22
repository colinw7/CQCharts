#ifndef CQChartsPositionEdit_H
#define CQChartsPositionEdit_H

#include <CQChartsPosition.h>
#include <QFrame>

class CQChartsUnitsEdit;
class CQPoint2DEdit;

/*!
 * \brief position edit
 * \ingroup Charts
 */
class CQChartsPositionEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsPosition position READ position WRITE setPosition)

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

/*!
 * \brief type for CQChartsPosition
 * \ingroup Charts
 */
class CQChartsPositionPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsPositionPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, bool inside) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "position"; }
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsPosition
 * \ingroup Charts
 */
class CQChartsPositionPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsPositionPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
