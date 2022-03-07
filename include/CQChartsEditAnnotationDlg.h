#ifndef CQChartsEditAnnotationDlg_H
#define CQChartsEditAnnotationDlg_H

#include <CQChartsUnits.h>
#include <CQChartsTypes.h>
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
class CQChartsColor;
class CQChartsColorLineEdit;
class CQChartsImage;
class CQChartsImageEdit;
class CQChartsAngle;
class CQChartsAngleEdit;
class CQChartsLineEdit;

class CQRealSpin;
class CQCheckBox;

class QFrame;
class QComboBox;
class QRadioButton;
class QLabel;
class QBoxLayout;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;

/*!
 * \brief dialog to edit an existing annotation
 * \ingroup Charts
 */
class CQChartsEditAnnotationDlg : public QDialog {
  Q_OBJECT

 public:
  using Annotation = CQChartsAnnotation;
  using Units      = CQChartsUnits::Type;

 public:
  CQChartsEditAnnotationDlg(QWidget *parent, Annotation *annotation);

  Annotation *annotation() const { return annotation_; }

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
    CQChartsImageEdit*    imageEdit         { nullptr };
    CQChartsImageEdit*    disabledImageEdit { nullptr };
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
    QComboBox*  orientationEdit { nullptr };
    CQRealSpin* positionEdit    { nullptr };
    CQRealSpin* startEdit       { nullptr };
    CQRealSpin* endEdit         { nullptr };
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

  struct GridData {
    QGridLayout *layout;
    int          row { 0 };
  };

 private:
  void initWidgets();

  void createRectFrame    ();
  void createEllipseFrame ();
  void createPolygonFrame ();
  void createPolyLineFrame();
  void createTextFrame    ();
  void createImageFrame   ();
  void createPathFrame    ();
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

  CQCheckBox*            addBoolEdit(GridData &gridData, const QString &name,
                                     const QString &label, bool checked, const QString &tip) const;
  CQChartsLineEdit*      addLineEdit(GridData &gridData, const QString &name,
                                     const QString &label, const QString &text,
                                     const QString &tip) const;
  CQRealSpin*            addRealEdit(GridData &gridData, const QString &name,
                                     const QString &label, double r, const QString &tip) const;
  QComboBox*             addComboEdit(GridData &gridData, const QString &name, const QString &label,
                                      const QStringList &items, const QString &tip) const;
  CQChartsPositionEdit*  addPositionEdit(GridData &gridData, const QString &name,
                                         const QString &label, const CQChartsPosition &pos,
                                         const QString &tip) const;
  CQChartsLengthEdit*    addLengthEdit(GridData &gridData, const QString &name,
                                       const QString &label, const CQChartsLength &len,
                                       const QString &tip) const;
  CQChartsRectEdit*      addRectEdit(GridData &gridData, const QString &name,
                                     const QString &label, const CQChartsRect &rect,
                                     const QString &tip) const;
  CQChartsPolygonEdit*   addPolygonEdit(GridData &gridData, const QString &name,
                                        const QString &label, const CQChartsPolygon &poly,
                                        const QString &tip) const;
  CQChartsMarginEdit*    addMarginEdit(GridData &gridData, const QString &name,
                                       const QString &label, const CQChartsMargin &margin,
                                       const QString &tip) const;
  CQChartsImageEdit*     addImageEdit(GridData &gridData, const QString &name,
                                      const QString &label, const CQChartsImage &i,
                                      const QString &tip) const;
  CQChartsColorLineEdit* addColorEdit(GridData &gridData, const QString &name,
                                      const QString &label, const CQChartsColor &c,
                                      const QString &tip) const;
  CQChartsAngleEdit*     addAngleEdit(GridData &gridData, const QString &name,
                                      const QString &label, const CQChartsAngle &a,
                                      const QString &tip) const;

  GridData createGrid(QBoxLayout *layout) const;

  //---

  bool updateRectangleAnnotation();
  bool updateEllipseAnnotation  ();
  bool updatePolygonAnnotation  ();
  bool updatePolylineAnnotation ();
  bool updateTextAnnotation     ();
  bool updateImageAnnotation    ();
  bool updatePathAnnotation     ();
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
  Annotation*       annotation_       { nullptr }; //!< annotation
  CQChartsLineEdit* idEdit_           { nullptr }; //!< id edit
  CQChartsLineEdit* tipEdit_          { nullptr }; //!< tip edit
  QVBoxLayout*      frameLayout_      { nullptr }; //!< frame layout
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
