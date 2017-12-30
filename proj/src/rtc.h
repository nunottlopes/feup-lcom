#ifndef __RTC_H
#define __RTC_H

#define RTC_ADDR_REG 		0x70
#define RTC_DATA_REG 		0x71
#define RTC_MINUTES			2
#define RTC_HOURS 			4
#define RTC_DAY				7
#define RTC_MONTH			8
#define RTC_YEAR			9

/** @defgroup rtc rtc
 * @{
 *
 * Functions for using the rtc
 */

/**
 * @brief Get computer time
 *
 * @param hour is the computer hour
 * @param min is the computer minutes
 *
 * @return Returns 0
 */
int rtc_get_time(unsigned long *hour, unsigned long *min);

/**
 * @brief Get computer date
 *
 * @param dia is the computer day
 * @param mes is the computer month
 * @param ano is the computer year
 *
 * @return Returns 0
 */
int rtc_get_date(unsigned long *dia, unsigned long *mes, unsigned long *ano);


#endif /* __RTC_H */
