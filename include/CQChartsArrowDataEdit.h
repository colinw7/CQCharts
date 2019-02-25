#ifndef CQChartsArrowDataEdit_H
#define CQChartsArrowDataEdit_H

#include <CQChartsData.h>
#include <CQChartsLineEditBase.h>

class CQChartsArrowDataEdit;
class CQChartsPlot;
class CQChartsView;

class CQChartsArrowDataLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsArrowData arrowData READ arrowData WRITE setArrowData)

 public:
  CQChartsArrowDataLineEdit(QWidget *parent=nullptr);

  const CQChartsArrowData &arrowData() const;
  void setArrowData(const CQChartsArrowData &c);

  void drawPreview(QPainter *painter, const QRect &rect) override;

 signals:
  void arrowDataChanged();

 private slots:
  void menuEditChanged();

 private:
  void textChanged() override;

  void updateArrowData(const CQChartsArrowData &c, bool updateText);

  void arrowDataToWidgets();

  void connectSlots(bool b) override;

 private:
  CQChartsArrowDataEdit* dataEdit_ { nullptr }; //! arrow data edit
};

//---

#include <CQChartsEditBase.h>

class CQChartsLengthEdit;
class CQChartsArrowDataEditPreview;
class CQAngleSpinBox;
class QCheckBox;

class CQChartsArrowDataEdit : public CQChartsEditBase {
  Q_OBJECT

 public:
  CQChartsArrowDataEdit(QWidget *parent=nullptr);

  const CQChartsArrowData &data() const { return data_; }
  void setData(const CQChartsArrowData &d);

  void setTitle(const QString &title);

  void setPreview(bool b);

 signals:
  void arrowDataChanged();

 private:
  void dataToWidgets();

 private slots:
  void widgetsToData();

 private:
  CQChartsArrowData             data_;                      //! arrow data
  QCheckBox*                    relativeEdit_  { nullptr }; //! relative edit
  CQChartsLengthEdit*           lengthEdit_    { nullptr }; //! length edit
  CQAngleSpinBox*               angleEdit_     { nullptr }; //! angle edit
  CQAngleSpinBox*               backAngleEdit_ { nullptr }; //! back angle edit
  QCheckBox*                    fheadEdit_     { nullptr }; //! start head edit
  QCheckBox*                    theadEdit_     { nullptr }; //! end head edit
  QCheckBox*                    lineEndsEdit_  { nullptr }; //! line ends edit
  CQChartsLengthEdit*           lineWidthEdit_ { nullptr }; //! line width edit
  CQChartsArrowDataEditPreview* preview_       { nullptr }; //! preview
};

//---

class CQChartsArrowDataEditPreview : public CQChartsEditPreview {
  Q_OBJECT

 public:
  CQChartsArrowDataEditPreview(CQChartsArrowDataEdit *edit);

  void draw(QPainter *painter) override;

  static void draw(QPainter *painter, const CQChartsArrowData &data, const QRect &rect,
                   CQChartsPlot *plot, CQChartsView *view);

 private:
  CQChartsArrowDataEdit *edit_ { nullptr }; //! arrow data edit
};

//------

#include <CQChartsPropertyViewEditor.h>

// type for CQChartsArrowData
class CQChartsArrowDataPropertyViewType : public CQChartsPropertyViewType {
 public:
  CQPropertyViewEditorFactory *getEditor() const override;

  void drawPreview(QPainter *painter, const QRect &rect, const QVariant &value,
                   CQChartsPlot *plot, CQChartsView *view) override;

  QString tip(const QVariant &value) const override;
};

//---

// editor factory for CQChartsArrowData
class CQChartsArrowDataPropertyViewEditor : public CQChartsPropertyViewEditorFactory {
 public:
  CQChartsLineEditBase *createPropertyEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
