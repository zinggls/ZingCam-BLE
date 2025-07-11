/*******************************************************************************
* File Name: basc.h
*
* Version 1.0
*
* Description:
*  Contains the function prototypes and constants available to the example
*  project.
*
********************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "main.h"


/***************************************
*       Function Prototypes
***************************************/
void BasInit(void);
void BasCallBack(uint32 event, void *eventParam);


/***************************************
* External data references
***************************************/
extern uint8 basNotification;


/* [] END OF FILE */
