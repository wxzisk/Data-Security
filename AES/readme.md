# C语言实现AES-128、AES-192、AES-256

本部分中我们实现了AES-128、AES-192、AES-256算法。正确性验证方面，我们使用不同长度bit测试样例进行了测试，并调用OpenSSL库的标准函数进行正确性验证。

## 作业题目

### 正确性验证（调用标准库验证）

调用OpenSSL库的标准函数进行正确性验证，我们使用check_algorithm.c来对check.txt中的字符串进行AES-128、AES-192、AES-256加密


- 字节流加密正确性
  
  在check_algorithm.c文件中，我们将key和input放入AES标准库中加密，得到正确的密文。使用方法如下：

    ```
    Input:
    32 43 f6 a8 88 5a 30 8d 31 31 98 a2 e0 37 07 34 
    ----------------AES-128----------------
    OpenSSL Encrypted Output:
    89 ed 5e 6a 05 ca 76 33 81 35 08 5f e2 1c 40 bd 
    ----------------AES-192----------------
    OpenSSL Encrypted Output:
    bc 3a aa b5 d9 7b aa 7b 32 5d 7b 8f 69 cd 7c a8 
    ----------------AES-256----------------
    OpenSSL Encrypted Output:
    9a 19 88 30 ff 9a 4e 39 ec 15 01 54 7d 4a 6b 1b
    ```
    在AES-128_192_256.c文件中我们修改输入文件为"check.txt"，经过我们的加密算法得到密文。使用方法如下：
    ```
    
    ```


### 分别对16K，4M的数据进行压缩，测试实现效率

文件16k.txt和4M.txt为我们随机生成的16KB、4MB文件。在Intel(R) Core(TM) i5-7500 CPU @ 3.40GHz下进行测试，调用 test_byte.py 进行字节流输入的长字符串压缩测试，结果如下。

| 测试项目     | 未优化版本用时(s) |
| ----------- | ----------- |
| SHA3-256(16K)    | 0.000948 | 
| SHA3-512(16K)    | 0.156942 | 
| SHA3-256(4M)    | 0.000687 | 
| SHA3-512(4M)    | 0.119552 | 

## 仓库结构

### SHA256.c/SHA256.h/SHA512.h/SHA512.h/SHACommon.h

SHA256.c/SHA256.h/SHA512.h/SHA512.h/SHACommon.h是实现算法的c语言程序。

### main.c

main.c是测试主程序

### Makefile

编译生成可执行文件sha

### sha

有比特流输入和字节流输入两种模式。比特流输入直接从控制台输入01字符串，解释为bit串。字节流输入从文件读取字符串，每个字符解释为一个字节。以下为示例：

    比特流:./sha3 -m <method> -s <任意二进制串>
        <method>: 
            256 
            512
        <任意二进制串>: 只能输入01串，每个字符会被解释为1bit。
    字节流:./sha3 -m <method> -f <文件名>
        <文件名>: 字符串文件，每个字符会被解释为一个字节。

### test_example.py

测试脚本，测试正确性和时间

运行: 
    
    python3 test_example.py -m 512 -f 16k.txt

### 16k.txt 4M.txt

测试文件的输入