/*******************************************************************************
 *  Network Interface file
 *
 *  Summary:
 *   Network Interface file for FreeRTOS-Plus-TCP stack
 *
 *  Description:
 *   - Interfaces PIC32 to the FreeRTOS TCP/IP stack
 *******************************************************************************/

/*******************************************************************************
 *  File Name:  pic32_NetworkInterface.c
 *  Copyright 2017 Microchip Technology Incorporated and its subsidiaries.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *of this software and associated documentation files (the "Software"), to deal
 *in the Software without restriction, including without limitation the rights
 *to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions: The above copyright
 *notice and this permission notice shall be included in all copies or
 *substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *IN THE SOFTWARE
 *******************************************************************************/
#ifndef PIC32_USE_ETHERNET
    #include <sys/kmem.h>

    #include "FreeRTOS.h"
    #include "FreeRTOS_IP.h"
    #include "FreeRTOS_IP_Private.h"
    #include "event_groups.h"
    #include "semphr.h"

    #include "NetworkBufferManagement.h"
    #include "NetworkInterface.h"
    #include "peripheral/eth/plib_eth.h"

    #include "system/command/sys_command.h"
    #include "system/console/sys_console.h"
    #include "system/debug/sys_debug.h"
    #include "system_config.h"

    #include "driver/ethmac/drv_ethmac.h"
    #include "driver/miim/drv_miim.h"
    #include "m2m_types.h"

    #include "tcpip/src/link_list.h"
    #include "tcpip/src/tcpip_private.h"
    #include "tcpip/tcpip.h"
    #include "wilc1000_task.h"

    #include "NetworkConfig.h"

    #include "iot_wifi.h"

/* local definitions and data */

/* FreeRTOS implementation functions */
BaseType_t xNetworkInterfaceInitialise( void )
{
    WIFINetworkParams_t xNetworkParams;

    xNetworkParams.ucSSIDLength = strnlen( clientcredentialWIFI_SSID,
                                           wificonfigMAX_SSID_LEN );
    memcpy( xNetworkParams.ucSSID,
            clientcredentialWIFI_SSID,
            xNetworkParams.ucSSIDLength );

    xNetworkParams.xPassword.xWPA
        .ucLength = strnlen( clientcredentialWIFI_PASSWORD,
                             wificonfigMAX_PASSPHRASE_LEN );
    memcpy( xNetworkParams.xPassword.xWPA.cPassphrase,
            clientcredentialWIFI_PASSWORD,
            xNetworkParams.xPassword.xWPA.ucLength );

    xNetworkParams.xSecurity = clientcredentialWIFI_SECURITY;
    xNetworkParams.ucChannel = M2M_WIFI_CH_ALL; /* Scan all channels (255) */

    /*Turn  WiFi ON */
    if( WIFI_On() != eWiFiSuccess )
    {
        return pdFAIL;
    }

    /* Connect to the AP */
    if( WIFI_ConnectAP( &xNetworkParams ) != eWiFiSuccess )
    {
        return pdFAIL;
    }

    return pdPASS;
}

/*-----------------------------------------------------------*/

BaseType_t xNetworkInterfaceOutput(
    NetworkBufferDescriptor_t * const pxDescriptor,
    BaseType_t xReleaseAfterSend )
{
    BaseType_t retRes = pdFALSE;

    if( ( pxDescriptor != 0 ) && ( pxDescriptor->pucEthernetBuffer != 0 ) &&
        ( pxDescriptor->xDataLength != 0 ) )
    {
        /* There you go */
        if( WDRV_EXT_DataSend( pxDescriptor->xDataLength,
                               pxDescriptor->pucEthernetBuffer ) == 0 )
        {
            retRes = pdTRUE;
        }

        /* The buffer has been sent so can be released. */
        if( xReleaseAfterSend != pdFALSE )
        {
            vReleaseNetworkBufferAndDescriptor( pxDescriptor );
        }
    }

    return retRes;
}

/************************************* Section: helper functions
 * ************************************************** */
/* */

/************************************* Section: worker code
 * ************************************************** */
/* */

void xNetworkFrameReceived( uint32_t len, uint8_t const * const frame )
{
    bool pktSuccess, pktLost;
    NetworkBufferDescriptor_t * pxNetworkBuffer = NULL;
    IPStackEvent_t xRxEvent = { eNetworkRxEvent, NULL };

    pktSuccess = pktLost = false;

    while( true )
    {
        if( eConsiderFrameForProcessing( frame ) != eProcessBuffer )
        {
            break;
        }

        /* get the network descriptor (no data buffer) to hold this packet */
        pxNetworkBuffer = pxGetNetworkBufferWithDescriptor( len, 0 );

        if( pxNetworkBuffer == NULL )
        {
            pktLost = true;
            break;
        }

        /* Set the actual packet length, in case a larger buffer was
         * returned. */
        pxNetworkBuffer->xDataLength = len;

        /* Copy the packet. */
        memcpy( pxNetworkBuffer->pucEthernetBuffer, frame, len );

        /* Send the data to the TCP/IP stack. */
        xRxEvent.pvData = ( void * ) pxNetworkBuffer;

        if( xSendEventStructToIPTask( &xRxEvent, 0 ) == pdFALSE )
        { /* failed */
            pktLost = true;
        }
        else
        { /* success */
            pktSuccess = true;
            iptraceNETWORK_INTERFACE_RECEIVE();
        }

        break;
    }

    if( !pktSuccess )
    { /* something went wrong; nothing sent to the */
        if( pxNetworkBuffer != NULL )
        {
            pxNetworkBuffer->pucEthernetBuffer = 0;
            vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer );
        }

        if( pktLost )
        {
            iptraceETHERNET_RX_EVENT_LOST();
        }
    }
}

#endif /* #ifndef PIC32_USE_ETHERNET */
