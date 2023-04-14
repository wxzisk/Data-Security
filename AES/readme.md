# C语言实现AES-128、AES-192、AES-256

本部分中我们实现了AES-128、AES-192、AES-256算法。正确性验证方面，我们使用不同长度bit测试样例进行了测试，并调用OpenSSL库的标准函数进行正确性验证。

## 作业题目

### 正确性验证（调用标准库验证）

调用OpenSSL库的标准函数进行正确性验证，我们使用check_algorithm.c来对check.txt中的字符串进行AES-128、AES-192、AES-256加密


- 字节流加密正确性
  
  在check_algorithm.c文件中，我们将key和input放入AES标准库中加密，得到正确的密文。使用方法如下：

    ```
    ./check_algorithm 
    Input:
    31 32 33 34 35 36 37 38 39 31 32 33 34 35 36 37 
    ----------------AES-128----------------
    OpenSSL Encrypted Output:
    25 2b 4a dc d0 64 e9 49 29 f9 9c 46 c3 7f 9f 20 
    ----------------AES-192----------------
    OpenSSL Encrypted Output:
    cc 57 55 7d 97 3c c0 1b 8d d3 fd 8b b2 54 21 d3 
    ----------------AES-256----------------
    OpenSSL Encrypted Output:
    db 4f 1c 20 d6 52 02 c0 fe 51 3c 45 21 5b 23 a8
    ```
    在AES-128_192_256.c文件中我们从输入文件"check.txt"中拿到明文，这和check_algorithm.c中的一样，经过我们的加密算法得到密文。可以运行make生成的可执行文件，或者使用我们提供的脚本。推荐使用python脚本，参数-m指操作类型包括加密/解密/验证正确性（encrypt/decrypt/check），参数-t指秘钥长度（128/192/256），参数-f指输入文件。使用方法如下：
    ```
    python test_byte.py -m check -t 128 -f check.txt
    result: The first line of Ciphertext(hex) : 25 2b 4a dc d0 64 e9 49 29 f9 9c 46 c3 7f 9f 20
    python test_byte.py -m check -t 192 -f check.txt
    result: The first line of Ciphertext(hex) : cc 57 55 7d 97 3c c0 1b 8d d3 fd 8b b2 54 21 d3
    python test_byte.py -m check -t 256 -f check.txt
    result: The first line of Ciphertext(hex) : db 4f 1c 20 d6 52 2 c0 fe 51 3c 45 21 5b 23 a8
    ```


### 分别对16K，4M的数据进行压缩，测试实现效率

文件16k.txt和4M.txt为我们随机生成的16KB、4MB文件。在Intel(R) Core(TM) i5-7500 CPU @ 3.40GHz下进行测试，调用 test_byte.py 进行字节流输入的长字符串压缩测试，结果如下。

| 测试项目     | 未优化版本用时(s) |
| ----------- | ----------- |
| AES-128(16K)    | 0.007696 | 
| AES-192(16K)    | 0.009306 | 
| AES-256(16K)    | 0.010825 | 
| AES-128(4M)    | 0.030784 | 
| AES-192(4M)    | 0.037224 | 
| AES-256(4M)    | 0.0433   | 

这里需要注意的是，由于AES是每16字节进行加密的。在我们的程序中，我们希望一次载入所有的数据然后同时进行加密，但是这需要一次性读入文件中的所有数据。当数据大小为4MB时，会因为段错误而无法执行。以上4MB数据的加密是通过对多次拆分4M.txt测试得到的结果。我们准备4M.txt只是为了展示程序性能的上限。

## 仓库结构

### AES-128_192_256.c

AES-128_192_256.c是实现AES算法的c语言程序。

### check_algorithm.c

check_algorithm.c是使用OpenSSL生成密文的测试程序

### Makefile

编译生成可执行文件 AES-128_192_256


### test_example.py

测试脚本，测试正确性和时间

运行: 
    
    python test_byte.py -m check -t 128 -f 16k.txt

### 16k.txt 4M.txt

测试文件的输入