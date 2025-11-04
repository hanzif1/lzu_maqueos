#include <xtos.h>

/* 异常处理框架演示程序 */
void exception_demo(void)
{
    printk("=== LoongArch异常处理框架演示 ===\n");
    printk("本框架支持LoongArch架构的所有32种异常类型\n");
    printk("\n");
    
    /* 显示所有支持的异常类型 */
    printk("支持的异常类型:\n");
    for (int i = 0; i < 32; i++) {
        printk("异常类型 ");
        print_debug("", i);
        printk(": ");
        printk((char*)exception_names[i]);
        printk("\n");
    }
    
    printk("\n框架特点:\n");
    printk("1. 支持所有LoongArch异常类型\n");
    printk("2. 统一的异常分发机制\n");
    printk("3. 详细的异常信息输出\n");
    printk("4. 简单的panic处理机制\n");
    
    printk("\n测试异常处理:\n");
    
    /* 测试几个常见的异常类型 */
    printk("测试异常类型0 (外部中断):\n");
    dispatch_exception_simple(0);
    
    printk("测试异常类型5 (系统调用):\n");
    dispatch_exception_simple(5);
    
    printk("测试异常类型8 (地址对齐错误):\n");
    dispatch_exception_simple(8);
    
    printk("测试异常类型11 (浮点异常):\n");
    dispatch_exception_simple(11);
    
    printk("测试未知异常类型99:\n");
    dispatch_exception_simple(99);
}





