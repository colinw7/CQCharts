#ifndef CQChartsShapeDataEdit_H
#define CQChartsShapeDataEdit_H

#include <CQChartsData.h>
#include <CQChartsLineEditBase.h>

class CQChartsShapeDataEdit;
class CQChartsPlot;
class CQChartsView;

/*!
 * \brief Shape Data line edit
 * \ingroup Charts
 */
class CQChartsShapeDataLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsShapeData shapeData READ shapeData WRITE setShapeData)

 public:
  CQChartsShapeDataLineEdit(QWidget *parent=nullptr);

  const CQChartsShapeData &shapeData() const;
  void setShapeData(const CQChartsShapeData &c);

  void drawPreview(QPainter *painter, const QRect &rect) override;

 signals:
  void shapeDataChanged();

 private slots:
  void menuEditChanged();

 private:
  void textChanged() override;

  void updateShapeData(const CQChartsShapeData &c, bool updateText);

  void shapeDataToWidgets();

  void connectSlots(bool b) override;

 private:
  CQChartsShapeDataEdit* dataEdit_ { nullptr };
};

//---

#include <CQChartsEditBase.h>

class CQChartsFillDataEdit;
class CQChartsStrokeDataEdit;
class CQChartsShapeDataEditPreview;

/*!
 * \brief Shape data edit
 * \ingroup Charts
 */
class CQChartsShapeDataEdit : public CQChartsEditBase {
  Q_OBJECT

 public:
  CQChartsShapeDataEdit(QWidget *parent=nullptr, bool tabbed=false);

  const CQChartsShapeData &data() const { return data_; }
  void setData(const CQChartsShapeData &d);

  void setPlot(CQChartsPlot *plot) override;
  void setView(CQChartsView *view) override;

  void setTitle(const QString &title);

  void setPreview(bool b);

 signals:
  void shapeDataChanged();

 private:
  void dataToWidgets();

  void connectSlots(bool b);

 private slots:
  void widgetsToData();

 private:
  CQChartsPlot*                 plot_       { nullptr };
  CQChartsView*                 view_       { nullptr };
  bool                          tabbed_     { false };
  CQChartsShapeData             data_;
  CQChartsFillDataEdit*         fillEdit_   { nullptr };
  CQChartsStrokeDataEdit*       strokeEdit_ { nullptr };
  CQChartsShapeDataEditPreview* preview_    { nullptr };
};

//---

/*!
 * \brief Shape data edit preview
 * \ingroup Charts
 */
class CQChartsShapeDataEditPreview : public CQChartsEditPreview {
  Q_OBJECT

 public:
  CQChartsShapeDataEditPreview(CQChartsShapeDataEdit *edit);

  void draw(QPainter *painter) override;

  static void draw(QPainter *painter, const CQChartsShapeData &data, const QRect &rect,
                   CQChartsPlot *plot, CQChartsView *view);

 private:
  CQChartsShapeDataEdit *edit_ { nullptr };
};

//------

#include <CQChartsPropertyViewEditor.h>

/*!
 * \brief type for CQChartsShapeData
 * \ingroup Charts
 */
class CQChartsShapeDataPropertyViewType : public CQChartsPropertyViewType {
 public:
  CQPropertyViewEditorFactory *getEditor() const override;

  void drawPreview(QPainter *painter, const QRect &rect, const QVariant &value,
                   CQChartsPlot *plot, CQChartsView *view) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "shape_data"; }
};

//---

/*!
 * \brief editor factory for CQChartsShapeData
 * \ingroup Charts
 */
class CQChartsShapeDataPropertyViewEditor : public CQChartsPropertyViewEditorFactory {
 public:
  CQChartsLineEditBase *createPropertyEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
