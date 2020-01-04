#ifndef __BASH_CMD_H__
#define __BASH_CMD_H__

#include "vtypes.h"
#include <QString>

class QBashCmd
{
public:
    static void stop_getty(void);
    static bool flush_storage(void);
    static bool make_usb_auto_mount(void);
    //ethernet
//    static void set_ipv4_address(uint32_t addr);
//    static void set_ipv4_mask(uint32_t mask);
//    static void set_ipv4_gateway(uint32_t addr);
    static void set_file_execute_mode(const QString &file);
};

#endif//__BASH_CMD_H__
