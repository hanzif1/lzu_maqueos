#include <xtos.h>

/* 测试异常处理框架的函数 */
void test_exception_framework(void)
{
    printk("=== LoongArch异常处理框架测试 ===\n");
    
    /* 测试各种异常类型的处理 */
    printk("测试异常类型0 (外部中断):\n");
    dispatch_exception(EXCEPTION_INT);
    
    printk("测试异常类型5 (系统调用):\n");
    dispatch_exception(EXCEPTION_SYS);
    
    printk("测试异常类型8 (地址对齐错误):\n");
    dispatch_exception(EXCEPTION_ALE);
    
    printk("测试异常类型11 (浮点异常):\n");
    dispatch_exception(EXCEPTION_FPE);
    
    printk("测试未知异常类型:\n");
    dispatch_exception(99);  /* 无效的异常类型 */
}

/* 模拟异常发生的函数 */
void simulate_exception(int exception_type)
{
    printk("模拟异常类型: ");
    print_debug("", exception_type);
    printk("\n");
    
    dispatch_exception(exception_type);
}

/* 异常处理框架演示 */
void demonstrate_exception_framework(void)
{
    printk("=== LoongArch异常处理框架演示 ===\n");
    printk("本框架支持LoongArch架构的所有32种异常类型:\n");
    
    /* 显示所有支持的异常类型 */
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
    printk("3. 可扩展的异常处理函数表\n");
    printk("4. 详细的异常信息输出\n");
    printk("5. 简单的panic处理机制\n");
}
