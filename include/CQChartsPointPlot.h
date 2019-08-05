#ifndef CQChartsPointPlot_H
#define CQChartsPointPlot_H

#include <CQChartsGroupPlot.h>

class CQChartsDataLabel;

/*!
 * \brief Point plot type (Base class for XY and Symbol Plot Types)
 * \ingroup Charts
 */
class CQChartsPointPlotType : public CQChartsGroupPlotType {
 public:
  CQChartsPointPlotType();

  Dimension dimension() const override { return Dimension::TWO_D; }

  QString xColumnName() const override { return "x"; }
  QString yColumnName() const override { return "y"; }

  void addMappingParameters();
};

//---

class CQChartsPointPlot : public CQChartsGroupPlot {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn symbolTypeColumn READ symbolTypeColumn WRITE setSymbolTypeColumn)
  Q_PROPERTY(CQChartsColumn symbolSizeColumn READ symbolSizeColumn WRITE setSymbolSizeColumn)
  Q_PROPERTY(CQChartsColumn fontSizeColumn   READ fontSizeColumn   WRITE setFontSizeColumn  )

  // symbol type map
  Q_PROPERTY(bool symbolTypeMapped READ isSymbolTypeMapped WRITE setSymbolTypeMapped)
  Q_PROPERTY(int  symbolTypeMapMin READ symbolTypeMapMin   WRITE setSymbolTypeMapMin)
  Q_PROPERTY(int  symbolTypeMapMax READ symbolTypeMapMax   WRITE setSymbolTypeMapMax)

  // symbol size map
  Q_PROPERTY(bool    symbolSizeMapped   READ isSymbolSizeMapped WRITE setSymbolSizeMapped  )
  Q_PROPERTY(double  symbolSizeMapMin   READ symbolSizeMapMin   WRITE setSymbolSizeMapMin  )
  Q_PROPERTY(double  symbolSizeMapMax   READ symbolSizeMapMax   WRITE setSymbolSizeMapMax  )
  Q_PROPERTY(QString symbolSizeMapUnits READ symbolSizeMapUnits WRITE setSymbolSizeMapUnits)

  // font size map
  Q_PROPERTY(bool    fontSizeMapped   READ isFontSizeMapped WRITE setFontSizeMapped  )
  Q_PROPERTY(double  fontSizeMapMin   READ fontSizeMapMin   WRITE setFontSizeMapMin  )
  Q_PROPERTY(double  fontSizeMapMax   READ fontSizeMapMax   WRITE setFontSizeMapMax  )
  Q_PROPERTY(QString fontSizeMapUnits READ fontSizeMapUnits WRITE setFontSizeMapUnits)

 public:
  CQChartsPointPlot(CQChartsView *view, CQChartsPlotType *plotType, const ModelP &model);

 ~CQChartsPointPlot();

  //---

  // data label
  const CQChartsDataLabel *dataLabel() const { return dataLabel_; }
  CQChartsDataLabel *dataLabel() { return dataLabel_; }

  //---

  // symbol type column and map
  const CQChartsColumn &symbolTypeColumn() const;
  void setSymbolTypeColumn(const CQChartsColumn &c);

  bool isSymbolTypeMapped() const;
  void setSymbolTypeMapped(bool b);

  int symbolTypeMapMin() const;
  void setSymbolTypeMapMin(int i);

  int symbolTypeMapMax() const;
  void setSymbolTypeMapMax(int i);

  //---

  // symbol size column and map
  const CQChartsColumn &symbolSizeColumn() const;
  void setSymbolSizeColumn(const CQChartsColumn &c);

  bool isSymbolSizeMapped() const;
  void setSymbolSizeMapped(bool b);

  double symbolSizeMapMin() const;
  void setSymbolSizeMapMin(double r);

  double symbolSizeMapMax() const;
  void setSymbolSizeMapMax(double r);

  const QString &symbolSizeMapUnits() const;
  void setSymbolSizeMapUnits(const QString &s);

  //---

  // label font size column and map
  const CQChartsColumn &fontSizeColumn() const;
  void setFontSizeColumn(const CQChartsColumn &c);

  bool isFontSizeMapped() const;
  void setFontSizeMapped(bool b);

  double fontSizeMapMin() const;
  void setFontSizeMapMin(double r);

  double fontSizeMapMax() const;
  void setFontSizeMapMax(double r);

  const QString &fontSizeMapUnits() const;
  void setFontSizeMapUnits(const QString &s);

  //---

  void addPointProperties();

  void getPropertyNames(QStringList &names, bool hidden) const override;

  //---

  // data labels
  bool isPointLabels() const;
  void setPointLabels(bool b);

  void setDataLabelFont(const CQChartsFont &font);

  //---

  void write(std::ostream &os, const QString &varName="",
             const QString &modelName="") const override;

 protected:
  void initSymbolTypeData() const;

  bool columnSymbolType(int row, const QModelIndex &parent, CQChartsSymbol &symbolType) const;

  void initSymbolSizeData() const;

  bool columnSymbolSize(int row, const QModelIndex &parent, CQChartsLength &symbolSize) const;

  void initFontSizeData() const;

  bool columnFontSize(int row, const QModelIndex &parent, CQChartsLength &fontSize) const;

 protected slots:
  void dataLabelChanged();

 protected:
  struct SymbolTypeData {
    CQChartsColumn column;             //!< symbol type column
    bool           valid    { false }; //!< symbol type valid
    bool           mapped   { false }; //!< symbol type values mapped
    int            data_min { 0 };     //!< model data min
    int            data_max { 1 };     //!< model data max
    int            map_min  { 0 };     //!< mapped size min
    int            map_max  { 1 };     //!< mapped size max
  };

  struct SymbolSizeData {
    CQChartsColumn column;              //!< symbol size column
    bool           valid     { false }; //!< symbol size valid
    bool           mapped    { false }; //!< symbol size values mapped
    double         data_min  { 0.0 };   //!< model data min
    double         data_max  { 1.0 };   //!< model data max
    double         data_mean { 0.0 };   //!< model data mean
    double         map_min   { 0.0 };   //!< mapped size min
    double         map_max   { 1.0 };   //!< mapped size max
    QString        units     { "px" };  //!< mapped size units
  };

  struct FontSizeData {
    CQChartsColumn column;             //!< font size column
    bool           valid    { false }; //!< font size valid
    bool           mapped   { false }; //!< font size values mapped
    double         data_min { 0.0 };   //!< model data min
    double         data_max { 1.0 };   //!< model data max
    double         map_min  { 0.0 };   //!< mapped size min
    double         map_max  { 1.0 };   //!< mapped size max
    QString        units    { "px" };  //!< mapped size units
  };

 protected:
  CQChartsDataLabel* dataLabel_ { nullptr }; //!< data label style

  SymbolTypeData symbolTypeData_; //!< symbol type column data
  SymbolSizeData symbolSizeData_; //!< symbol size column data
  FontSizeData   fontSizeData_;   //!< font size column data
};

#endif
