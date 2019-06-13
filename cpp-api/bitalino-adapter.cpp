#include "bitalino-adapter.h"

using v8::Context;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Local;
using v8::NewStringType;
using v8::Number;
using v8::Boolean;
using v8::Object;
using v8::Array;
using v8::Persistent;
using v8::String;
using v8::Value;
using v8::Exception;
using v8::Handle;

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
  NODE_SET_PROTOTYPE_METHOD(tpl, "start", Start);
  NODE_SET_PROTOTYPE_METHOD(tpl, "stop", Stop);
  NODE_SET_PROTOTYPE_METHOD(tpl, "read", Read);


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

void BITalinoAdapter::Start(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  BITalinoAdapter* obj = ObjectWrap::Unwrap<BITalinoAdapter>(args.Holder());

  if (args.Length() < 3) {
      // Throw an Error that is passed back to JavaScript
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate,
                              "Wrong number of arguments",
                              NewStringType::kNormal).ToLocalChecked()));
      return;
  }

  if (!args[0]->IsNumber() || !args[1]->IsArray() || !args[2]->IsBoolean()) {
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate,
                              "Wrong arguments",
                              NewStringType::kNormal).ToLocalChecked()));
      return;
  }

  int samplingRate = args[0].As<Number>()->Value();
  BITalino::Vint channels = v8ArrayToVint(Handle<Array>::Cast(args[1].As<Array>()));
  bool simulated = args[2].As<Boolean>()->Value();

  obj->adaptee.start(samplingRate, channels, simulated);

  args.GetReturnValue().SetUndefined();
}

void BITalinoAdapter::Stop(const FunctionCallbackInfo<Value>& args) {
  // Isolate* isolate = args.GetIsolate();

  BITalinoAdapter* obj = ObjectWrap::Unwrap<BITalinoAdapter>(args.Holder());

  obj->adaptee.stop();
  
  args.GetReturnValue().SetUndefined();
}

void BITalinoAdapter::Read(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  BITalinoAdapter* obj = ObjectWrap::Unwrap<BITalinoAdapter>(args.Holder());

  if (args.Length() < 1) {
      // Throw an Error that is passed back to JavaScript
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate,
                              "Wrong number of arguments",
                              NewStringType::kNormal).ToLocalChecked()));
      return;
  }

  if (!args[0]->IsNumber()) {
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate,
                              "Wrong arguments",
                              NewStringType::kNormal).ToLocalChecked()));
      return;
  }
  int numberOfFrames = args[0].As<Number>()->Value();
  BITalino::VFrame frames(numberOfFrames);

  int framesRead = obj->adaptee.read(frames);
  
  if(framesRead < numberOfFrames) {
    isolate->ThrowException(Exception::Error(
          String::NewFromUtf8(isolate,
                              "Device timeout",
                              NewStringType::kNormal).ToLocalChecked()));
    return;
  }

  Local<Array> framesArray = Array::New(isolate, numberOfFrames);
    for(int i = 0; i < numberOfFrames; i++) {
        Local<Object> frame = Object::New(isolate);
        frame->Set(
            String::NewFromUtf8(isolate, "seq", NewStringType::kNormal).ToLocalChecked(),
            Number::New(isolate, frames[i].seq)
          );

        Local<Array> digitalArray = Array::New(isolate, 4);
        for(int a = 0; a < 4; a++) {
            digitalArray->Set(a, Number::New(isolate, frames[i].digital[a]));
        }
        frame->Set(
            String::NewFromUtf8(isolate, "digital", NewStringType::kNormal).ToLocalChecked(),
            digitalArray);

        Local<Array> analogArray = Array::New(isolate, 6);
        for(int a = 0; a < 6; a++) {
            analogArray->Set(a, Number::New(isolate, frames[i].analog[a]));
        }
        frame->Set(
            String::NewFromUtf8(isolate, "analog", NewStringType::kNormal).ToLocalChecked(),
            analogArray);

        framesArray->Set(i, frame);
    }
  
  args.GetReturnValue().Set(framesArray);
}

BITalino::Vint v8ArrayToVint(Handle<Array> array) {
  return {1, 2, 3, 4, 5, 6};  // Stub.
}