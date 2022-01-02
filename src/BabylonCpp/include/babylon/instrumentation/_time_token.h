#ifndef BABYLON_INSTRUMENTATION_TIME_TOKEN_H
#define BABYLON_INSTRUMENTATION_TIME_TOKEN_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

namespace GL {
class IGLQuery;
using IGLQueryPtr = std::shared_ptr<IGLQuery>;
} // end of namespace GL

struct BABYLON_SHARED_EXPORT _TimeToken {
  GL::IGLQueryPtr _startTimeQuery   = nullptr;
  GL::IGLQueryPtr _endTimeQuery     = nullptr;
  GL::IGLQueryPtr _timeElapsedQuery = nullptr;
  bool _timeElapsedQueryEnded       = false;
}; // end of struct _TimeToken

} // end of namespace BABYLON

#endif // end of BABYLON_INSTRUMENTATION_TIME_TOKEN_H
