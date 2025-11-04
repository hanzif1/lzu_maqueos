#include <xtos.h>

void main()
{
	con_init();
	excp_init();
	int_on();
	
	/* 演示异常处理框架 */
	printk("=== LoongArch操作系统启动 ===\n");
	printk("异常处理框架已初始化\n");
	printk("支持所有32种LoongArch异常类型\n");
	printk("\n");
	
	/* 显示异常处理框架信息 */
	demonstrate_exception_framework_simple();
	
	/* 测试异常处理 */
	printk("\n开始测试异常处理:\n");
	test_exception_framework_simple();
	
	printk("\n系统进入主循环...\n");
	while (1)
		;
}