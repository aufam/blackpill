#ifndef ETL_HEAP_H
#define ETL_HEAP_H

#include "FreeRTOS.h"
#include "etl/getter_setter.h"

namespace Project::etl::heap {

    inline const Getter<size_t, size_t(*)()> freeSize = { xPortGetFreeHeapSize };
    inline const Getter<size_t, size_t(*)()> minimumEverFreeSize = { xPortGetMinimumEverFreeHeapSize };
    
}

#endif