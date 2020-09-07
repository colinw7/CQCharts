#ifndef CQChartsTextOptions_H
#define CQChartsTextOptions_H

#include <CQChartsAngle.h>
#include <CQChartsAlpha.h>
#include <CQChartsLength.h>
#include <QString>

/*!
 * \brief Text Options Data
 * \ingroup Charts
 */
struct CQChartsTextOptions {
  enum ValueType {
    ANGLE       = (1<<0),
    CONTRAST    = (1<<1),
    ALIGN       = (1<<2),
    FORMATTED   = (1<<3),
    SCALED      = (1<<4),
    HTML        = (1<<5),
    CLIP_LENGTH = (1<<6),

    NONE   = 0,
    SIMPLE = (ANGLE | CONTRAST | HTML),
    ALL    = (ANGLE | CONTRAST | ALIGN | FORMATTED | SCALED | HTML | CLIP_LENGTH)
  };

  CQChartsAngle  angle;                                                  //!< angle
  bool           contrast         { false };                             //!< contrast
  CQChartsAlpha  contrastAlpha    { 0.5 };                               //!< contrast alpha
  Qt::Alignment  align            { Qt::AlignHCenter|Qt::AlignVCenter }; //!< align
  bool           formatted        { false };                             //!< formatted
  QString        formatSeps;                                             //!< format separators
  bool           scaled           { false };                             //!< scaled
  double         scale            { -1.0 };                              //!< fixed scale
  bool           html             { false };                             //!< html
  CQChartsLength clipLength;                                             //!< clip length
  bool           clipped          { true };                              //!< clipped
  int            margin           { 0 };                                 //!< margin (pixels)
  double         minScaleFontSize { 6.0 };                               //!< min scaled font size
  double         maxScaleFontSize { 48.0 };                              //!< max scaled font size

  CQChartsTextOptions() = default;
};

#endif
