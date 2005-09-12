#ifndef __MOSDPGPEERACK_H
#define __MOSDPGPEERACK_H

#include "msg/Message.h"
#include "osd/OSD.h"

class MOSDPGPeerAck : public Message {
  __uint64_t       map_version;

 public:
  list<pg_t>                pg_dne;   // pg dne
  map<pg_t, PGReplicaInfo > pg_state; // state, lists, etc.

  __uint64_t get_version() { return map_version; }

  MOSDPGPeerAck() {}
  MOSDPGPeerAck(__uint64_t v) :
	Message(MSG_OSD_PG_PEERACK) {
	this->map_version = v;
  }
  
  char *get_type_name() { return "PGPeer"; }

  void encode_payload() {
	payload.append((char*)&map_version, sizeof(map_version));
	_encode(pg_dne, payload);
	
	int n = pg_state.size();
	payload.append((char*)&n, sizeof(n));
	for (map<pg_t, PGReplicaInfo >::iterator it = pg_state.begin();
		 it != pg_state.end();
		 it++) {
	  payload.append((char*)&it->first, sizeof(it->first));
	  it->second._encode(payload);
	}
  }
  void decode_payload() {
	int off = 0;
	payload.copy(off, sizeof(map_version), (char*)&map_version);
	off += sizeof(map_version);
	_decode(pg_dne, payload, off);

	int n;
	payload.copy(off, sizeof(n), (char*)&n);
	off += sizeof(n);
	for (int i=0; i<n; i++) {
	  pg_t pgid;
	  payload.copy(off, sizeof(pgid), (char*)&pgid);
	  off += sizeof(pgid);
	  pg_state[pgid]._decode(payload, off);
	}
  }
};

#endif
