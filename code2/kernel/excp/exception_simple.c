#include <xtos.h>

/* CSR寄存器定义 */
#define CSR_ESTAT 0x5

/* 异常名称数组 */
const char* exception_names[32] = {
    "External Interrupt",
    "IPI Interrupt", 
    "Timer Interrupt",
    "Instruction Page Fault",
    "Instruction Access Error",
    "System Call",
    "Breakpoint",
    "Instruction Not Exist",
    "Address Alignment Error",
    "Data Page Fault",
    "Data Access Error",
    "Floating Point Exception",
    "Privileged Instruction",
    "Address Space Exception",
    "Floating Point Disabled",
    "Watchpoint",
    "Machine Error",
    "General Register Exception",
    "Virtualization Exception",
    "General CSR Exception",
    "General CSR Exception",
    "System Call Exception",
    "Hypervisor Exception",
    "General Register Exception",
    "Hypervisor CSR Exception",
    "Floating Point Disabled",
    "LSX Exception",
    "LASX Exception",
    "LBT Exception",
    "LBT CSR Exception",
    "LBT CSR Exception 1",
    "LBT CSR Exception 2"
};

/* 获取异常类型 */
int get_exception_type(void)
{
    unsigned int estat;
    estat = read_csr_32(CSR_ESTAT);
    return (estat >> 2) & 0x1F;  /* ESTAT的ECode字段 */
}

/* 通用异常处理函数 */
void handle_exception_generic(int exception_type)
{
    if (exception_type >= 0 && exception_type < 32) {
        printk("Exception Type: ");
        printk((char*)exception_names[exception_type]);
        printk(" (Code: ");
        print_debug("", exception_type);
        printk(")\n");
        panic("Unhandled Exception");
    } else {
        printk("Unknown Exception Type: ");
        print_debug("", exception_type);
        panic("Unknown Exception");
    }
}

/* 简化的异常分发函数 */
void dispatch_exception_simple(int exception_type)
{
    printk("=== LoongArch Exception Handling ===\n");
    printk("Exception Type: ");
    print_debug("", exception_type);
    printk("\n");
    
    if (exception_type >= 0 && exception_type < 32) {
        printk("Exception Name: ");
        printk((char*)exception_names[exception_type]);
        printk("\n");
    }
    
    /* 简单的panic处理 */
    panic("Exception Framework Demo");
}

/* 测试异常处理框架 */
void test_exception_framework_simple(void)
{
    printk("=== LoongArch Exception Framework Test ===\n");
    
    /* 测试各种异常类型 */
    for (int i = 0; i < 5; i++) {
        printk("Test Exception Type ");
        print_debug("", i);
        printk(": ");
        printk((char*)exception_names[i]);
        printk("\n");
        dispatch_exception_simple(i);
    }
}

/* 异常处理框架演示 */
void demonstrate_exception_framework_simple(void)
{
    printk("=== LoongArch Exception Framework Demo ===\n");
    printk("This framework supports all 32 LoongArch exception types:\n");
    
    /* 显示所有支持的异常类型 */
    for (int i = 0; i < 32; i++) {
        printk("Exception Type ");
        print_debug("", i);
        printk(": ");
        printk((char*)exception_names[i]);
        printk("\n");
    }
    
    printk("\nFramework Features:\n");
    printk("1. Support all LoongArch exception types\n");
    printk("2. Unified exception dispatch mechanism\n");
    printk("3. Detailed exception information output\n");
    printk("4. Simple panic handling mechanism\n");
}
