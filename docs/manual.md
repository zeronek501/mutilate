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

```shell
server(janux-05)> memcached -t 4 -c 32768 -p 11212

agent1(janux-01)> mutilate -T 16 -A

master(janux-00)> mutilate -s janux-05:11212 --loadonly
master(janux-00)> mutilate -s janux-05:11212 --noload -B -T 16 -a janux-01 -c 4 -q 200000
```

reference : https://github.com/leverich/mutilate









