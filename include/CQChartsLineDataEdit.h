#ifndef CQChartsLineDataEdit_H
#define CQChartsLineDataEdit_H

#include <CQChartsData.h>
#include <CQChartsLineEditBase.h>

class CQChartsLineDataEdit;
class CQChartsPlot;
class CQChartsView;

class CQChartsLineDataLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsLineData lineData READ lineData WRITE setLineData)

 public:
  CQChartsLineDataLineEdit(QWidget *parent=nullptr);

  const CQChartsLineData &lineData() const;
  void setLineData(const CQChartsLineData &c);

  void drawPreview(QPainter *painter, const QRect &rect) override;

 signals:
  void lineDataChanged();

 private slots:
  void menuEditChanged();

 private:
  void textChanged() override;

  void updateLineData(const CQChartsLineData &c, bool updateText);

  void lineDataToWidgets();

  void connectSlots(bool b) override;

 private:
  CQChartsLineDataEdit* dataEdit_ { nullptr };
};

//---

#include <CQChartsEditBase.h>

class CQChartsColorLineEdit;
class CQChartsAlphaEdit;
class CQChartsLengthEdit;
class CQChartsLineDashEdit;
class CQChartsLineDataEditPreview;
class QGroupBox;

class CQChartsLineDataEdit : public CQChartsEditBase {
  Q_OBJECT

 public:
  CQChartsLineDataEdit(QWidget *parent=nullptr);

  const CQChartsLineData &data() const { return data_; }
  void setData(const CQChartsLineData &d);

 signals:
  void lineDataChanged();

 private:
  void dataToWidgets();

 private slots:
  void widgetsToData();

 private:
  CQChartsPlot*                plot_      { nullptr };
  CQChartsView*                view_      { nullptr };
  CQChartsLineData             data_;
  QGroupBox*                   groupBox_  { nullptr };
  CQChartsColorLineEdit*       colorEdit_ { nullptr };
  CQChartsAlphaEdit*           alphaEdit_ { nullptr };
  CQChartsLengthEdit*          widthEdit_ { nullptr };
  CQChartsLineDashEdit*        dashEdit_  { nullptr };
  CQChartsLineDataEditPreview* preview_   { nullptr };
};

//---

class CQChartsLineDataEditPreview : public CQChartsEditPreview {
  Q_OBJECT

 public:
  CQChartsLineDataEditPreview(CQChartsLineDataEdit *edit);

  void draw(QPainter *painter) override;

  static void draw(QPainter *painter, const CQChartsLineData &data, const QRect &rect,
                   CQChartsPlot *plot, CQChartsView *view);

 private:
  CQChartsLineDataEdit *edit_ { nullptr };
};

//------

#include <CQChartsPropertyViewEditor.h>

// type for CQChartsLineData
class CQChartsLineDataPropertyViewType : public CQChartsPropertyViewType {
 public:
  CQPropertyViewEditorFactory *getEditor() const override;

  void drawPreview(QPainter *painter, const QRect &rect, const QVariant &value,
                   CQChartsPlot *plot, CQChartsView *view) override;

  QString tip(const QVariant &value) const override;
};

//---

// editor factory for CQChartsLineData
class CQChartsLineDataPropertyViewEditor : public CQChartsPropertyViewEditorFactory {
 public:
  CQChartsLineEditBase *createPropertyEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
