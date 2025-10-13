/**
 * M4KK1 Timer Implementation
 * 高精度定时器系统实现
 */

#include "../../include/timer.h"
#include "../../include/idt.h"
#include "../../include/stdint.h"
#include "../../include/console.h"
#include "../../include/kernel.h"
#include "../../include/string.h"

/* I/O端口操作函数 */
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

/* 定时器全局变量 */
static uint32_t timer_ticks = 0;           /* 时钟滴答计数 */
static uint32_t timer_frequency = 1000;    /* 定时器频率（Hz） */
static uint64_t timer_nanoseconds = 0;     /* 纳秒级时间戳 */
static void (*timer_callback)(void) = NULL; /* 定时器中断处理函数 */

/* 闹钟系统变量 */
#define MAX_ALARMS 256
static timer_alarm_t alarms[MAX_ALARMS];   /* 闹钟数组 */
static uint32_t next_alarm_id = 1;         /* 下一个闹钟ID */
static uint32_t active_alarms = 0;         /* 活跃闹钟数量 */

/* CPU频率（用于高精度计时） */
static uint32_t cpu_frequency_mhz = 0;

/**
 * 读取CMOS RTC时间
 */
static uint8_t cmos_read(uint8_t reg) {
    outb(0x70, reg);
    return inb(0x71);
}

/**
 * 写入CMOS RTC寄存器
 */
static void cmos_write(uint8_t reg, uint8_t value) {
    outb(0x70, reg);
    outb(0x71, value);
}

/**
 * 读取RTC时间
 */
void timer_read_rtc(time_t *time) {
    uint8_t status_b;

    /* 读取状态寄存器B */
    status_b = cmos_read(RTC_STATUS_B);

    /* 等待更新完成 */
    while (cmos_read(RTC_STATUS_A) & 0x80);

    /* 读取时间 */
    time->second = cmos_read(RTC_SECONDS);
    time->minute = cmos_read(RTC_MINUTES);
    time->hour = cmos_read(RTC_HOURS);
    time->day = cmos_read(RTC_DAY);
    time->month = cmos_read(RTC_MONTH);
    time->year = cmos_read(RTC_YEAR) + 2000;

    /* 如果是BCD模式，转换为二进制 */
    if (!(status_b & RTC_BINARY_MODE)) {
        time->second = (time->second >> 4) * 10 + (time->second & 0x0F);
        time->minute = (time->minute >> 4) * 10 + (time->minute & 0x0F);
        time->hour = (time->hour >> 4) * 10 + (time->hour & 0x0F);
        time->day = (time->day >> 4) * 10 + (time->day & 0x0F);
        time->month = (time->month >> 4) * 10 + (time->month & 0x0F);
        time->year = (time->year >> 4) * 10 + (time->year & 0x0F) + 2000;
    }
}

/**
 * 设置RTC时间
 */
void timer_set_rtc(time_t *time) {
    uint8_t status_b;

    /* 读取状态寄存器B */
    status_b = cmos_read(RTC_STATUS_B);

    /* 等待更新完成 */
    while (cmos_read(RTC_STATUS_A) & 0x80);

    /* 如果是二进制模式，转换为BCD */
    if (!(status_b & RTC_BINARY_MODE)) {
        time->second = ((time->second / 10) << 4) | (time->second % 10);
        time->minute = ((time->minute / 10) << 4) | (time->minute % 10);
        time->hour = ((time->hour / 10) << 4) | (time->hour % 10);
        time->day = ((time->day / 10) << 4) | (time->day % 10);
        time->month = ((time->month / 10) << 4) | (time->month % 10);
        time->year = ((time->year / 10) << 4) | (time->year % 10);
    }

    /* 写入时间 */
    cmos_write(RTC_SECONDS, time->second);
    cmos_write(RTC_MINUTES, time->minute);
    cmos_write(RTC_HOURS, time->hour);
    cmos_write(RTC_DAY, time->day);
    cmos_write(RTC_MONTH, time->month);
    cmos_write(RTC_YEAR, time->year);
}

/**
 * 初始化定时器系统
 */
void timer_init(uint32_t frequency) {
    uint32_t divisor;
    uint8_t status_b;

    KLOG_INFO("Initializing timer system...");

    /* 初始化闹钟数组 */
    memset(alarms, 0, sizeof(alarms));

    /* 设置定时器频率 */
    timer_frequency = frequency;
    divisor = TIMER_FREQUENCY / frequency;

    /* 初始化PIT */
    outb(PIT_COMMAND, PIT_CHANNEL_0 | PIT_LOW | PIT_HIGH | PIT_MODE_3 | PIT_BINARY);

    /* 设置除数 */
    outb(PIT_CHANNEL_0, divisor & 0xFF);
    outb(PIT_CHANNEL_0, (divisor >> 8) & 0xFF);

    /* 读取RTC时间作为基准 */
    time_t rtc_time;
    timer_read_rtc(&rtc_time);

    /* 读取状态寄存器B以检查是否启用24小时模式 */
    status_b = cmos_read(RTC_STATUS_B);

    /* 启用RTC中断（如果需要） */
    if (!(status_b & RTC_PERIODIC_INT)) {
        cmos_write(RTC_STATUS_B, status_b | RTC_PERIODIC_INT);
    }

    /* 初始化CPU频率检测 */
    timer_calibrate();

    KLOG_INFO("Timer initialized with frequency: ");
    console_write_dec(frequency);
    console_write(" Hz\n");
}

/**
 * 设置PIT频率
 */
void timer_set_frequency(uint32_t frequency) {
    uint32_t divisor;

    timer_frequency = frequency;
    divisor = TIMER_FREQUENCY / frequency;

    /* 设置新的除数 */
    outb(PIT_COMMAND, PIT_CHANNEL_0 | PIT_LOW | PIT_HIGH | PIT_MODE_3 | PIT_BINARY);
    outb(PIT_CHANNEL_0, divisor & 0xFF);
    outb(PIT_CHANNEL_0, (divisor >> 8) & 0xFF);

    KLOG_INFO("Timer frequency set to: ");
    console_write_dec(frequency);
    console_write(" Hz\n");
}

/**
 * 获取时钟计数
 */
uint32_t timer_get_ticks(void) {
    return timer_ticks;
}

/**
 * 等待指定毫秒数
 */
void timer_wait(uint32_t milliseconds) {
    uint32_t start_ticks = timer_ticks;
    uint32_t ticks_to_wait = milliseconds * timer_frequency / 1000;

    while ((timer_ticks - start_ticks) < ticks_to_wait) {
        /* 忙等待 */
        asm volatile("hlt");
    }
}

/**
 * 获取系统运行时间（毫秒）
 */
uint32_t timer_get_uptime(void) {
    return timer_ticks * 1000 / timer_frequency;
}

/**
 * 定时器中断处理函数
 */
void timer_handler(void) {
    uint32_t i;

    /* 增加时钟滴答计数 */
    timer_ticks++;
    timer_nanoseconds += (1000000000ULL / timer_frequency);

    /* 处理闹钟 */
    for (i = 0; i < MAX_ALARMS; i++) {
        if (alarms[i].active && alarms[i].callback) {
            alarms[i].remaining_ms--;

            if (alarms[i].remaining_ms == 0) {
                /* 触发闹钟 */
                alarms[i].callback();

                /* 如果是周期性闹钟，重置计数器 */
                if (alarms[i].interval_ms > 0) {
                    alarms[i].remaining_ms = alarms[i].interval_ms;
                } else {
                    /* 单次闹钟，标记为非活跃 */
                    alarms[i].active = 0;
                    active_alarms--;
                }
            }
        }
    }

    /* 调用用户注册的中断处理函数 */
    if (timer_callback) {
        timer_callback();
    }
}

/**
 * 睡眠指定微秒数
 */
void timer_usleep(uint32_t microseconds) {
    uint64_t start_ns = timer_nanoseconds;
    uint64_t nanoseconds_to_wait = (uint64_t)microseconds * 1000;

    while ((timer_nanoseconds - start_ns) < nanoseconds_to_wait) {
        asm volatile("hlt");
    }
}

/**
 * 睡眠指定秒数
 */
void timer_sleep(uint32_t seconds) {
    timer_wait(seconds * 1000);
}

/**
 * 获取CPU频率（MHz）
 */
uint32_t timer_get_cpu_frequency(void) {
    return cpu_frequency_mhz;
}

/**
 * 获取定时器频率（Hz）
 */
uint32_t timer_get_frequency(void) {
    return timer_frequency;
}

/**
 * 校准定时器（测量CPU频率）
 */
void timer_calibrate(void) {
    uint32_t start_ticks, end_ticks;
    uint64_t start_tsc, end_tsc;

    KLOG_INFO("Calibrating timer...");

    /* 使用时间戳计数器（TSC）校准 */
    start_ticks = timer_get_ticks();

    /* 读取TSC */
    asm volatile("rdtsc" : "=A" (start_tsc));

    /* 等待一段时间 */
    timer_wait(100); /* 等待100毫秒 */

    end_ticks = timer_get_ticks();
    asm volatile("rdtsc" : "=A" (end_tsc));

    /* 计算CPU频率 */
    uint64_t tsc_diff = end_tsc - start_tsc;
    uint32_t ticks_diff = end_ticks - start_ticks;
    uint32_t time_ms = ticks_diff * 1000 / timer_frequency;

    if (time_ms > 0) {
        /* 避免64位除法，使用移位运算 */
        /* 计算频率：(TSC差值 * 1000) / (时间毫秒数 * 1000) */
        /* 简化为：TSC差值 / 时间毫秒数 */
        uint32_t time_ms_32 = time_ms;
        uint32_t tsc_diff_32 = (uint32_t)tsc_diff;

        if (time_ms_32 > 0 && tsc_diff_32 > 0) {
            cpu_frequency_mhz = tsc_diff_32 / time_ms_32;
        } else {
            cpu_frequency_mhz = 1000; /* 默认值 */
        }
    } else {
        cpu_frequency_mhz = 1000; /* 默认值 */
    }

    KLOG_INFO("CPU frequency calibrated: ");
    console_write_dec(cpu_frequency_mhz);
    console_write(" MHz\n");
}

/**
 * 注册定时器中断处理函数
 */
void timer_register_handler(void (*handler)(void)) {
    timer_callback = handler;

    /* 注册到IDT系统 */
    /* 注意：idt_register_handler在idt.c中定义，这里暂时注释掉 */
    // idt_register_handler(IDT_TIMER, (interrupt_handler_t)timer_callback);

    KLOG_INFO("Timer handler registered");
}

/**
 * 创建定时器闹钟
 */
uint32_t timer_create_alarm(uint32_t interval_ms, void (*callback)(void)) {
    uint32_t i;

    if (!callback || interval_ms == 0) {
        return 0;
    }

    /* 寻找空闲槽位 */
    for (i = 0; i < MAX_ALARMS; i++) {
        if (!alarms[i].active) {
            alarms[i].id = next_alarm_id++;
            alarms[i].interval_ms = interval_ms;
            alarms[i].remaining_ms = interval_ms;
            alarms[i].active = 1;
            alarms[i].callback = callback;
            active_alarms++;

            KLOG_INFO("Alarm created: ID=");
            console_write_dec(alarms[i].id);
            console_write(", Interval=");
            console_write_dec(interval_ms);
            console_write("ms\n");

            return alarms[i].id;
        }
    }

    KLOG_WARN("Maximum alarms reached, cannot create more alarms");
    return 0;
}

/**
 * 销毁定时器闹钟
 */
int32_t timer_destroy_alarm(uint32_t alarm_id) {
    uint32_t i;

    for (i = 0; i < MAX_ALARMS; i++) {
        if (alarms[i].active && alarms[i].id == alarm_id) {
            alarms[i].active = 0;
            alarms[i].callback = NULL;
            active_alarms--;

            KLOG_INFO("Alarm destroyed: ID=");
            console_write_dec(alarm_id);
            console_write("\n");

            return 0;
        }
    }

    KLOG_WARN("Alarm not found: ID=");
    console_write_dec(alarm_id);
    console_write("\n");

    return -1;
}

/**
 * 获取纳秒级时间戳
 */
uint64_t timer_get_nanoseconds(void) {
    return timer_nanoseconds;
}

/**
 * 纳秒级睡眠
 */
void timer_nsleep(uint64_t nanoseconds) {
    uint64_t start_ns = timer_nanoseconds;

    while ((timer_nanoseconds - start_ns) < nanoseconds) {
        asm volatile("hlt");
    }
}