# C语言实现SHA3-256和SHA3-512

本部分中我们实现了SHA3-256、SHA3-512算法。正确性验证方面，我们使用官方提供的不同长度bit测试样例进行了测试，并使用python库hashlib进行字节流测试和性能测试。

## 作业题目

### 正确性验证（调用标准库验证）

正确性验证我们使用了官方提供的6中比特流样例以及python的hashlib库进行了验证。其中官方样本验证的是比特流压缩的正确性，用hashlib比对验证的则是字节流压缩的正确性。


- 字节流压缩正确性
  
  python的hashlib库中实现了SHA3的四种算法，但只限于用字节流输入。我们利用hashlib作为标准库对比压缩结果，实现在 test_byte.py 文件中。有关正确性的结果将输出两行，其中第一行为调用sh.hexdigest()产生的结果，第二行为调用自己实现的SHA3算法压缩的结果。例如：

    ```
    python3 test_byte.py -x SHA3-512 -s crypto
    expect: 601057e930458c0ad2deb75f514fa6e31149877287495c1959e1528bc686071dde5e99ec31a24452b2731409bc3f7521a4d5da00b116e21f5c88ef53765a8ee5
    result: 601057e930458c0ad2deb75f514fa6e31149877287495c1959e1528bc686071dde5e99ec31a24452b2731409bc3f7521a4d5da00b116e21f5c88ef53765a8ee5
    Time Cost(without IO):0.000020
    ```


### 分别对16K，4M的数据进行压缩，测试实现效率

文件16k.txt和4M.txt为我们随机生成的16KB、4MB文件。在Intel(R) Core(TM) i5-7500 CPU @ 3.40GHz下进行测试，调用 test_byte.py 进行字节流输入的长字符串压缩测试，结果如下。


| 测试项目     | 未优化版本用时(s) |
| ----------- | ----------- |
| SHA3-256(16K)    | 0.002203 | 
| SHA3-512(16K)    | 0.004135 | 
| SHA3-256(4M)    | 0.549819 | 
| SHA3-512(4M)    | 1.038731 | 



## 仓库结构

### mysha3.c & mysha3_old.c

mysha3.c是实现算法的c语言程序。

### makefile

编译mysha3.c生成可执行文件mysha3和

### mysha3 

有比特流输入和字节流输入两种模式。比特流输入直接从控制台输入01字符串，解释为bit串。字节流输入从文件读取字符串，每个字符解释为一个字节。以下为示例：

    比特流:./mysha3 -x <method> -m <任意二进制串>
        <method>: 
            SHA3-256 
            SHA3-512
        <任意二进制串>: 只能输入01串，每个字符会被解释为1bit。
    字节流:./mysha3 -x <method编号> -f <文件名>
        <文件名>: 字符串文件，每个字符会被解释为一个字节。

### test_example.py

测试脚本，测试正确性和时间

运行: 
    
    python3 test_example.py -x SHA-512 -f 16k.txt

