#ifndef ADAPTER_H
#define ADAPTER_H

#include <node.h>
#include <node_object_wrap.h>
#include <cstring>
#include "lib/bitalino.h"

BITalino::Vint v8ArrayToVint(v8::Handle<v8::Array> array);

class BITalinoAdapter : public node::ObjectWrap {
 public:
  static void Init(v8::Local<v8::Object> exports);

 private:
  explicit BITalinoAdapter(const char* address);
  ~BITalinoAdapter();

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Version(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Start(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Stop(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Read(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Battery(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Trigger(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Pwm(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void State(const v8::FunctionCallbackInfo<v8::Value>& args);
  static v8::Persistent<v8::Function> constructor;
  BITalino adaptee;
};

#endif