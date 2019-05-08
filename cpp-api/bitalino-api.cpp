#include <node.h>
#include "bitalino-adapter.h"

namespace bitalino {

using v8::Local;
using v8::Object;

void InitAll(Local<Object> exports) {
  BITalinoAdapter::Init(exports);
}

NODE_MODULE(NODE_GYP_BITALINO, InitAll)

}