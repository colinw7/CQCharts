#ifndef CQChartsFillDataEdit_H
#define CQChartsFillDataEdit_H

#include <CQChartsData.h>
#include <CQChartsLineEditBase.h>

class CQChartsFillDataEdit;
class CQChartsPlot;
class CQChartsView;

class CQChartsFillDataLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsFillData fillData READ fillData WRITE setFillData)

 public:
  CQChartsFillDataLineEdit(QWidget *parent=nullptr);

  const CQChartsFillData &fillData() const;
  void setFillData(const CQChartsFillData &c);

  void drawPreview(QPainter *painter, const QRect &rect) override;

 signals:
  void fillDataChanged();

 private slots:
  void menuEditChanged();

 private:
  void textChanged() override;

  void updateFillData(const CQChartsFillData &c, bool updateText);

  void fillDataToWidgets();

  void connectSlots(bool b) override;

 private:
  CQChartsFillDataEdit* dataEdit_ { nullptr };
};

//---

#include <CQChartsEditBase.h>

class CQChartsColorLineEdit;
class CQChartsAlphaEdit;
class CQChartsFillPatternEdit;
class CQChartsFillDataEditPreview;
class CQGroupBox;

class CQChartsFillDataEdit : public CQChartsEditBase {
  Q_OBJECT

 public:
  CQChartsFillDataEdit(QWidget *parent=nullptr);

  const CQChartsFillData &data() const { return data_; }
  void setData(const CQChartsFillData &d);

  void setTitle(const QString &title);

  void setPreview(bool b);

 signals:
  void fillDataChanged();

 private:
  void dataToWidgets();

 private slots:
  void widgetsToData();

 private:
  CQChartsPlot*                plot_        { nullptr };
  CQChartsView*                view_        { nullptr };
  CQChartsFillData             data_;
  CQGroupBox*                  groupBox_    { nullptr };
  CQChartsColorLineEdit*       colorEdit_   { nullptr };
  CQChartsAlphaEdit*           alphaEdit_   { nullptr };
  CQChartsFillPatternEdit*     patternEdit_ { nullptr };
  CQChartsFillDataEditPreview* preview_     { nullptr };
};

//---

class CQChartsFillDataEditPreview : public CQChartsEditPreview {
  Q_OBJECT

 public:
  CQChartsFillDataEditPreview(CQChartsFillDataEdit *edit);

  void draw(QPainter *painter) override;

  static void draw(QPainter *painter, const CQChartsFillData &data, const QRect &rect,
                   CQChartsPlot *plot, CQChartsView *view);

 private:
  CQChartsFillDataEdit *edit_ { nullptr };
};

//------

#include <CQChartsPropertyViewEditor.h>

// type for CQChartsFillData
class CQChartsFillDataPropertyViewType : public CQChartsPropertyViewType {
 public:
  CQPropertyViewEditorFactory *getEditor() const override;

  void drawPreview(QPainter *painter, const QRect &rect, const QVariant &value,
                   CQChartsPlot *plot, CQChartsView *view) override;

  QString tip(const QVariant &value) const override;
};

//---

// editor factory for CQChartsFillData
class CQChartsFillDataPropertyViewEditor : public CQChartsPropertyViewEditorFactory {
 public:
  CQChartsLineEditBase *createPropertyEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

//------

#endif
