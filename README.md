# gem5-cache-replacement

1. Clone ``gem5`` from [https://github.com/gem5/gem5](https://github.com/gem5/gem5)
2. Place ``lin_rp.cc``, ``lin_rp.hh``, ``log_rp.cc``, ``log_rp.hh`` in ``src/mem/cache/replacement_policies/
``
3. Add classes for the two new policies in ``src/mem/cache/replacement_policies/ReplacementPolicies.py``
```
class LinRP(BaseReplacementPolicy):
    type = "LinRP"
    cxx_class = "gem5::replacement_policy::LinRP"
    cxx_header = "mem/cache/replacement_policies/lin_rp.hh"
    learning_rate = Param.Float(0.01, "Learning rate for the linear regression model")


class LogRP(BaseReplacementPolicy):
    type = "LogRP"
    cxx_class = "gem5::replacement_policy::LogRP"
    cxx_header = "mem/cache/replacement_policies/log_rp.hh"
    learning_rate = Param.Float(0.001, "Learning rate for the log regression model")
    reuseThreshold = Param.Unsigned(10000, "Threshold for binary")
```
4. In ``src/mem/cache/replacement_policies/SConscript`` add the two policies to ``SimObject``
```
SimObject( …, 'LinRP', 'LogRP'])
```
5. Also in ``src/mem/cache/replacement_policies/SConscript`` add the two policies as new ``Source``, after the other
``Source('AAA.cc')``
```
...
Source('lin_rp.cc')
Source('log_rp.cc')
```
6. In ``configs/learning_gem5/part1``, edit ``caches.py`` and ``two_level.py`` or copy the ones in this repository
-  In ``two_level.py`` changed to
```
from caches import *
```
- In ``caches.py`` added ``replacement_policy=LogRP()`` under ``tgts_per_mshr`` in ``L1Cache`` and ``L2Cache`` classes
7. If you edit any of the replacement policies, do
  ```
scond build/X86/gem5.opt -j64
  ```
before running tests

8. To test the different policies, swap out the class used for ``replacement_policy=X_RP()`` in ``configs/learning_gem5/part1/caches.py``
9. Then run
```
build/X86/gem5.opt configs/learning_gem5/part1/two_level.py --l1i_size=32kB --l1d_size=32kB --l2_size=256kB tests/{test program binary}
```
10. For the two benchmarks discussed in the paper, clone ``https://github.com/darchr/microbench``, build the X86 binaries
- Assuming they are in the ``microbench`` directory within ``tests``
```
build/X86/gem5.opt configs/learning_gem5/part1/two_level.py --l1i_size=32kB --l1d_size=32kB --l2_size=256kB tests/microbench/ML2/bench.X86 
```
