#ifndef CQChartsImageEdit_H
#define CQChartsImageEdit_H

#include <CQChartsImage.h>
#include <CQChartsEditBase.h>
#include <QFrame>

class CQChartsLineEdit;
class CQIconButton;

/*!
 * \brief image edit
 * \ingroup Charts
 */
class CQChartsImageEdit : public CQChartsFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsImage image READ image WRITE setImage)

 public:
  CQChartsImageEdit(QWidget *parent=nullptr);

  const CQChartsImage &image() const;
  void setImage(const CQChartsImage &image);

 private:
  void connectSlots(bool b);

 Q_SIGNALS:
  void imageChanged();

 private Q_SLOTS:
  void editChanged();

  void fileSlot();

 private:
  CQChartsImage     image_;
  CQChartsLineEdit* edit_   { nullptr };
  CQIconButton*     button_ { nullptr };
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsImage
 * \ingroup Charts
 */
class CQChartsImagePropertyViewType : public CQPropertyViewType {
 public:
  CQChartsImagePropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "image"; }
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsImage
 * \ingroup Charts
 */
class CQChartsImagePropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsImagePropertyViewEditor();

  QWidget *createEdit(QWidget *parent) override;

  void connect(QWidget *w, QObject *obj, const char *method) override;

  QVariant getValue(QWidget *w) override;

  void setValue(QWidget *w, const QVariant &var) override;
};

#endif
