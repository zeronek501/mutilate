docker run --rm --net spark-net --name inmemory --volumes-from data zeronek501/myinmemory:run /data/ml-latest /data/myratings.csv --master spark://spark-master:7077
