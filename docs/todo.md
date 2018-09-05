

#### CoreMemController logic

Since we decided to change the algorithm2 to simple version, the alternative of Algorithm 2 in the paper is necessary.

* one way is still to use measureMemoryBW() logic with intel's pcm and discard other prediction things or complex logics.

```shell
sudo ./pcm.x -i = 1
```

* another way is to use only slack and decrease resources alternatively when the slack is low.



#### mutilate report logic

I implemented a simple report logic but now it has some problems.

There can be many clients(consisting a master and agents) and a single client can have many threads and a single thread can have many connections.

I am not sure when and where to report exactly.



#### pid-capturing logic

For now there is no c++ logic to capture pids of LC task and BE task.

Later we will need this logic and use this in the init code.

One possible way is to use `./mutilate -pid` option and read that pid file in the controller code.



#### Fine tuning

we need to determine values to tune the Heracles code.

* target latency
* typical maximum load of mutilate (it seems too low.. it is just about 10000qps when I had a test)
* thread number, connection number and agent number ...
* etc.





