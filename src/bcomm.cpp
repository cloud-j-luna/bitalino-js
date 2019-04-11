/**
 * \author      Joao Luna
 * \date        April 2019
 * 
 * 
 * 
 */

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
using v8::Array;

struct sockaddr_rc addr = { 0 };
int s, status;
static const unsigned char CRC4tab[16] = {0, 3, 6, 5, 12, 15, 10, 9, 11, 8, 13, 14, 7, 4, 1, 2};

static bool checkCRC4(const unsigned char *data, int len)
{
   unsigned char crc = 0;

   for (int i = 0; i < len-1; i++)
   {
      const unsigned char b = data[i];
      crc = CRC4tab[crc] ^ (b >> 4);
      crc = CRC4tab[crc] ^ (b & 0x0F);
   }

   // CRC for last byte
   crc = CRC4tab[crc] ^ (data[len-1] >> 4);
   crc = CRC4tab[crc];

   return (crc == (data[len-1] & 0x0F));
}

int recv(char *buffer, int bytesToRead) {
    printf("reading %d bytes\n", bytesToRead);
    //printf("size of buffer: %d bytes\n", sizeof(buffer));

    for(int n = 0; n < bytesToRead;) {
        int bytesRead = recv(s, (char *) buffer + n, bytesToRead - n, MSG_WAITALL);  // Guarantee number of bytes.
        printf("received [%d] (%d) iteration %d\n", *buffer, bytesRead, n);
        n += bytesRead;
    }

    return bytesToRead;
}

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
    if(s < 0) {
        isolate->ThrowException(Exception::Error(
            String::NewFromUtf8(isolate,
                                "Error creating socket",
                                NewStringType::kNormal).ToLocalChecked()));
        return;
    }

    // set the connection parameters (who to connect to)
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 1;
    str2ba( dest, &addr.rc_bdaddr );

    // connect to server
    status = connect(s, (struct sockaddr *)&addr, sizeof(addr));
    //printf("status: %d\n", status);

    args.GetReturnValue().Set(1);
}

void Send(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();

    usleep(150000);

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

    //printf("Sending value: %d\n", buffer[0]);

    // send a message
    int bytesSent = send(s, buffer, 1, 0);
    printf("Sent %d, status code: %d\n", *buffer, bytesSent);

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

    char *buf = (char *) malloc(1);
    int n = recv(buf, 1);

    //printf("Result: %c (bytes read %d)\n", *buf, n);

    Local<Array> res = Array::New(isolate, 1);
    for(int i = 0; i < 1; i++) {
        res->Set(i, Number::New(isolate, *buf));
    }

/*
    Local<Object> res = Object::New(isolate);
    res->Set(
        String::NewFromUtf8(isolate, "seq", NewStringType::kNormal).ToLocalChecked(),
        Number::New(isolate, buf[6] & 0xF0));
        */

    // Set the return value (using the passed in
    // FunctionCallbackInfo<Value>&)
    args.GetReturnValue().Set(res);
}


/**
 * Read frames from the bitalino
 */
void ReadFrame(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();

    // Check the number of arguments passed.
    if (args.Length() < 2) {
        // Throw an Error that is passed back to JavaScript
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate,
                                "Wrong number of arguments",
                                NewStringType::kNormal).ToLocalChecked()));
        return;
    }

    // Check the argument types
    if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate,
                                "Wrong arguments",
                                NewStringType::kNormal).ToLocalChecked()));
        return;
    }

    int nChannels = args[0].As<Number>()->Value();
    int nFrames = args[1].As<Number>()->Value();
    int nBytes = nChannels + 2 + ((nChannels >= 3 && nChannels <= 5) ? 1 : 0);
    unsigned char buffer[8];

    //printf("number of bytes: %d\n", nBytes);

    for(int i=0; i < nFrames; i++) {    // For each frame wanted.
        recv((char *) buffer, nBytes);

        while(!checkCRC4(buffer, nBytes)) {
            memmove(buffer, buffer+1, nBytes-1);
            if(recv(s, (char *) buffer + nBytes - 1, 1, MSG_WAITALL) != 1) return; // A timeout ocurred.
            printf("crc4 failed, reading new byte %d\n", buffer[nBytes-1]);
        }
    }

    //printf("Result: %s (bytes read %d)\n", buf, total_bytes_read);

    Local<Object> res = Object::New(isolate);
    res->Set(
        String::NewFromUtf8(isolate, "seq", NewStringType::kNormal).ToLocalChecked(),
        Number::New(isolate, buffer[nBytes - 1] >> 4));

    Local<Array> digitalArray = Array::New(isolate, 4);
    for(int i = 0; i < 4; i++) {
        digitalArray->Set(i, Number::New(isolate, ((buffer[nBytes-2] & (0x80 >> i)) != 0) ? 1 : 0));
    }
    res->Set(
            String::NewFromUtf8(isolate, "digital", NewStringType::kNormal).ToLocalChecked(),
            digitalArray);
    // Set the return value (using the passed in
    // FunctionCallbackInfo<Value>&)
    args.GetReturnValue().Set(res);
}

void Close(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    
    close(s);

    args.GetReturnValue().Set(1);
}

void Init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "send", Send);
  NODE_SET_METHOD(exports, "recv", Recv);
  NODE_SET_METHOD(exports, "connect", Connect);
  NODE_SET_METHOD(exports, "readFrame", ReadFrame);
  NODE_SET_METHOD(exports, "close", Close);
}

NODE_MODULE(NODE_GYP_BCOMM, Init)

}  // namespace demo