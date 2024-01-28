#ifndef CQChartsTextOptions_H
#define CQChartsTextOptions_H

#include <CQChartsFont.h>
#include <CQChartsAlpha.h>
#include <CQChartsAngle.h>
#include <QString>

/*!
 * \brief Text Options Data
 * \ingroup Charts
 */
struct CQChartsTextOptions {
  enum ValueType {
    ANGLE          = (1<<0),
    CONTRAST       = (1<<1),
    CONTRAST_ALPHA = (1<<2),
    ALIGN          = (1<<3),
    FORMATTED      = (1<<4),
    SCALED         = (1<<5),
    HTML           = (1<<6),
    CLIP_LENGTH    = (1<<7),
    CLIP_ELIDE     = (1<<8),
    DATA           = (1<<9),

    NONE   = 0,
    SIMPLE = (ANGLE | CONTRAST | CONTRAST_ALPHA | HTML),
    ALL    = (ANGLE | CONTRAST | CONTRAST_ALPHA | ALIGN | FORMATTED |
              SCALED | HTML | CLIP_LENGTH | CLIP_ELIDE)
  };

  QColor            color; //!< color
  CQChartsFont      font;  //!< font
  CQChartsAlpha     alpha; //!< alpha

  CQChartsAngle     angle;                                 //!< angle
  bool              contrast         { false };            //!< contrast
  CQChartsAlpha     contrastAlpha    { 0.5 };              //!< contrast alpha
  Qt::Alignment     align            { Qt::AlignHCenter |
                                       Qt::AlignVCenter }; //!< align
  bool              formatted        { false };            //!< formatted
  QString           formatSeps;                            //!< format separators
  bool              scaled           { false };            //!< scaled
  double            scale            { -1.0 };             //!< fixed scale
  bool              html             { false };            //!< html
  bool              alignHtml        { false };            //!< align html (TODO)
  double            clipLength       { 0.0 };              //!< clip length (pixels)
  Qt::TextElideMode clipElide        { Qt::ElideRight };   //!< clip elide
  bool              clipped          { true };             //!< clipped
  int               margin           { 0 };                //!< margin (pixels)
  double            minScaleFontSize { 6.0 };              //!< min scaled font size
  double            maxScaleFontSize { 48.0 };             //!< max scaled font size

  bool           skipDraw      { false }; //!< skip draw
  mutable double calcFontScale { 1.0 };   //!< calculated scale value

  CQChartsTextOptions() = default;
};

#endif
