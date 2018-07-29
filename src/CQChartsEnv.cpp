#include <CQChartsEnv.h>
#include <cstdlib>
#include <cstring>
#include <cassert>

namespace CQChartsEnv {

bool getBool(const char *name, bool def) {
  char *env = getenv(name);

  if (! env)
    return def;

  if      (strcmp(env, "0") == 0 || strcmp(env, "false") == 0 || strcmp(env, "no" ) == 0)
    return false;
  else if (strcmp(env, "1") == 0 || strcmp(env, "true" ) == 0 || strcmp(env, "yes") == 0)
    return true;

  assert(false);

  return true;
}

}
