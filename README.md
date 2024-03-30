### LSMKV For Practice

#### After reading materials about LSM tree -> LevelDB, I want to implement a new LSMKV better than in class.
#### Mainly, I'd like to learn more about moder C++. 11/14/17/20/23

- 03/30/2024 A simple command parser, I want to support commands as follows:

```C
  PUT KEY key VALUE value
  GET KEY
  DEL KEY
  BATCH_PUT KEY k1 VALUE v1 KEY k2 VALUE v2...
  BATCH_GET KEYS k1 k2 k3...
```

- 03/30/2024 A simple memory allocator, like slab:
  - for 32B, 64B, 128B, 256B, 512B, 1024B, 4K, 2M.

### run

```C
  mkdir build
  cd build
  cmake ..
  make
  ./main
```
