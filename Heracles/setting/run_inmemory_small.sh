docker run --rm --net spark-net --name inmemory --volumes-from data zeronek501/myinmemory:run /data/ml-latest-small /data/myratings.csv --master spark://spark-master:7077
