#include <node.h>
#include "bitalino-adapter.h"

// Alternative to specifying the library in the external dependencies field in project settings, as this was not developed in a IDE.
#pragma comment(lib, "ws2_32.lib")

namespace bitalino {

using v8::Local;
using v8::Object;

void InitAll(Local<Object> exports) {
  BITalinoAdapter::Init(exports);
}

NODE_MODULE(NODE_GYP_BITALINO, InitAll)

}