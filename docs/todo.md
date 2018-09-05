

#### CoreMemController logic

the alternative of Algorithm 2 in the paper is necessary.

* one way is to use measureMemoryBW() logic with intel's pcm.

```shell
sudo ./pcm.x -i = 1
```

* another way is to use only slack and decrease resources when the slack is low.



####  report logic

I implemented a simple report logic but now it has some problems.

There can be many clients(consisting a master and agents) and a single client can have many threads and a single thread can have many connections.

I am not sure when to report exactly.



#### pid-capturing logic

For now there is no c++ logic to capture pids of LC task and BE task.

Later we will need this logic and use this in the init code.

One possible way is to use `./mutilate -pid` option and read that pid file in controller code.







