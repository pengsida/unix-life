# 从A主机ssh到B主机不需要密码

步骤如下：

1. 在 A 机器上：ssh-keygen -t rsa，随后默认按enter键。生成的密钥对 id_rsa ， id_rsa.pub ，默认存储在 /home/username/.ssh 目录下。

2. 将 /home/username/.ssh/id_rsa.pub 的内容复制到 A、B 两台机器 /home/username/.ssh/authorized_keys 文件中，如果机器上已经有 authorized_keys 这个文件了，就在文件末尾加上 id_rsa.pub 中的内容

3. 改变A这台机器上/home/username/.ssh/id_rsa、/home/username/.ssh/authorized_keys这两个文件的权限：

    ```
    sudo chmod 600 /home/username/.ssh/id_rsa
    sudo chmod 600 /home/username/.ssh/authorized_keys
    ```