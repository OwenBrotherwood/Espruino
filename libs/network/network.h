/*
 * This file is part of Espruino, a JavaScript interpreter for Microcontrollers
 *
 * Copyright (C) 2013 Gordon Williams <gw@pur3.co.uk>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ----------------------------------------------------------------------------
 * Contains functions for handling JsNetwork and doing common networking tasks
 * ----------------------------------------------------------------------------
 */

#ifndef _NETWORK_H
#define _NETWORK_H

#include "jsutils.h"
#include "jsvar.h"
#include "jshardware.h"

#define NETWORK_VAR_NAME JS_HIDDEN_CHAR_STR"net"

typedef enum {
  NETWORKSTATE_OFFLINE,
  NETWORKSTATE_CONNECTED, // connected but not online (no DHCP)
  NETWORKSTATE_ONLINE, // DHCP (or manual address)
} PACKED_FLAGS JsNetworkState;

extern JsNetworkState networkState; // FIXME put this in JsNetwork

// This is all code for handling multiple types of network access with one binary
typedef enum {
  JSNETWORKTYPE_SOCKET,  ///< Standard linux socket API
  JSNETWORKTYPE_CC3000,  ///< TI CC3000 support
  JSNETWORKTYPE_W5500,  ///< WIZnet W5500 support
  // enc28j60?
} JsNetworkType;

typedef struct {
  JsNetworkType type;
  // Info for accessing specific devices
  //IOEventFlags spi;
  //Pin pinCS, pinIRQ, pinEN;
} PACKED_FLAGS JsNetworkData;

typedef struct JsNetwork {
  JsVar *networkVar; // this won't be locked again - we just know that it is already locked by something else
  JsNetworkData data;
  unsigned char _blank; ///< this is needed as jsvGetString for 'data' wants to add a trailing zero  

  /// Called on idle. Do any checks required for this device
  void (*idle)(struct JsNetwork *net);
  /// Call just before returning to idle loop. This checks for errors and tries to recover. Returns true if no errors.
  bool (*checkError)(struct JsNetwork *net);

  /// if host=0, creates a server otherwise creates a client (and automatically connects). Returns >=0 on success
  int (*createsocket)(struct JsNetwork *net, unsigned long host, unsigned short port);
  /// destroys the given socket
  void (*closesocket)(struct JsNetwork *net, int sckt);
  /// If the given server socket can accept a connection, return it (or return < 0)
  int (*accept)(struct JsNetwork *net, int sckt);
  /// Get an IP address from a name
  void (*gethostbyname)(struct JsNetwork *net, char * hostName, unsigned long* out_ip_addr);
  /// Receive data if possible. returns nBytes on success, 0 on no data, or -1 on failure
  int (*recv)(struct JsNetwork *net, int sckt, void *buf, size_t len);
  /// Send data if possible. returns nBytes on success, 0 on no data, or -1 on failure
  int (*send)(struct JsNetwork *net, int sckt, const void *buf, size_t len);
} PACKED_FLAGS JsNetwork;

// ---------------------------------- these are in network.c
// Get the relevant info for JsNetwork (done from a var in root scope)
void networkCreate(JsNetwork *net, JsNetworkType type); // create the network object (ONLY to be used by network drivers)
bool networkGetFromVar(JsNetwork *net);
bool networkGetFromVarIfOnline(JsNetwork *net); // only return true (and network) if we're online, otherwise warn
void networkSet(JsNetwork *net);
void networkFree(JsNetwork *net);
// ---------------------------------------------------------

/// Use this for getting the hostname, as it parses the name to see if it is an IP address first
void networkGetHostByName(JsNetwork *net, char * hostName, unsigned long* out_ip_addr);

#endif // _NETWORK_H
