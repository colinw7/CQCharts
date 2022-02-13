#ifndef CQChartsMapKey_H
#define CQChartsMapKey_H

#include <CQChartsBoxObj.h>
#include <CQChartsColorColumnData.h>
#include <CQChartsSymbolSizeData.h>
#include <CQChartsSymbolTypeData.h>

class CQChartsMapKey : public CQChartsBoxObj,
 public CQChartsObjTextData      <CQChartsMapKey>,
 public CQChartsObjHeaderTextData<CQChartsMapKey> {
  Q_OBJECT

  Q_PROPERTY(CQChartsKeyLocation location  READ location   WRITE setLocation )
  Q_PROPERTY(bool                insideX   READ isInsideX  WRITE setInsideX  )
  Q_PROPERTY(bool                insideY   READ isInsideY  WRITE setInsideY  )
  Q_PROPERTY(double              margin    READ margin     WRITE setMargin   )
  Q_PROPERTY(double              padding   READ padding    WRITE setPadding  )
  Q_PROPERTY(CQChartsPosition    position  READ position   WRITE setPosition )
  Q_PROPERTY(Qt::Alignment       align     READ align      WRITE setAlign    )
  Q_PROPERTY(QString             header    READ headerStr  WRITE setHeaderStr)

  // auto set by plot
  Q_PROPERTY(bool numeric   READ isNumeric  WRITE setNumeric  )
  Q_PROPERTY(bool integral  READ isIntegral WRITE setIntegral )
  Q_PROPERTY(bool native    READ isNative   WRITE setNative   )
  Q_PROPERTY(bool mapped    READ isMapped   WRITE setMapped   )
  Q_PROPERTY(int  numUnique READ numUnique  WRITE setNumUnique)

  CQCHARTS_TEXT_DATA_PROPERTIES

  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(Header, header)

 public:
  using Plot          = CQChartsPlot;
  using Location      = CQChartsKeyLocation;
  using Position      = CQChartsPosition;
  using PropertyModel = CQPropertyViewModel;
  using PaintDevice   = CQChartsPaintDevice;
  using Font          = CQChartsFont;
  using Color         = CQChartsColor;
  using Alpha         = CQChartsAlpha;
  using Angle         = CQChartsAngle;
  using BBox          = CQChartsGeom::BBox;
  using Point         = CQChartsGeom::Point;

  enum DrawType {
    NONE,
    VIEW,
    WIDGET
  };

 public:
  CQChartsMapKey(Plot *plot);

  //---

  //! get plot
  Plot *plot() const { return plot_; }

  //---

  //! get/set location
  const Location &location() const { return location_; }
  void setLocation(const Location &l);

  //---

  //! get/set inside plot in x direction
  bool isInsideX() const { return insideX_; }
  void setInsideX(bool b);

  //---

  //! get/set inside plot in y direction
  bool isInsideY() const { return insideY_; }
  void setInsideY(bool b);

  //---

  //! get/set margin
  double margin() const { return margin_; }
  void setMargin(double r);

  bool hasCustomMargin() const override { return true; }

  //! get/set padding
  double padding() const { return padding_; }
  void setPadding(double r);

  bool hasCustomPadding() const override { return true; }

  //! get/set position
  const Position &position() const { return position_; }
  void setPosition(const Position &p);

  //! get/set align
  const Qt::Alignment &align() const { return align_; }
  void setAlign(const Qt::Alignment &a);

  //---

  //! get/set header text
  const QString &headerStr() const { return header_; }
  void setHeaderStr(const QString &s);

  //---

  //! get/set is numeric
  bool isNumeric() const { return numeric_; }
  void setNumeric(bool b) { numeric_ = b; }

  //! get/set is integral
  bool isIntegral() const { return integral_; }
  void setIntegral(bool b) { integral_ = b; }

  //! get/set is native type
  bool isNative() const { return native_; }
  void setNative(bool b) { native_ = b; }

  //! get/set is mapped
  bool isMapped() const { return mapped_; }
  void setMapped(bool b) { mapped_ = b; }

  //! get/set num unique
  int numUnique() const { return numUnique_; }
  void setNumUnique(int i) { numUnique_ = i; }

  //! get/set unique values
  const QVariantList &uniqueValues() const { return uniqueValues_; }
  void setUniqueValues(const QVariantList &v) { uniqueValues_ = v; }

  //---

  //! get/set bbox
  const BBox &bbox() const { return bbox_; }
  void setBBox(const BBox &b) { bbox_ = b; }

  //---

  void addProperties(PropertyModel *model, const QString &path, const QString &desc="") override;

  //---

  QFont calcDrawFont(const Font &textFont) const;

  void setDrawPainterFont(PaintDevice *device, const Font &textFont);

  //---

  virtual bool selectPressType(const Point &w, SelMod selMod, DrawType) {
    return selectPress(w, selMod);
  }

  //---

  // Implement edit interface
  bool editPress (const Point &) override;
  bool editMove  (const Point &) override;
  bool editMotion(const Point &) override;

  //---

  struct DrawData {
    DrawData() { }

    bool usePenBrush { false };
    bool isWidget    { false };

    QFont font;
  };

  virtual bool inside(const Point &p, DrawType drawType) const = 0;

  virtual void draw(PaintDevice *device, const DrawData &drawData, DrawType drawType) = 0;

  virtual QSize calcSize(const DrawData &drawData) const = 0;

  QColor bgColor(PaintDevice *device) const;

  void setEditHandlesBBox() const override;

 protected:
  virtual void invalidate() = 0;

  void calcPosition(Position &pos, Qt::Alignment &align) const;

  void calcCenter();

  void calcAlignedBBox();

 protected:
  using EditHandlesP = std::unique_ptr<EditHandles>;

  // position
  Location      location_;                                          //!< key location
  bool          insideX_   { true };                                //!< inside plot x
  bool          insideY_   { true };                                //!< inside plot y
  double        margin_    { 4.0 };                                 //!< external margin in pixels
  double        padding_   { 4.0 };                                 //!< internal padding in pixels
  Position      position_;                                          //!< key position
  Qt::Alignment align_     { Qt::AlignHCenter | Qt::AlignVCenter }; //!< key align

  // header
  QString header_; //!< header


  // data
  bool numeric_  { false }; //!< is numeric
  bool integral_ { false }; //!< is integral
  bool native_   { false }; //!< is native
  bool mapped_   { false }; //!< is mapped

  // unique data
  int           numUnique_ { -1 }; //!< num unique
  QVariantList  uniqueValues_;     //!< unique values

  // draw data
  mutable double        kw_     { 0.0 };
  mutable double        kh_     { 0.0 };
  mutable double        xm_     { 0.0 };
  mutable double        ym_     { 0.0 };
  mutable int           ndp_    { 0 };
  mutable double        twl_    { 0.0 };
  mutable double        twr_    { 0.0 };
  mutable Qt::Alignment talign_ { Qt::AlignHCenter | Qt::AlignVCenter }; //!< calculated align
  mutable BBox          pbbox_;
  mutable DrawData      drawData_;

  // per draw drawn bbox
  using TypeBox = std::map<DrawType, BBox>;

  TypeBox typeBBox_;
};

//-----

class CQChartsColorMapKey : public CQChartsMapKey {
  Q_OBJECT

  Q_PROPERTY(double              dataMin     READ dataMin     WRITE setDataMin    )
  Q_PROPERTY(double              dataMax     READ dataMax     WRITE setDataMax    )
  Q_PROPERTY(double              mapMin      READ mapMin      WRITE setMapMin     )
  Q_PROPERTY(double              mapMax      READ mapMax      WRITE setMapMax     )
  Q_PROPERTY(CQChartsPaletteName paletteName READ paletteName WRITE setPaletteName)

 public:
  using ColorData   = CQChartsColorColumnData;
  using PenBrush    = CQChartsPenBrush;
  using BrushData   = CQChartsBrushData;
  using PenData     = CQChartsPenData;
  using PaletteName = CQChartsPaletteName;
  using ColorInd    = CQChartsUtil::ColorInd;

  using BBox    = CQChartsGeom::BBox;
  using Point   = CQChartsGeom::Point;
  using RMinMax = CQChartsGeom::RMinMax;

 public:
  CQChartsColorMapKey(Plot *plot);

  //---

  QString calcId() const override { return "color_map_key"; }

  //---

  void setData(const ColorData &data) { colorData_ = data; }

  // data range
  double dataMin() const { return colorData_.data_min; }
  void setDataMin(double r) { colorData_.data_min = r; invalidate(); }

  double dataMax() const { return colorData_.data_max; }
  void setDataMax(double r) { colorData_.data_max = r; invalidate(); }

  //---

  // map range
  double mapMin() const { return colorData_.map_min; }
  void setMapMin(double r) { colorData_.map_min = r; invalidate(); }

  double mapMax() const { return colorData_.map_max; }
  void setMapMax(double r) { colorData_.map_max = r; invalidate(); }

  //---

  //! get/set color palette name
  const PaletteName &paletteName() const { return paletteName_; }
  void setPaletteName(const PaletteName &n) { paletteName_ = n; invalidate(); }

  //---

  void addProperties(PropertyModel *model, const QString &path, const QString &desc="") override;

  //---

  bool isContiguous() const;

  bool inside(const Point &p, DrawType drawType) const override;

  void draw(PaintDevice *device, const DrawData &drawData=DrawData(),
            DrawType drawType=DrawType::VIEW) override;

  QSize calcSize(const DrawData &drawData) const override;

  QSize calcHeaderSize() const;

  //---

  bool selectPress(const Point &w, SelMod selMod) override;

  bool selectPressType(const Point &w, SelMod selMod, DrawType drawType) override;

 private:
  void invalidate() override;

  void drawHeader(PaintDevice *device);

  void drawContiguous(PaintDevice *device);
  void drawDiscreet  (PaintDevice *device, DrawType drawType);

  QSize calcContiguousSize() const;
  QSize calcDiscreetSize() const;

 signals:
  void dataChanged();

  void itemSelected(const QColor &color, bool visible);

 private:
  QString valueText(double value) const;

 private:
  struct ItemBox {
    QColor color;
    BBox   rect;

    ItemBox(const QColor &color, const BBox &rect) :
     color(color), rect(rect) {
    }
  };

  using ItemBoxes     = std::vector<ItemBox>;
  using TypeItemBoxes = std::map<DrawType, ItemBoxes>;

  ColorData colorData_;

  PaletteName paletteName_; //!< custom palette

  BBox tbbox_;

  TypeItemBoxes itemBoxes_;

  mutable double xoffset_ { 0.0 };
  mutable double yoffset_ { 0.0 };
};

//-----

// TODO:
//  . custom palette
//  . shape
//  . spread/overlay
//  . text align left/center/right
//  . separate border style
class CQChartsSymbolSizeMapKey : public CQChartsMapKey {
  Q_OBJECT

  Q_PROPERTY(double              dataMin     READ dataMin     WRITE setDataMin    )
  Q_PROPERTY(double              dataMax     READ dataMax     WRITE setDataMax    )
  Q_PROPERTY(double              mapMin      READ mapMin      WRITE setMapMin     )
  Q_PROPERTY(double              mapMax      READ mapMax      WRITE setMapMax     )
  Q_PROPERTY(double              scale       READ scale       WRITE setScale      )
  Q_PROPERTY(bool                stacked     READ isStacked   WRITE setStacked    )
  Q_PROPERTY(int                 rows        READ rows        WRITE setRows       )
  Q_PROPERTY(CQChartsAlpha       alpha       READ alpha       WRITE setAlpha      )
  Q_PROPERTY(Qt::Alignment       textAlign   READ textAlign   WRITE setTextAlign  )
  Q_PROPERTY(CQChartsPaletteName paletteName READ paletteName WRITE setPaletteName)

 public:
  using Alpha          = CQChartsAlpha;
  using PenBrush       = CQChartsPenBrush;
  using BrushData      = CQChartsBrushData;
  using PenData        = CQChartsPenData;
  using PaletteName    = CQChartsPaletteName;
  using Length         = CQChartsLength;
  using SymbolSizeData = CQChartsSymbolSizeData;
  using ColorInd       = CQChartsUtil::ColorInd;

  using BBox  = CQChartsGeom::BBox;
  using Point = CQChartsGeom::Point;

 public:
  CQChartsSymbolSizeMapKey(Plot *plot);

  //---

  QString calcId() const override { return "symbol_size_map_key"; }

  //---

  void setData(const SymbolSizeData &data) { symbolSizeData_ = data; }

  // data range
  double dataMin() const { return symbolSizeData_.data_min; }
  void setDataMin(double r) { symbolSizeData_.data_min = r; invalidate(); }

  double dataMax() const { return symbolSizeData_.data_max; }
  void setDataMax(double r) { symbolSizeData_.data_max = r; invalidate(); }

  //---

  // map range
  double mapMin() const { return symbolSizeData_.map_min; }
  void setMapMin(double r) { symbolSizeData_.map_min = r; invalidate(); }

  double mapMax() const { return symbolSizeData_.map_max; }
  void setMapMax(double r) { symbolSizeData_.map_min = r; invalidate(); }

  //---

  double scale() const { return scale_; }
  void setScale(double r) { scale_ = r; invalidate(); }

  bool isStacked() const { return stacked_; }
  void setStacked(bool b) { stacked_ = b; invalidate(); }

  int rows() const { return rows_; }
  void setRows(int i) { rows_ = i; invalidate(); }

  //---

  const Alpha &alpha() const { return alpha_; }
  void setAlpha(const Alpha &a) { alpha_ = a; invalidate(); }

  const Qt::Alignment &textAlign() const { return textAlign_; }
  void setTextAlign(const Qt::Alignment &a) { textAlign_ = a; invalidate(); }

  const PaletteName &paletteName() const { return paletteName_; }
  void setPaletteName(const PaletteName &n) { paletteName_ = n; invalidate(); }

  const RMinMax &paletteMinMax() const { return paletteMinMax_; }
  void setPaletteMinMax(const RMinMax &r) { paletteMinMax_ = r; invalidate(); }

  //---

  void addProperties(PropertyModel *model, const QString &path, const QString &desc="") override;

  //---

  bool isContiguous() const;

  bool inside(const Point &p, DrawType drawType) const override;

  void draw(PaintDevice *device, const DrawData &drawData=DrawData(),
            DrawType drawType=DrawType::VIEW) override;

  QSize calcSize(const DrawData &drawData) const override;

  QSize calcHeaderSize() const;

  //---

  void initDraw(PaintDevice *device);

  void drawHeader(PaintDevice *device);

  void drawCircles(PaintDevice *device, DrawType drawType, bool usePenBrush=false);

  void drawText(PaintDevice *device, const CQChartsTextOptions &textOptions, bool usePenBrush);

  void drawBorder(PaintDevice *device, bool usePenBrush=false);

  //---

  bool selectPress(const Point &w, SelMod selMod) override;

  bool selectPressType(const Point &w, SelMod selMod, DrawType drawType) override;

 private:
  void invalidate() override;

  void drawContiguous(PaintDevice *device, DrawType drawType);
  void drawDiscreet(PaintDevice *device, DrawType drawType);

  QSize calcContiguousSize() const;
  QSize calcDiscreetSize() const;

 signals:
  void dataChanged();

  void itemSelected(const CQChartsLength &size, bool visible);

 private:
  void calcSymbolBoxes() const;
  void calcTextBBox   () const;
  void alignBoxes     (PaintDevice *device) const;

  QString valueText(double value) const;

 private:
  struct ItemBox {
    Length size;
    BBox   rect;

    ItemBox(const Length &size, const BBox &rect) :
     size(size), rect(rect) {
    }
  };

  using BBoxes        = std::vector<BBox>;
  using ItemBoxes     = std::vector<ItemBox>;
  using TypeItemBoxes = std::map<DrawType, ItemBoxes>;

  SymbolSizeData symbolSizeData_;

  double scale_ { 1.0 }; //!< scale symbol sizes

  bool stacked_ { false }; //!< draw stacked
  int  rows_    { 3 };     //!< number of symbol rows

  Alpha         alpha_     { 0.6 };                               //!< background alpha
  Qt::Alignment textAlign_ { Qt::AlignRight | Qt::AlignVCenter }; //!< text align

  PaletteName paletteName_;                //!< custom palette
  RMinMax     paletteMinMax_ { 0.0, 1.0 }; //!< custom palette range

  mutable BBox   psbbox_;
  mutable BBox   sbbox_;
  mutable BBox   ptbbox_;
  mutable BBox   tbbox_;
  mutable BBox   pbbox_;
  mutable BBoxes symbolBoxes_;
  mutable Point  pcenter_;
  mutable Point  center_;
  mutable double yoffset_ { 0.0 };

  TypeItemBoxes itemBoxes_;
};

//-----

class CQChartsSymbolTypeMapKey : public CQChartsMapKey {
  Q_OBJECT

  Q_PROPERTY(long    dataMin   READ dataMin   WRITE setDataMin  )
  Q_PROPERTY(long    dataMax   READ dataMax   WRITE setDataMax  )
  Q_PROPERTY(long    mapMin    READ mapMin    WRITE setMapMin   )
  Q_PROPERTY(long    mapMax    READ mapMax    WRITE setMapMax   )
  Q_PROPERTY(QString symbolSet READ symbolSet WRITE setSymbolSet)

 public:
  using PenBrush       = CQChartsPenBrush;
  using BrushData      = CQChartsBrushData;
  using PenData        = CQChartsPenData;
  using Symbol         = CQChartsSymbol;
  using Length         = CQChartsLength;
  using SymbolTypeData = CQChartsSymbolTypeData;
  using ColorInd       = CQChartsUtil::ColorInd;

  using BBox  = CQChartsGeom::BBox;
  using Point = CQChartsGeom::Point;

 public:
  CQChartsSymbolTypeMapKey(Plot *plot);

  //---

  QString calcId() const override { return "symbol_type_map_key"; }

  //---

  void setData(const SymbolTypeData &data) { symbolTypeData_ = data; }

  // data range
  long dataMin() const { return symbolTypeData_.data_min; }
  void setDataMin(long i) { symbolTypeData_.data_min = i; invalidate(); }

  long dataMax() const { return symbolTypeData_.data_max; }
  void setDataMax(long i) { symbolTypeData_.data_max = i; invalidate(); }

  //---

  // map range
  long mapMin() const { return symbolTypeData_.map_min; }
  void setMapMin(long i) { symbolTypeData_.map_min = i; invalidate(); }

  long mapMax() const { return symbolTypeData_.map_max; }
  void setMapMax(long i) { symbolTypeData_.map_max = i; invalidate(); }

  //---

  // symbol set
  const QString &symbolSet() const { return symbolTypeData_.setName; }
  void setSymbolSet(const QString &s) { symbolTypeData_.setName = s; }

  //---

  void addProperties(PropertyModel *model, const QString &path, const QString &desc="") override;

  //---

  bool isContiguous() const;

  bool inside(const Point &p, DrawType drawType) const override;

  void draw(PaintDevice *device, const DrawData &drawData=DrawData(),
            DrawType drawType=DrawType::VIEW) override;

  QSize calcSize(const DrawData &drawData) const override;

  //---

  // implement select interface
  bool selectPress(const Point &w, SelMod selMod) override;

  bool selectPressType(const Point &w, SelMod selMod, DrawType drawType) override;

 private:
  void invalidate() override;

 signals:
  void dataChanged();

  void itemSelected(const CQChartsSymbol &symbol, bool visible);

 private:
  QString valueText(double value) const;

 private:
  struct ItemBox {
    Symbol symbol;
    BBox   rect;

    ItemBox(const Symbol &symbol, const BBox &rect) :
     symbol(symbol), rect(rect) {
    }
  };

  using ItemBoxes     = std::vector<ItemBox>;
  using TypeItemBoxes = std::map<DrawType, ItemBoxes>;

  SymbolTypeData symbolTypeData_;
  TypeItemBoxes  itemBoxes_;
};

//---

#include <QFrame>

class CQChartsMapKeyFrame;
class QScrollArea;

class CQChartsMapKeyWidget : public QFrame {
  Q_OBJECT

 public:
  CQChartsMapKeyWidget(CQChartsMapKey *key=nullptr);

  CQChartsMapKey *key() const { return key_; }
  void setKey(CQChartsMapKey *key);

 private:
  CQChartsMapKey*      key_        { nullptr };
  QScrollArea*         scrollArea_ { nullptr };
  CQChartsMapKeyFrame* keyFrame_   { nullptr };
};

//---

class CQChartsMapKeyFrame : public QFrame {
  Q_OBJECT

 public:
  CQChartsMapKeyFrame(CQChartsMapKeyWidget *w);

  bool updateSize();

  void paintEvent(QPaintEvent *) override;

  void mousePressEvent(QMouseEvent *) override;

  void contextMenuEvent(QContextMenuEvent *e) override;

 private slots:
  void showKeySlot(bool b);

 private:
  CQChartsMapKeyWidget* w_ { nullptr };
};

#endif
