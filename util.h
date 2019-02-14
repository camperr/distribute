#include <pthread>
#include <iostream>

namespace util {

    // TODO:需要禁用拷贝构造和赋值构造函数
    
    class MutexLock{

        public:
            MutexLock():holder_(0){
                pthread_mutex_init(&mutex_, NULL);
            }

            ~MutexLock(){
                pthread_mutex_destory(&mutex_);
            }


            bool isLockedByThisThread(){
                return holder_ == CurrentThread::tid();
            }

            pthread_mutex_t* getPthreadMutex(){
                return &mutex_;
            }

            void lock(){
                pthread_mutex_lock(&mutex);
                holder_ = CurrentThread::tid();
            }

            void unlock(){
                holder_=0;
                pthread_mutex_unlock(&mutex);
            }


        private:
            pthread_mutex_t mutex_;
            pid_t holder_;

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
                pthread_cond_destory(&cond);
            }

            void wait(){
                pthread_cond_wait(&cond_, mtx_.getPthreadMutex());
            }

            void notifyOne(){
                pthread_cond_signal(&cond);
            }

            void notifyAll(){
                pthread_cond_broadcast(&cond);
            }

        private:
            MutexLock mtx_;
            pthread_cond_t cond_;

    };

}
