// the lock server implementation

#include "lock_server.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "rpc/slock.h"

lock_server::lock_server():
  nacquire (0)
{
  lock_state_map.clear();
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);
}

lock_protocol::status
lock_server::stat(int clt, lock_protocol::lockid_t lid, int &r) {

  lock_protocol::status ret = lock_protocol::OK;
  printf("stat request from clt %d\n", clt);
  r = nacquire;
  return ret;
}

lock_protocol::status 
lock_server::acquire(int clt, lock_protocol::lockid_t lid, lock_protocol::status &r) {

	ScopedLock lk(&mutex);
    
    std::map<lock_protocol::lockid_t, lock_state*>::iterator ite = lock_state_map.find(lid);
    if (ite == lock_state_map.end()) {
    
        lock_state *lock_val = new lock_state(lock_state::LOCKED);
        lock_state_map[lid] = lock_val;
    } else {
    
        while (ite->second->state == lock_state::LOCKED) {
            pthread_cond_wait(&ite->second->lock_cond_, &mutex);
        }
    
        ite->second->state = lock_state::LOCKED;
    }

	return lock_protocol::OK;
}

lock_protocol::status 
lock_server::release(int clt, lock_protocol::lockid_t lid, lock_protocol::status &r) {
	
    lock_protocol::status ret = lock_protocol::OK;
    ScopedLock lk(&mutex);

    std::map<lock_protocol::lockid_t, lock_state*>::iterator ite = lock_state_map.find(lid);
    if (ite != lock_state_map.end()) {
        ite->second->state = lock_state::FREE;
        ret = pthread_cond_broadcast(&ite->second->lock_cond_) ? ret : lock_protocol::RETRY;
    } else {
        ret =  lock_protocol::NOENT;
    }

	return ret;
}
