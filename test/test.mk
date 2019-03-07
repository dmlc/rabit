# this is a makefile used to show testcases of rabit
.PHONY: all

all: model_recover_10_10k  model_recover_10_10k_die_same model_recover_10_10k_die_hard local_recover_10_10k

# this experiment test recovery with actually process exit, use keepalive to keep program alive
model_recover_10_10k:
	../dmlc-core/tracker/dmlc-submit --cluster local --num-workers=2 model_recover 10000 mock=0,0,1,0 mock=1,1,1,0 rabit_num_trial=0 DMLC_NUM_ATTEMPT=10

model_recover_10_10k_die_same:
	../dmlc-core/tracker/dmlc-submit --cluster local --num-workers=10 model_recover 10000 mock=0,0,1,0 mock=1,1,1,0 mock=0,1,1,0 mock=4,1,1,0 mock=9,1,1,0 rabit_num_trial=0 DMLC_NUM_ATTEMPT=10

model_recover_10_10k_die_hard:
	../dmlc-core/tracker/dmlc-submit --cluster local --num-workers=10 model_recover 10000 mock=0,0,1,0 mock=1,1,1,0 mock=1,1,1,1 mock=0,1,1,0 mock=4,1,1,0 mock=9,1,1,0 mock=8,1,2,0 mock=4,1,3,0 rabit_num_trial=0 DMLC_NUM_ATTEMPT=10

# TODO: add those tests once we fix issue in rabit
local_recover_10_10k:
	../dmlc-core/tracker/dmlc-submit --cluster local --num-workers=10 local_recover 10000 mock=0,0,1,0 mock=1,1,1,0 mock=0,1,1,0 mock=4,1,1,0 mock=9,1,1,0 mock=1,1,1,1 rabit_num_trial=0 DMLC_NUM_ATTEMPT=10

pylocal_recover_10_10k:
	../dmlc-core/tracker/dmlc-submit --cluster local --num-workers=10  local_recover.py 10000 mock=0,0,1,0 mock=1,1,1,0 mock=0,1,1,0 mock=4,1,1,0 mock=9,1,1,0 mock=1,1,1,1 rabit_num_trial=0 DMLC_NUM_ATTEMPT=10

lazy_recover_10_10k_die_hard:
	../dmlc-core/tracker/dmlc-submit --cluster local --num-workers=10 lazy_recover 10000 mock=0,0,1,0 mock=1,1,1,0 mock=1,1,1,1 mock=0,1,1,0 mock=4,1,1,0 mock=9,1,1,0 mock=8,1,2,0 mock=4,1,3,0 rabit_num_trial=0 DMLC_NUM_ATTEMPT=10

lazy_recover_10_10k_die_same:
	../dmlc-core/tracker/dmlc-submit --cluster local --num-workers=10 lazy_recover 10000 mock=0,0,1,0 mock=1,1,1,0 mock=0,1,1,0 mock=4,1,1,0 mock=9,1,1,0 rabit_num_trial=0 DMLC_NUM_ATTEMPT=10

ringallreduce_10_10k:
	../dmlc-core/tracker/dmlc-submit --cluster local --num-workers=10 model_recover 100 rabit_reduce_ring_mincount=10
