#ifndef CQChartsArrowDataEdit_H
#define CQChartsArrowDataEdit_H

#include <CQChartsData.h>
#include <CQChartsArrowData.h>
#include <CQChartsLineEditBase.h>

class CQChartsArrowDataEdit;
class CQChartsPlot;
class CQChartsView;

/*!
 * \brief Arrow data line edit
 * \ingroup Charts
 */
class CQChartsArrowDataLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsArrowData arrowData READ arrowData WRITE setArrowData)

 public:
  using ArrowData = CQChartsArrowData;

 public:
  CQChartsArrowDataLineEdit(QWidget *parent=nullptr);

  const ArrowData &arrowData() const;
  void setArrowData(const ArrowData &c);

  void drawPreview(QPainter *painter, const QRect &rect) override;

 Q_SIGNALS:
  void arrowDataChanged();

 private Q_SLOTS:
  void menuEditChanged();

 private:
  void textChanged() override;

  void updateArrowData(const ArrowData &c, bool updateText);

  void arrowDataToWidgets();

  void connectSlots(bool b);

 private:
  CQChartsArrowDataEdit* dataEdit_ { nullptr }; //!< arrow data edit
};

//---

#include <CQChartsEditBase.h>

class CQChartsLengthEdit;
class CQChartsArrowDataEditPreview;
class CQChartsAngleEdit;
class CQCheckBox;

/*!
 * \brief Arrow data edit
 * \ingroup Charts
 */
class CQChartsArrowDataEdit : public CQChartsEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsArrowData data READ data WRITE setData)

 public:
  using ArrowData = CQChartsArrowData;

 public:
  CQChartsArrowDataEdit(QWidget *parent=nullptr);

  const ArrowData &data() const { return data_; }
  void setData(const ArrowData &d);

  void setTitle(const QString &title);

  void setPreview(bool b);

  CQChartsLengthEdit *lengthEdit() const { return lengthEdit_; }

 Q_SIGNALS:
  void arrowDataChanged();

 private:
  void connectSlots(bool b);

  void dataToWidgets();

 private Q_SLOTS:
  void widgetsToData();

 private:
  using Preview = CQChartsArrowDataEditPreview;

  ArrowData           data_;                      //!< arrow data
//CQCheckBox*         relativeEdit_  { nullptr }; //!< relative edit
  CQChartsLengthEdit* lengthEdit_    { nullptr }; //!< length edit
  CQChartsAngleEdit*  angleEdit_     { nullptr }; //!< angle edit
  CQChartsAngleEdit*  backAngleEdit_ { nullptr }; //!< back angle edit
  CQCheckBox*         fheadEdit_     { nullptr }; //!< start head edit
  CQCheckBox*         theadEdit_     { nullptr }; //!< end head edit
  CQCheckBox*         lineEndsEdit_  { nullptr }; //!< line ends edit
  CQChartsLengthEdit* lineWidthEdit_ { nullptr }; //!< line width edit
  Preview*            preview_       { nullptr }; //!< preview
  bool                connected_     { false };   //!< is connected
};

//---

/*!
 * \brief Arrow data edit preview
 * \ingroup Charts
 */
class CQChartsArrowDataEditPreview : public CQChartsEditPreview {
  Q_OBJECT

 public:
  using View      = CQChartsView;
  using Plot      = CQChartsPlot;
  using ArrowData = CQChartsArrowData;

 public:
  CQChartsArrowDataEditPreview(CQChartsArrowDataEdit *edit);

  void draw(QPainter *painter) override;

  static void draw(QPainter *painter, const ArrowData &data, const QRect &rect,
                   Plot *plot, View *view);

 private:
  CQChartsArrowDataEdit *edit_ { nullptr }; //!< arrow data edit
};

//------

#include <CQChartsPropertyViewEditor.h>

/*!
 * \brief type for CQChartsArrowData
 * \ingroup Charts
 */
class CQChartsArrowDataPropertyViewType : public CQChartsPropertyViewType {
 public:
  using View = CQChartsView;
  using Plot = CQChartsPlot;

 public:
  CQPropertyViewEditorFactory *getEditor() const override;

  void drawPreview(QPainter *painter, const QRect &rect, const QVariant &value,
                   Plot *plot, View *view) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "arrow_data"; }
};

//---

/*!
 * \brief editor factory for CQChartsArrowData
 * \ingroup Charts
 */
class CQChartsArrowDataPropertyViewEditor : public CQChartsPropertyViewEditorFactory {
 public:
  CQChartsLineEditBase *createPropertyEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
