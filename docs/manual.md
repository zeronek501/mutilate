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



## Heracles

#### initial setting and test

ssh to janux-05

```shell
janux-05> sudo mkdir /sys/fs/cgroup/cpuset/lc
janux-05> sudo mount -t resctrl resctrl /sys/fs/resctrl
janux-05> sudo mkdir /sys/fs/resctrl/lc
janux-05> cd ~/mutilate/Heracles
janux-05> sudo ./TestCores
janux-05> sudo ./TestLLC
```

#### TestCores

use cgroup interface

make a lc task and add and removes lc task's cores.

Reads lc task's cores after each operation and checks if it is being done right.



#### TestLLC

use resctrl interface

make a lc task and add and removes lc task's LLC ways.

Reads lc task's LLC ways after each operation and checks if it is being done right.

At first, I used iostream library in Util.cc s_write(), but it doesn't work in resctrl,  so I used system() function instead.

























