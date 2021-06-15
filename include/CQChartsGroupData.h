#ifndef CQChartsGroupData_H
#define CQChartsGroupData_H

#include <CQChartsColumn.h>
#include <CQChartsReals.h>
#include <CQBucketer.h>

/*!
 * \brief group data
 *
 * Note: if no columns, path, row then no group (all in same group)
 */
struct CQChartsGroupData {
  CQChartsColumns columns;               //!< columns for header grouping
  CQChartsColumn  column;                //!< use group column value for id
  bool            exactValue  { false }; //!< force use exact value for group id
  bool            rowGrouping { false }; //!< group using row values
  bool            usePath     { true };  //!< use hierarchical path
  bool            useRow      { false }; //!< use row number as group id
  CQBucketer      bucketer;              //!< bucket data
  CQChartsReals   bucketStops;           //!< bucket stops (cache)
  bool            hier        { false }; //!< support hierarchical names
};

#endif
