# affinity\_sets\_generic

Benchmarks for the affinity sets feature using a generic pipeline with
synthetic data.

```
# mkdir build
# cd build
# cmake -DAFFINITY_LOGIC=0 -DNUM_SHARDS=5 -DNUM_CATEGORIES=10 -DBENCHMARK_TIME=1500 -DRUN_MODE=rdma ..
# make -j `nproc`
```

