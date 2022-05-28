# thread-pool
A simple thread pool implementation with C++17.

#### usage

```cpp
// create a thread pool with 4 threads
thread_pool pool{4};

// push a task into the thread_pool
auto ret = pool.push([](int number){return number;}, 2);

// get return val;
std::cout << ret.get() << std::endl;
```
#### test

```
mkdir build
cd build
cmake ..
make -j4
./thread_pool_test
```
