#include "bluetooth.h"
#include <node.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <cstring>
#include <string>

namespace bluetooth {

using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::NewStringType;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;

struct sockaddr_rc addr = { 0 };
int s, status;

void Connect(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();

    // Check the number of arguments passed.
    if (args.Length() < 1) {
        // Throw an Error that is passed back to JavaScript
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate,
                                "Wrong number of arguments",
                                NewStringType::kNormal).ToLocalChecked()));
        return;
    }

    // Check the argument types
    if (!args[0]->IsString()) {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate,
                                "Wrong arguments",
                                NewStringType::kNormal).ToLocalChecked()));
        return;
    }

    v8::String::Utf8Value address(args[0]->ToString());

    char dest[18] = {};
    strcpy(dest, std::string(*address).c_str());   

    //printf("%s\n", *address);

    // allocate a socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // set the connection parameters (who to connect to)
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 1;
    str2ba( dest, &addr.rc_bdaddr );

    // connect to server
    status = connect(s, (struct sockaddr *)&addr, sizeof(addr));

    args.GetReturnValue().Set(1);
}

void Send(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();

    // Check the number of arguments passed.
    if (args.Length() < 1) {
        // Throw an Error that is passed back to JavaScript
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate,
                                "Wrong number of arguments",
                                NewStringType::kNormal).ToLocalChecked()));
        return;
    }

    // Check the argument types
    if (!args[0]->IsNumber()) {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate,
                                "Wrong arguments",
                                NewStringType::kNormal).ToLocalChecked()));
        return;
    }

    int buffer[1] = {args[0].As<Number>()->Value()};

    // send a message
    if( status == 0 ) {
        status = write(s, buffer, 6);
    }

    if( status < 0 ) perror("Error: ");

    // Set the return value (using the passed in
    // FunctionCallbackInfo<Value>&)
    args.GetReturnValue().Set(1);
}

void Recv(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();

    // Check the number of arguments passed.
    if (args.Length() < 1) {
        // Throw an Error that is passed back to JavaScript
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate,
                                "Wrong number of arguments",
                                NewStringType::kNormal).ToLocalChecked()));
        return;
    }

    // Check the argument types
    if (!args[0]->IsNumber()) {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate,
                                "Wrong arguments",
                                NewStringType::kNormal).ToLocalChecked()));
        return;
    }

    int total_bytes_read = 0, cursor = 0;
    int bytes_to_read = args[0].As<Number>()->Value();
    char buf[1024] = { 0 };

    memset(buf, 0, sizeof(buf));

    // read data from the client
    while(total_bytes_read < bytes_to_read) {
        char tmp[32] = { 0 };
        memset(tmp, 0, sizeof(tmp));

        int bytes_read = read(s, tmp, sizeof(tmp));
        total_bytes_read += bytes_read;
        for(int i = 0; i < total_bytes_read && total_bytes_read - bytes_read + i < bytes_to_read; i++) {
            buf[cursor + i] = tmp[i];
        }
        //printf("received [%s] (%d)\n", tmp, bytes_read);
        cursor += total_bytes_read;
    }

    //printf("Result: %s (bytes read %d)\n", buf, total_bytes_read);

    Local<String> res = String::NewFromUtf8(isolate, buf, NewStringType::kNormal).ToLocalChecked();

    // Set the return value (using the passed in
    // FunctionCallbackInfo<Value>&)
    args.GetReturnValue().Set(res);
}

void Init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "send", Send);
  NODE_SET_METHOD(exports, "recv", Recv);
  NODE_SET_METHOD(exports, "connect", Connect);
}

NODE_MODULE(NODE_GYP_BLUETOOTH, Init)

}  // namespace demo