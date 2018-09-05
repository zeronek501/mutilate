# Manual

List

* mutilate
* Heracles

## mutilate

#### Connection.cc

Determine whether a connection is the master connection by checking stats.sampling and isagent variables.

When the master connection is destroyed, report stats by calling report_stats();

#### mutilate.cc

cmdline parameter "n" is added and it means the number of repetition.



#### how to do a simple test

(self-test)

run 

```shell
./server_monitor
```

then, 

```shell
./mutilate -s localhost -n 5
```

server_monitor prints latency and qps every cycle.



(multi-node test)

fixing some problems

```shell
master>
```









