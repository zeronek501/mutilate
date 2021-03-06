SCRIPT=$(readlink -f "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
SERVER="10.0.0.5"
WORKER1="janux-02"
WORKER2="janux-03"

docker rm data
docker stop inmemory
docker rm inmemory

docker create --name data cloudsuite/movielens-dataset

rm -f cids.txt
docker run --rm -d --name inmemory --cidfile="./cids.txt" --volumes-from data zeronek501/myinmemory:run /data/ml-latest /data/myratings.csv --driver-memory 5g --executor-memory 5g

docker pause inmemory
