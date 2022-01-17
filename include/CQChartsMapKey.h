#ifndef CQChartsMapKey_H
#define CQChartsMapKey_H

#include <CQChartsTextBoxObj.h>

class CQChartsMapKey : public CQChartsTextBoxObj {
  Q_OBJECT

  Q_PROPERTY(CQChartsKeyLocation location  READ location   WRITE setLocation )
  Q_PROPERTY(bool                insideX   READ isInsideX  WRITE setInsideX  )
  Q_PROPERTY(bool                insideY   READ isInsideY  WRITE setInsideY  )
  Q_PROPERTY(double              margin    READ margin     WRITE setMargin   )
  Q_PROPERTY(CQChartsPosition    position  READ position   WRITE setPosition )
  Q_PROPERTY(Qt::Alignment       align     READ align      WRITE setAlign    )
  Q_PROPERTY(bool                numeric   READ isNumeric  WRITE setNumeric  )
  Q_PROPERTY(bool                integral  READ isIntegral WRITE setIntegral )
  Q_PROPERTY(bool                native    READ isNative   WRITE setNative   )
  Q_PROPERTY(bool                mapped    READ isMapped   WRITE setMapped   )
  Q_PROPERTY(int                 numUnique READ numUnique  WRITE setNumUnique)

 public:
  using Plot          = CQChartsPlot;
  using Location      = CQChartsKeyLocation;
  using Position      = CQChartsPosition;
  using PropertyModel = CQPropertyViewModel;
  using PaintDevice   = CQChartsPaintDevice;
  using Font          = CQChartsFont;
  using Color         = CQChartsColor;
  using Alpha         = CQChartsAlpha;
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

  //! get/set position
  const Position &position() const { return position_; }
  void setPosition(const Position &p);

  //! get/set align
  const Qt::Alignment &align() const { return align_; }
  void setAlign(const Qt::Alignment &a);

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

  virtual void draw(PaintDevice *device, const DrawData &drawData, DrawType drawType) = 0;

  virtual QSize calcSize(const DrawData &drawData) const = 0;

  QColor bgColor(PaintDevice *device) const;

 protected:
  virtual void invalidate() = 0;

  void calcPosition(Position &pos, Qt::Alignment &align) const;

  void calcCenter();

  void calcAlignedBBox();

 protected:
  using EditHandlesP = std::unique_ptr<EditHandles>;

  Location      location_;                                          //!< key location
  bool          insideX_   { true };                                //!< inside plot x
  bool          insideY_   { true };                                //!< inside plot y
  double        margin_    { 4.0 };                                 //!< margin in pixels
  Position      position_;                                          //!< key position
  Qt::Alignment align_     { Qt::AlignHCenter | Qt::AlignVCenter }; //!< key align
  bool          numeric_   { false };                               //!< is numeric
  bool          integral_  { false };                               //!< is integral
  bool          native_    { false };                               //!< is native
  bool          mapped_    { false };                               //!< is mapped
  int           numUnique_ { -1 };                                  //!< num unique
  QVariantList  uniqueValues_;                                      //!< unique values

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

  // data range
  double dataMin() const { return dataMin_; }
  void setDataMin(double r) { dataMin_ = r; invalidate(); }

  double dataMax() const { return dataMax_; }
  void setDataMax(double r) { dataMax_ = r; invalidate(); }

  //---

  // map range
  double mapMin() const { return mapMin_; }
  void setMapMin(double r) { mapMin_ = r; invalidate(); }

  double mapMax() const { return mapMax_; }
  void setMapMax(double r) { mapMax_ = r; invalidate(); }

  //---

  //! get/set color palette name
  const PaletteName &paletteName() const { return paletteName_; }
  void setPaletteName(const PaletteName &n) { paletteName_ = n; invalidate(); }

  //---

  void addProperties(PropertyModel *model, const QString &path, const QString &desc="") override;

  //---

  void draw(PaintDevice *device, const DrawData &drawData=DrawData(),
            DrawType drawType=DrawType::VIEW) override;

  QSize calcSize(const DrawData &drawData) const override;

  //---

  bool selectPress(const Point &w, SelMod selMod) override;

  bool selectPressType(const Point &w, SelMod selMod, DrawType drawType) override;

 private:
  void invalidate() override;

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

  double dataMin_ { 0.0 }; //!< model data min
  double dataMax_ { 1.0 }; //!< model data max

  double mapMin_ { 0.0 }; //!< mapped color min
  double mapMax_ { 1.0 }; //!< mapped color max

  PaletteName paletteName_; //!< custom palette

  BBox tbbox_;

  TypeItemBoxes itemBoxes_;
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
  using Alpha       = CQChartsAlpha;
  using PenBrush    = CQChartsPenBrush;
  using BrushData   = CQChartsBrushData;
  using PenData     = CQChartsPenData;
  using PaletteName = CQChartsPaletteName;
  using Length      = CQChartsLength;
  using ColorInd    = CQChartsUtil::ColorInd;

  using BBox  = CQChartsGeom::BBox;
  using Point = CQChartsGeom::Point;

 public:
  CQChartsSymbolSizeMapKey(Plot *plot);

  //---

  QString calcId() const override { return "symbol_size_map_key"; }

  //---

  // data range
  double dataMin() const { return dataMin_; }
  void setDataMin(double r) { dataMin_ = r; invalidate(); }

  double dataMax() const { return dataMax_; }
  void setDataMax(double r) { dataMax_ = r; invalidate(); }

  //---

  // map range
  double mapMin() const { return mapMin_; }
  void setMapMin(double r) { mapMin_ = r; invalidate(); }

  double mapMax() const { return mapMax_; }
  void setMapMax(double r) { mapMax_ = r; invalidate(); }

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

  void draw(PaintDevice *device, const DrawData &drawData=DrawData(),
            DrawType drawType=DrawType::VIEW) override;

  QSize calcSize(const DrawData &drawData) const override;

  //---

  void initDraw(PaintDevice *device);

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

  double dataMin_ { 0.0 }; //!< model data min
  double dataMax_ { 1.0 }; //!< model data max

  double mapMin_ { 5.0 };  //!< mapped symbol size min (pixels)
  double mapMax_ { 17.0 }; //!< mapped symbol size max (pixels)

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

  TypeItemBoxes itemBoxes_;
};

//-----

class CQChartsSymbolTypeMapKey : public CQChartsMapKey {
  Q_OBJECT

  Q_PROPERTY(int     dataMin   READ dataMin   WRITE setDataMin  )
  Q_PROPERTY(int     dataMax   READ dataMax   WRITE setDataMax  )
  Q_PROPERTY(int     mapMin    READ mapMin    WRITE setMapMin   )
  Q_PROPERTY(int     mapMax    READ mapMax    WRITE setMapMax   )
  Q_PROPERTY(QString symbolSet READ symbolSet WRITE setSymbolSet)

 public:
  using PenBrush  = CQChartsPenBrush;
  using BrushData = CQChartsBrushData;
  using PenData   = CQChartsPenData;
  using Symbol    = CQChartsSymbol;
  using ColorInd  = CQChartsUtil::ColorInd;

  using BBox  = CQChartsGeom::BBox;
  using Point = CQChartsGeom::Point;

 public:
  CQChartsSymbolTypeMapKey(Plot *plot);

  //---

  QString calcId() const override { return "symbol_type_map_key"; }

  //---

  // data range
  int dataMin() const { return dataMin_; }
  void setDataMin(int i) { dataMin_ = i; invalidate(); }

  int dataMax() const { return dataMax_; }
  void setDataMax(int i) { dataMax_ = i; invalidate(); }

  //---

  // map range
  int mapMin() const { return mapMin_; }
  void setMapMin(int i) { mapMin_ = i; invalidate(); }

  int mapMax() const { return mapMax_; }
  void setMapMax(int i) { mapMax_ = i; invalidate(); }

  //---

  const QString &symbolSet() const { return symbolSet_; }
  void setSymbolSet(const QString &s) { symbolSet_ = s; }

  //---

  void addProperties(PropertyModel *model, const QString &path, const QString &desc="") override;

  //---

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

  int dataMin_ { 0 }; //!< model data min
  int dataMax_ { 1 }; //!< model data max

  int mapMin_ { 0 }; //!< mapped symbol type min
  int mapMax_ { 1 }; //!< mapped symbol type max

  QString symbolSet_;

  TypeItemBoxes itemBoxes_;
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

 private:
  CQChartsMapKeyWidget* w_ { nullptr };
};

#endif
