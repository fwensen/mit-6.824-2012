#ifndef _LOCK_STATE_H
#define _LOCK_STATE_H
#include <pthread.h>

class lock_state {

public:
    enum status {FREE, LOCKED};
    int state;
    
    lock_state(int st) {
        state = st; 
        pthread_cond_init(&lock_cond_, NULL);
    }

    pthread_cond_t lock_cond_;

};

#endif // _LOCK_STATE_H
