# PaaS Fuzzer setup guide.
The PaaS Fuzzer is a C++-based system utilized for fuzzing Kubernetes to uncover vulnerabilities within the Kubernetes platform itself.

## Prerequesties 
1. Kubernetes C client.
2. Vcpkg 
3. CMake

## Project File Structure (recomended)
Below is a recommended directory structure for the Pass-Fuzzer application, including necessary external libraries and clients:

```
📦 PASS-FUZZER 
 ┣ 📂 c
 ┣ 📂 libwebsockets
 ┣ 📂 pass-fuzzer
   ┗ 📂 data
   ┗ 📂 include
   ┗ 📂 setup-guide
   ┗ 📂 src
   ┃  ┗ 📂 driver
   ┃  ┗ 📂 QueryGeneration
   ┃    .....
   ┗ 📂 static
   ┗ 📂 vcpkg
   ┗ 📜 CMakeLists.txt
   ┗ 📜 toolchain.cmake
```

## Step-by-step guide
1. Setup the Kubernetes c client inside the main directory(PASS-FUZZER)
   * Run the Script `./setup-k8s-c-client.sh`.
  
    >  ***Note:*** The provided shell script will guide you through installing the ***Kubernetes C client*** in the parent directory (please refer to the script for details).

   <br>
2. Build the system
   *  Build by making a build directory (i.e. `build/`), run `cmake` in `drive` directory, and then use `make` to build the target.
  
   ```
    cd ../src/driver
    mkdir build && cd build
    cmake ..
    make
   ```
   And then you can run the system by `.\ConfigGenerator`