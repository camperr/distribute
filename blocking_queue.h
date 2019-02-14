#include <iostream>
#include <queue>

#include "util.h"

template <class T>
class BlockingQueue{

    public:
        explicit BlockingQueue(size_t size){
            size_ = size;
        }

        void push(const T& t){
            MutexLockGuard lk(mtx_);
            if(size_ >= 0){
                while(true){
                    if(queue_.size() < size_){
                        queue_.push(t);
                        notEmpty.notifyOne();
                        break;
                    }else{
                        notFullCond.wait();
                    }
            }
            }else{
                queue_.push(t);
                notEmptyCond.notifyOne();
            }
        }

        T pop(){
            MutexLockGuard lk(mtx_);
            while(true){
                if(!queue_.empty()){
                    T t = queue_.front();
                    queue_.pop();
                    notFullCond.notifyOne();
                    return t;
                }else{
                    notEmptyCond.wait();
                }
            }

        }


    private:
        size_t size_;
        MutexLock mtx_;
        Condition notFullCond(mtx_);
        Condition notEmptyCond(mtx_);
        std::queue<T> queue_;
};
