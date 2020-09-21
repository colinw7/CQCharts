#ifndef CQChartsEditAnnotationDlg_H
#define CQChartsEditAnnotationDlg_H

#include <QDialog>

class CQChartsAnnotation;
class CQChartsPosition;
class CQChartsPositionEdit;
class CQChartsRect;
class CQChartsRectEdit;
class CQChartsLength;
class CQChartsLengthEdit;
class CQChartsMargin;
class CQChartsMarginEdit;
class CQChartsSidesEdit;
class CQChartsPolygon;
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

class QFrame;
class QRadioButton;
class QLabel;
class QBoxLayout;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;

/*!
 * \brief dialog to create a new annotation
 * \ingroup Charts
 */
class CQChartsEditAnnotationDlg : public QDialog {
  Q_OBJECT

 public:
  CQChartsEditAnnotationDlg(QWidget *parent, CQChartsAnnotation *annotation);

  CQChartsAnnotation *annotation() const { return annotation_; }

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

  //---

  void addFillWidgets  (Widgets &widgets, QBoxLayout *playout);
  void addStrokeWidgets(Widgets &widgets, QBoxLayout *playout, bool corner);

  void addSidesWidget(Widgets &widgets, QBoxLayout *playout);

  QHBoxLayout *addLabelWidget(QBoxLayout *playout, const QString &label, QWidget *widget);

  //---

  CQChartsLineEdit*     createLineEdit(const QString &name, const QString &text,
                                       const QString &tip) const;
  CQChartsPositionEdit* createPositionEdit(const QString &name, const CQChartsPosition &pos,
                                           const QString &tip) const;
  CQChartsLengthEdit*   createLengthEdit(const QString &name, const CQChartsLength &len,
                                         const QString &tip) const;
  CQChartsRectEdit*     createRectEdit(const QString &name, const CQChartsRect &rect,
                                       const QString &tip) const;
  CQChartsPolygonEdit*  createPolygonEdit(const QString &name, const CQChartsPolygon &poly,
                                          const QString &tip) const;
  CQChartsMarginEdit*   createMarginEdit(const QString &name, const CQChartsMargin &margin,
                                         const QString &tip) const;

  //---

  bool updateRectangleAnnotation();
  bool updateEllipseAnnotation  ();
  bool updatePolygonAnnotation  ();
  bool updatePolylineAnnotation ();
  bool updateTextAnnotation     ();
  bool updateImageAnnotation    ();
  bool updateArrowAnnotation    ();
  bool updatePointAnnotation    ();
  bool updatePieSliceAnnotation ();
  bool updateAxisAnnotation     ();
  bool updateKeyAnnotation      ();
  bool updatePointSetAnnotation ();
  bool updateValueSetAnnotation ();
  bool updateButtonAnnotation   ();

  //---

  bool setErrorMsg(const QString &msg);
  void clearErrorMsg();

 private slots:
  void ellipseCenterSlot(bool);
  void textPositionSlot (bool);
  void imagePositionSlot(bool);

  void okSlot();
  bool applySlot();
  void cancelSlot();

 private:
  CQChartsAnnotation* annotation_       { nullptr }; //!< annotation
  CQChartsLineEdit*   idEdit_           { nullptr }; //!< id edit
  CQChartsLineEdit*   tipEdit_          { nullptr }; //!< tip edit
  QVBoxLayout*        frameLayout_      { nullptr }; //!< frame layout
  RectWidgets         rectWidgets_;                  //!< rect edit widgets
  EllipseWidgets      ellipseWidgets_;               //!< ellipse edit widgets
  PolygonWidgets      polygonWidgets_;               //!< polygon edit widgets
  PolylineWidgets     polylineWidgets_;              //!< polyline edit widgets
  TextWidgets         textWidgets_;                  //!< text edit widgets
  ImageWidgets        imageWidgets_;                 //!< image edit widgets
  ArrowWidgets        arrowWidgets_;                 //!< arrow edit widgets
  PointWidgets        pointWidgets_;                 //!< point edit widgets
  PieSliceWidgets     pieSliceWidgets_;              //!< pie slice widgets
  AxisWidgets         axisWidgets_;                  //!< axis widgets
  KeyWidgets          keyWidgets_;                   //!< key widgets
  PointSetWidgets     pointSetWidgets_;              //!< point set widgets
  ValueSetWidgets     valueSetWidgets_;              //!< value set widgets
  ButtonWidgets       buttonWidgets_;                //!< button widgets
  QLabel*             msgLabel_         { nullptr }; //!< error message label
};

#endif
