#ifndef PCAP04_ASSETS_H
#define PCAP04_ASSETS_H

#include <stdint.h>
#include "pcap04_register.h"

/* 资源说明：
 * - PCAP04_DefaultConfig：固件验证过的寄存器默认表。
 * - PCAP04_FirmwareImage：PCAP04 官方固件镜像，用于升级。
 * - PCAP04_FirmwareImageSize：镜像长度（字节）。*/

extern const uint8_t PCAP04_DefaultConfig[PCAP04_REG_COUNT];
extern const uint8_t PCAP04_FirmwareImage[1024];
extern const uint16_t PCAP04_FirmwareImageSize;

#endif /* PCAP04_ASSETS_H */


