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
        ```bash
        sudo apt install libhiredis-dev
        ```
    - libjsoncpp-dev => 用于解析json
        ```bash
        sudo apt install libjsoncpp-dev
        ```

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

- **测试接口**
    ```bash
    # 首先启动server
    ./MIRCertificationManager
    
    # 然后再另一个窗口测试客户端
    ./testRequest -f testjson/testAddOrUpdateCertificateRequet.json
    
    ./testRequest -f testjson/testExistsCertificate.json
  
    ./testRequest -f testjson/testGetCertificateLifetime.json
  
    ./testRequest -f testjson/testRemoveCertificateRequest.json
    ```

## 通信接口

- **证书下发和更新**

  1. 可以通过这个接口将用户的证书安装的MIR上，以便用户签名的Interest和Data在流经MIR的时候被正确验证；
  2. 同时支持设置证书的有效期，在有效期结束之后，证书会被自动移除。

  ```json
  // 请求
  {
      "code": 1,
      "data": {
          // 字符串类型
          "certStr": "证书的base64编码字符串",		
          
          // long型整数，表示整数有效期，单位为ms，如果期望证书不过期，一直可用，可以传递-1
          "lifetime": 1000,
          
          // 是否强制更新 => 指定强制更新，则证书已存在的情况下，会覆盖同名的旧的证书记录
          "forceUpdate": false
      }
  }
  
  // 回复
  {
      "code": 0,			// 0 => 安装成功， -1 => 证书格式错误， -2 => 存在同名证书，且未覆盖
      "errMsg": "",		// 如果有错误展示错误描述，没有错误则为空字符串
  }
  ```

- **证书撤销**

  1. 通过这个接口，可以撤销MIR上正在生效的某个证书，撤销之后，对应秘钥签名的Interest和Data将无法通过MIR的验证。

  ```json
  // 请求
  {
      "code": 2,
      "data": {
          // 字符串类型
          "certStr": "证书的base64编码字符串"
      }
  }
  
  // 回复
  {
      "code": 0,			// 0 => 撤销成功， -1 => 证书格式错误, -2 => 没有对应的条目
      "errMsg": ""		// 如果有错误展示错误描述，没有错误则为空字符串
  }
  ```

- **查询证书有效期**

  1. 通过这个接口，可以查询指定证书在MIR上的有效期。

  ```json
  // 请求
  {
      "code": 3,
      "data": {
          // 字符串类型
          "certStr": "证书的base64编码字符串"
      }
  }
  
  // 回复
  {
      "code": 0,			// 0 => 成功，-2 => 没有对应的条目
      "errMsg": "",		// 如果有错误展示错误描述，没有错误则为空字符串
      "data": 5000		// >=0的值 => 表示当前剩余存活期， -1 => 表示没有设置存活期，持久保存
  }
  ```

- **查询证书**

  1. 这个接口可以判断指定的证书在MIR上是否已经安装，并且是有效的；

  ```json
  // 请求
  {
      "code": 4,
      "data": {
          // 字符串类型
          "certStr": "证书的base64编码字符串"
      }
  }
  
  // 回复
  {
      "code": 0,			// 0 => 存在，-1 => 没有对应的条目
      "errMsg": ""		// 如果有错误展示错误描述，没有错误则为空字符串
  }
  ```

  