#ifndef MAIN_H
#define MAIN_H

#include "project.h"

#if CYBLE_GAP_ROLE_CENTRAL
#define HBLE CYBLE_GAP_ROLE_CENTRAL
#else
#define HBLE 0
#endif

#if CYBLE_GAP_ROLE_PERIPHERAL
#define DBLE CYBLE_GAP_ROLE_PERIPHERAL
#else
#define DBLE 0
#endif
    
#endif