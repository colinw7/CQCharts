#ifndef CQChartsInputData_H
#define CQChartsInputData_H

#include <QVariant>
#include <QString>
#include <vector>

/*!
 * \brief data used when loading a model from a file.
 * \ingroup Charts
 */
struct CQChartsInputData {
  using Values = std::vector<QVariant>;

  bool        commentHeader     { false }; //!< horizontal header comes from first comment line
  bool        firstLineHeader   { false }; //!< horizontal header comes from first line
  bool        firstColumnHeader { false }; //!< vertical header comes from first column
  bool        transpose         { false }; //!< transpose data (rows, columns switched)
  QString     separator;                   //!< value separator characters
  QStringList columns;                     //!< specific input column names/numbers
  int         numRows           { 100 };   //!< number of rows to generate with tcl expression
  int         maxRows           { -1 };    //!< maximum number of rows to read from file
  QString     filter;                      //!< tcl expression filter
  QString     fold;                        //!< fold definition for folded model
  QString     sort;                        //!< arguments for model sort
  Values      vars;                        //!< tcl variables to read data from
};

#endif
