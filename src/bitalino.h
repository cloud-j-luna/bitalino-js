#ifndef _BITALINOHEADER_
#define _BITALINOHEADER_

#include <string>
#include <vector>

#ifdef _WIN32 // 32-bit or 64-bit Windows

#include <winsock2.h>

#endif

typedef std::vector<bool> Vbool; ///< Vector of bools.
typedef std::vector<int> Vint;   ///< Vector of ints.

/// A frame returned by BITalino::read()
struct Frame
{
  /// %Frame sequence number (0...15).
  /// This number is incremented by 1 on each consecutive frame, and it overflows to 0 after 15 (it is a 4-bit number).
  /// This number can be used to detect if frames were dropped while transmitting data.
  char seq;

  /// Array of digital ports states (false for low level or true for high level).
  /// On original %BITalino, the array contents are: I1 I2 I3 I4.
  /// On %BITalino 2, the array contents are: I1 I2 O1 O2.
  bool digital[4];

  /// Array of analog inputs values (0...1023 on the first 4 channels and 0...63 on the remaining channels)
  short analog[6];
};
typedef std::vector<Frame> VFrame; ///< Vector of Frame's.

void send(char cmd);
int recv(void *data, int nbyttoread);

#ifdef _WIN32
   SOCKET	fd;
   timeval  readtimeout;
   HANDLE   hCom;
#else // Linux or Mac OS
   int      fd;
   bool     isTTY;
#endif

#endif // _BITALINOHEADER_