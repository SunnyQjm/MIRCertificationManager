# MIRCertificationManager

## 编译

- **编译要求**
    - boost => 安装ndn-cxx和nfd之前需要安装boost库
    - ndn-cxx => v0.6.6
    - nfd => v0.6.6
    - redis-server => 提供redis存储服务，要在本机安装redis-server，代码编译时不需要用到，但是运行代码会尝试连接本机的redis服务；
        ```bash
        sudo apt install redis-server
        ```
    - libhiredis-dev => C语言实现的redis客户端API的连接库（代码中使用该库来连接redis本机的redis服务器）

- **编译**
    ```bash
    git clone https://github.com/SunnyQjm/MIRCertificationManager.git
    cd MIRCertificationManager
    cmake .
    make
    ```
  
## 使用

- **用命令行安装证书到redis**
    ```bash
    ./installCertificate -f <证书文件路径>
    ```
  
- **用命令行从redis卸载证书**
    ```bash
    ./uninstallCertificate -f <证书文件路径>
    ```