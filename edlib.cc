#include <node_api.h>
#include "edlib.h"
#include <stdio.h>
#include <stdlib.h>

static napi_status get_utf8_string(napi_env env, napi_value str,
                                   char** out_buff, size_t* buffsize);

namespace edlib {

napi_value Method(napi_env env, napi_callback_info args) {
  napi_value editDistance;
  napi_status status;

  size_t argc = 2;
  napi_value argv[2];
  status = napi_get_cb_info(env, args, &argc, argv, NULL, NULL);

  size_t queryLength;
  char* query;
  size_t targetLength;
  char* target;

  status = get_utf8_string(env, argv[0], &query, &queryLength);
  if (status != napi_ok) {
    // const napi_extended_error_info* result;
    // napi_get_last_error_info(env, &result);
    // printf("%s\n", result->error_message);

    return nullptr;
  }

  status = get_utf8_string(env, argv[1], &target, &targetLength);
  if (status != napi_ok) return nullptr;

  EdlibAlignResult result = edlibAlign(query, queryLength, target, targetLength,
                                       edlibDefaultAlignConfig());
  int resultStatus = result.status;
  int resultEditDistance = result.editDistance;
  edlibFreeAlignResult(result);
  free(query);
  free(target);

  if (resultStatus != EDLIB_STATUS_OK) {
    return nullptr;
  }

  status = napi_create_int32(env, resultEditDistance, &editDistance);
  if (status != napi_ok) return nullptr;

  return editDistance;
}

napi_value init(napi_env env, napi_value exports) {
  napi_status status;
  napi_value fn;

  status = napi_create_function(env, nullptr, 0, Method, nullptr, &fn);
  if (status != napi_ok) return nullptr;

  status = napi_set_named_property(env, exports, "edit_distance", fn);
  if (status != napi_ok) return nullptr;
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init)

}  // namespace edlib

// h/t: https://blog.sqreen.com/building-a-native-add-on-for-node-js-in-2019/
static napi_status get_utf8_string(napi_env env, napi_value str,
                                   char** out_buff, size_t* buffsize) {
  size_t len;
  size_t len2;
  char* buff;
  napi_status res;

  if (out_buff == NULL) {
    return napi_invalid_arg;
  }

  res = napi_get_value_string_utf8(env, str, NULL, 0, &len);
  if (res != napi_ok) {
    return res;
  }

  /* +1 since napi_get_value_string_utf8 doesn't include NULL terminator
   * c.f. https://nodejs.org/api/n-api.html#n_api_napi_get_value_string_utf8
   */
  buff = (char*)malloc(len + 1);
  if (!buff) {
    return napi_generic_failure;
  }

  res = napi_get_value_string_utf8(env, str, buff, len + 1, &len2);
  if (res != napi_ok) {
    free(buff);
    return napi_generic_failure;
  }

  *out_buff = buff;

  if (buffsize) {
    *buffsize = len;
  }
  return napi_ok;
}
