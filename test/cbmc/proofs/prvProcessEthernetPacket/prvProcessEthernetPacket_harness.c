/* Standard includes. */
#include <stdint.h>
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_ARP.h"
#include "FreeRTOS_DHCP.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_UDP_IP.h"
#include "NetworkBufferManagement.h"
#include "NetworkInterface.h"

#include "freertos_api.c"
#include "memory_assignments.c"

/****************************************************************
 * Signature of function under test
 ****************************************************************/
void __CPROVER_file_local_FreeRTOS_IP_c_prvProcessEthernetPacket(
    NetworkBufferDescriptor_t * const pxNetworkBuffer );

/* This function has been proved to be memory safe in another proof (in
 * ARP/ARPRefreshCacheEntry). Hence we assume it to be correct here. */
void vARPRefreshCacheEntry( const MACAddress_t * pxMACAddress,
                            const uint32_t ulIPAddress )
{
    /* pxMACAddress can be NULL or non-NULL. No need to assert. */
}

/* This function has been proved to be memory safe in another proof (in
 * ARP/ARPProcessPacket). Hence we assume it to be correct here. */
eFrameProcessingResult_t eARPProcessPacket( ARPPacket_t * const pxARPFrame )
{
    __CPROVER_assert( pxARPFrame != NULL, "pxARPFrame cannot be NULL" );

    eFrameProcessingResult_t eReturn;

    return eReturn;
}

/* This function has been proved to be memory safe in another proof (in
 * parsing/ProcessIPPacket). Hence we assume it to be correct here. */
eFrameProcessingResult_t __CPROVER_file_local_FreeRTOS_IP_c_prvProcessIPPacket(
    IPPacket_t * pxIPPacket,
    NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
    __CPROVER_assert( pxIPPacket != NULL, "pxIPPacket cannot be NULL" );
    __CPROVER_assert( pxNetworkBuffer != NULL,
                      "pxNetworkBuffer cannot be NULL" );

    eFrameProcessingResult_t result;

    return result;
}

BaseType_t NetworkInterfaceOutputFunction_Stub(
    struct xNetworkInterface * pxDescriptor,
    NetworkBufferDescriptor_t * const pxNetworkBuffer,
    BaseType_t xReleaseAfterSend )
{
    BaseType_t xRet;

    __CPROVER_assert( pxDescriptor != NULL,
                      "The network interface cannot be NULL." );
    __CPROVER_assert( pxNetworkBuffer != NULL,
                      "The network buffer descriptor cannot be NULL." );
    __CPROVER_assert( pxNetworkBuffer->pucEthernetBuffer != NULL,
                      "The Ethernet buffer cannot be NULL." );

    return xRet;
}

void harness()
{
    NetworkBufferDescriptor_t * const
        pxNetworkBuffer = pxGetNetworkBufferWithDescriptor(
            ipTOTAL_ETHERNET_FRAME_SIZE,
            0 );

    /* The network buffer cannot be NULL for this function call. If it is, it
     * will hit an assert in the function. */
    __CPROVER_assume( pxNetworkBuffer != NULL );

    /* Add an endpoint */
    pxNetworkBuffer->pxEndPoint = ( NetworkEndPoint_t * ) malloc(
        sizeof( NetworkEndPoint_t ) );
    __CPROVER_assume( pxNetworkBuffer->pxEndPoint != NULL );

    /* Interface init. */
    pxNetworkBuffer->pxEndPoint->pxNetworkInterface = ( NetworkInterface_t * )
        malloc( sizeof( NetworkInterface_t ) );
    __CPROVER_assume( pxNetworkBuffer->pxEndPoint->pxNetworkInterface != NULL );
    pxNetworkBuffer->pxEndPoint->pxNetworkInterface
        ->pfOutput = NetworkInterfaceOutputFunction_Stub;

    __CPROVER_file_local_FreeRTOS_IP_c_prvProcessEthernetPacket(
        pxNetworkBuffer );
}
