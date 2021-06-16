#### 2.3进程同步

---

##### 一、进程同步的基本概念

* 临界资源（对临界资源的访问，必须互斥的进行）
  * 进入区
  * 临界区
  * 退出区
  * 剩余区
* 同步（直接制约关系）
* 互斥（间接制约关系），访问临界资源遵循以下原则：
  * 空闲让进
  * 忙则等待
  * 有限等待
  * 让权等待

 

---

##### 二、实现临界区互斥的基本方法

* 软件实现方法（Peterson解决方案）

  ```
  Perterson解决方案适用于两个进程交错执行临界区与剩余区。
  前置条件：两个进程共享两个数据项：
  int turn;
  boolean flag[2];
  
  do {
  	//进入区，请求进入临界资源
  	flag[0] = true ;
  	turn = 1;
  	while (flag[1] && turn == 1);
  	
  	临界区
  	
  	//退出区
  	flag[0] = false;
  	
  	剩余区
  	
  } while(true);
  
  1、到达进入区flag[0]=flag[1]==true; turn只能为0或1
  2、turn==0，P0进入临界区，P1等待
  3、P0结束，flag[0]=false，P1进入临界区
  ```

   

* 硬件同步（元方法、低级方法）

  * test_and_set

    ```
    boolean test_and_set(boolean *target) {
    	boolean rv = *target;
    	*target = true;
    	
    	return rv;
    }
    
    do {
    	//进入区
    	while(test_and_set(&lock));
    	
    	//critical section
    	
    	lock = flase;
    	
    	//reminder section
    
    } while(true);
    ```

     

  * compare_and_swap

    ```
    int compare_and_swap(int *value, int expected, int new_value) {
    	int temp = *value;
    	
    	if(*value == expected)
    		*value = new_value;
    	
    	return temp;
    }
    ```
    
     
  
* 互斥锁

* 信号量

* 管程（了解）

 

---

##### 三、信号量

一个信号量（semaphore）S是个整型变量，它除了初始化外只能通过两个标准原子操作：wait（）和signal（）来访问。

```
P:
wait(S) {
	while (S <= 0)
		;
	S--;
}

V:
signal(S) {
	S++;
}

利用信号量实现前驱关系
初始化synch为0
P1:
...
signal(synch);

P2:
wait(synch);
...
```

```
typedef struct{
	int value;
	struct process *list;
}semaphore;

wait(semaphore *S) {
	S->value--;
	if(S->value < 0){
		add this process to S->list;
		block();	//阻塞该进程
	}
}

signal(semaphore *S) {
	S->value++;
	if(S->value <= 0){
		remove this process from S->list;
		weakup(P);	//唤醒该进程
	}
}
```

 

---

##### 四、管程

 封装了对共享变量的一组操作

```
monitor DiningPhilosophers
{
	enum {THINKING, HUNGRY, EATING} state[5];
	condition self[5];
	
	void pickup(int i){
		state[i] = HUNGRY;
		test(i);
		if(state[i] != EATING)
			self[i].wait();
	}
	
	void putdown(int i) {
		state[i] = THINKING;
		test((i + 4) % 5);
		test((i + 1) % 5);
	}
	
	void test(int i) {
		if((state[(i + 5) % 5] != EATING) && 
		(state[i] == HUNGRY) && 
		(state[(i + 5) % 5] != EATING) && ) {
			state[i] = EATING;
			self[i].signal();
		}
	}
	
	initialization_code() {
		for (int i=0; i<5; i++)
			state[i] = THINKING;
	}
}

DiningPhilosophers.pickup(i);
	...
	eat
	...
DiningPhilosophers.putdown(i);
```





---

##### 经典同步问题

* 生产者—消费者问题（有界缓冲问题）

  ``` 
  共享数据结构：
  int n;
  semaphore mutex = 1;
  semaphore empty = n;
  semaphore full = 0;
  
  生产者进程：
  do {
  	/* produce an item in next_produced */
  	
  	wait(empty);
  	wait(mutex);
  	
  	/* add next_produced to the buffer */
  	
  	signal(mutex);
  	signal(full);
  	
  } while(true);
  
  消费者进程：
  do {
  	wait(full);
  	wait(mutex);
  	
  	/* remove an item from buffer to next_consumed */
  	
  	signal(mutex);
  	signal(empty);
  	
  	/* consume the item in next_consumed */
  	
  } while(true);
  ```

  ```
  例题：桌子上有一个盘子，每次只能向其中放入一个水果。爸爸专向盘子中放苹果，妈妈专向盘子中放橘子，儿子专等吃盘子中的橘子，女儿专等吃盘子中的苹果。只有盘子为空时，爸爸或妈妈才可向盘子中放入一个水果；仅当盘子中有自己需要的水果时，儿子或女儿才可以从盘子中取出。
  
  爸爸放苹果        妈妈放橘子
     ↓  ⇠此处连续执行⇢  ↓
  女儿吃苹果        儿子吃橘子
  
  解：
  semaphore plate=1,apple=0,orange=0;
  
  dad() {
  	do {
  		wait(plate);
  		
  		/* place an apple */
  		
  		signal(apple);
  	} while(true);
  }
  
  mom() {
  	do {
  		wait(plate);
  		
  		/* place an orange */
  		
  		signal(orange);
  	} while(true);
  }
  
  son() {
  	do {
  		wait(apple);
  		
  		/* take the apple */
  		
  		signal(plate);
  	} while(true);
  }
  
  daughter() {
  	do {
  		wait(orange);
  		
  		/* take the orange */
  		
  		signal(plate);
  	} while(true);
  }
  ```

   

* 读者—作者问题

  ```
  问题描述：
  1、允许多个读者可以同时对文件执行读操作
  2、只允许一个写者往文件中写信息
  3、任一写者在完成写操作之前不允许其他读者或写者工作
  4、写者执行写操作前，应让已有的读者和写者全部退出
  
  semaphore rw_mutex = 1;
  semaphore mutex = 1;
  int read_count = 0;
  写者进程：
  do {
  	wait(rw_mutex);
  	
  	/* writing is performed */
  	
  	signal(rw_mutex);
  } while(true);
  
  读者进程：
  do {
  	wait(mutex);
  	read_count++;
  	if (read_count == 1)
  		wait(rw_mutex);
  	signal(mutex);
  	
  	/* reading is performed */
  	
  	wait(mutex);
  	read_count--;
  	if (read_count == 0)
  		signal(rw_mutex);
  	signal(mutex);
  } while(true);
  ```

  ```
  读写公平（相对）：
  semaphore rw_mutex = 1;
  semaphore mutex = 1;
  semaphore w = 1;
  int read_count = 0;
  writer() {
      do {
          wait(w);
          wait(rw_mutex);
  
          /* writing is performed */
  
          signal(rw_mutex);
          signal(w);
      } while(true);
  }
  
  reader() {
      do {
      	wait(w);
          wait(mutex);
          read_count++;
          if (read_count == 1)
              wait(rw_mutex);
          signal(mutex);
          signal(w);
  
          /* reading is performed */
  
          wait(mutex);
          read_count--;
          if (read_count == 0)
              signal(rw_mutex);
          signal(mutex);
      } while(true);
  }
  ```

   

* 哲学家就餐问题

  ```
  问题描述：
  5支筷子，没人每次只能拿相邻的一只，拿到两支才能干饭。
  
  semaphore chopstick[5] = {1,1,1,1,1};
  哲学家i:
  do {
  	wait(chopstick[i]);
  	wait(chopstick[(i+1) % 5]);
  	
  	/* eat for awhile */
  	
  	signal(chopstick[i]);
  	signal(chopstick[(i+1) % 5]);
  	
  	/* think for awhile */
  } while(true);
  
  问题：
  1、会导致死锁
  2、需要保证没有一位哲学家被饿死
  
  semaphore chopstick[5] = {1,1,1,1,1};
  semaphore mutex = 1;
  Pi() {
  	do{
  		P(mutex);
  		P(chopstick[i]);
  		P(chopstick[(i+1)%5]);
  		V(mutex);
  		
  		eat;
  		
  		V(chopstick[i]);
  		V(chopstick[(i+1)%5]);
  		
  		...
  	}while(1);
  }
  ```
  
   
  
* 吸烟者问题

  ```
  问题描述：
  问题描述:假设一个系统有三个抽烟者进程和一个供应者进程。每个抽烟者不停地卷烟并抽掉它，但要卷起并抽掉一支烟，抽烟者需要有三种材料:烟草、纸和胶水。三个抽烟者中，第一个拥有烟草，第二个拥有纸，第三个拥有胶水。供应者进程无限地提供三种材料，供应者每次将两种材料放到桌子上，拥有剩下那种材料的抽烟者卷一根烟并抽掉它，并给供应者一个信号告诉已完成，此时供应者就会将另外两种材料放到桌上，如此重复(让三个抽烟者轮流地抽烟)。
  
  
  int num = 0;
  semaphore offer1=0;
  semaphore offer2=0;
  semaphore offer3=0;
  semaphore finish=1;
  
  producer () {
  	do {
  		num = num++;
  		num = num % 3;
  		
  		if(num == 0)
  			V(offer1);
  		if(num == 1)
  			V(offer2);
  		if(num == 2)
  			V(offer3);
  		
  		P(finish);
  	} while(true);
  }
  
  smoker1 () {
  	do {
  		P(offer1);
  		
  		/* get materials, smoke */
  		
  		V(finish);
  	} while(true);
  
  }
  ```

  
