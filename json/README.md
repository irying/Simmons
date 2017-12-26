From https://zhuanlan.zhihu.com/json-tutorial

我们要实现的 JSON 库，主要是完成 3 个需求：

1. 把 JSON 文本解析为一个树状数据结构（parse）。
2. 提供接口访问该数据结构（access）。
3. 把数据结构转换成 JSON 文本（stringify）。



1.宏的名字必须是唯一的，通常习惯以_ _*H__* 作为后缀。由于 leptjson 只有一个头文件，可以简单命名为 LEPTJSON_H__。如果项目有多个文件或目录结构，可以用项目名称_目录_文件名称_ _H__ 这种命名方式。

2.JSON 中有 6 种数据类型，如果把 true 和 false 当作两个类型就是 7 种，我们为此声明一个枚举类型（enumeration type）：