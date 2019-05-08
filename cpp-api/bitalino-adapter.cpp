#include "bitalino-adapter.h"

using v8::Context;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Local;
using v8::NewStringType;
using v8::Number;
using v8::Object;
using v8::Persistent;
using v8::String;
using v8::Value;

Persistent<Function> BITalinoAdapter::constructor;

BITalinoAdapter::BITalinoAdapter(const char* address) : adaptee(address) {}

BITalinoAdapter::~BITalinoAdapter() {}

void BITalinoAdapter::Init(Local<Object> exports) {
  Isolate* isolate = exports->GetIsolate();

  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(
      isolate, "BITalinoAdapter", NewStringType::kNormal).ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  NODE_SET_PROTOTYPE_METHOD(tpl, "version", Version);

  Local<Context> context = isolate->GetCurrentContext();
  constructor.Reset(isolate, tpl->GetFunction(context).ToLocalChecked());
  exports->Set(context, String::NewFromUtf8(
      isolate, "BITalinoAdapter", NewStringType::kNormal).ToLocalChecked(),
               tpl->GetFunction(context).ToLocalChecked()).FromJust();
}

void BITalinoAdapter::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();

  if (args.IsConstructCall()) {
    String::Utf8Value unconvertedAddress(args[0]);
    // Invoked as constructor: `new BITalinoAdapter(...)`
    const char* address = args[0]->IsUndefined() ?
        "" : *unconvertedAddress;
    BITalinoAdapter* obj = new BITalinoAdapter(address);
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    // Invoked as plain function `BITalinoAdapter(...)`, turn into construct call.
    const int argc = 1;
    Local<Value> argv[argc] = { args[0] };
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    Local<Object> result =
        cons->NewInstance(context, argc, argv).ToLocalChecked();
    args.GetReturnValue().Set(result);
  }
}

void BITalinoAdapter::Version(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  BITalinoAdapter* obj = ObjectWrap::Unwrap<BITalinoAdapter>(args.Holder());

  args.GetReturnValue().Set(String::NewFromUtf8(
    isolate,
    obj->adaptee.version().c_str(),
    NewStringType::kNormal).ToLocalChecked()
  );
}