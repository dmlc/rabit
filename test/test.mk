RABIT_BUILD_DMLC = 0

ifeq ($(RABIT_BUILD_DMLC),1)
    DMLC=../dmlc-core
else
    DMLC=../../dmlc-core
endif

# this is a makefile used to show testcases of rabit
.PHONY: all

all: model_recover_10_10k  model_recover_10_10k_die_same model_recover_10_10k_die_hard local_recover_10_10k lazy_recover_10_10k_die_hard lazy_recover_10_10k_die_same ringallreduce_10_10k pylocal_recover_10_10k

# this experiment test recovery with actually process exit, use keepalive to keep program alive
model_recover_10_10k:
	$(DMLC)/tracker/dmlc-submit --cluster local --num-workers=10 --local-num-attempt=20 model_recover 10000 mock=0,0,1,0 mock=1,1,1,0 rabit_bootstrap_cache=-1 rabit_debug=1 rabit_reduce_ring_mincount=1 rabit_timeout=5

model_recover_10_10k_die_same:
	$(DMLC)/tracker/dmlc-submit --cluster local --num-workers=10 --local-num-attempt=20 model_recover 10000 mock=0,0,1,0 mock=1,1,1,0 mock=0,1,1,0 mock=4,1,1,0 mock=9,1,1,0 rabit_bootstrap_cache=1 rabit_timeout=5

model_recover_10_10k_die_hard:
	$(DMLC)/tracker/dmlc-submit --cluster local --num-workers=10 --local-num-attempt=20 model_recover 10000 mock=0,0,1,0 mock=1,1,1,0 mock=1,1,1,1 mock=0,1,1,0 mock=4,1,1,0 mock=9,1,1,0 mock=8,1,2,0 mock=4,1,3,0 rabit_bootstrap_cache=1 rabit_timeout=5

local_recover_10_10k:
	$(DMLC)/tracker/dmlc-submit --cluster local --num-workers=10 --local-num-attempt=20 local_recover 10000 mock=0,0,1,0 mock=1,1,1,0 mock=0,1,1,0 mock=4,1,1,0 mock=9,1,1,0 mock=1,1,1,1 rabit_timeout=5

pylocal_recover_10_10k:
	$(DMLC)/tracker/dmlc-submit --cluster local --num-workers=10 --local-num-attempt=20 local_recover.py 10000 mock=0,0,1,0 mock=1,1,1,0 mock=0,1,1,0 mock=4,1,1,0 mock=9,1,1,0 mock=1,1,1,1 rabit_timeout=5

lazy_recover_10_10k_die_hard:
	$(DMLC)/tracker/dmlc-submit --cluster local --num-workers=10 --local-num-attempt=20 lazy_recover 10000 mock=0,0,1,0 mock=1,1,1,0 mock=1,1,1,1 mock=0,1,1,0 mock=4,1,1,0 mock=9,1,1,0 mock=8,1,2,0 mock=4,1,3,0 rabit_timeout=5

lazy_recover_10_10k_die_same:
	$(DMLC)/tracker/dmlc-submit --cluster local --num-workers=10 --local-num-attempt=20 lazy_recover 10000 mock=0,0,1,0 mock=1,1,1,0 mock=0,1,1,0 mock=4,1,1,0 mock=9,1,1,0 rabit_timeout=5

ringallreduce_10_10k:
	$(DMLC)/tracker/dmlc-submit --cluster local --num-workers=10 model_recover 10000 rabit_reduce_ring_mincount=10
