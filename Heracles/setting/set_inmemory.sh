SCRIPT=$(readlink -f "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
SERVER="10.0.0.5"
WORKER1="janux-02"
WORKER2="janux-03"

docker swarm leave --force
docker stop spark-master
docker rm spark-master
docker rm data

docker swarm init --advertise-addr $SERVER | tee token.out
TOKEN="$(cat token.out | grep -m 1 token)"
docker network create --attachable --subnet 10.11.0.0/24 -d overlay spark-net
ssh $WORKER1 "docker swarm leave --force; docker stop spark-worker1; docker rm spark-worker1; $TOKEN"
ssh $WORKER2 "docker swarm leave --force; docker stop spark-worker2; docker rm spark-worker2; $TOKEN"
docker create --name data cloudsuite/movielens-dataset
docker run -dP --net spark-net --hostname spark-master --name spark-master cloudsuite/spark:1.5.1 master
ssh $WORKER1 "docker rm data; docker create --name data cloudsuite/movielens-dataset; docker run -dP --net spark-net --volumes-from data --name spark-worker1 cloudsuite/spark:1.5.1 worker spark://spark-master:7077"
ssh $WORKER2 "docker rm data; docker create --name data cloudsuite/movielens-dataset; docker run -dP --net spark-net --volumes-from data --name spark-worker2 cloudsuite/spark:1.5.1 worker spark://spark-master:7077"
