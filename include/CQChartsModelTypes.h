#ifndef CQChartsModelTypes_H
#define CQChartsModelTypes_H

#include <CQBaseModelTypes.h>
#include <CQChartsNameValues.h>

struct CQChartsModelTypeData {
  CQBaseModelType    type             { CQBaseModelType::NONE };
  CQBaseModelType    baseType         { CQBaseModelType::NONE };
  CQChartsNameValues nameValues;
  CQBaseModelType    headerType       { CQBaseModelType::STRING };
  CQChartsNameValues headerNameValues;
};

#endif
