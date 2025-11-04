#include <xtos.h>

/* CSR寄存器定义 */
#define CSR_ESTAT 0x5

/* 异常处理函数指针数组 */
typedef void (*exception_handler_t)(void);

/* 异常处理函数表 */
static exception_handler_t exception_handlers[32] = {
    handle_exception_int,        /* 0: 外部中断 */
    handle_exception_ipi,        /* 1: 处理器间中断 */
    handle_exception_timer,      /* 2: 定时器中断 */
    handle_exception_ipf,        /* 3: 指令页错误 */
    handle_exception_ipe,        /* 4: 指令访问错误 */
    handle_exception_sys,        /* 5: 系统调用 */
    handle_exception_brk,        /* 6: 断点异常 */
    handle_exception_ine,        /* 7: 指令不存在 */
    handle_exception_ale,        /* 8: 地址对齐错误 */
    handle_exception_dpe,        /* 9: 数据页错误 */
    handle_exception_dbe,        /* 10: 数据访问错误 */
    handle_exception_fpe,        /* 11: 浮点异常 */
    handle_exception_sxd,        /* 12: 特权指令异常 */
    handle_exception_asxd,       /* 13: 地址空间异常 */
    handle_exception_fpd,        /* 14: 浮点禁用异常 */
    handle_exception_watch,      /* 15: 监视点异常 */
    handle_exception_merr,       /* 16: 机器错误异常 */
    handle_exception_gspr,       /* 17: 通用寄存器异常 */
    handle_exception_hvc,        /* 18: 虚拟化异常 */
    handle_exception_gcsc,       /* 19: 通用CSR异常 */
    handle_exception_gcsh,       /* 20: 通用CSR异常 */
    handle_exception_sxs,        /* 21: 系统调用异常 */
    handle_exception_hyp,        /* 22: 超虚拟化异常 */
    handle_exception_gspe,       /* 23: 通用寄存器异常 */
    handle_exception_hyp_csr,    /* 24: 超虚拟化CSR异常 */
    handle_exception_fpdis,      /* 25: 浮点禁用异常 */
    handle_exception_lsx,        /* 26: LSX异常 */
    handle_exception_lasx,       /* 27: LASX异常 */
    handle_exception_lbt,        /* 28: LBT异常 */
    handle_exception_lbt_csr,    /* 29: LBT CSR异常 */
    handle_exception_lbt_csr_1,  /* 30: LBT CSR异常1 */
    handle_exception_lbt_csr_2,  /* 31: LBT CSR异常2 */
};

/* 异常名称字符串数组 */
const char* exception_names[32] = {
    "外部中断",
    "处理器间中断", 
    "定时器中断",
    "指令页错误",
    "指令访问错误",
    "系统调用",
    "断点异常",
    "指令不存在",
    "地址对齐错误",
    "数据页错误",
    "数据访问错误",
    "浮点异常",
    "特权指令异常",
    "地址空间异常",
    "浮点禁用异常",
    "监视点异常",
    "机器错误异常",
    "通用寄存器异常",
    "虚拟化异常",
    "通用CSR异常",
    "通用CSR异常",
    "系统调用异常",
    "超虚拟化异常",
    "通用寄存器异常",
    "超虚拟化CSR异常",
    "浮点禁用异常",
    "LSX异常",
    "LASX异常",
    "LBT异常",
    "LBT CSR异常",
    "LBT CSR异常1",
    "LBT CSR异常2"
};

/* 通用异常处理函数 */
void handle_exception_generic(int exception_type)
{
    if (exception_type >= 0 && exception_type < 32) {
        printk("异常类型: ");
        printk((char*)exception_names[exception_type]);
        printk(" (代码: ");
        print_debug("", exception_type);
        printk(")\n");
        panic("未处理的异常");
    } else {
        printk("未知异常类型: ");
        print_debug("", exception_type);
        panic("未知异常");
    }
}

/* 异常分发函数 */
void dispatch_exception(int exception_type)
{
    if (exception_type >= 0 && exception_type < 32) {
        if (exception_handlers[exception_type] != NULL) {
            exception_handlers[exception_type]();
        } else {
            handle_exception_generic(exception_type);
        }
    } else {
        handle_exception_generic(exception_type);
    }
}

/* 获取异常类型 */
int get_exception_type(void)
{
    unsigned int estat;
    estat = read_csr_32(CSR_ESTAT);
    return (estat >> 2) & 0x1F;  /* ESTAT的ECode字段 */
}

/* 更新后的do_exception函数 */
void do_exception_new(void)
{
    int exception_type;
    
    exception_type = get_exception_type();
    dispatch_exception(exception_type);
}
