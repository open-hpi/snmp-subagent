/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.notify.conf,v 5.2.2.1 2004/04/15 12:29:06 dts12 Exp $
 */
#ifndef HPIB0101_H
#define HPIB0101_H

/* function declarations */
int send_saHpiSensorNotification_trap(void);
int send_saHpiWatchdogNotification_trap(void);
int send_saHpiSensorEnableChangeNotification_trap(void);
int send_saHpiUserNotifications_trap(void);
int send_saHpiOEMNotifications_trap(void);
int send_saHpiSoftwareNotifications_trap(void);
int send_saHpiResourceNotifications_trap(void);
int send_saHpiHotSwapNotification_trap(void);
int send_saHpiDomainNotifications_trap(void);

#endif /* HPIB0101_H */
