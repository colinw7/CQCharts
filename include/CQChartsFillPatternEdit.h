#ifndef CQChartsFillPatternEdit_H
#define CQChartsFillPatternEdit_H

#include <CQChartsFillPattern.h>
#include <CQChartsLineEditBase.h>

class CQChartsFillPatternEdit;
class CQChartsColorLineEdit;
class CQChartsAlphaEdit;
class CQCharts;

/*!
 * \brief FillPattern line edit
 * \ingroup Charts
 */
class CQChartsFillPatternLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsFillPattern fillPattern READ fillPattern WRITE setFillPattern)

 public:
  CQChartsFillPatternLineEdit(QWidget *parent=nullptr);

  void setCharts(CQCharts *charts);

  const CQChartsFillPattern &fillPattern() const;
  void setFillPattern(const CQChartsFillPattern &c);

  void setNoFocus();

  void drawPreview(QPainter *painter, const QRect &rect) override;

 signals:
  void fillPatternChanged();

 private slots:
  void menuEditChanged();

 private:
  void textChanged() override;

  void updateFillPattern(const CQChartsFillPattern &c, bool updateText);

  void fillPatternToWidgets();

  void connectSlots(bool b);

 private:
  CQChartsFillPatternEdit* dataEdit_ { nullptr }; //!< fillPattern data edit
};

//---

#include <CQChartsEditBase.h>

class CQChartsPaletteNameEdit;
class CQChartsAngleEdit;
class CQChartsImageEdit;

class CQRealSpin;

class QComboBox;

/*!
 * \brief FillPattern edit
 * \ingroup Charts
 */
class CQChartsFillPatternEdit : public CQChartsEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsFillPattern fillPattern READ fillPattern WRITE setFillPattern)

 public:
  CQChartsFillPatternEdit(QWidget *parent=nullptr);

  void setCharts(CQCharts *charts);

  const CQChartsFillPattern &fillPattern() const { return fillPattern_; }
  void setFillPattern(const CQChartsFillPattern &c);

  void setNoFocus();

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

 signals:
  void fillPatternChanged();

 private slots:
  void widgetsToFillPattern();

  void updateState();

 private:
  void fillPatternToWidgets();

  void connectSlots(bool b);

 private:
  using WidgetLabels = std::map<QWidget*, QWidget*>;

  CQChartsFillPattern      fillPattern_;              //!< fill pattern
  QComboBox*               typeCombo_    { nullptr }; //!< type combo
  CQRealSpin*              scaleEdit_    { nullptr }; //!< scale edit
  CQChartsPaletteNameEdit* paletteEdit_  { nullptr }; //!< palette name edit
  CQChartsImageEdit*       imageEdit_    { nullptr }; //!< image edit
  CQChartsAngleEdit*       angleEdit_    { nullptr }; //!< angle edit
  CQChartsColorLineEdit*   altColorEdit_ { nullptr }; //!< alt color edit
  CQChartsAlphaEdit*       altAlphaEdit_ { nullptr }; //!< alt alpha edit
  WidgetLabels             widgetLabels_;             //!< widget labels
  bool                     connected_    { false };   //!< is connected
};

//------

#include <CQPropertyViewType.h>

/*!
 * \brief type for CQChartsFillPattern
 * \ingroup Charts
 */
class CQChartsFillPatternPropertyViewType : public CQPropertyViewType {
 public:
  CQChartsFillPatternPropertyViewType();

  CQPropertyViewEditorFactory *getEditor() const override;

  bool setEditorData(CQPropertyViewItem *item, const QVariant &value) override;

  void draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
            const QStyleOptionViewItem &option, const QModelIndex &index,
            const QVariant &value, const ItemState &itemState) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "fillPattern"; }
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsFillPattern
 * \ingroup Charts
 */
class CQChartsFillPatternPropertyViewEditor : public CQPropertyViewEditorFactory {
 public:
  CQChartsFillPatternPropertyViewEditor();

  QWidget *createEdit(QWidget *parent);

  void connect(QWidget *w, QObject *obj, const char *method);

  QVariant getValue(QWidget *w);

  void setValue(QWidget *w, const QVariant &var);
};

#endif
