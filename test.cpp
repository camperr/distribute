#include <iostream>
#include <pthread.h>
#include "unistd.h"
#include "blocking_queue.h"

using namespace std;

BlockingQueue<std::pair<unsigned long, int> > q(10);
const int count = 100;

void* produce(void* args){
    for(int i=0; i<count; i++){
        cout<<"produce "<<pthread_self()<<": "<<i<<endl;
        q.push(std::make_pair<unsigned long, int>(static_cast<unsigned long>(pthread_self()), i));
        usleep(1000*50);
    }
    cout<<"produce done!"<<endl;
}

void* consume(void* args){
    for(int i=0; i<count; i++){
        std::pair<unsigned long, int> pr = q.pop();
        cout<<"consume "<<pthread_self()<<": "<<i<<" "<<pr.first<<" "<<pr.second<<endl;
        usleep(1000*200);
    }
    cout<<"consume done!"<<endl;
}

int main(){
    pthread_t pt[count];
    pthread_t ct[count];
    int ths = 5;
    for(int i=0; i<ths; i++){
        pthread_create(&ct[i], NULL, consume, NULL);
        pthread_create(&pt[i], NULL, produce, NULL);
    }

    for(int i=0; i<ths; i++){
        pthread_join(pt[i], NULL);
        pthread_join(ct[i], NULL);
    }
}


