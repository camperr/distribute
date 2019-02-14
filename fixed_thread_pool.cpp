#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>
#include <iostream>
#include <random>
#include <functional>
#include <unistd.h>

class FixedThreadPool{

    public:
        explicit FixedThreadPool(size_t thread_count)
            :data(std::make_shared<Data>()){
            for(int i=0; i<thread_count; i++){
                std::thread([=]{
                    std::unique_lock<std::mutex> lk(data->mtx);
                    while(true){
                        if(!data->tasks.empty()){
                            auto current = data->tasks.front();
                            data->tasks.pop();
                            lk.unlock();
                            current();
                            lk.lock();
                        }else if(data->is_shutdown){
                            break;
                        }else{
                            data->cond.wait(lk);
                        }
                    }
                }).detach();
            }
        }

        FixedThreadPool() = default;
        FixedThreadPool(FixedThreadPool&&) = default;
        ~FixedThreadPool(){
            if(data){
                {
                    std::lock_guard<std::mutex> guard(data->mtx);
                    data->is_shutdown = true;
                }

                data->cond.notify_all();
            }
        }

        template <class F>
        void execute(F&& func){
            if(data){
                {
                    std::lock_guard<std::mutex> guard(data->mtx);
                    data->tasks.emplace(std::forward<F>(func));
                }
                data->cond.notify_one();
            }
        }
            
        void wait_done(){
            if(data){
                while(!data->tasks.empty()){
                    sleep(1);
                    std::cout<<"current task num:"<<data->tasks.size()<<std::endl;
                }
            }
        }
    
    private:
        struct Data{
            std::mutex mtx;
            std::condition_variable cond;
            bool is_shutdown=false;
            std::queue<std::function<void()>> tasks;
        };

        std::shared_ptr<Data> data;

};

std::default_random_engine e(42);
void run(){
    int r = e();
    int X=1019;
    int res = 1;
    for(int i=1; i<r; i++){
            res = (res+i)%X;
    }
    std::cout<<r<<" "<<res<<std::endl;
}

int main(){
    FixedThreadPool pool(4);
    for(int i=0; i<20; i++) {
        for(int j=0; j<5; j++) pool.execute<void()>(run);
        sleep(1);
    }
    pool.wait_done();
}
