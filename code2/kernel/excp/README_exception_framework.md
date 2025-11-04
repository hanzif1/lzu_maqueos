# LoongArch异常处理框架

## 概述

本框架为LoongArch架构实现了完整的异常处理机制，支持所有32种异常类型的处理。

## 文件结构

- `exception_simple.c` - 简化的异常处理框架实现
- `exception.c` - 主要的异常处理逻辑
- `exception_handler.S` - 汇编异常处理器
- `xtos.h` - 头文件定义

## 支持的异常类型

框架支持LoongArch架构的所有32种异常类型：

0. 外部中断
1. 处理器间中断
2. 定时器中断
3. 指令页错误
4. 指令访问错误
5. 系统调用
6. 断点异常
7. 指令不存在
8. 地址对齐错误
9. 数据页错误
10. 数据访问错误
11. 浮点异常
12. 特权指令异常
13. 地址空间异常
14. 浮点禁用异常
15. 监视点异常
16. 机器错误异常
17. 通用寄存器异常
18. 虚拟化异常
19. 通用CSR异常
20. 通用CSR异常
21. 系统调用异常
22. 超虚拟化异常
23. 通用寄存器异常
24. 超虚拟化CSR异常
25. 浮点禁用异常
26. LSX异常
27. LASX异常
28. LBT异常
29. LBT CSR异常
30. LBT CSR异常1
31. LBT CSR异常2

## 主要功能

### 1. 异常类型识别
- `get_exception_type()` - 从CSR_ESTAT寄存器读取异常类型

### 2. 异常分发
- `dispatch_exception_simple()` - 简化的异常分发函数
- 支持所有32种异常类型的统一处理

### 3. 异常处理
- `handle_exception_generic()` - 通用异常处理函数
- 输出详细的异常信息
- 调用panic函数终止系统

### 4. 测试功能
- `test_exception_framework_simple()` - 测试异常处理框架
- `demonstrate_exception_framework_simple()` - 演示框架功能

## 使用方法

### 1. 初始化异常处理
```c
excp_init();  // 初始化异常处理
int_on();     // 开启中断
```

### 2. 处理异常
异常发生时，系统会自动调用`do_exception()`函数，该函数会：
1. 检查是否是定时器中断
2. 如果是定时器中断，调用`timer_interrupt()`
3. 否则，获取异常类型并调用`dispatch_exception_simple()`

### 3. 测试异常处理
```c
// 演示异常处理框架
demonstrate_exception_framework_simple();

// 测试异常处理
test_exception_framework_simple();
```

## 框架特点

1. **完整性** - 支持LoongArch架构的所有异常类型
2. **统一性** - 提供统一的异常处理接口
3. **可扩展性** - 易于添加新的异常处理逻辑
4. **调试友好** - 提供详细的异常信息输出
5. **简单性** - 使用panic机制简化异常处理

## 编译

框架已集成到Makefile中，编译时会自动包含相关源文件：

```bash
make clean
make
```

## 注意事项

1. 当前实现使用简单的panic机制处理所有异常
2. 可以根据需要为特定异常类型实现更复杂的处理逻辑
3. 定时器中断有特殊处理，不会触发panic
4. 异常处理函数在汇编层面保存和恢复寄存器状态

## 扩展

要添加新的异常处理逻辑，可以：

1. 修改`dispatch_exception_simple()`函数
2. 为特定异常类型添加处理函数
3. 更新异常处理函数表

这个框架为LoongArch操作系统提供了完整的异常处理基础设施。
