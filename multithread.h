#ifndef _UTILS_H
#define _UTILS_H

#include <iostream>
#include <pthread.h>

namespace multithread {

    // TODO:需要禁用拷贝构造和赋值构造函数
    
    class MutexLock{

        public:
            MutexLock():holder_(0){
                pthread_mutex_init(&mutex_, NULL);
            }

            ~MutexLock(){
                pthread_mutex_destroy(&mutex_);
            }


            bool isLockedByThisThread(){
                return holder_ == pthread_self();
            }

            pthread_mutex_t* getPthreadMutex(){
                return &mutex_;
            }

            void lock(){
                pthread_mutex_lock(&mutex_);
                holder_ = pthread_self();
            }

            void unlock(){
                holder_=0;
                pthread_mutex_unlock(&mutex_);
            }


        private:
            pthread_mutex_t mutex_;
            pthread_t holder_;

    };


    class MutexLockGuard{

        public:
            MutexLockGuard(MutexLock& mtx){
                mtx_ = mtx;
                mtx_.lock();
            }

            ~MutexLockGuard(){
                mtx_.unlock();
            }

        private:
            MutexLock mtx_;


    };

    class Condition{

        public:
            explicit Condition(MutexLock& mtx){
                mtx_ = mtx;
                pthread_cond_init(&cond_, NULL);
            }

            ~Condition(){
                pthread_cond_destroy(&cond_);
            }

            void wait(){
                pthread_cond_wait(&cond_, mtx_.getPthreadMutex());
            }

            void notifyOne(){
                pthread_cond_signal(&cond_);
            }

            void notifyAll(){
                pthread_cond_broadcast(&cond_);
            }

        private:
            MutexLock mtx_;
            pthread_cond_t cond_;

    };

}
#endif
