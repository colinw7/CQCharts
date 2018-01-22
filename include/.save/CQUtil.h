#ifndef CQUTIL_H
#define CQUTIL_H

#include <Qt>
#include <QColor>
#include <QEvent>
#include <QVariant>
#include <QTableWidget>
#include <QPen>

#ifdef CQUTIL_EVENT
#include <CEvent.h>
#endif

#ifdef CQUTIL_RGB
#include <CRGBA.h>
#endif

#ifdef CQUTIL_LINE_DASH
#include <CLineDash.h>
#endif

#ifdef CQUTIL_ANGLE
#include <CAngle.h>
#endif

#ifdef CQUTIL_FONT
#include <CFontStyle.h>
#include <CFont.h>
#endif

#ifdef CQUTIL_IMAGE
#include <CImageLib.h>
#endif

#include <CPoint2D.h>
#include <CBBox2D.h>
#include <CMatrix2D.h>

#ifdef CQUTIL_BRUSH
#include <CBrush.h>
#endif

#ifdef CQUTIL_PEN
#include <CPen.h>
#endif

//---

#ifdef CQUTIL_LINE_DASH
Q_DECLARE_METATYPE(CLineDash)
#endif

#ifdef CQUTIL_ANGLE
Q_DECLARE_METATYPE(CAngle)
#endif

#define CQUTIL_DCL_META_STREAM(TYPE, GETTER, SETTER) \
QDataStream &operator<<(QDataStream &out, const TYPE &t) { \
  QString str = t.GETTER().c_str(); \
\
  out << str; \
\
  return out; \
} \
\
QDataStream &operator>>(QDataStream &in, TYPE &t) { \
  QString str; \
\
  in >> str; \
\
  t.fromString(str.toStdString()); \
\
  return in; \
}

//---

class QMouseEvent;
class QKeyEvent;
class QMetaObject;
class QAbstractButton;
class QDockWidget;
class QLabel;

class CLinearGradient;
class CRadialGradient;

namespace CQUtil {
  class PropInfo {
   public:
    PropInfo() {
     type_         = QVariant::Invalid;
     is_writable_  = false;
     is_enum_type_ = false;
    }

    void init(const QMetaProperty &mp);

    QString        name    () const { return name_; }
    QVariant::Type type    () const { return type_; }
    QString        typeName() const { return typeName_; }

    bool isWritable() const { return is_writable_ ; }
    bool isEnumType() const { return is_enum_type_; }

    const QStringList &enumNames() const { return enumNames_; }

   private:
    QString        name_;
    QVariant::Type type_;
    QString        typeName_;
    bool           is_writable_;
    bool           is_enum_type_;
    QStringList    enumNames_;
  };

  void initProperties();

#ifdef CQUTIL_EVENT
  CMouseEvent *convertEvent(QMouseEvent *event);
  CKeyEvent   *convertEvent(QKeyEvent *event);

  CMouseButton   convertButton(Qt::MouseButton button);
  CKeyType       convertKey(int key, Qt::KeyboardModifiers modifiers);
  CEventModifier convertModifier(Qt::KeyboardModifiers modifiers);
#endif

#ifdef CQUTIL_RGB
  QColor rgbToColor(const CRGB &rgb);
  QColor rgbaToColor(const CRGBA &rgba);

  inline QColor toQColor(const CRGB  &rgb ) { return rgbToColor (rgb ); }
  inline QColor toQColor(const CRGBA &rgba) { return rgbaToColor(rgba); }

  uint rgbaToInt(const CRGBA &rgba);

  CRGB  colorToRGB(const QColor &color);
  CRGBA colorToRGBA(const QColor &color);

  inline CRGBA fromQColor(const QColor &color) { return colorToRGBA(color); }
#endif

#ifdef CQUTIL_BRUSH
  QBrush toQBrush(const CBrush &brush);
#endif

#ifdef CQUTIL_PEN
  QPen toQPen(const CPen &pen);

  Qt::PenCapStyle toPenCapStyle(const CLineCapType &lineCap);
  Qt::PenJoinStyle toPenJoinStyle(const CLineJoinType &lineJoin);
#endif

#ifdef CQUTIL_FONT
  void decodeFont(const QFont &font, QString &family, CFontStyle &style, int &pixelSize);
#endif

  QString fullName(const QObject* object);

  QObject *nameToObject(const QString &name);
  QObject *nameToObject(QObject *parent, const QString &name);

  QObject *hierChildObject(QObject *object, int ind, const QStringList &names);

  QObject *childObject(QObject *parent, const QString &name);

  QWidget *getToplevelWidget(QWidget *widget);

  //---

  int getNumProperties(const QObject *object, bool inherited=true);

  QStringList getPropertyList(const QObject *object, bool inherited=true);

  QStringList getPropertyList(const QObject *object, bool inherited,
                              const QMetaObject* metaObject);

  QString getPropertyName(const QObject *object, int ind, bool inherited=true);

  QVariant::Type getPropertyType(const QObject *object, int ind, bool inherited=true);

  QVariant getPropertyValue(const QObject *object, int ind, bool inherited=true);

  bool getPropertyValueIsEnum(const QObject *object, int ind, bool inherited=true);

  QString getPropertyEnumName(const QObject *object, int ind, bool inherited=true);

  QString getPropertyEnumValue(const QObject *object, int ind, bool inherited=true);

  QStringList getMetaPropertyEnumNames(const QObject *object, int ind, bool inherited=true);

  QList<int> getMetaPropertyEnumValues(const QObject *object, int ind, bool inherited=true);

  bool getMetaProperty(const QObject *object, int ind, bool inherited,
                       QMetaProperty &metaProperty);

  bool getProperty(const QObject *object, const QString &name, QVariant &value);

  bool setPropertyValue(QObject *object, int ind, const QVariant &value, bool inherited=true);

  bool setProperty(const QObject *object, const QString &name, const QString &value);
  bool setProperty(const QObject *object, const QString &name, const QVariant &value);

  bool getPropertyInfo(const QObject *object, int ind, PropInfo *info, bool inherited=true);

  bool getPropInfo(const QObject *object, const QString &name, PropInfo *info);

  //---

  QString className(const QObject *object);

  QStringList hierClassNames(const QObject *object);

  const QMetaObject *baseClass(QMetaObject *metaObject, const char *name);

  //---

  int numSignals(const QObject *object, bool hierarchical=true);

  QString signalName(const QObject *object, int ind, bool hierarchical=true);

  QStringList signalNames(const QObject* object, bool inherited=true);

  //---

  int numSlots(const QObject *object, bool hierarchical=true);

  QString slotName(const QObject *object, int ind, bool hierarchical=true);

  QStringList slotNames(const QObject* object, bool inherited=true);

  //---

  QString eventTypeToName(QEvent::Type type);
  QString eventToString(QEvent *event);

  QColor getForeground(QWidget *widget);
  void   setForeground(QWidget *widget, const QColor &color);

  QColor getBackground(QWidget *widget);
  void   setBackground(QWidget *widget, const QColor &color);

#ifdef CQUTIL_POINT
  QPointF  toQPoint  (const CPoint2D &point);
  QPoint   toQPointI (const CPoint2D &point);
  CPoint2D fromQPoint(const QPointF &point);

  QPoint    toQPoint   (const CIPoint2D &point);
  CIPoint2D fromQPoint (const QPoint &point);
  CPoint2D  fromQPointF(const QPoint &point);
#endif

#ifdef CQUTIL_SIZE
  QSizeF  toQSize  (const CSize2D &size);
  CSize2D fromQSize(const QSizeF &size);

  QSize    toQSize  (const CISize2D &size);
  CISize2D fromQSize(const QSize &size);
#endif

#ifdef CQUTIL_BBOX
  QRectF  toQRect  (const CBBox2D &rect);
  QRect   toQRectI (const CBBox2D &rect);
  CBBox2D fromQRect(const QRectF &rect);

  QRect    toQRect  (const CIBBox2D &rect);
  CIBBox2D fromQRect(const QRect &rect);
#endif

#ifdef CQUTIL_MATRIX
  QMatrix   toQMatrix  (const CMatrix2D &m);
  CMatrix2D fromQMatrix(const QMatrix &m);

  QTransform toQTransform  (const CMatrix2D &m);
  CMatrix2D  fromQTransform(const QTransform &t);
#endif

#ifdef CQUTIL_FONT
  QFont    toQFont  (CFontPtr font);
  CFontPtr fromQFont(QFont font);
#endif

  QString variantToString(const QVariant &var);
  bool variantToString(const QVariant &var, QString &str);

  bool stringToVariant(const QString &str, QVariant::Type type,
                       const char *typeName, QVariant &var);

#ifdef CQUTIL_PALETTE
  bool paletteFromString(QPalette &palette, const QString &paletteDef);
  QString paletteToString(const QPalette &palette);
#endif

  bool activateSlot(QObject *receiver, const char *slotName, const char *valuesStr);
  bool activateSignal(QObject* sender, const char *signalName, const char *valuesStr);

  /*! template function to get table cell widget of type */
  template<typename T>
  T tableCellWidget(QTableWidget *table, int row, int col) {
    return qobject_cast<T>(table->cellWidget(row, col));
  }

#ifdef CQUTIL_IMAGE
  QIcon imageToIcon(CImagePtr image);

  QImage toQImage(CImagePtr image);
#endif

#ifdef CQUTIL_GRADIENT
  QLinearGradient toQGradient(const CLinearGradient *lgradient,
                              QGradient::CoordinateMode mode=QGradient::ObjectBoundingMode);
  QRadialGradient toQGradient(const CRadialGradient *rgradient,
                              QGradient::CoordinateMode mode=QGradient::ObjectBoundingMode);
#endif

  void getScreenSize(uint *w, uint *h);

  void setSelectText(const std::string &text);

  void setDockVisible(QDockWidget *dock, bool visible);

#ifdef CQUTIL_LINE_DASH
  void penSetLineDash(QPen &pen, const CLineDash &dash);
#endif

#ifdef CQUTIL_ALIGN
  Qt::Alignment toQAlign(CHAlignType halign);
  Qt::Alignment toQAlign(CVAlignType valign);

  CHAlignType toHAlign(Qt::Alignment align);
  CVAlignType toVAlign(Qt::Alignment align);
#endif

  //-----

  QPixmap getPixmap(const QString &name);

  QIcon getIcon(const QString &name);

  //-----

  uint nameWidgetTree(QWidget *widget);

  void nameWidget(QWidget *widget);

  void nameWidgetButton(QAbstractButton *button);
  void nameWidgetLabel(QLabel *label);
  void nameWidgetGen(QWidget *widget);

  //-----

  void recolorImage(QImage &image, const QColor &fg, const QColor &bg);

  //-----

  void drawHtmlText(QWidget *w, QPainter *painter, const QString &text,
                    const QPalette &palette, const QRect &rect, bool active=false);

  QString colorToHtml(const QColor &c);

  //----

  QString policyToString(QSizePolicy::Policy policy);

  //----

#ifdef CQUTIL_RGB
  QColor blendColors(const QColor &c1, const QColor &c2, double f);
#endif
}

namespace CQUtil {
  template<class T>
  T *makeWidget(const QString &name) {
    T *t = new T;

    t->setObjectName(name);

    return t;
  }

  template<class T>
  T *makeLayout(QWidget *parent, int margin=2, int spacing=2) {
    T *t = new T(parent);

    t->setMargin (margin );
    t->setSpacing(spacing);

    return t;
  }

  template<class T>
  T *makeLayout(int border=2, int spacing=2) {
    T *t = new T;

    t->setBorder (border );
    t->setSpacing(spacing);

    return t;
  }
}

class CQWidgetPtr : public QObject {
  Q_OBJECT

 public:
  CQWidgetPtr(QWidget *w=nullptr) :
   w_(w) {
    watch();
  }

  bool isValid() const {
    return (w_ != nullptr);
  }

  void set(QWidget *w) {
    w_ = w;

    watch();
  }

  QWidget *get() const {
    return w_;
  }

  template<typename T>
  T *get() const {
    return dynamic_cast<T *>(w_);
  }

 private:
  void watch() {
    if (w_)
      connect(w_, SIGNAL(destroyed(QObject *)), this, SLOT(resetSlot()));
  }

 private slots:
  void resetSlot() {
    w_ = nullptr;
  }

 private:
  QWidget *w_;
};

#endif
