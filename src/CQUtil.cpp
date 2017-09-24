#include <CQUtil.h>

//#include <CQApp.h>

#ifdef CQUTIL_IMAGE
#include <CQImage.h>
#endif

#ifdef CQUTIL_FONT
#include <CQFont.h>
#endif

#ifdef CQUTIL_EVENT
#include <CEvent.h>
#endif

//#include <CRegExp.h>

#ifdef CQUTIL_LINE_DASH
#include <CLineDash.h>
#endif

#ifdef CQUTIL_ANGLE
#include <CAngle.h>
#endif

#ifdef CQUTIL_GRADIENT
#include <CLinearGradient.h>
#include <CRadialGradient.h>
#endif

#include <Qt>
#include <QApplication>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QAction>
#include <QMouseEvent>
#include <QMetaObject>
#include <QMetaProperty>
#include <QPalette>
#include <QLabel>
#include <QClipboard>
#include <QAbstractButton>
#include <QAbstractTextDocumentLayout>
#include <QPainter>

#define DCL_META_STREAM(TYPE, GETTER, SETTER) \
QDataStream &operator<<(QDataStream &out, const TYPE &t) { \
  QString str = t.GETTER().c_str(); \
\
  out << str; \
\
  return out; \
} \
\
QDataStream &operator>>(QDataStream &in, TYPE &t) {\
  QString str; \
\
  in >> str; \
\
  t.fromString(str.toStdString()); \
\
  return in; \
}

#ifdef CQUTIL_LINE_DASH
DCL_META_STREAM(CLineDash, toString, fromString)
#endif

#ifdef CQUTIL_ANGLE
DCL_META_STREAM(CAngle, toString, fromString)
#endif

//------

namespace {

QString metaMethodSignature(QMetaMethod &m) {
#if QT_VERSION < 0x050000
  return m.signature();
#else
  return m.methodSignature();
#endif
}

}

//------

void
CQUtil::
initProperties()
{
#ifdef CQUTIL_LINE_DASH
  qRegisterMetaType<CLineDash>("CLineDash");
  qRegisterMetaTypeStreamOperators<CLineDash>("CLineDash");
#endif

#ifdef CQUTIL_ANGLE
  qRegisterMetaType<CAngle>("CAngle");
  qRegisterMetaTypeStreamOperators<CAngle>("CAngle");
#endif
}

#ifdef CQUTIL_EVENT
CMouseEvent *
CQUtil::
convertEvent(QMouseEvent *event)
{
  static CMouseEvent event1;

  event1 = CMouseEvent(CIPoint2D(event->x(), event->y()), convertButton(event->button()),
                       1, convertModifier(event->modifiers()));

  return &event1;
}

CKeyEvent *
CQUtil::
convertEvent(QKeyEvent *event)
{
  static CKeyEvent event1;

  CIPoint2D position;

  event1 = CKeyEvent(position, convertKey(event->key(), event->modifiers()),
                     event->text().toStdString(), convertModifier(event->modifiers()));

  return &event1;
}

CMouseButton
CQUtil::
convertButton(Qt::MouseButton button)
{
  switch (button) {
    case Qt::NoButton:    return CBUTTON_NONE;
    case Qt::LeftButton:  return CBUTTON_LEFT;
    case Qt::MidButton:   return CBUTTON_MIDDLE;
    case Qt::RightButton: return CBUTTON_RIGHT;
    case Qt::XButton1:    return CBUTTON_NONE;
    case Qt::XButton2:    return CBUTTON_NONE;
    default:              return CBUTTON_NONE;
  }
}

CKeyType
CQUtil::
convertKey(int key, Qt::KeyboardModifiers modifiers)
{
  if (key >= Qt::Key_A && key <= Qt::Key_Z) {
    if (modifiers & Qt::ShiftModifier)
      return CKeyType(CKEY_TYPE_A + (key - Qt::Key_A));
    else
      return CKeyType(CKEY_TYPE_a + (key - Qt::Key_A));
  }

  switch (key) {
    case Qt::Key_Escape:                return CKEY_TYPE_Escape;
    case Qt::Key_Tab:                   return CKEY_TYPE_Tab;
    case Qt::Key_Backtab:               return CKEY_TYPE_Backtab;
    case Qt::Key_Backspace:             return CKEY_TYPE_BackSpace;
    case Qt::Key_Return:                return CKEY_TYPE_Return;
    case Qt::Key_Enter:                 return CKEY_TYPE_KP_Enter;
    case Qt::Key_Insert:                return CKEY_TYPE_Insert;
    case Qt::Key_Delete:                return CKEY_TYPE_DEL;
    case Qt::Key_Pause:                 return CKEY_TYPE_Pause;
    case Qt::Key_Print:                 return CKEY_TYPE_Print;
    case Qt::Key_SysReq:                return CKEY_TYPE_Sys_Req;
    case Qt::Key_Clear:                 return CKEY_TYPE_Clear;
    case Qt::Key_Home:                  return CKEY_TYPE_Home;
    case Qt::Key_End:                   return CKEY_TYPE_End;
    case Qt::Key_Left:                  return CKEY_TYPE_Left;
    case Qt::Key_Up:                    return CKEY_TYPE_Up;
    case Qt::Key_Right:                 return CKEY_TYPE_Right;
    case Qt::Key_Down:                  return CKEY_TYPE_Down;
    case Qt::Key_PageUp:                return CKEY_TYPE_Page_Up;
    case Qt::Key_PageDown:              return CKEY_TYPE_Page_Down;
    case Qt::Key_Shift:                 return CKEY_TYPE_Shift_L;
    case Qt::Key_Control:               return CKEY_TYPE_Control_L;
    case Qt::Key_Meta:                  return CKEY_TYPE_Meta_L;
    case Qt::Key_Alt:                   return CKEY_TYPE_Alt_L;
    case Qt::Key_AltGr:                 return CKEY_TYPE_Super_L;
    case Qt::Key_CapsLock:              return CKEY_TYPE_Caps_Lock;
    case Qt::Key_NumLock:               return CKEY_TYPE_Num_Lock;
    case Qt::Key_ScrollLock:            return CKEY_TYPE_Scroll_Lock;
    case Qt::Key_F1:                    return CKEY_TYPE_F1;
    case Qt::Key_F2:                    return CKEY_TYPE_F2;
    case Qt::Key_F3:                    return CKEY_TYPE_F3;
    case Qt::Key_F4:                    return CKEY_TYPE_F4;
    case Qt::Key_F5:                    return CKEY_TYPE_F5;
    case Qt::Key_F6:                    return CKEY_TYPE_F6;
    case Qt::Key_F7:                    return CKEY_TYPE_F7;
    case Qt::Key_F8:                    return CKEY_TYPE_F8;
    case Qt::Key_F9:                    return CKEY_TYPE_F9;
    case Qt::Key_F10:                   return CKEY_TYPE_F10;
    case Qt::Key_F11:                   return CKEY_TYPE_F11;
    case Qt::Key_F12:                   return CKEY_TYPE_F12;
    case Qt::Key_F13:                   return CKEY_TYPE_F13;
    case Qt::Key_F14:                   return CKEY_TYPE_F14;
    case Qt::Key_F15:                   return CKEY_TYPE_F15;
    case Qt::Key_F16:                   return CKEY_TYPE_F16;
    case Qt::Key_F17:                   return CKEY_TYPE_F17;
    case Qt::Key_F18:                   return CKEY_TYPE_F18;
    case Qt::Key_F19:                   return CKEY_TYPE_F19;
    case Qt::Key_F20:                   return CKEY_TYPE_F20;
    case Qt::Key_F21:                   return CKEY_TYPE_F21;
    case Qt::Key_F22:                   return CKEY_TYPE_F22;
    case Qt::Key_F23:                   return CKEY_TYPE_F23;
    case Qt::Key_F24:                   return CKEY_TYPE_F24;
    case Qt::Key_F25:                   return CKEY_TYPE_F25;
    case Qt::Key_F26:                   return CKEY_TYPE_F26;
    case Qt::Key_F27:                   return CKEY_TYPE_F27;
    case Qt::Key_F28:                   return CKEY_TYPE_F28;
    case Qt::Key_F29:                   return CKEY_TYPE_F29;
    case Qt::Key_F30:                   return CKEY_TYPE_F30;
    case Qt::Key_F31:                   return CKEY_TYPE_F31;
    case Qt::Key_F32:                   return CKEY_TYPE_F32;
    case Qt::Key_F33:                   return CKEY_TYPE_F33;
    case Qt::Key_F34:                   return CKEY_TYPE_F34;
    case Qt::Key_F35:                   return CKEY_TYPE_F35;
    case Qt::Key_Super_L:               return CKEY_TYPE_Super_L;
    case Qt::Key_Super_R:               return CKEY_TYPE_Super_R;
    case Qt::Key_Menu:                  return CKEY_TYPE_Menu;
    case Qt::Key_Hyper_L:               return CKEY_TYPE_Hyper_L;
    case Qt::Key_Hyper_R:               return CKEY_TYPE_Hyper_R;
    case Qt::Key_Help:                  return CKEY_TYPE_Help;
    case Qt::Key_Direction_L:           return CKEY_TYPE_NUL;
    case Qt::Key_Direction_R:           return CKEY_TYPE_NUL;
    case Qt::Key_Space:                 return CKEY_TYPE_Space;
    case Qt::Key_Exclam:                return CKEY_TYPE_Exclam;
    case Qt::Key_QuoteDbl:              return CKEY_TYPE_QuoteDbl;
    case Qt::Key_NumberSign:            return CKEY_TYPE_NumberSign;
    case Qt::Key_Dollar:                return CKEY_TYPE_Dollar;
    case Qt::Key_Percent:               return CKEY_TYPE_Percent;
    case Qt::Key_Ampersand:             return CKEY_TYPE_Ampersand;
    case Qt::Key_Apostrophe:            return CKEY_TYPE_Apostrophe;
    case Qt::Key_ParenLeft:             return CKEY_TYPE_ParenLeft;
    case Qt::Key_ParenRight:            return CKEY_TYPE_ParenRight;
    case Qt::Key_Asterisk:              return CKEY_TYPE_Asterisk;
    case Qt::Key_Plus:                  return CKEY_TYPE_Plus;
    case Qt::Key_Comma:                 return CKEY_TYPE_Comma;
    case Qt::Key_Minus:                 return CKEY_TYPE_Minus;
    case Qt::Key_Period:                return CKEY_TYPE_Period;
    case Qt::Key_Slash:                 return CKEY_TYPE_Slash;
    case Qt::Key_0:                     return CKEY_TYPE_0;
    case Qt::Key_1:                     return CKEY_TYPE_1;
    case Qt::Key_2:                     return CKEY_TYPE_2;
    case Qt::Key_3:                     return CKEY_TYPE_3;
    case Qt::Key_4:                     return CKEY_TYPE_4;
    case Qt::Key_5:                     return CKEY_TYPE_5;
    case Qt::Key_6:                     return CKEY_TYPE_6;
    case Qt::Key_7:                     return CKEY_TYPE_7;
    case Qt::Key_8:                     return CKEY_TYPE_8;
    case Qt::Key_9:                     return CKEY_TYPE_9;
    case Qt::Key_Colon:                 return CKEY_TYPE_Colon;
    case Qt::Key_Semicolon:             return CKEY_TYPE_Semicolon;
    case Qt::Key_Less:                  return CKEY_TYPE_Less;
    case Qt::Key_Equal:                 return CKEY_TYPE_Equal;
    case Qt::Key_Greater:               return CKEY_TYPE_Greater;
    case Qt::Key_Question:              return CKEY_TYPE_Question;
    case Qt::Key_At:                    return CKEY_TYPE_At;
    case Qt::Key_BracketLeft:           return CKEY_TYPE_BracketLeft;
    case Qt::Key_Backslash:             return CKEY_TYPE_Backslash;
    case Qt::Key_BracketRight:          return CKEY_TYPE_BracketRight;
    case Qt::Key_AsciiCircum:           return CKEY_TYPE_AsciiCircum;
    case Qt::Key_Underscore:            return CKEY_TYPE_Underscore;
    case Qt::Key_QuoteLeft:             return CKEY_TYPE_QuoteLeft;
    case Qt::Key_BraceLeft:             return CKEY_TYPE_BraceLeft;
    case Qt::Key_Bar:                   return CKEY_TYPE_Bar;
    case Qt::Key_BraceRight:            return CKEY_TYPE_BraceRight;
    case Qt::Key_AsciiTilde:            return CKEY_TYPE_AsciiTilde;
    case Qt::Key_nobreakspace:          return CKEY_TYPE_nobreakspace;
    case Qt::Key_exclamdown:            return CKEY_TYPE_exclamdown;
    case Qt::Key_cent:                  return CKEY_TYPE_cent;
    case Qt::Key_sterling:              return CKEY_TYPE_sterling;
    case Qt::Key_currency:              return CKEY_TYPE_currency;
    case Qt::Key_yen:                   return CKEY_TYPE_yen;
    case Qt::Key_brokenbar:             return CKEY_TYPE_brokenbar;
    case Qt::Key_section:               return CKEY_TYPE_section;
    case Qt::Key_diaeresis:             return CKEY_TYPE_diaeresis;
    case Qt::Key_copyright:             return CKEY_TYPE_copyright;
    case Qt::Key_ordfeminine:           return CKEY_TYPE_ordfeminine;
    case Qt::Key_guillemotleft:         return CKEY_TYPE_guillemotleft;
    case Qt::Key_notsign:               return CKEY_TYPE_notsign;
    case Qt::Key_hyphen:                return CKEY_TYPE_hyphen;
    case Qt::Key_registered:            return CKEY_TYPE_registered;
    case Qt::Key_macron:                return CKEY_TYPE_macron;
    case Qt::Key_degree:                return CKEY_TYPE_degree;
    case Qt::Key_plusminus:             return CKEY_TYPE_plusminus;
    case Qt::Key_twosuperior:           return CKEY_TYPE_twosuperior;
    case Qt::Key_threesuperior:         return CKEY_TYPE_threesuperior;
    case Qt::Key_acute:                 return CKEY_TYPE_acute;
    case Qt::Key_mu:                    return CKEY_TYPE_mu;
    case Qt::Key_paragraph:             return CKEY_TYPE_paragraph;
    case Qt::Key_periodcentered:        return CKEY_TYPE_periodcentered;
    case Qt::Key_cedilla:               return CKEY_TYPE_cedilla;
    case Qt::Key_onesuperior:           return CKEY_TYPE_onesuperior;
    case Qt::Key_masculine:             return CKEY_TYPE_masculine;
    case Qt::Key_guillemotright:        return CKEY_TYPE_guillemotright;
    case Qt::Key_onequarter:            return CKEY_TYPE_onequarter;
    case Qt::Key_onehalf:               return CKEY_TYPE_onehalf;
    case Qt::Key_threequarters:         return CKEY_TYPE_threequarters;
    case Qt::Key_questiondown:          return CKEY_TYPE_questiondown;
    case Qt::Key_Agrave:                return CKEY_TYPE_Agrave;
    case Qt::Key_Aacute:                return CKEY_TYPE_Aacute;
    case Qt::Key_Acircumflex:           return CKEY_TYPE_Acircumflex;
    case Qt::Key_Atilde:                return CKEY_TYPE_Atilde;
    case Qt::Key_Adiaeresis:            return CKEY_TYPE_Adiaeresis;
    case Qt::Key_Aring:                 return CKEY_TYPE_Aring;
    case Qt::Key_AE:                    return CKEY_TYPE_AE;
    case Qt::Key_Ccedilla:              return CKEY_TYPE_Ccedilla;
    case Qt::Key_Egrave:                return CKEY_TYPE_Egrave;
    case Qt::Key_Eacute:                return CKEY_TYPE_Eacute;
    case Qt::Key_Ecircumflex:           return CKEY_TYPE_Ecircumflex;
    case Qt::Key_Ediaeresis:            return CKEY_TYPE_Ediaeresis;
    case Qt::Key_Igrave:                return CKEY_TYPE_Igrave;
    case Qt::Key_Iacute:                return CKEY_TYPE_Iacute;
    case Qt::Key_Icircumflex:           return CKEY_TYPE_Icircumflex;
    case Qt::Key_Idiaeresis:            return CKEY_TYPE_Idiaeresis;
    case Qt::Key_ETH:                   return CKEY_TYPE_ETH;
    case Qt::Key_Ntilde:                return CKEY_TYPE_Ntilde;
    case Qt::Key_Ograve:                return CKEY_TYPE_Ograve;
    case Qt::Key_Oacute:                return CKEY_TYPE_Oacute;
    case Qt::Key_Ocircumflex:           return CKEY_TYPE_Ocircumflex;
    case Qt::Key_Otilde:                return CKEY_TYPE_Otilde;
    case Qt::Key_Odiaeresis:            return CKEY_TYPE_Odiaeresis;
    case Qt::Key_multiply:              return CKEY_TYPE_multiply;
    case Qt::Key_Ooblique:              return CKEY_TYPE_Ooblique;
    case Qt::Key_Ugrave:                return CKEY_TYPE_Ugrave;
    case Qt::Key_Uacute:                return CKEY_TYPE_Uacute;
    case Qt::Key_Ucircumflex:           return CKEY_TYPE_Ucircumflex;
    case Qt::Key_Udiaeresis:            return CKEY_TYPE_Udiaeresis;
    case Qt::Key_Yacute:                return CKEY_TYPE_Yacute;
    case Qt::Key_THORN:                 return CKEY_TYPE_THORN;
    case Qt::Key_ssharp:                return CKEY_TYPE_ssharp;
    case Qt::Key_division:              return CKEY_TYPE_division;
    case Qt::Key_ydiaeresis:            return CKEY_TYPE_ydiaeresis;
    case Qt::Key_Multi_key:             return CKEY_TYPE_NUL;
    case Qt::Key_Codeinput:             return CKEY_TYPE_NUL;
    case Qt::Key_SingleCandidate:       return CKEY_TYPE_NUL;
    case Qt::Key_MultipleCandidate:     return CKEY_TYPE_NUL;
    case Qt::Key_PreviousCandidate:     return CKEY_TYPE_NUL;
    case Qt::Key_Mode_switch:           return CKEY_TYPE_Mode_switch;
    case Qt::Key_Kanji:                 return CKEY_TYPE_NUL;
    case Qt::Key_Muhenkan:              return CKEY_TYPE_NUL;
    case Qt::Key_Henkan:                return CKEY_TYPE_NUL;
    case Qt::Key_Romaji:                return CKEY_TYPE_NUL;
    case Qt::Key_Hiragana:              return CKEY_TYPE_NUL;
    case Qt::Key_Katakana:              return CKEY_TYPE_NUL;
    case Qt::Key_Hiragana_Katakana:     return CKEY_TYPE_NUL;
    case Qt::Key_Zenkaku:               return CKEY_TYPE_NUL;
    case Qt::Key_Hankaku:               return CKEY_TYPE_NUL;
    case Qt::Key_Zenkaku_Hankaku:       return CKEY_TYPE_NUL;
    case Qt::Key_Touroku:               return CKEY_TYPE_NUL;
    case Qt::Key_Massyo:                return CKEY_TYPE_NUL;
    case Qt::Key_Kana_Lock:             return CKEY_TYPE_NUL;
    case Qt::Key_Kana_Shift:            return CKEY_TYPE_NUL;
    case Qt::Key_Eisu_Shift:            return CKEY_TYPE_NUL;
    case Qt::Key_Eisu_toggle:           return CKEY_TYPE_NUL;
    case Qt::Key_Hangul:                return CKEY_TYPE_NUL;
    case Qt::Key_Hangul_Start:          return CKEY_TYPE_NUL;
    case Qt::Key_Hangul_End:            return CKEY_TYPE_NUL;
    case Qt::Key_Hangul_Hanja:          return CKEY_TYPE_NUL;
    case Qt::Key_Hangul_Jamo:           return CKEY_TYPE_NUL;
    case Qt::Key_Hangul_Romaja:         return CKEY_TYPE_NUL;
    case Qt::Key_Hangul_Jeonja:         return CKEY_TYPE_NUL;
    case Qt::Key_Hangul_Banja:          return CKEY_TYPE_NUL;
    case Qt::Key_Hangul_PreHanja:       return CKEY_TYPE_NUL;
    case Qt::Key_Hangul_PostHanja:      return CKEY_TYPE_NUL;
    case Qt::Key_Hangul_Special:        return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Grave:            return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Acute:            return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Circumflex:       return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Tilde:            return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Macron:           return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Breve:            return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Abovedot:         return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Diaeresis:        return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Abovering:        return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Doubleacute:      return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Caron:            return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Cedilla:          return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Ogonek:           return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Iota:             return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Voiced_Sound:     return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Semivoiced_Sound: return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Belowdot:         return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Hook:             return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Horn:             return CKEY_TYPE_NUL;
    case Qt::Key_Back:                  return CKEY_TYPE_NUL;
    case Qt::Key_Forward:               return CKEY_TYPE_NUL;
    case Qt::Key_Stop:                  return CKEY_TYPE_NUL;
    case Qt::Key_Refresh:               return CKEY_TYPE_NUL;
    case Qt::Key_VolumeDown:            return CKEY_TYPE_NUL;
    case Qt::Key_VolumeMute:            return CKEY_TYPE_NUL;
    case Qt::Key_VolumeUp:              return CKEY_TYPE_NUL;
    case Qt::Key_BassBoost:             return CKEY_TYPE_NUL;
    case Qt::Key_BassUp:                return CKEY_TYPE_NUL;
    case Qt::Key_BassDown:              return CKEY_TYPE_NUL;
    case Qt::Key_TrebleUp:              return CKEY_TYPE_NUL;
    case Qt::Key_TrebleDown:            return CKEY_TYPE_NUL;
    case Qt::Key_MediaPlay:             return CKEY_TYPE_NUL;
    case Qt::Key_MediaStop:             return CKEY_TYPE_NUL;
    case Qt::Key_MediaPrevious:         return CKEY_TYPE_NUL;
    case Qt::Key_MediaNext:             return CKEY_TYPE_NUL;
    case Qt::Key_MediaRecord:           return CKEY_TYPE_NUL;
    case Qt::Key_HomePage:              return CKEY_TYPE_NUL;
    case Qt::Key_Favorites:             return CKEY_TYPE_NUL;
    case Qt::Key_Search:                return CKEY_TYPE_NUL;
    case Qt::Key_Standby:               return CKEY_TYPE_NUL;
    case Qt::Key_OpenUrl:               return CKEY_TYPE_NUL;
    case Qt::Key_LaunchMail:            return CKEY_TYPE_NUL;
    case Qt::Key_LaunchMedia:           return CKEY_TYPE_NUL;
    case Qt::Key_Launch0:               return CKEY_TYPE_NUL;
    case Qt::Key_Launch1:               return CKEY_TYPE_NUL;
    case Qt::Key_Launch2:               return CKEY_TYPE_NUL;
    case Qt::Key_Launch3:               return CKEY_TYPE_NUL;
    case Qt::Key_Launch4:               return CKEY_TYPE_NUL;
    case Qt::Key_Launch5:               return CKEY_TYPE_NUL;
    case Qt::Key_Launch6:               return CKEY_TYPE_NUL;
    case Qt::Key_Launch7:               return CKEY_TYPE_NUL;
    case Qt::Key_Launch8:               return CKEY_TYPE_NUL;
    case Qt::Key_Launch9:               return CKEY_TYPE_NUL;
    case Qt::Key_LaunchA:               return CKEY_TYPE_NUL;
    case Qt::Key_LaunchB:               return CKEY_TYPE_NUL;
    case Qt::Key_LaunchC:               return CKEY_TYPE_NUL;
    case Qt::Key_LaunchD:               return CKEY_TYPE_NUL;
    case Qt::Key_LaunchE:               return CKEY_TYPE_NUL;
    case Qt::Key_LaunchF:               return CKEY_TYPE_NUL;
    case Qt::Key_MediaLast:             return CKEY_TYPE_NUL;
    case Qt::Key_unknown:               return CKEY_TYPE_NUL;
    case Qt::Key_Call:                  return CKEY_TYPE_NUL;
    case Qt::Key_Context1:              return CKEY_TYPE_NUL;
    case Qt::Key_Context2:              return CKEY_TYPE_NUL;
    case Qt::Key_Context3:              return CKEY_TYPE_NUL;
    case Qt::Key_Context4:              return CKEY_TYPE_NUL;
    case Qt::Key_Flip:                  return CKEY_TYPE_NUL;
    case Qt::Key_Hangup:                return CKEY_TYPE_NUL;
    case Qt::Key_No:                    return CKEY_TYPE_NUL;
    case Qt::Key_Select:                return CKEY_TYPE_Select;
    case Qt::Key_Yes:                   return CKEY_TYPE_NUL;
    default:                            return CKEY_TYPE_NUL;
  }
}

CEventModifier
CQUtil::
convertModifier(Qt::KeyboardModifiers modifiers)
{
  uint modifiers1 = CMODIFIER_NONE;

  if (modifiers & Qt::ShiftModifier  ) modifiers1 |= CMODIFIER_SHIFT;
  if (modifiers & Qt::ControlModifier) modifiers1 |= CMODIFIER_CONTROL;
  if (modifiers & Qt::AltModifier    ) modifiers1 |= CMODIFIER_ALT;
  if (modifiers & Qt::MetaModifier   ) modifiers1 |= CMODIFIER_META;
  if (modifiers & Qt::KeypadModifier ) modifiers1 |= CMODIFIER_KEYPAD;

  return (CEventModifier) modifiers1;
}
#endif

#ifdef CQUTIL_RGBA
QColor
CQUtil::
rgbToColor(const CRGB &rgb)
{
  return QColor(rgb.getRedI(), rgb.getGreenI(), rgb.getBlueI());
}

QColor
CQUtil::
rgbaToColor(const CRGBA &rgba)
{
  return QColor(rgba.getRedI(), rgba.getGreenI(), rgba.getBlueI(), rgba.getAlphaI());
}

uint
CQUtil::
rgbaToInt(const CRGBA &rgba)
{
  // #AARRGGBB
  return (rgba.getAlphaI() << 24) |
         (rgba.getRedI  () << 16) |
         (rgba.getGreenI() << 8 ) |
         (rgba.getBlueI ()      );
}

CRGB
CQUtil::
colorToRGB(const QColor &color)
{
  return CRGB(color.redF(), color.greenF(), color.blueF());
}

CRGBA
CQUtil::
colorToRGBA(const QColor &color)
{
  return CRGBA(color.redF(), color.greenF(), color.blueF(), color.alphaF());
}
#endif

#ifdef CQUTIL_BRUSH
QBrush
CQUtil::
toQBrush(const CBrush &brush)
{
  QBrush qbrush;

  if      (brush.getStyle() == CBRUSH_STYLE_SOLID) {
    qbrush = QBrush(CQUtil::rgbaToColor(brush.getColor()));
  }
  else if (brush.getStyle() == CBRUSH_STYLE_PATTERN) {
    //qbrush = QBrush(CQUtil::toQPattern(brush.getPattern()));
    std::cerr << "Invalid pattern brush" << std::endl;
  }
  else if (brush.getStyle() == CBRUSH_STYLE_GRADIENT) {
    CLinearGradient *lgradient = dynamic_cast<CLinearGradient *>(brush.getGradient().get());
    CRadialGradient *rgradient = dynamic_cast<CRadialGradient *>(brush.getGradient().get());

    if      (lgradient)
      qbrush = QBrush(CQUtil::toQGradient(lgradient));
    else if (rgradient)
      qbrush = QBrush(CQUtil::toQGradient(rgradient));
    else
      std::cerr << "Invalid gradient type" << std::endl;
  }
  else if (brush.getStyle() == CBRUSH_STYLE_TEXTURE) {
    qbrush = QBrush(CQUtil::toQImage(brush.getTexture()));
  }
  else {
    std::cerr << "Invalid brush" << std::endl;
  }

  return qbrush;
}
#endif

#ifdef CQUTIL_PEN
QPen
CQUtil::
toQPen(const CPen &pen)
{
  QPen qpen;

  penSetLineDash(qpen, pen.getLineDash());

  qpen.setColor    (rgbaToColor(pen.getColor()));
  qpen.setWidthF   (pen.getWidth());
  qpen.setCapStyle (toPenCapStyle(pen.getLineCap()));
  qpen.setJoinStyle(toPenJoinStyle(pen.getLineJoin()));

  return qpen;
}

Qt::PenCapStyle
CQUtil::
toPenCapStyle(const CLineCapType &lineCap)
{
  switch (lineCap) {
    default:
    case LINE_CAP_TYPE_BUTT  : return Qt::FlatCap;
    case LINE_CAP_TYPE_ROUND : return Qt::RoundCap;
    case LINE_CAP_TYPE_SQUARE: return Qt::SquareCap;
  }
}

Qt::PenJoinStyle
CQUtil::
toPenJoinStyle(const CLineJoinType &lineJoin)
{
  switch (lineJoin) {
    default:
    case LINE_JOIN_TYPE_MITRE: return Qt::MiterJoin;
    case LINE_JOIN_TYPE_ROUND: return Qt::RoundJoin;
    case LINE_JOIN_TYPE_BEVEL: return Qt::BevelJoin;
  }
}
#endif

#ifdef CQUTIL_FONT
void
CQUtil::
decodeFont(const QFont &qfont, QString &family, CFontStyle &style, int &pixelSize)
{
  family = qfont.family();

  if      (qfont.bold() && qfont.italic())
    style = CFONT_STYLE_BOLD_ITALIC;
  else if (qfont.bold())
    style = CFONT_STYLE_BOLD;
  else if (qfont.italic())
    style = CFONT_STYLE_ITALIC;
  else
    style = CFONT_STYLE_NORMAL;

  pixelSize = qfont.pixelSize();

  if (pixelSize == -1) {
    //int dpi = QX11Info::appDpiY();

    int pointSize = qfont.pointSize();

    //pixelSize = pointSize*dpi/72;

    pixelSize = pointSize;
  }
}
#endif

QString
CQUtil::
fullName(const QObject *object)
{
  if (! object)
    return "";

  if (object == qApp)
    return QApplication::applicationName();

  QString name;

  QString sep("|");

  const QObject *o = object;

  QString id;

  while (o) {
    if (name.length())
      name = o->objectName() + sep + name;
    else
      name = o->objectName();

    o = o->parent();
  }

  return name;
}

QObject *
CQUtil::
nameToObject(const QString &name)
{
  if (! name.length())
    return nullptr;

//if (name == QApplication::applicationName())
  if (name == "APPLICATION")
    return qApp;

  QStringList names = name.split("|");

  int num_names = names.size();

  if (num_names == 0)
    return nullptr;

  QString baseName = names[0];

  QObject *current = nullptr;

  QWidgetList wlist = QApplication::topLevelWidgets();

  for (QWidgetList::const_iterator pw = wlist.begin(); pw != wlist.end(); ++pw) {
    if ((*pw)->objectName() == baseName) {
      current = *pw;
      break;
    }
  }

  if (! current)
    return nullptr;

  if (names.size() > 1)
    return hierChildObject(current, 1, names);
  else
    return current;
}

QObject *
CQUtil::
hierChildObject(QObject *object, int ind, const QStringList &names)
{
  QObject *child = childObject(object, names[ind]);

  if (child) {
    ++ind;

    if (ind < names.size())
      return hierChildObject(child, ind, names);
    else
      return child;
  }

  if (names[ind] == "*") {
    const QObjectList &children = object->children();

    for (QObjectList::const_iterator po = children.begin(); po != children.end(); ++po) {
      QObject *child1 = hierChildObject(*po, ind + 1, names);

      if (child1)
        return child1;
    }
  }

  return nullptr;
}

QObject *
CQUtil::
childObject(QObject *parent, const QString &name)
{
  return parent->findChild<QObject *>(name);
}

QWidget *
CQUtil::
getToplevelWidget(QWidget *widget)
{
  QWidget *parent = widget;

  QWidget *parent1;

  while ((parent1 = parent->parentWidget()) != nullptr)
    parent = parent1;

  return parent;
}

int
CQUtil::
getNumProperties(const QObject *object, bool inherited)
{
  if (! object)
    return 0;

  const QMetaObject *metaObject = object->metaObject();

  if (! metaObject)
    return 0;

  int firstProp = (inherited ? 0 : metaObject->propertyOffset());

  return metaObject->propertyCount() - firstProp;
}

QStringList
CQUtil::
getPropertyList(const QObject *object, bool inherited)
{
  const QMetaObject *metaObject = object->metaObject();

  return getPropertyList(object, inherited, metaObject);
}

QStringList
CQUtil::
getPropertyList(const QObject *object, bool inherited, const QMetaObject *metaObject)
{
  QStringList names;

  if (! object || ! metaObject)
    return names;

  int firstProp = (inherited ? 0 : metaObject->propertyOffset());

  for (int p = firstProp; p < metaObject->propertyCount(); ++p) {
    QMetaProperty metaProperty = metaObject->property(p);

    names.push_back(metaProperty.name());
  }

  return names;
}

QString
CQUtil::
getPropertyName(const QObject *object, int ind, bool inherited)
{
  QMetaProperty metaProperty;

  if (! getMetaProperty(object, ind, inherited, metaProperty))
    return QString();

  return metaProperty.name();
}

QVariant::Type
CQUtil::
getPropertyType(const QObject *object, int ind, bool inherited)
{
  QMetaProperty metaProperty;

  if (! getMetaProperty(object, ind, inherited, metaProperty))
    return QVariant::Invalid;

  return metaProperty.type();
}

QVariant
CQUtil::
getPropertyValue(const QObject *object, int ind, bool inherited)
{
  QMetaProperty metaProperty;

  if (! getMetaProperty(object, ind, inherited, metaProperty))
    return QVariant();

  return object->property(metaProperty.name());
}

bool
CQUtil::
getPropertyValueIsEnum(const QObject *object, int ind, bool inherited)
{
  QMetaProperty metaProperty;

  if (! getMetaProperty(object, ind, inherited, metaProperty))
    return false;

  return metaProperty.isEnumType();
}

QString
CQUtil::
getPropertyEnumName(const QObject *object, int ind, bool inherited)
{
  QMetaProperty metaProperty;

  if (! getMetaProperty(object, ind, inherited, metaProperty))
    return QString();

  if (! metaProperty.isEnumType())
    return QString();

  QVariant value = object->property(metaProperty.name());

  QMetaEnum me = metaProperty.enumerator();

  return me.name();
}

QString
CQUtil::
getPropertyEnumValue(const QObject *object, int ind, bool inherited)
{
  QMetaProperty metaProperty;

  if (! getMetaProperty(object, ind, inherited, metaProperty))
    return QString();

  if (! metaProperty.isEnumType())
    return QString();

  QVariant value = object->property(metaProperty.name());

  int eind = value.toInt();

  QMetaEnum me = metaProperty.enumerator();

  int numEnums = me.keyCount();

  if (eind < 0 || eind >= numEnums)
    return QString();

  return me.valueToKey(eind);
}

QStringList
CQUtil::
getMetaPropertyEnumNames(const QObject *object, int ind, bool inherited)
{
  QMetaProperty metaProperty;

  if (! getMetaProperty(object, ind, inherited, metaProperty))
    return QStringList();

  if (! metaProperty.isEnumType())
    return QStringList();

  QMetaEnum me = metaProperty.enumerator();

  QStringList names;

  for (int i = 0; i < me.keyCount(); ++i)
    names.push_back(me.key(i));

  return names;
}

QList<int>
CQUtil::
getMetaPropertyEnumValues(const QObject *object, int ind, bool inherited)
{
  QMetaProperty metaProperty;

  if (! getMetaProperty(object, ind, inherited, metaProperty))
    return QList<int>();

  if (! metaProperty.isEnumType())
    return QList<int>();

  QMetaEnum me = metaProperty.enumerator();

  QList<int> values;

  for (int i = 0; i < me.keyCount(); ++i)
    values.push_back(me.value(i));

  return values;
}

bool
CQUtil::
getMetaProperty(const QObject *object, int ind, bool inherited, QMetaProperty &metaProperty)
{
  if (! object)
    return false;

  const QMetaObject *metaObject = object->metaObject();

  if (! metaObject)
    return false;

  int firstProp = (inherited ? 0 : metaObject->propertyOffset());

  if (ind < 0 || firstProp + ind >= metaObject->propertyCount())
    return false;

  metaProperty = metaObject->property(firstProp + ind);

  return true;
}

bool
CQUtil::
getProperty(const QObject *object, const QString &propName, QVariant &v)
{
  if (! object)
    return false;

  if (propName.isEmpty())
    return false;

  QObject *obj = const_cast<QObject *>(object);

  const QMetaObject *meta = obj->metaObject();

  if (meta) {
    int propIndex = meta->indexOfProperty(propName.toLatin1().data());

    if (propIndex >= 0) {
      //QMetaProperty mP = meta->property(propIndex);

      v = obj->property(propName.toLatin1().data());

      return true;
    }
  }

  return false;
}

bool
CQUtil::
setPropertyValue(QObject *object, int ind, const QVariant &value, bool inherited)
{
  if (! object)
    return false;

  QMetaProperty metaProperty;

  if (! getMetaProperty(object, ind, inherited, metaProperty))
    return false;

  if (! metaProperty.isWritable())
    return false;

  return object->setProperty(metaProperty.name(), value);
}

bool
CQUtil::
setProperty(const QObject *object, const QString &propName, const QString &str)
{
  if (! object)
    return false;

  if (propName.isEmpty())
    return false;

  QObject *obj = const_cast<QObject *>(object);

  const QMetaObject *meta = obj->metaObject();
  if (! meta) return false;

  int propIndex = meta->indexOfProperty(propName.toLatin1().data());
  if (propIndex < 0) return false;

  QMetaProperty mP = meta->property(propIndex);
  if (! mP.isWritable()) return false;

  QVariant v;

  if (! stringToVariant(str, mP.type(), mP.typeName(), v))
    return false;

  return obj->setProperty(propName.toLatin1().data(), v);
}

bool
CQUtil::
setProperty(const QObject *object, const QString &propName, const QVariant &v)
{
  if (! object)
    return false;

  if (propName.isEmpty())
    return false;

  QObject *obj = const_cast<QObject *>(object);

  const QMetaObject *meta = obj->metaObject();
  if (! meta) return false;

  int propIndex = meta->indexOfProperty(propName.toLatin1().data());
  if (propIndex < 0) return false;

  QMetaProperty mP = meta->property(propIndex);
  if (! mP.isWritable()) return false;

  QVariant v1 = v;

  QString typeName = v1.typeName();

  if (typeName == "QString" && ! stringToVariant(v.toString(), mP.type(), mP.typeName(), v1))
    return false;

  return obj->setProperty(propName.toLatin1().data(), v1);
}

bool
CQUtil::
getPropertyInfo(const QObject *object, int ind, PropInfo *propInfo, bool inherited)
{
  if (! object)
    return false;

  QMetaProperty metaProperty;

  if (! getMetaProperty(object, ind, inherited, metaProperty))
    return false;

  QObject *obj = const_cast<QObject *>(object);

  const QMetaObject *meta = obj->metaObject();

  if (! meta)
    return false;

  propInfo->init(metaProperty);

  return true;
}

bool
CQUtil::
getPropInfo(const QObject *object, const QString &propName, PropInfo *propInfo)
{
  if (! object)
    return false;

  if (propName.isEmpty())
    return false;

  QObject *obj = const_cast<QObject *>(object);

  const QMetaObject *meta = obj->metaObject();

  if (meta) {
    int propIndex = meta->indexOfProperty(propName.toLatin1().data());

    if (propIndex >= 0) {
      QMetaProperty mP = meta->property(propIndex);

      propInfo->init(mP);

      return true;
    }
  }

  return false;
}

QString
CQUtil::
className(const QObject *object)
{
  const QMetaObject *mo = object->metaObject();

  if (! mo)
    return "";

  return mo->className();
}

QStringList
CQUtil::
hierClassNames(const QObject *object)
{
  QStringList names;

  const QMetaObject *mo = object->metaObject();

  while (mo) {
    names.push_back(mo->className());

    mo = mo->superClass();
  }

  return names;
}

const QMetaObject *
CQUtil::
baseClass(QMetaObject *metaObject, const char *name)
{
  const QMetaObject *baseMetaObject = metaObject;

  QString str(name);

  while (baseMetaObject) {
    if (str == baseMetaObject->className())
      break;

    baseMetaObject = baseMetaObject->superClass();
  }

  return baseMetaObject;
}

int
CQUtil::
numSignals(const QObject *object, bool inherited)
{
  if (! object)
    return 0;

  const QMetaObject *meta = object->metaObject();

  int start = (inherited ? 0 : meta->methodOffset());

  int num = 0;

  for (int pos = start; pos < meta->methodCount(); ++pos) {
    QMetaMethod metaMethod = meta->method(pos);

    if (metaMethod.attributes() & QMetaMethod::Compatibility)
      continue;

    if (metaMethod.methodType() != QMetaMethod::Signal)
      continue;

    ++num;
  }

  return num;
}

QString
CQUtil::
signalName(const QObject *object, int ind, bool inherited)
{
  if (! object)
    return QString();

  const QMetaObject *meta = object->metaObject();

  int start = (inherited ? 0 : meta->methodOffset());

  int num = 0;

  for (int pos = start; pos < meta->methodCount(); ++pos) {
    QMetaMethod metaMethod = meta->method(pos);

    if (metaMethod.attributes() & QMetaMethod::Compatibility)
      continue;

    if (metaMethod.methodType() != QMetaMethod::Signal)
      continue;

    if (num == ind)
      return metaMethodSignature(metaMethod);

    ++num;
  }

  return "";
}

QStringList
CQUtil::
signalNames(const QObject *object, bool inherited)
{
  QStringList names;

  if (! object)
    return names;

  const QMetaObject *meta = object->metaObject();

  int start = (inherited ? 0 : meta->methodOffset());

  for (int pos = start; pos < meta->methodCount(); ++pos) {
    QMetaMethod metaMethod = meta->method(pos);

    if (metaMethod.attributes() & QMetaMethod::Compatibility)
      continue;

    if (metaMethod.methodType() != QMetaMethod::Signal)
      continue;

    names.append(metaMethodSignature(metaMethod));
  }

  return names;
}

int
CQUtil::
numSlots(const QObject *object, bool inherited)
{
  if (! object)
    return 0;

  const QMetaObject *meta = object->metaObject();

  int start = (inherited ? 0 : meta->methodOffset());

  int num = 0;

  for (int pos = start; pos < meta->methodCount(); ++pos) {
    QMetaMethod metaMethod = meta->method(pos);

    if (metaMethod.attributes() & QMetaMethod::Compatibility)
      continue;

    if (metaMethod.methodType() != QMetaMethod::Slot)
      continue;

    ++num;
  }

  return num;
}

QString
CQUtil::
slotName(const QObject *object, int ind, bool inherited)
{
  if (! object)
    return QString();

  const QMetaObject *meta = object->metaObject();

  int start = (inherited ? 0 : meta->methodOffset());

  int num = 0;

  for (int pos = start; pos < meta->methodCount(); ++pos) {
    QMetaMethod metaMethod = meta->method(pos);

    if (metaMethod.attributes() & QMetaMethod::Compatibility)
      continue;

    if (metaMethod.methodType() != QMetaMethod::Slot)
      continue;

    if (ind == num)
      return metaMethodSignature(metaMethod);

    ++num;
  }

  return QString();
}

QStringList
CQUtil::
slotNames(const QObject *object, bool inherited)
{
  QStringList names;

  const QMetaObject *meta = object->metaObject();

  int start = (inherited ? 0 : meta->methodOffset());

  for (int pos = start; pos < meta->methodCount(); ++pos) {
    QMetaMethod metaMethod = meta->method(pos);

    if (metaMethod.attributes() & QMetaMethod::Compatibility)
      continue;

    if (metaMethod.methodType() != QMetaMethod::Slot)
      continue;

    names.append(metaMethodSignature(metaMethod));
  }

  return names;
}

QString
CQUtil::
eventTypeToName(QEvent::Type type)
{
  switch (type) {
    case QEvent::KeyPress:
      return "KeyPress";
    case QEvent::KeyRelease:
      return "KeyRelease";
    case QEvent::MouseButtonDblClick:
      return "MouseButtonDblClick";
    case QEvent::MouseButtonPress:
      return "MouseButtonPress";
    case QEvent::MouseButtonRelease:
      return "MouseButtonRelease";
    case QEvent::MouseMove:
      return "MouseMove";
    case QEvent::MouseTrackingChange:
      return "MouseTrackingChange";
    default:
      return "Unknown";
  }
}

QString
CQUtil::
eventToString(QEvent *event)
{
  QString name = eventTypeToName(event->type());

  return name;
}

void
CQUtil::
setForeground(QWidget *widget, const QColor &color)
{
  QPalette palette = widget->palette();

  palette.setColor(widget->foregroundRole(), color);

  widget->setPalette(palette);
}

QColor
CQUtil::
getForeground(QWidget *widget)
{
  QPalette palette = widget->palette();

  return palette.color(widget->foregroundRole());
}

void
CQUtil::
setBackground(QWidget *widget, const QColor &color)
{
  QPalette palette = widget->palette();

  palette.setColor(widget->backgroundRole(), color);

  widget->setPalette(palette);

  widget->setAutoFillBackground(true);
}

QColor
CQUtil::
getBackground(QWidget *widget)
{
  QPalette palette = widget->palette();

  return palette.color(widget->backgroundRole());
}

QString
CQUtil::
variantToString(const QVariant &var)
{
  QString valueStr;

  variantToString(var, valueStr);

  return valueStr;
}

bool
CQUtil::
variantToString(const QVariant &var, QString &valueStr)
{
  valueStr = "";

  QVariant::Type type = var.type();

  if      (type == QVariant::Palette) {
#ifdef CQUTIL_PALETTE
    QPalette palette = var.value<QPalette>();

    valueStr = CQUtil::paletteToString(palette);
#endif
  }
  else if (type == QVariant::Point) {
    QPoint point = var.value<QPoint>();

    valueStr = QString("%1 %2").arg(point.x()).arg(point.y());
  }
  else if (type == QVariant::PointF) {
    QPointF point = var.value<QPointF>();

    valueStr = QString("%1 %2").arg(point.x()).arg(point.y());
  }
  else if (type == QVariant::Rect) {
    QRect rect = var.value<QRect>();

    valueStr = QString("{%1 %2} {%3 %4}").
               arg(rect.left ()).arg(rect.bottom()).
               arg(rect.right()).arg(rect.top   ());
  }
  else if (type == QVariant::RectF) {
    QRectF rect = var.value<QRectF>();

    valueStr = QString("{%1 %2} {%3 %4}").
               arg(rect.left ()).arg(rect.bottom()).
               arg(rect.right()).arg(rect.top   ());
  }
  else if (type == QVariant::Size) {
    QSize size = var.value<QSize>();

    valueStr = QString("%1 %2").arg(size.width()).arg(size.height());
  }
  else if (type == QVariant::SizeF) {
    QSizeF size = var.value<QSizeF>();

    valueStr = QString("%1 %2").arg(size.width()).arg(size.height());
  }
  else if (type == QVariant::Bool) {
    bool b = var.value<bool>();

    valueStr = (b ? "true" : "false");
  }
  else if (type == QVariant::Region) {
    QRegion region = var.value<QRegion>();

    QRect r = region.boundingRect();

    valueStr = QString("{%1 %2} {%3 %4}").
               arg(r.left ()).arg(r.bottom()).
               arg(r.right()).arg(r.top   ());
  }
  else if (type == QVariant::Locale) {
    QLocale locale = var.value<QLocale>();

    valueStr = locale.name();
  }
  else if (type == QVariant::SizePolicy) {
    QSizePolicy sp = var.value<QSizePolicy>();

    valueStr = QString("%1 %2 %3 %4").arg(policyToString(sp.horizontalPolicy())).
                                      arg(policyToString(sp.verticalPolicy  ())).
                                      arg(sp.horizontalStretch()).
                                      arg(sp.verticalStretch  ());
  }
  else if (type == QVariant::UserType) {
    if (! var.typeName()) {
      return false;
    }
#ifdef CQUTIL_LINE_DASH
    else if (strcmp(var.typeName(), "CLineDash") == 0) {
      CLineDash lineDash = var.value<CLineDash>();

      valueStr = lineDash.toString().c_str();
    }
#endif
#ifdef CQUTIL_ANGLE
    else if (strcmp(var.typeName(), "CAngle") == 0) {
      CAngle angle = var.value<CAngle>();

      valueStr = angle.toString().c_str();
    }
#endif
    else
      return false;
  }
  else {
    if (! var.canConvert(QVariant::String))
      return false;

    valueStr = var.toString();
  }

  return true;
}

bool
CQUtil::
stringToVariant(const QString &str, QVariant::Type type, const char *typeName, QVariant &var)
{
  // Qt suuports QString ->
  //   QVariant::StringList, QVariant::ByteArray, QVariant::Int      , QVariant::UInt,
  //   QVariant::Bool      , QVariant::Double   , QVariant::Date     , QVariant::Time,
  //   QVariant::DateTime  , QVariant::LongLong , QVariant::ULongLong, QVariant::Char,
  //   QVariant::Url       , QVariant::Uuid

  if      (type == QVariant::Bool) {
    QString lstr = str.toLower();

    if      (lstr == "0" || lstr == "false" || lstr == "no"  || lstr == "off")
      var = QVariant(false);
    else if (lstr == "1" || lstr == "true"  || lstr == "yes" || lstr == "on" )
      var = QVariant(true);
    else
      return false;
  }
  else if (type == QVariant::Point) {
    QRegExp rx("\\s*(\\S+)\\s+(\\S+)\\s*");

    int pos = rx.indexIn(str);

    if (pos == -1)
      return false;

    bool b1; int x = rx.cap(1).toInt(&b1);
    bool b2; int y = rx.cap(2).toInt(&b2);

    if (! b1 && ! b2)
      return false;

    QPoint p(x, y);

    var = QVariant(p);
  }
  else if (type == QVariant::PointF) {
    QRegExp rx("\\s*(\\S+)\\s+(\\S+)\\s*");

    int pos = rx.indexIn(str);

    if (pos == -1)
      return false;

    bool b1; double x = rx.cap(1).toDouble(&b1);
    bool b2; double y = rx.cap(2).toDouble(&b2);

    if (! b1 && ! b2)
      return false;

    QPointF p(x, y);

    var = QVariant(p);
  }
  else if (type == QVariant::Rect) {
    QRegExp rx("\\{\\s*(\\S+)\\s+(\\S+)\\s*\\}\\s*\\{\\s*(\\S+)\\s+(\\S+)\\s*\\}");

    int pos = rx.indexIn(str);

    if (pos == -1)
      return false;

    bool b1; int x1 = rx.cap(1).toInt(&b1);
    bool b2; int y1 = rx.cap(2).toInt(&b2);
    bool b3; int x2 = rx.cap(3).toInt(&b3);
    bool b4; int y2 = rx.cap(4).toInt(&b4);

    if (! b1 && ! b2 && ! b3 && ! b4)
      return false;

    QRect r;

    r.setCoords(x1, y1, x2, y2);

    var = QVariant(r);
  }
  else if (type == QVariant::RectF) {
    QRegExp rx("\\{\\s*(\\S+)\\s+(\\S+)\\s*\\}\\s*\\{\\s*(\\S+)\\s+(\\S+)\\s*\\}");

    int pos = rx.indexIn(str);

    if (pos == -1)
      return false;

    bool b1; double x1 = rx.cap(1).toDouble(&b1);
    bool b2; double y1 = rx.cap(2).toDouble(&b2);
    bool b3; double x2 = rx.cap(3).toDouble(&b3);
    bool b4; double y2 = rx.cap(4).toDouble(&b4);

    if (! b1 && ! b2 && ! b3 && ! b4)
      return false;

    QRectF r;

    r.setCoords(x1, y1, x2, y2);

    var = QVariant(r);
  }
  else if (type == QVariant::Size) {
    QRegExp rx("\\s*(\\S+)\\s+(\\S+)\\s*");

    int pos = rx.indexIn(str);

    if (pos == -1)
      return false;

    bool b1; int w = rx.cap(1).toInt(&b1);
    bool b2; int h = rx.cap(2).toInt(&b2);

    if (! b1 && ! b2)
      return false;

    QSize s(w, h);

    var = QVariant(s);
  }
  else if (type == QVariant::SizeF) {
    QRegExp rx("\\s*(\\S+)\\s+(\\S+)\\s*");

    int pos = rx.indexIn(str);

    if (pos == -1)
      return false;

    bool b1; double w = rx.cap(1).toDouble(&b1);
    bool b2; double h = rx.cap(2).toDouble(&b2);

    if (! b1 && ! b2)
      return false;

    QSizeF s(w, h);

    var = QVariant(s);
  }
  else if (type == QVariant::UserType) {
    if (! typeName) {
      return false;
    }
#ifdef CQUTIL_LINE_DASH
    if      (strcmp(typeName, "CLineDash") == 0) {
      CLineDash lineDash(str.toStdString());

      var = QVariant::fromValue(lineDash);
    }
#endif
#ifdef CQUTIL_ANGLE
    else if (strcmp(typeName, "CAngle") == 0) {
      CAngle angle;

      angle.fromString(str.toStdString());

      var = QVariant::fromValue(angle);
    }
#endif
    else
      return false;
  }
  else {
    var = QVariant(str);

    if (! var.canConvert(type))
      return false;

    if (! var.convert(type))
      return false;
  }

  return true;
}

#ifdef CQUTIL_PALETTE
bool
CQUtil::
paletteFromString(QPalette &palette, const QString &paletteDef)
{
  CRegExp regexp("fg=\"\\(.*\\)\" bg=\"\\(.*\\)\"");

  if (regexp.find(paletteDef.toStdString())) {
    std::vector<std::string> matches;

    regexp.getSubMatches(matches);

    palette.setColor(QPalette::WindowText, QString(matches[0].c_str()));
    palette.setColor(QPalette::Window    , QString(matches[1].c_str()));
  }

  return true;
}

QString
CQUtil::
paletteToString(const QPalette &palette)
{
  QColor fg = palette.color(QPalette::WindowText);
  QColor bg = palette.color(QPalette::Window    );

  return QString("fg=\"%1\" bg=\"%2\"").arg(fg.name()).arg(bg.name());
}
#endif

#ifdef CQUTIL_FONT
QFont
CQUtil::
toQFont(CFontPtr font)
{
  CQFont *cqfont = font.cast<CQFont>();

  if (cqfont)
    return cqfont->getQFont();
  else
    return QFont();
}

CFontPtr
CQUtil::
fromQFont(QFont font)
{
  return CQFontMgrInst->lookupFont(font);
}
#endif

//------------

bool
CQUtil::
activateSlot(QObject *receiver, const char *slotName, const char *valuesStr)
{
#if 0
  QString procName(slotName);

  procName = procName.trimmed();

  QString sigCmd = "send_signal";

  sigCmd += procName.mid(procName.find("("));

  // attach a temporary slotmachine
  QtclSlotMachine *pSlotMachine = new QtclSlotMachine(0, "temporary");

  bool bOk = QtclSlotMachine::connectWidgets(pSlotMachine, sigCmd, receiver, procName,
                                             0, false, false);

  if (! bOk)
    return false;

  //TODO: distinguish argument type

  QStringList valueList;

  int nValues = QtclUtils::convertToStringList(&valueList, valuesStr);

  if (nValues >= 0) {
    const int MAXPARAM = 4;

    const char *enumType = 0;

    int idx = procName.find("(") + 1;

    // skip blanks
    while (procName[idx].isSpace())
      ++idx;

    QVariant *v[MAXPARAM];

    for (int iValue=0; iValue < MAXPARAM; ++iValue) {
      if (iValue < nValues) {
        v[iValue] = new QVariant(valueList.at(iValue));

        if (procName.mid(idx, 3) == "int")
          v[iValue]->cast(QVariant::Int);

        if (procName.mid(idx, 11) == "Orientation") {
          QString strVal = v[iValue]->asString();
          v[iValue]->cast(QVariant::String);
          enumType = "Orientation";
        }
        else if (procName.mid(idx, 6) == "double")
          v[iValue]->cast(QVariant::Double);
        else if (procName.mid(idx, 4) == "bool") {
          QtclVariant tmp(*v[iValue]);
          tmp.castFromTclString(QVariant::Bool);
          *v[iValue] = tmp.asBool();
          v[iValue]->cast(QVariant::Bool);
        }
        else if (procName.mid(idx, 14) == "const QString&")
          v[iValue]->cast(QVariant::String);
        else if (procName.mid(idx, 13) == "const QColor&") {
          v[iValue]->cast(QVariant::Color);
        }

        if (iValue < nValues - 1) {
          int newIdx = procName.find(",", idx) + 1;

          while (procName[newIdx] == ' ')
            ++newIdx;

          if (newIdx <= idx)
            return false;

          idx = newIdx;
        }
      }
      else
        v[iValue] = 0;
    }

    //now call slot function
    pSlotMachine->emit_signal(MAXPARAM, v, enumType);

    for (int i = 0; i < MAXPARAM; ++i)
      delete v[i];
  }

  // remove temporary slotmachine
  delete pSlotMachine;

  return true;
#else
  QString slotNameStr(slotName);

  slotNameStr = slotNameStr.replace("const QString&", "QString");
  slotNameStr = slotNameStr.replace("const QColor&" , "QColor" );

  int iOpen  = slotNameStr.indexOf("(");
  int iClose = slotNameStr.indexOf(")");

  if (iOpen <= 0 || iClose <= iOpen)
    return false;

  QString plainSlotName = slotNameStr.left(iOpen);

  QString args = slotNameStr.mid(iOpen + 1, iClose - iOpen - 1);

  //------

  // check slot exists

  bool found = false;

  for (const QMetaObject *pM = receiver->metaObject();
         pM && ! found; pM = pM->superClass()) {
    for (int i = 0; i < pM->methodCount(); ++i) {
      QMetaMethod metaMethod = pM->method(i);

      if (metaMethod.methodType() != QMetaMethod::Slot)
        continue;

      if (slotNameStr == metaMethodSignature(metaMethod)) {
        found = true;
        break;
      }
    }
  }

  if (! found)
    return false;

  //------

  QGenericArgument qArgs[10];

  QStringList argTypeList =
    args.split(",", QString::SkipEmptyParts);

  QStringList::size_type nArgs = argTypeList.count();

  QStringList valueList =
    QString(valuesStr).split(",", QString::SkipEmptyParts);

  QStringList::size_type nValues = valueList.count();

  if (nArgs != nValues)
    return false;

  std::vector<QColor> colors;

  for (QStringList::size_type iArg = 0; iArg < nArgs; ++iArg) {
    QVariant v(valueList[iArg]);

    QString typeString(argTypeList[iArg]);

    QVariant::Type type = QVariant::nameToType(typeString.toLatin1().data());

    if (! v.canConvert(type)) {
      qDebug("cannot convert slot argument '%s' to type '%s'",
             valueList[iArg].toLatin1().data(), typeString.toLatin1().data());
      return false;
    }

    switch (type) {
      case QVariant::Int:
        qArgs[iArg] = Q_ARG(int, v.toInt());
        break;

      case QVariant::Double:
        qArgs[iArg] = Q_ARG(double, v.toDouble());
        break;

      case QVariant::Bool:
        qArgs[iArg] = Q_ARG(bool, v.toBool());
        break;

      case QVariant::Color: {
        // need to save the color so it doesn't get deleted before we use it
        QColor color(valueList[iArg]);
        colors.push_back(color);
        qArgs[iArg] = Q_ARG(QColor, colors.back());
        break;
      }

      case QVariant::String:
        qArgs[iArg] = Q_ARG(QString, valueList[iArg]);
        break;

      case QVariant::Point:
        qArgs[iArg] = Q_ARG(QPoint, v.value<QPoint>());
        break;

      default:
        qDebug("slot argument of type '%s' not supported",
               typeString.toLatin1().data());
        break;
    }
  }

  bool bReturn =
    QMetaObject::invokeMethod(receiver, plainSlotName.toLatin1().data(),
                              Qt::AutoConnection,
                              qArgs[0], qArgs[1], qArgs[2], qArgs[3], qArgs[4],
                              qArgs[5], qArgs[6], qArgs[7], qArgs[8], qArgs[9]);

  if (! bReturn) {
    qDebug("cannot activate slot '%s'", slotName);
    return false;
  }

  return true;
#endif
}

bool
CQUtil::
activateSignal(QObject *sender, const char *signalName, const char *valuesStr)
{
  QString signalNameStr(signalName);

  signalNameStr = signalNameStr.replace("const QString&", "QString");
  signalNameStr = signalNameStr.replace("const QColor&" , "QColor" );

  int iOpen  = signalNameStr.indexOf("(");
  int iClose = signalNameStr.indexOf(")");

  if (iOpen <= 0 || iClose <= iOpen)
    return false;

  QString plainSignalName = signalNameStr.left(iOpen);

  QString args = signalNameStr.mid(iOpen + 1, iClose - iOpen - 1);

  //------

  // check signal exists

  bool found = false;

  for (const QMetaObject *pM = sender->metaObject();
         pM && ! found; pM = pM->superClass()) {
    for (int i = 0; i < pM->methodCount(); ++i) {
      QMetaMethod metaMethod = pM->method(i);

      if (metaMethod.methodType() != QMetaMethod::Signal)
        continue;

      if (signalNameStr == metaMethodSignature(metaMethod)) {
        found = true;
        break;
      }
    }
  }

  if (! found)
    return false;

  //------

  QGenericArgument qArgs[10];

  QStringList argTypeList =
    args.split(",", QString::SkipEmptyParts);

  QStringList::size_type nArgs = argTypeList.count();

  QStringList valueList =
    QString(valuesStr).split(",", QString::SkipEmptyParts);

  QStringList::size_type nValues = valueList.count();

  if (nArgs != nValues)
    return false;

  std::vector<QColor> colors;

  for (QStringList::size_type iArg = 0; iArg < nArgs; ++iArg) {
    QVariant v(valueList[iArg]);

    QString typeString(argTypeList[iArg]);

    QVariant::Type type = QVariant::nameToType(typeString.toLatin1().data());

    if (! v.canConvert(type)) {
      qDebug("cannot convert signal argument '%s' to type '%s'",
             valueList[iArg].toLatin1().data(), typeString.toLatin1().data());
      return false;
    }

    switch (type) {
      case QVariant::Int:
        qArgs[iArg] = Q_ARG(int, v.toInt());
        break;

      case QVariant::Double:
        qArgs[iArg] = Q_ARG(double, v.toDouble());
        break;

      case QVariant::Bool:
        qArgs[iArg] = Q_ARG(bool, v.toBool());
        break;

      case QVariant::Color: {
        // need to save the color so it doesn't get deleted before we use it
        QColor color(valueList[iArg]);
        colors.push_back(color);
        qArgs[iArg] = Q_ARG(QColor, colors.back());
        break;
      }

      case QVariant::String:
        qArgs[iArg] = Q_ARG(QString, valueList[iArg]);
        break;

      case QVariant::Point:
        qArgs[iArg] = Q_ARG(QPoint, v.value<QPoint>());
        break;

      default:
        qDebug("signal argument of type '%s' not supported",
               typeString.toLatin1().data());
        break;
    }
  }

  bool bReturn =
    QMetaObject::invokeMethod(sender, plainSignalName.toLatin1().data(),
                              Qt::AutoConnection,
                              qArgs[0], qArgs[1], qArgs[2], qArgs[3], qArgs[4],
                              qArgs[5], qArgs[6], qArgs[7], qArgs[8], qArgs[9]);

  if (! bReturn) {
    qDebug("cannot activate signal '%s'", signalName);
    return false;
  }

  return true;
}

#ifdef CQUTIL_IMAGE
QIcon
CQUtil::
imageToIcon(CImagePtr image)
{
  QImage qimage = toQImage(image);

  return QIcon(QPixmap::fromImage(qimage));
}

QImage
CQUtil::
toQImage(CImagePtr image)
{
  return image.cast<CQImage>()->getQImage();
}
#endif

#ifdef CQUTIL_GRADIENT
QLinearGradient
CQUtil::
toQGradient(const CLinearGradient *lgradient, QGradient::CoordinateMode mode)
{
  QLinearGradient qgradient(lgradient->getX1(), lgradient->getY1(),
                            lgradient->getX2(), lgradient->getY2());

  if      (lgradient->getSpread() == CGRADIENT_SPREAD_PAD)
    qgradient.setSpread(QGradient::PadSpread);
  else if (lgradient->getSpread() == CGRADIENT_SPREAD_REPEAT)
    qgradient.setSpread(QGradient::RepeatSpread);
  else if (lgradient->getSpread() == CGRADIENT_SPREAD_REFLECT)
    qgradient.setSpread(QGradient::ReflectSpread);

  qgradient.setCoordinateMode(mode);

  QGradientStops stops;

  for (CLinearGradient::StopList::const_iterator ps = lgradient->beginStops();
         ps != lgradient->endStops(); ++ps) {
    const CGradientStop &stop = *ps;

    stops.push_back(QGradientStop(stop.getOffset(), CQUtil::rgbaToColor(stop.getColor())));
  }

  qgradient.setStops(stops);

  return qgradient;
}

QRadialGradient
CQUtil::
toQGradient(const CRadialGradient *rgradient, QGradient::CoordinateMode mode)
{
  QRadialGradient qgradient(rgradient->getCenterX(), rgradient->getCenterY(),
                            rgradient->getRadius(),
                            rgradient->getFocusX(), rgradient->getFocusY());

  if      (rgradient->getSpread() == CGRADIENT_SPREAD_PAD)
    qgradient.setSpread(QGradient::PadSpread);
  else if (rgradient->getSpread() == CGRADIENT_SPREAD_REPEAT)
    qgradient.setSpread(QGradient::RepeatSpread);
  else if (rgradient->getSpread() == CGRADIENT_SPREAD_REFLECT)
    qgradient.setSpread(QGradient::ReflectSpread);

  qgradient.setCoordinateMode(mode);

  QGradientStops stops;

  for (CRadialGradient::StopList::const_iterator ps = rgradient->beginStops();
         ps != rgradient->endStops(); ++ps) {
    const CGradientStop &stop = *ps;

    stops.push_back(QGradientStop(stop.getOffset(), CQUtil::rgbaToColor(stop.getColor())));
  }

  qgradient.setStops(stops);

  return qgradient;
}
#endif

void
CQUtil::
getScreenSize(uint *w, uint *h)
{
  QRect r = QApplication::desktop()->screenGeometry();

  *w = r.width();
  *h = r.height();
}

void
CQUtil::
setSelectText(const std::string &text)
{
  QClipboard *clipboard = QApplication::clipboard();

  clipboard->setText(text.c_str(), QClipboard::Selection);
}

#ifdef CQUTIL_POINT
QPoint
CQUtil::
toQPoint(const CIPoint2D &point)
{
  return QPoint(point.x, point.y);
}

CIPoint2D
CQUtil::
fromQPoint(const QPoint &point)
{
  return CIPoint2D(point.x(), point.y());
}

CPoint2D
CQUtil::
fromQPointF(const QPoint &point)
{
  return CPoint2D(point.x(), point.y());
}

QPointF
CQUtil::
toQPoint(const CPoint2D &point)
{
  return QPointF(point.x, point.y);
}

QPoint
CQUtil::
toQPointI(const CPoint2D &point)
{
  return QPoint(point.x, point.y);
}

CPoint2D
CQUtil::
fromQPoint(const QPointF &point)
{
  return CPoint2D(point.x(), point.y());
}
#endif

#ifdef CQUTIL_SIZE
QSize
CQUtil::
toQSize(const CISize2D &size)
{
  return QSize(size.width, size.height);
}

CISize2D
CQUtil::
fromQSize(const QSize &size)
{
  return CISize2D(size.width(), size.height());
}

QSizeF
CQUtil::
toQSize(const CSize2D &size)
{
  return QSizeF(size.width, size.height);
}

CSize2D
CQUtil::
fromQSize(const QSizeF &size)
{
  return CSize2D(size.width(), size.height());
}
#endif

#ifdef CQUTIL_BBOX
QRectF
CQUtil::
toQRect(const CBBox2D &rect)
{
  return QRectF(toQPoint(rect.getLL()), toQPoint(rect.getUR())).normalized();
}

QRect
CQUtil::
toQRectI(const CBBox2D &rect)
{
  return QRect(toQPointI(rect.getLL()), toQPointI(rect.getUR())).normalized();
}

CBBox2D
CQUtil::
fromQRect(const QRectF &rect)
{
  return CBBox2D(fromQPoint(rect.bottomLeft()), fromQPoint(rect.topRight()));
}

QRect
CQUtil::
toQRect(const CIBBox2D &rect)
{
  return QRect(toQPoint(rect.getLL()), toQPoint(rect.getUR())).normalized();
}

CIBBox2D
CQUtil::
fromQRect(const QRect &rect)
{
  return CIBBox2D(fromQPoint(rect.bottomLeft()), fromQPoint(rect.topRight()));
}
#endif

#ifdef CQUTIL_MATRIX
QMatrix
CQUtil::
toQMatrix(const CMatrix2D &m)
{
  double a, b, c, d, tx, ty;

  m.getValues(&a, &b, &c, &d, &tx, &ty);

  //return QMatrix(a, b, c, d, tx, ty);
  return QMatrix(a, c, b, d, tx, ty);
}

CMatrix2D
CQUtil::
fromQMatrix(const QMatrix &m)
{
  return CMatrix2D(m.m11(), m.m12(), m.m21(), m.m22(), m.dx(), m.dy());
}

QTransform
CQUtil::
toQTransform(const CMatrix2D &m)
{
  double a, b, c, d, tx, ty;

  m.getValues(&a, &b, &c, &d, &tx, &ty);

  //return QTransform(a, b, c, d, tx, ty);
  return QTransform(a, c, b, d, tx, ty);
}

CMatrix2D
CQUtil::
fromQTransform(const QTransform &t)
{
  return CMatrix2D(t.m11(), t.m12(), t.m21(), t.m22(), t.dx(), t.dy());
}
#endif

void
CQUtil::
setDockVisible(QDockWidget *dock, bool visible)
{
  QAction *action = dock->toggleViewAction();

  if (visible != ! dock->isHidden())
    action->trigger();
}

#ifdef CQUTIL_LINE_DASH
void
CQUtil::
penSetLineDash(QPen &pen, const CLineDash &dash)
{
  int num = dash.getNumLengths();

  if (num > 0) {
    pen.setStyle(Qt::CustomDashLine);

    pen.setDashOffset(dash.getOffset());

    QVector<qreal> dashes;

    double w = pen.widthF();

    if (w <= 0.0) w = 1.0;

    for (int i = 0; i < num; ++i)
      dashes << dash.getLength(i)*w;

    if (num & 1)
      dashes << dash.getLength(0)*w;

    pen.setDashPattern(dashes);
  }
  else
    pen.setStyle(Qt::SolidLine);
}
#endif

#ifdef CQUTIL_ALIGN
Qt::Alignment
CQUtil::
toQAlign(CHAlignType halign)
{
  switch (halign) {
    case CHALIGN_TYPE_LEFT   : return Qt::AlignLeft;
    case CHALIGN_TYPE_CENTER : return Qt::AlignHCenter;
    case CHALIGN_TYPE_RIGHT  : return Qt::AlignRight;
    case CHALIGN_TYPE_JUSTIFY: return Qt::AlignJustify;
    default                  : return Qt::AlignHCenter;
  }
}

Qt::Alignment
CQUtil::
toQAlign(CVAlignType valign)
{
  switch (valign) {
    case CVALIGN_TYPE_TOP     : return Qt::AlignTop;
    case CVALIGN_TYPE_CENTER  : return Qt::AlignVCenter;
    case CVALIGN_TYPE_BOTTOM  : return Qt::AlignBottom;
    case CVALIGN_TYPE_BASELINE: return Qt::AlignBottom;
    default                   : return Qt::AlignVCenter;
  }
}

CHAlignType
CQUtil::
toHAlign(Qt::Alignment align)
{
  switch (align) {
    case Qt::AlignLeft:    return CHALIGN_TYPE_LEFT;
    case Qt::AlignHCenter: return CHALIGN_TYPE_CENTER;
    case Qt::AlignRight:   return CHALIGN_TYPE_RIGHT;
    case Qt::AlignJustify: return CHALIGN_TYPE_JUSTIFY;
    default:               return CHALIGN_TYPE_CENTER;
  }
}

CVAlignType
CQUtil::
toVAlign(Qt::Alignment align)
{
  switch (align) {
    case Qt::AlignTop:     return CVALIGN_TYPE_TOP;
    case Qt::AlignVCenter: return CVALIGN_TYPE_CENTER;
    case Qt::AlignBottom:  return CVALIGN_TYPE_BOTTOM;
    default:               return CVALIGN_TYPE_CENTER;
  }
}
#endif

//------------

QPixmap
CQUtil::
getPixmap(const QString &name)
{
  // TODO: search path
  QPixmap pm;

  pm.load(name);

  return pm;
}

QIcon
CQUtil::
getIcon(const QString &name)
{
  return QIcon(getPixmap(name));
}

//------------

uint
CQUtil::
nameWidgetTree(QWidget *widget)
{
  assert(widget != nullptr);

  uint num = 0;

  QString name = widget->objectName();

  if (name.isNull() || name.isEmpty()) {
    nameWidget(widget);

    ++num;
  }

  const QObjectList &children = widget->children();

  for (QObjectList::const_iterator po = children.begin(); po != children.end(); ++po) {
    QWidget *widget1 = qobject_cast<QWidget *>(*po);

    if (widget1)
      num += nameWidgetTree(widget1);
  }

  return num;
}

void
CQUtil::
nameWidget(QWidget *widget)
{
  QAbstractButton *button = qobject_cast<QAbstractButton *>(widget);

  if (button)
    nameWidgetButton(button);
  else {
    QLabel *label = qobject_cast<QLabel *>(widget);

    if (label)
      nameWidgetLabel(label);
    else
      nameWidgetGen(widget);
  }
}

void
CQUtil::
nameWidgetButton(QAbstractButton *button)
{
  QString text = button->text();

  if (text.isNull() || text.isEmpty())
    return CQUtil::nameWidgetGen(static_cast<QWidget*>(button));

  text.replace(QChar(' '),QChar('_'));
  text.replace(QChar('&'),QString(""));
  text.replace(QChar('$'),QString(""));

  button->setObjectName(text);
}

void
CQUtil::
nameWidgetLabel(QLabel *label)
{
  QString text = label->text();

  if (text.isNull() || text.isEmpty())
    return CQUtil::nameWidgetGen(static_cast<QWidget*>(label));

  text.replace(QChar(' '),QChar('_'));
  text.replace(QChar('&'),QString(""));
  text.replace(QChar('$'),QString(""));

  label->setObjectName(text);
}

void
CQUtil::
nameWidgetGen(QWidget *widget)
{
  const QMetaObject *mo = widget->metaObject();

  const char *cname = mo->className();

  int num = 1;

  QString name = QString("%1_%2").arg(cname).arg(num);

  while (widget->parent() && widget->parent()->findChild<QWidget *>(name)) {
    ++num;

    name = QString("%1_%2").arg(cname).arg(num);
  }

  widget->setObjectName(name);
}

//------------

void
CQUtil::
recolorImage(QImage &image, const QColor &fg, const QColor &bg)
{
  double fg_r = fg.red  ()/255.0;
  double fg_g = fg.green()/255.0;
  double fg_b = fg.blue ()/255.0;
//double fg_a = fg.alpha()/255.0; needed ?

  double bg_r = bg.red  ()/255.0;
  double bg_g = bg.green()/255.0;
  double bg_b = bg.blue ()/255.0;
//double bg_a = bg.alpha()/255.0; needed ?

  if      (image.format() == QImage::Format_ARGB32) {
    for (int y = 0; y < image.height(); ++y) {
      for (int x = 0; x < image.width(); ++x) {
        QRgb rgb = image.pixel(x, y);

        int fg_gray = 255 - qGray(rgb);
        int bg_gray = 255 - fg_gray;

        rgb = qRgba(int(fg_gray*fg_r + bg_gray*bg_r),
                    int(fg_gray*fg_g + bg_gray*bg_g),
                    int(fg_gray*fg_b + bg_gray*bg_b), qAlpha(rgb));

        image.setPixel(x, y, rgb);
      }
    }
  }
  else if (image.format() == QImage::Format_Indexed8) {
    int ncolors = image.colorCount();

    for (int i = 0; i < ncolors; ++i) {
      QRgb rgb = image.color(i);

      int fg_gray = 255 - qGray(rgb);
      int bg_gray = 255 - fg_gray;

      rgb = qRgba(int(fg_gray*fg_r + bg_gray*bg_r),
                  int(fg_gray*fg_g + bg_gray*bg_g),
                  int(fg_gray*fg_b + bg_gray*bg_b), qAlpha(rgb));

      image.setColor(i, rgb);
    }
  }
  else
    assert(false);
}

//------------

void
CQUtil::
drawHtmlText(QWidget *w, QPainter *painter, const QString &text,
             const QPalette &palette, const QRect &rect, bool active)
{
  painter->setRenderHints(QPainter::Antialiasing);

  QTextDocument td;

  td.setHtml(text);

  QRect trect = rect.translated(-rect.x(), -rect.y());

  painter->translate(rect.x(), rect.y());

  painter->setClipRect(trect);

  QAbstractTextDocumentLayout::PaintContext ctx;

  if (active)
    ctx.palette.setColor(QPalette::Text, palette.highlightedText().color());
  else
    ctx.palette.setColor(QPalette::Text, palette.text().color());

  QAbstractTextDocumentLayout *layout = td.documentLayout();

  layout->setPaintDevice(w);

  layout->draw(painter, ctx);

  painter->translate(-rect.x(), -rect.y());
}

QString
CQUtil::
colorToHtml(const QColor &c)
{
  // TODO: transparency
  QChar pad('0');

  return QString("#%1%2%3").
    arg(c.red(), 2, 16, pad).arg(c.green(), 2, 16, pad).arg(c.blue(), 2, 16, pad);
}

QString
CQUtil::
policyToString(QSizePolicy::Policy policy)
{
  switch (policy) {
    case QSizePolicy::Fixed           : return "Fixed";
    case QSizePolicy::Minimum         : return "Minimum";
    case QSizePolicy::Maximum         : return "Maximum";
    case QSizePolicy::Preferred       : return "Preferred";
    case QSizePolicy::Expanding       : return "Expanding";
    case QSizePolicy::MinimumExpanding: return "MinimumExpanding";
    case QSizePolicy::Ignored         : return "Ignored";
    default                           : return QString("%1").arg(policy);
  }
}

//------------

#ifdef CQUTIL_RGBA
QColor
CQUtil::
blendColors(const QColor &c1, const QColor &c2, double f)
{
  CRGBA rgba1 = colorToRGBA(c1);
  CRGBA rgba2 = colorToRGBA(c2);

  CRGBA rgba = rgba1.blended(rgba2, f);

  return rgbaToColor(rgba);
}
#endif

//------------

void
CQUtil::PropInfo::
init(const QMetaProperty &mp)
{
  name_         = mp.name();
  type_         = mp.type();
  typeName_     = mp.typeName();
  is_writable_  = mp.isWritable();
  is_enum_type_ = mp.isEnumType();

  QMetaEnum me = mp.enumerator();

  int num_enums = me.keyCount();

  enumNames_.clear();

  for (int i = 0; i < num_enums; ++i)
    enumNames_.push_back(me.valueToKey(i));
}
