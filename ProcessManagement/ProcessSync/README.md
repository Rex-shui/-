#### 进程同步

* 软件实现临界资源互斥
* 硬件实现临界资源互斥
* 信号量



#### 软件方法实现临界资源互斥

---

* Peterson's Algorithm（两个进程间）

  ```C
  //共享变量
  bool flag[2];
  int turn;
  ```

  ```C
  //进程Pi
  flag[i] = true;
  turn = j;
  while(flag[j] && turn == j);
  //进入临界区
  flag[i] = false;
  ```

  ```C
  //进程Pj
  flag[j] = true;
  turn = i;
  while(flag[i] && trun == i);
  //进入临界区
  flag[j] = false;
  ```



#### 硬件方法实现临界资源互斥

---

* 中断屏蔽方法

* 硬件指令TestAndSet（原子操作）

  ```C
  bool TestAndSet(bool *lock) {
    bool old;
    old = *lock;
    *lock = true;
    return old;
  }
  ```

  ```C
  //进入区
  while(TestAndSet(&lock));
  //临界区
  lock = false;
  ```

* 硬件指令Swap

  ```C
  int CompareAndSwap(int *value, int expected, int new_value) {
    int temp = *value;
    
    if(*value == expected) *value = new_value;
    
    return temp;
  }
  ```

  ```C
  while( CompareAndSwap(&lock, 0, 1) != 0 );
  //临界区
  lock = 0;
  ```





#### 信号量

---

* 整型信号量

  ```
  wait(S) {
  	while(S <= 0);
  	S = S - 1;
  }
  
  signal(S) {
  	S = S + 1;
  }
  ```

* 记录型信号量

  ```C
  typedef struct {
    int value;
    struct process *L;
  } semaphore;
  ```

  ```C
  void wait(semaphore S) {
    S.value--;
    if(S.value < 0) {
      add this process to S.L;
      block(S.L);
    }
  }
  
  void signal(semaphore S) {
    S.value++;
    if(S.value <= 0) {
      remove a process P from S.L;
      wakeup(P);
    }
  }
  ```



#### 信号量应用

* 信号量实现进程互斥

* 信号量实现前驱关系

  ```mermaid
  graph LR
  S1 --> S2
  S1 --> S3
  
  S2 --> S4
  S2 --> S5
  
  S3 --> S6
  S4 --> S6
  S5 --> S6
  ```

  ```C
  semaphore a1=a2=b1=b2=c=d=e=0;
  
  S1() {
    ...;
    V(a1);V(a2);	//S1完成
  }
  
  S2() {
    P(a1);				//等待S1完成
    ...;
    V(b1);V(b2);	//S2完成
  }
  
  S3() {
    P(a2);				//等待S1完成
    ...;
    V(c);					//S3完成
  }
  
  S4() {
    P(b1);
    ...;
    V(d);
  }
  
  S5() {
    P(b2);
    ...;
    V(e);
  }
  
  S6() {
    P(c);P(d);P(e);
    ...;
  }
  ```





#### 经典同步问题

* 生产者-消费者问题

  ```shell
  问题描述：
  1、一组生产者进程和一组消费者进程共享一个初始为空、大小为n的缓冲区
  2、缓冲区不满时生产者可放入消息，不空时消费者可取出消息
  3、一次只允许一个生产者放入消息或一个消费者取出消息
  ```

  ```C
  semaphore mutex = 1;
  semaphore empty = n;
  semaphore full = 0;
  
  producer() {
    while(1) {
      produce;
      P(empty);
      P(mutex);
      add to buffer;
      V(mutex);
      V(full);
    }
  }
  
  consumer() {
    while(1);
    P(full);
    P(mutex);
    remove from buffer;
    V(mutex);
    V(empty);
    consume;
  }
  ```

* 生产者-消费者问题plus

  ```mermaid
  graph TD
  A[爸爸放苹果] --> C[盘子]
  B[妈妈放橘子] --> C
  C --> D[女儿吃苹果]
  C --> E[儿子吃橘子]
  ```

  ```C
  semaphore plate=1, apple=0, orange=0;
  
  dad() {
    P(plate);
    add apple;
    V(apple);
  }
  
  mam() {
    P(plate);
    add orange;
    V(orange);
  }
  
  dau() {
    P(apple);
    get apple;
    V(plate);
  }
  
  son() {
    P(orange);
    get orange;
    V(plate);
  }
  ```

* 读者-写者问题

  ```shell
  问题描述：
  1、允许多个读者同时对文件执行读操作
  2、只允许一个写者往文件中写信息
  3、任一写者完成写操作之前不允许其他读者或写者工作
  4、写者执行写操作前，应让已有的写者和读者全部退出
  ```

  ```C
  int count = 0;				//当前读者数量
  semaphore mutex = 1;	//更新count
  semaphore rw = 1;
  
  writer() {
    while(1) {
      P(rw);
      writing;
      V(rw);
    }
  }
  
  reader() {
    while(1) {
      P(mutex);
      if(count == 0) P(rw);
      count++;
      V(mutex);
      
      reading;
      
      P(mutex);
      count--;							//读结束计数减1
      if(count == 0) V(rw);
      V(mutex);
    }
  }
  
  //会导致写进程饿死
  ```

  ```C
  int count = 0;
  semaphore mutex = 1;
  semaphore rw = 1;
  semaphore w = 1;		//相对写优先
  
  writer() {
    while(1) {
      P(w);
      P(rw);
      writing;
      V(rw);
      V(w);
    }
  }
  
  reader() {
    while(1) {
      P(w);
      P(mutex);
      if(count == 0) P(rw);
      count++;
      V(mutex);
      V(w);
      
      reading;
      
      P(mutex);
      count--;
      if(count == 0) V(rw);
      V(mutex);
    }
  }
  ```

* 哲学家进餐问题

  ```mermaid
  graph LR
  A --> |单只筷子| B
  B --> |单只筷子| C
  C --> |单只筷子| D
  D --> |单只筷子| E
  E --> |单只筷子| A
  ```

  ```shell
  问题描述：
  1、哲学家饥饿时会试图拿起左右两根筷子（一根一根的拿起）。
  2、若筷子已在他人手上，则需要等待。
  3、同时拿到两根筷子才可以进餐，结束后放下筷子继续思考。
  ```

  ```C
  semaphore chopstick[5] = {1,1,1,1,1};
  semaphore mutex = 1;
  Pi() {
    while(1) {
      P(mutex);
    	P(chopstick[i]);
    	P(chopstick[(i+1)%5]);
    	V(mutex);
    	eat;
    	V(chopstick[i]);
    	V(chopstick[(i+1)%5]);
      think;
    }
  }
  ```

* 吸烟者问题

  ```shell
  问题描述：
  1、系统有三个抽烟者进程和一个供应者进程
  2、每个抽烟者不停的卷烟并抽掉
  3、卷烟需要**烟草、纸和胶水**三种材料，每个抽烟者有其中一种
  4、供应者进程无限提供三种材料，每次提供其中两种。
  5、拥有剩下材料的抽烟者卷烟抽掉，并给供应者信号告知已完成，此时供应者就会将另外两种材料放在桌子上
  ```

   ```C
   int num = 0;
   semaphore offer1=offer2=offer3=0;
   semaphore finish = 0;
   
   supply() {
     while(1) {
       num++;
       num = num % 3;
       if(num == 0) V(offer1);
       else if(num == 1) V(offer2);
       else V(offer3);
       
       P(finish);
     }
   }
   
   smoke_i() {
     while(1) {
       P(offer_i);
       ...;
       V(finish);
     }
   }
   ```

  





