#ifndef CQChartsGroupData_H
#define CQChartsGroupData_H

#include <CQChartsColumn.h>
#include <CQBucketer.h>

// if no columns, path, row then no group (all in same group)

struct CQChartsGroupData {
  CQChartsColumns      columns;               //!< columns for header grouping
  CQChartsColumn       column;                //!< use group column value for id
  bool                 exactValue  { true };  //!< use exact value for group id
  bool                 rowGrouping { false }; //!< group using row values
  bool                 usePath     { true };  //!< use hierarchical path
  bool                 useRow      { false }; //!< use row number as group id
  CQBucketer           bucketer;              //!< bucket data
  bool                 hier        { false }; //!< support hierarchical names
};

#endif
