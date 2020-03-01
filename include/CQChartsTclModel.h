#ifndef CQChartsTclModel_H
#define CQChartsTclModel_H

#include <CQDataModel.h>

/*!
 * \brief Wrapper class for CQDataModel to remember tcl variable data
 * \ingroup Charts
 */
class CQChartsTclModel : public CQDataModel {
 public:
  CQChartsTclModel(int numCols, int numRows);
};

#endif
