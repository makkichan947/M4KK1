/**
 * M4KK1 Timer Header
 * 定时器和时钟管理函数定义
 */

#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

/**
 * 时钟频率（Hz）
 */
#define TIMER_FREQUENCY 1193180

/**
 * PIT通道
 */
#define PIT_CHANNEL_0   0x40
#define PIT_CHANNEL_1   0x41
#define PIT_CHANNEL_2   0x42
#define PIT_COMMAND     0x43

/**
 * PIT命令标志
 */
#define PIT_BINARY      0x00
#define PIT_BCD         0x01
#define PIT_MODE_0      0x00
#define PIT_MODE_1      0x02
#define PIT_MODE_2      0x04
#define PIT_MODE_3      0x06
#define PIT_MODE_4      0x08
#define PIT_MODE_5      0x0A
#define PIT_LATCH       0x00
#define PIT_LOW         0x10
#define PIT_HIGH        0x20
#define PIT_BOTH        0x30

/**
 * RTC寄存器
 */
#define RTC_SECONDS     0x00
#define RTC_SECONDS_ALARM 0x01
#define RTC_MINUTES     0x02
#define RTC_MINUTES_ALARM 0x03
#define RTC_HOURS       0x04
#define RTC_HOURS_ALARM 0x05
#define RTC_WEEKDAY     0x06
#define RTC_DAY         0x07
#define RTC_MONTH       0x08
#define RTC_YEAR        0x09
#define RTC_STATUS_A    0x0A
#define RTC_STATUS_B    0x0B
#define RTC_STATUS_C    0x0C
#define RTC_STATUS_D    0x0D

/**
 * RTC状态B标志
 */
#define RTC_CLOCK_24H   0x02
#define RTC_BINARY_MODE 0x04
#define RTC_SQUARE_WAVE 0x08
#define RTC_UPDATE_INT  0x10
#define RTC_ALARM_INT   0x20
#define RTC_PERIODIC_INT 0x40
#define RTC_UPDATE_END_INT 0x80

/**
 * 时间结构
 */
typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint32_t year;
} time_t;

/**
 * 初始化定时器
 */
void timer_init(uint32_t frequency);

/**
 * 设置PIT频率
 */
void timer_set_frequency(uint32_t frequency);

/**
 * 获取时钟计数
 */
uint32_t timer_get_ticks(void);

/**
 * 等待指定毫秒数
 */
void timer_wait(uint32_t milliseconds);

/**
 * 获取系统运行时间（毫秒）
 */
uint32_t timer_get_uptime(void);

/**
 * 读取RTC时间
 */
void timer_read_rtc(time_t *time);

/**
 * 设置RTC时间
 */
void timer_set_rtc(time_t *time);

/**
 * 定时器中断处理函数
 */
void timer_handler(void);

/**
 * 睡眠指定微秒数
 */
void timer_usleep(uint32_t microseconds);

/**
 * 睡眠指定秒数
 */
void timer_sleep(uint32_t seconds);

/**
 * 获取CPU频率（MHz）
 */
uint32_t timer_get_cpu_frequency(void);

/**
 * 校准定时器
 */
void timer_calibrate(void);

/**
 * 定时器闹钟结构
 */
typedef struct {
    uint32_t id;              /* 闹钟ID */
    uint32_t interval_ms;     /* 闹钟间隔（毫秒） */
    uint32_t remaining_ms;    /* 剩余时间（毫秒） */
    uint8_t active;           /* 是否激活 */
    void (*callback)(void);   /* 闹钟回调函数 */
} timer_alarm_t;

/**
 * 注册定时器中断处理函数
 * @param handler 中断处理函数指针
 */
void timer_register_handler(void (*handler)(void));

/**
 * 创建定时器闹钟
 * @param interval_ms 闹钟间隔（毫秒）
 * @param callback 闹钟回调函数
 * @return 闹钟ID，失败返回0
 */
uint32_t timer_create_alarm(uint32_t interval_ms, void (*callback)(void));

/**
 * 销毁定时器闹钟
 * @param alarm_id 闹钟ID
 * @return 成功返回0，失败返回-1
 */
int32_t timer_destroy_alarm(uint32_t alarm_id);

/**
 * 获取纳秒级时间戳
 * @return 当前纳秒级时间戳
 */
uint64_t timer_get_nanoseconds(void);

/**
 * 纳秒级睡眠
 * @param nanoseconds 睡眠纳秒数
 */
void timer_nsleep(uint64_t nanoseconds);

#endif /* __TIMER_H__ */