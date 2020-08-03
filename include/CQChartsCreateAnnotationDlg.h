#ifndef CQChartsCreateAnnotationDlg_H
#define CQChartsCreateAnnotationDlg_H

#include <QDialog>

class CQChartsView;
class CQChartsPlot;
class CQChartsPositionEdit;
class CQChartsRectEdit;
class CQChartsLengthEdit;
class CQChartsMarginEdit;
class CQChartsSidesEdit;
class CQChartsPolygonEdit;
class CQChartsSymbolDataEdit;
class CQChartsTextDataEdit;
class CQChartsFillDataEdit;
class CQChartsStrokeDataEdit;
class CQChartsArrowDataEdit;
class CQChartsColorLineEdit;
class CQChartsAngleEdit;
class CQChartsLineEdit;

class CQRealSpin;
class CQCheckBox;

class QStackedWidget;
class QFrame;
class QComboBox;
class QRadioButton;
class QLabel;
class QBoxLayout;
class QHBoxLayout;
class QGridLayout;

/*!
 * \brief dialog to create a new annotation
 * \ingroup Charts
 */
class CQChartsCreateAnnotationDlg : public QDialog {
  Q_OBJECT

 public:
  CQChartsCreateAnnotationDlg(QWidget *parent, CQChartsView *view);
  CQChartsCreateAnnotationDlg(QWidget *parent, CQChartsPlot *plot);

  CQChartsView *view() const { return view_; }
  CQChartsPlot *plot() const { return plot_; }

 private:
  struct Widgets {
    QFrame*                 frame              { nullptr };
    CQChartsFillDataEdit*   backgroundDataEdit { nullptr };
    CQChartsStrokeDataEdit* strokeDataEdit     { nullptr };
    CQChartsSidesEdit*      borderSidesEdit    { nullptr };
  };

  struct RectWidgets : public Widgets {
    CQChartsRectEdit*   rectEdit    { nullptr };
    CQChartsMarginEdit* marginEdit  { nullptr };
    CQChartsMarginEdit* paddingEdit { nullptr };
  };

  struct EllipseWidgets : public Widgets {
    QRadioButton*         centerRadio { nullptr };
    QRadioButton*         rectRadio   { nullptr };
    CQChartsPositionEdit* centerEdit  { nullptr };
    CQChartsLengthEdit*   rxEdit      { nullptr };
    CQChartsLengthEdit*   ryEdit      { nullptr };
    CQChartsRectEdit*     rectEdit    { nullptr };
    CQChartsLineEdit*     paddingEdit { nullptr };
  };

  struct PolygonWidgets : public Widgets {
    CQChartsPolygonEdit* pointsEdit { nullptr };
  };

  struct PolylineWidgets : public Widgets {
    CQChartsPolygonEdit* pointsEdit { nullptr };
  };

  struct TextWidgets : public Widgets {
    QRadioButton*         positionRadio { nullptr };
    QRadioButton*         rectRadio     { nullptr };
    CQChartsPositionEdit* positionEdit  { nullptr };
    CQChartsRectEdit*     rectEdit      { nullptr };
    CQChartsLineEdit*     textEdit      { nullptr };
    CQChartsTextDataEdit* dataEdit      { nullptr };
  };

  struct ImageWidgets : public Widgets {
    QRadioButton*         positionRadio     { nullptr };
    QRadioButton*         rectRadio         { nullptr };
    CQChartsPositionEdit* positionEdit      { nullptr };
    CQChartsRectEdit*     rectEdit          { nullptr };
    CQChartsLineEdit*     imageEdit         { nullptr };
    CQChartsLineEdit*     disabledImageEdit { nullptr };
  };

  struct ArrowWidgets : public Widgets {
    CQChartsPositionEdit*  startEdit       { nullptr };
    CQChartsPositionEdit*  endEdit         { nullptr };
    CQChartsArrowDataEdit* dataEdit        { nullptr };
    CQChartsLengthEdit*    strokeWidthEdit { nullptr };
    CQChartsColorLineEdit* strokeColorEdit { nullptr };
    CQCheckBox*            filledCheck     { nullptr };
    CQChartsColorLineEdit* fillColorEdit   { nullptr };
  };

  struct PointWidgets : public Widgets {
    CQChartsPositionEdit*   positionEdit { nullptr };
    CQChartsSymbolDataEdit* dataEdit     { nullptr };
  };

  struct PieSliceWidgets : public Widgets {
    CQChartsPositionEdit* centerEdit      { nullptr };
    CQChartsLengthEdit*   innerRadiusEdit { nullptr };
    CQChartsLengthEdit*   outerRadiusEdit { nullptr };
    CQChartsAngleEdit*    startAngleEdit  { nullptr };
    CQChartsAngleEdit*    spanAngleEdit   { nullptr };
  };

  struct AxisWidgets : public Widgets {
  };

  struct KeyWidgets : public Widgets {
  };

  struct PointSetWidgets : public Widgets {
  };

  struct ValueSetWidgets : public Widgets {
  };

  struct ButtonWidgets : public Widgets {
    CQChartsPositionEdit* positionEdit { nullptr };
    CQChartsLineEdit*     textEdit     { nullptr };
  };

 private:
  QStringList typeNames() const;

  void initWidgets();

  void createRectFrame    ();
  void createEllipseFrame ();
  void createPolygonFrame ();
  void createPolyLineFrame();
  void createTextFrame    ();
  void createImageFrame   ();
  void createArrowFrame   ();
  void createPointFrame   ();
  void createPieSliceFrame();
  void createAxisFrame    ();
  void createKeyFrame     ();
  void createPointSetFrame();
  void createValueSetFrame();
  void createButtonFrame  ();

  void addFillWidgets  (Widgets &widgets, QBoxLayout *playout);
  void addStrokeWidgets(Widgets &widgets, QBoxLayout *playout, bool corner);

  void addSidesWidget(Widgets &widgets, QBoxLayout *playout);

  QHBoxLayout *addLabelWidget(QBoxLayout *playout, const QString &label, QWidget *widget);

  //---

  CQChartsLineEdit *createLineEdit(const QString &name, const QString &tip="") const;

  CQChartsPositionEdit* createPositionEdit(const QString &name, double x, double y,
                                           const QString &tip="") const;

  CQChartsLengthEdit *createLengthEdit(const QString &name, double l,
                                       const QString &tip="") const;

  CQChartsRectEdit *createRectEdit(const QString &name, const QString &tip="") const;

  CQChartsPolygonEdit *createPolygonEdit(const QString &name, const QString &tip="") const;

  CQChartsMarginEdit *createMarginEdit(const QString &name, const QString &tip="") const;

  CQChartsColorLineEdit *createColorEdit(const QString &name, const QString &tip="") const;

  CQChartsAngleEdit *createAngleEdit(const QString &name, double a,
                                     const QString &tip="") const;

  //---

  bool createRectangleAnnotation();
  bool createEllipseAnnotation  ();
  bool createPolygonAnnotation  ();
  bool createPolylineAnnotation ();
  bool createTextAnnotation     ();
  bool createImageAnnotation    ();
  bool createArrowAnnotation    ();
  bool createPointAnnotation    ();
  bool createPieSliceAnnotation ();
  bool createAxisAnnotation     ();
  bool createKeyAnnotation      ();
  bool createPointSetAnnotation ();
  bool createValueSetAnnotation ();
  bool createButtonAnnotation   ();

  bool setErrorMsg(const QString &msg);
  void clearErrorMsg();

 private slots:
  void typeSlot(int ind);

  void ellipseCenterSlot(bool);
  void textPositionSlot (bool);
  void imagePositionSlot(bool);

  void okSlot();
  bool applySlot();
  void cancelSlot();

 private:
  CQChartsView*     view_             { nullptr }; //!< associated view
  CQChartsPlot*     plot_             { nullptr }; //!< associated plot
  QComboBox*        typeCombo_        { nullptr }; //!< type combo
  CQChartsLineEdit* idEdit_           { nullptr }; //!< id edit
  CQChartsLineEdit* tipEdit_          { nullptr }; //!< tip edit
  QStackedWidget*   typeStack_        { nullptr }; //!< type stacked widget
  RectWidgets       rectWidgets_;                  //!< rect edit widgets
  EllipseWidgets    ellipseWidgets_;               //!< ellipse edit widgets
  PolygonWidgets    polygonWidgets_;               //!< polygon edit widgets
  PolylineWidgets   polylineWidgets_;              //!< polyline edit widgets
  TextWidgets       textWidgets_;                  //!< text edit widgets
  ImageWidgets      imageWidgets_;                 //!< image edit widgets
  ArrowWidgets      arrowWidgets_;                 //!< arrow edit widgets
  PointWidgets      pointWidgets_;                 //!< point edit widgets
  PieSliceWidgets   pieSliceWidgets_;              //!< pie slice widgets
  AxisWidgets       axisWidgets_;                  //!< axis widgets
  KeyWidgets        keyWidgets_;                   //!< key widgets
  PointSetWidgets   pointSetWidgets_;              //!< point set widgets
  ValueSetWidgets   valueSetWidgets_;              //!< value set widgets
  ButtonWidgets     buttonWidgets_;                //!< button widgets
  QLabel*           msgLabel_         { nullptr }; //!< error message label
};

#endif
