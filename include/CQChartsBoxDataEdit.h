#ifndef CQChartsBoxDataEdit_H
#define CQChartsBoxDataEdit_H

#include <CQChartsData.h>
#include <CQChartsLineEditBase.h>

class CQChartsBoxDataEdit;
class CQChartsMarginEdit;
class CQChartsView;
class CQChartsPlot;

/*!
 * \brief Box data line edit
 * \ingroup Charts
 */
class CQChartsBoxDataLineEdit : public CQChartsLineEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsBoxData boxData READ boxData WRITE setBoxData)

 public:
  CQChartsBoxDataLineEdit(QWidget *parent=nullptr);

  const CQChartsBoxData &boxData() const;
  void setBoxData(const CQChartsBoxData &c);

  void drawPreview(QPainter *painter, const QRect &rect) override;

 Q_SIGNALS:
  void boxDataChanged();

 private Q_SLOTS:
  void menuEditChanged();

 private:
  void textChanged() override;

  void updateBoxData(const CQChartsBoxData &c, bool updateText);

  void boxDataToWidgets();

  void connectSlots(bool b);

 private:
  CQChartsBoxDataEdit* dataEdit_ { nullptr };
};

//---

#include <CQChartsEditBase.h>

class CQChartsShapeDataEdit;
class CQChartsSidesEdit;
class CQChartsBoxDataEditPreview;
class CQRealSpin;
class CQGroupBox;

/*!
 * \brief Box data edit
 * \ingroup Charts
 */
class CQChartsBoxDataEdit : public CQChartsEditBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsBoxData data READ data WRITE setData)

 public:
  using View = CQChartsView;
  using Plot = CQChartsPlot;

 public:
  CQChartsBoxDataEdit(QWidget *parent=nullptr);

  const CQChartsBoxData &data() const { return data_; }
  void setData(const CQChartsBoxData &d);

  void setPlot(Plot *plot) override;
  void setView(View *view) override;

  void setTitle(const QString &title);

  void setPreview(bool b);

 Q_SIGNALS:
  void boxDataChanged();

 private:
  void dataToWidgets();

  void connectSlots(bool b);

 private Q_SLOTS:
  void widgetsToData();

 private:
  CQChartsBoxData             data_;                    //!< box data
  CQGroupBox*                 groupBox_    { nullptr }; //!< group box
  CQChartsMarginEdit*         marginEdit_  { nullptr }; //!< outer margin edit
  CQChartsMarginEdit*         paddingEdit_ { nullptr }; //!< inner padding edit
  CQChartsShapeDataEdit*      shapeEdit_   { nullptr }; //!< shape edit
  CQChartsSidesEdit*          sidesEdit_   { nullptr }; //!< sides edit
  CQChartsBoxDataEditPreview* preview_     { nullptr }; //!< preview widget
  bool                        connected_   { false };   //!< is connected
};

//---

/*!
 * \brief Box data edit preview
 * \ingroup Charts
 */
class CQChartsBoxDataEditPreview : public CQChartsEditPreview {
  Q_OBJECT

 public:
  CQChartsBoxDataEditPreview(CQChartsBoxDataEdit *edit);

  void draw(QPainter *painter) override;

  static void draw(QPainter *painter, const CQChartsBoxData &data, const QRect &rect,
                   Plot *plot, View *view);

 private:
  CQChartsBoxDataEdit *edit_ { nullptr };
};

//------

#include <CQChartsPropertyViewEditor.h>

/*!
 * \brief type for CQChartsBoxData
 * \ingroup Charts
 */
class CQChartsBoxDataPropertyViewType : public CQChartsPropertyViewType {
 public:
  using View = CQChartsView;
  using Plot = CQChartsPlot;

 public:
  CQPropertyViewEditorFactory *getEditor() const override;

  void drawPreview(QPainter *painter, const QRect &rect, const QVariant &value,
                   Plot *plot, View *view) override;

  QString tip(const QVariant &value) const override;

  QString userName() const override { return "box_data"; }
};

//---

#include <CQPropertyViewEditor.h>

/*!
 * \brief editor factory for CQChartsBoxData
 * \ingroup Charts
 */
class CQChartsBoxDataPropertyViewEditor : public CQChartsPropertyViewEditorFactory {
 public:
  CQChartsLineEditBase *createPropertyEdit(QWidget *parent) override;

  void connect(QWidget *w, QObject *obj, const char *method) override;

  QVariant getValue(QWidget *w) override;

  void setValue(QWidget *w, const QVariant &var) override;
};

#endif
