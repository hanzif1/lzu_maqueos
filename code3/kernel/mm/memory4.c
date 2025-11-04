#include <xtos.h>

#define CSR_DMW0 0x180
#define CSR_DMW0_PLV0 (1UL << 0)
#define MEMORY_SIZE 0x8000000
#define PAGE_SIZE 4096
#define NR_PAGE (MEMORY_SIZE >> 12)
#define KERNEL_START_PAGE (0x200000UL >> 12)
#define KERNEL_END_PAGE (0x300000UL >> 12)

// 64 位系统
#define BITS_PER_LONG 64
// 计算位图需要多少个 unsigned long
#define BITMAP_SIZE ((NR_PAGE + BITS_PER_LONG - 1) / BITS_PER_LONG)

// 原来的 char mem_map[NR_PAGE]; 被替换为位图
unsigned long mem_bitmap[BITMAP_SIZE];

// 记录下一个空闲页的起始扫描位置（页号）
static unsigned long next_free_page = KERNEL_END_PAGE;

//位图操作辅助函数
static inline int test_bit(int nr, const unsigned long *addr)
{
    unsigned long mask = 1UL << (nr % BITS_PER_LONG);
    const unsigned long *p = addr + (nr / BITS_PER_LONG);
    return (*p & mask) != 0;
}

static inline void set_bit(int nr, unsigned long *addr)
{
    unsigned long mask = 1UL << (nr % BITS_PER_LONG);
    unsigned long *p = addr + (nr / BITS_PER_LONG);
    *p |= mask;
}

static inline void clear_bit(int nr, unsigned long *addr)
{
    unsigned long mask = 1UL << (nr % BITS_PER_LONG);
    unsigned long *p = addr + (nr / BITS_PER_LONG);
    *p &= ~mask;
}

unsigned long get_page()
{
    unsigned long i, page;
    int flag=0;
    // 1. 从 next_free_page 开始扫描到末尾
    for (i = next_free_page; i < NR_PAGE; i++)
    {
        if (!test_bit(i, mem_bitmap)) // 找到了一个空闲页 (bit 为 0)
        {
            flag=1;
            break;
        }
    }
    // 2. 如果上面没找到，则从头开始扫描到 next_free_page (回卷)
    if(!flag)
        for (i = 0; i < next_free_page; i++)
        {
            if (!test_bit(i, mem_bitmap)) // 找到了一个空闲页
            {
                flag=1;
                break;
            }
        }
    if(!flag){ // 遍历了整个位图都没找到
        panic("panic: out of memory!\n");
        return 0;
    }
    // 3. 标记该页已被占用 (将 bit 设为 1)
    set_bit(i, mem_bitmap);
    // 4. 更新下一个空闲页的起始位置
    next_free_page = i + 1;
    if (next_free_page >= NR_PAGE)
    {
        next_free_page = 0; // 如果到了末尾，回卷到开头
    }

    // 5. 清零该空闲页的内容
    page = (i << 12) | DMW_MASK;
    set_mem((char *)page, 0, PAGE_SIZE);
    return page;
}

void free_page(unsigned long page)
{
    unsigned long i;
    i = (page & ~DMW_MASK) >> 12;
    if (i >= NR_PAGE)
    {
        panic("panic: try to free invalid page address!\n");
    }
    if (!test_bit(i, mem_bitmap)) // 检查是否 bit 已经是 0
    {
        panic("panic: try to free free page!\n");
    }
    // 将 bit 设为 0
    clear_bit(i, mem_bitmap);
}

void mem_init()
{
    int i;
    // 1. 将整个位图清零 (所有页标记为空闲)
    set_mem((char *)mem_bitmap, 0, sizeof(mem_bitmap));
    // 2. 标记内核占用的页 (将 bit 设为 1)
    for (i = KERNEL_START_PAGE; i < KERNEL_END_PAGE; i++)
    {
        set_bit(i, mem_bitmap);
    }
    // 3. 设置 next_free_page 的初始扫描位置
    next_free_page = KERNEL_END_PAGE;
    write_csr_64(CSR_DMW0_PLV0 | DMW_MASK, CSR_DMW0);
}