如果让你写一个Json数据解析器，你觉得会要做哪些工作？你的开发流程是怎么样的？会遇到哪些问题？怎么解决？

一、要做的工作

1.熟悉json的解析规则，有哪些数据类型需要解析，布尔值、数字、字符串、数组和对象等。

2.每一种数据类型的解析流程是怎么样的。



二、开发流程

1.定义数据结构

2.定义方法

3.写单元测试

4.写方法



在开发流程的每一步都会遇到问题，那从开发流程的每一步说起。

## 数据结构

### 3.字符串

#### 解析过程：

我们只需**要先备份栈顶，然后把解析到的字符压栈，最后计算出长度并一次性把所有字符弹出，再设置至值里便可以**。

```C
#define PUTC(c, ch) do { *(char*)lept_context_push(c, sizeof(char)) = (ch); } while(0)

static int lept_parse_string(lept_context* c, lept_value* v) {
    size_t head = c->top, len;
    const char* p;
    EXPECT(c, '\"');
    p = c->json;
    for (;;) {
        char ch = *p++;
        switch (ch) {
            case '\"':
                len = c->top - head;
                lept_set_string(v, (const char*)lept_context_pop(c, len), len);
                c->json = p;
                return LEPT_PARSE_OK;
            case '\0':
                c->top = head;
                return LEPT_PARSE_MISS_QUOTATION_MARK;
            default:
                PUTC(c, ch);
        }
    }
}
```

### 4.数组

JSON 数组的语法很简单，**实现的难点不在语法上，而是怎样管理内存。**



内存管理

JSON 数组存储零至多个元素，最简单就是使用 C 语言的数组

- 数组最大的好处是**能以 ![O(1)](https://www.zhihu.com/equation?tex=O%281%29) 用索引访问任意元素**，**次要好处是内存布局紧凑，省内存之余还有高缓存一致性（cache coherence）**。但数组的缺点是不能快速插入元素，而且我们在解析 JSON 数组的时候，<u>**还不知道应该分配多大的数组才合适。**</u>
- 另一个选择是链表（linked list），它的最大优点是可快速地插入元素（开端、末端或中间），但需要以 ![O(n)](https://www.zhihu.com/equation?tex=O%28n%29) 时间去经索引取得内容。如果我们只需顺序遍历，那么是没有问题的。还有一个小缺点，就是相对数组而言，链表在存储每个元素时有**额外内存开销（存储下一节点的指针）**，而且遍历时元素所在的内存可能不连续，令缓存不命中（cache miss）的机会上升。



### 5.对象

**语法：一些键值对的集合**

JSON 对象**以花括号 {}（U+007B、U+007D）包裹表示**，另外 JSON 对象由对象成员（member）组成，而 JSON 数组由 JSON 值组成。所谓对象成员，就是键值对，键必须为 JSON 字符串，然后值是任何 JSON 值，中间以冒号 :（**U+003A**）分隔。完整语法如下：

```
member = string ws %x3A ws value
object = %x7B ws [ member *( ws %x2C ws member ) ] ws %x7D
```

数据结构：

要表示键值对的集合，有很多数据结构可供选择，例如：

- 动态数组（dynamic array）：可扩展容量的数组，如 C++ 的 [std::vector](https://link.zhihu.com/?target=http%3A//en.cppreference.com/w/cpp/container/vector)。
- 有序动态数组（sorted dynamic array）：和动态数组相同，但保证元素已排序，可用二分搜寻查询成员。
- 平衡树（balanced tree）：平衡二叉树可有序地遍历成员，如红黑树和 C++ 的 [std::map](https://link.zhihu.com/?target=http%3A//en.cppreference.com/w/cpp/container/map)（[std::multi_map](https://link.zhihu.com/?target=http%3A//en.cppreference.com/w/cpp/container/multimap) 支持重复键）。
- 哈希表（hash table）：通过哈希函数能实现平均 O(1) 查询，如 C++11 的 [std::unordered_map](https://link.zhihu.com/?target=http%3A//en.cppreference.com/w/cpp/container/unordered_map)（[unordered_multimap](https://link.zhihu.com/?target=http%3A//en.cppreference.com/w/cpp/container/unordered_multimap) 支持重复键）。

设一个对象有 n 个成员，数据结构的容量是 m，n ⩽ m，那么一些常用操作的时间／空间复杂度如下：

![](https://pic2.zhimg.com/80/v2-d21b818cfeaa1932d89e049836c47ebb_hd.jpg)

```c
typedef struct lept_value lept_value;
typedef struct lept_member lept_member;

struct lept_value {
    union {
        struct { lept_member* m; size_t size; }o;
        struct { lept_value* e; size_t size; }a;
        struct { char* s; size_t len; }s;
        double n;
    }u;
    lept_type type;
};

struct lept_member {
    char* k; size_t klen;   /* member key string, key string length */
    lept_value v;           /* member value */
};
```

先用动态数组的方案，每个对象仅仅是成员的数组。





会遇到哪些问题

解析方法的注意点：

1.传入的 JSON 文本是一个 C 字符串（空结尾字符串／null-terminated string），由于我们不应该改动这个输入字符串，所以使用 **const char*** 类型。

另一注意点是，传入的根节点**指针 v 是由使用方负责分配**的，所以一般用法是：

```
lept_value v;
const char json[] = ...;
int ret = lept_parse(&v, json);
```

2.为了减少解析函数之间传递多个参数，我们把这些数据都放进一个 **lept_context 结构体**

3.缓冲区与堆栈

我们解析字符串（以及数组、对象）时，需要把解析的结果先储存在一个临时的缓冲区，最后再用lept_set_string() 把缓冲区的结果设进值之中。在完成解析一个字符串之前，这个缓冲区的大小是不能预知的。因此，我们可以采用动态数组（dynamic array）这种数据结构，即数组空间不足时，能自动扩展。C++ 标准库的 std::vector 也是一种动态数组。

如果每次解析字符串时，都重新建一个动态数组，那么是比较耗时的。我们可以重用这个动态数组，每次解析 JSON 时就只需要创建一个。**而且我们将会发现，无论是解析字符串、数组或对象，我们也只需要以先进后出的方式访问这个动态数组。换句话说，我们需要一个动态的堆栈数据结构。**

我们把一个动态堆栈的数据放进 lept_context 里：

```C
typedef struct {
    const char* json;
    char* stack;
    size_t size, top;
}lept_context;
```

当中 size 是当前的堆栈容量，top 是栈顶的位置（**由于我们会扩展 stack，所以不要把 top 用指针形式存储**）。

```c
int lept_parse(lept_value* v, const char* json) {
    lept_context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    c.stack = NULL;        /* <- */
    c.size = c.top = 0;    /* <- */
    lept_init(v);
    lept_parse_whitespace(&c);
    if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK) {
        /* ... */
    }
    assert(c.top == 0);    /* <- */
    free(c.stack);         /* <- */
    return ret;
}
```

然后，我们实现堆栈的压入及弹出操作。和普通的堆栈不一样，**我们这个堆栈是以字节储存的**。每次可要求压入任意大小的数据，它会返回数据起始的指针。

```c
#ifndef LEPT_PARSE_STACK_INIT_SIZE
#define LEPT_PARSE_STACK_INIT_SIZE 256
#endif

static void* lept_context_push(lept_context* c, size_t size) {
    void* ret;
    assert(size > 0);
    if (c->top + size >= c->size) {
        if (c->size == 0)
            c->size = LEPT_PARSE_STACK_INIT_SIZE;
        while (c->top + size >= c->size)
            c->size += c->size >> 1;  /* c->size * 1.5 */
        c->stack = (char*)realloc(c->stack, c->size);
    }
    ret = c->stack + c->top;
    c->top += size;
    return ret;
}

static void* lept_context_pop(lept_context* c, size_t size) {
    assert(c->top >= size);
    return c->stack + (c->top -= size);
}
```

初始大小以宏 LEPT_PARSE_STACK_INIT_SIZE 的形式定义，**使用 #ifndef X #define X ... #endif 方式的好处是，使用者可在编译选项中自行设置宏，没设置的话就用缺省值。**