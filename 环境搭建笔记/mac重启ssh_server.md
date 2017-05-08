# mac重启ssh_server

## 步骤如下：
```
1.停止sshd服务：
sudo launchctl unload -w /System/Library/LaunchDaemons/ssh.plist

2.启动sshd服务：
sudo launchctl load -w /System/Library/LaunchDaemons/ssh.plist

3.查看是否启动：
sudo launchctl list | grep ssh
```