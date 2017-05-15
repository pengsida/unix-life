# git合并分支

操作如下：

```bash
# 切换到想合并到的分支
pengsidadeMacBook-Pro:Linux-Device-Driver pengsida$ git checkout master
Switched to branch 'master'
Your branch is up-to-date with 'origin/master'.
# 将分支psd合并到分支master
pengsidadeMacBook-Pro:Linux-Device-Driver pengsida$ git merge psd
Updating 549ccee..b6e3ed8
Fast-forward
...
# 推送到远程github仓库
pengsidadeMacBook-Pro:Linux-Device-Driver pengsida$ git push origin master
Total 0 (delta 0), reused 0 (delta 0)
To https://github.com/wwtalwtaw/Linux-Device-Driver.git
    549ccee..b6e3ed8  master -> master
```