#ifndef USERROLES_H
#define USERROLES_H

#include <QtCore/qnamespace.h>

namespace Boot
{

//Новые роли
enum SomeUserRole
{
    WhatsThis_UserRole=Qt::UserRole,
    WhatsSoft_UserRole,
    WhatsStorage_UserRole
};

enum WhatsThis { Root, Storage, Device, File };
enum WhatsStorage { OtherSrorage, KsuStorage, UsbStorage };
enum WhatsSoft { OtherSoft, WorkKSU, LoaderKSU, SysKSU, KI, KPT };

}// namespace Boot

#endif // USERROLES_H
