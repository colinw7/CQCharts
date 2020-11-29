#ifndef CQChartsImageEdit_H
#define CQChartsImageEdit_H

#include <CQChartsImage.h>
#include <QFrame>

class CQChartsLineEdit;

class QToolButton;

/*!
 * \brief image edit
 * \ingroup Charts
 */
class CQChartsImageEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(CQChartsImage image READ image WRITE setImage)

 public:
  CQChartsImageEdit(QWidget *parent=nullptr);

  const CQChartsImage &image() const;
  void setImage(const CQChartsImage &image);

 private:
  void connectSlots(bool b);

 signals:
  void imageChanged();

 private slots:
  void editChanged();

  void fileSlot();

 private:
  CQChartsImage     image_;
  CQChartsLineEdit* edit_   { nullptr };
  QToolButton*      button_ { nullptr };
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

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
