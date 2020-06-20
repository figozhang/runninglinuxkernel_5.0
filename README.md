# 蓝色《奔跑吧linux内核》第二版 实验平台 中文注释版本

## 目的
为了帮助大家更好的理解liux内核，我们想在原版linux内核的基础上，对关键的内核代码写中文注释。

## 欢迎共享中文注释

欢迎大家来提pull request，主要提pull request到zh_comments这个分支。

1. rlk_5.0：实验平台的主 分支。
2. zh_comments： 中文注释分支。

## patch要求

所有代码patch都必须经过checkpatch.pl脚本的检查。
```
$ git diff | ./scripts/checkpatch.pl --no-tree
```
