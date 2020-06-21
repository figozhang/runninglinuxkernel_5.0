# 蓝色《奔跑吧linux内核》第二版 实验平台 中文注释版本

## 1. 目的
为了帮助大家更好的理解liux内核，我们想在原版linux内核的基础上，对关键的内核代码写中文注释。

## 2. 欢迎共享中文注释

欢迎大家来提pull request，主要提pull request（pr）到zh_comments这个分支。

1. rlk_5.0：实验平台的主开发分支。
2. zh_comments： 中文注释分支。

## 3. 提pr流程
1. clone：

```
$ git clone https://github.com/figozhang/runninglinuxkernel_5.0.git
```

2. 基于zh_comments分支建立一个自己工作分支，例如zh_comments_1
```
$ git checkout zh_comments
$ git checkout -b zh_comments_1
```
3. 为自己感兴趣的模块添加注释
4. 检查patch格式（可选）
所有patch尽可能地经过checkpatch.pl脚本的检查。
```
$ git diff | ./scripts/checkpatch.pl --no-tree
```

5. push 这个分支：zh_comments_1
```
$ git push --set-upstream origin zh_comments_1
```
6. 到主仓库网页提交pr，选择目标分支为：zh_comments，就会有自有分支到zh_comments的合并请求
