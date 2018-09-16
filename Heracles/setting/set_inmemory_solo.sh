SCRIPT=$(readlink -f "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
SERVER="10.0.0.5"
WORKER1="janux-02"
WORKER2="janux-03"

docker rm data

docker create --name data cloudsuite/movielens-dataset

