/**
   @file proximityadaptor-ascii.cpp
   @brief ProximityAdaptor that reads proximity value from ascii interface

   <p>
   Copyright (C) 2009-2010 Intel Corporation
   Copyright (C) 2009-2010 Nokia Corporation

   @author Leo Yan <leo.yan@intel.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
   @author Matias Muhonen <ext-matias.muhonen@nokia.com>
   @author Tapio Rantala <ext-tapio.rantala@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>
   @author Lihan Guo <ext-lihan.4.guo@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>

   This file is part of Sensord.

   Sensord is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License
   version 2.1 as published by the Free Software Foundation.

   Sensord is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with Sensord.  If not, see <http://www.gnu.org/licenses/>.
   </p>
*/

#include <errno.h>
#include <string.h>
#include "logging.h"
#include "config.h"
#include "proximityadaptor-pa224.h"
#include "datatypes/utils.h"
#include <unistd.h>

ProximityAdaptorPa224::ProximityAdaptorPa224(const QString& id) :
    SysfsAdaptor(id, SysfsAdaptor::IntervalMode)
{
    proximityBuffer_ = new DeviceAdaptorRingBuffer<ProximityData>(1);
    setAdaptedSensor("proximity", "pa224 ascii", proximityBuffer_);
}

ProximityAdaptorPa224::~ProximityAdaptorPa224()
{
    delete proximityBuffer_;
}

void ProximityAdaptorPa224::processSample(int, int fd)
{
    char buf[16];
    lseek(fd, 0, SEEK_SET);
    if (read(fd, buf, sizeof(buf)) <= 0) {
        sensordLogW() << "read(): " << strerror(errno);
        return;
    }
    sensordLogT() << "Proximity output value: " << buf;

    ProximityData* proximity = proximityBuffer_->nextSlot();
    sscanf(buf, "%d", &proximity->value_);
    sensordLogT() << "Proximity output value: " << proximity->value_ << buf;
    proximity->withinProximity_ = (proximity->value_ >= 64);
    proximity->timestamp_ = Utils::getTimeStamp();
    proximityBuffer_->commit();
    proximityBuffer_->wakeUpReaders();
}
