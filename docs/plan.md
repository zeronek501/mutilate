## Heracles Implementation Plan

#### Use mutilate as a base implementation

https://github.com/leverich/mutilate

brief explanation:

* mutilate.cc: launches agent execute go for each agent  and execute do_mutilate for each thread.
* Connection.cc: send memcached req, get resp, many event logics including timer events and defines FSM.



#### Modification

* Connection.cc: modify it to open a socket to the server and send a nth latency after its execution for a period(period is given by -t option).
* mutilate.cc: modify mutilate.cc that it repeats entire execution for r times (defining a new parameter). 
* ServerMonitor.cc(new): open a receiver socket and repeats receiving latency from the master client connection. Then, later I will call any controlling methods following some latency criteria.
* ResourceController.cc(new): control CPU, Memory, LLC, Network resources.



#### Resource Controller

the isolation mechanisms of each resource are as follows.

* CPU: cgroup cpuset
* Memory BW: intel MBA (resctrl)
* LLC: intel CAT (resctrl)
* Network BW: cgroup net_cls, Linux tc, qdisc and HTB



#### Control Algorithm 

Same as ones in Heracles paper.



#### Monitoring Logic

We need a monitoring logic for each resource which is accessible through C code.
