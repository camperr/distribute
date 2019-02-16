#include <iostream>
#include <queue>

#include "multithread.h"
#include "unistd.h"

template <class T>

class BlockingQueue{

    public:
        explicit BlockingQueue(size_t size){
            notFullCond = new multithread::Condition(mtx_);
            notEmptyCond = new multithread::Condition(mtx_);
            size_ = size;
        }

        void push(const T& t){
            //sleep(1);
            {
                multithread::MutexLockGuard lk(mtx_);
                if(size_ >= 0){
                    while(true){
                        if(queue_.size() < size_){
                            queue_.push(t);
                            break;
                        }else{
                            std::cout<<"push wait"<<std::endl;
                            notFullCond->wait();
                        }
                    }
                }else{
                    queue_.push(t);
                }
            }
            notEmptyCond->notifyOne();

        }

        T front(){
            multithread::MutexLockGuard lk(mtx_);
            while(true){
                if(!queue_.empty()){
                    T t = queue_.front();
                    return t;
                }else{
                    notEmptyCond->wait();
                }
            }
        }

        T pop(){
            T t;
            {
                multithread::MutexLockGuard lk(mtx_);
                while(true){
                    if(!queue_.empty()){
                        t = queue_.front();
                        queue_.pop();
                        break;
                    }else{
                        std::cout<<"pop wait"<<std::endl;
                        notEmptyCond->wait();
                    }
                }
            }
            notFullCond->notifyOne();
            return t;
        }


    private:
        size_t size_;
        multithread::MutexLock mtx_;
        multithread::Condition *notFullCond;
        multithread::Condition *notEmptyCond;
        std::queue<T> queue_;
};
