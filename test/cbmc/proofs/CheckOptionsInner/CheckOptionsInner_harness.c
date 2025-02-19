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

#include "cbmc.h"

/****************************************************************
 * Signature of function under test
 ****************************************************************/

void __CPROVER_file_local_FreeRTOS_TCP_Reception_c_prvReadSackOption(
    const uint8_t * const pucPtr,
    size_t uxIndex,
    FreeRTOS_Socket_t * const pxSocket );

/****************************************************************
 * Proof of prvReadSackOption function contract
 ****************************************************************/

void harness()
{
    /* pucPtr points into a buffer */
    size_t buffer_size;
    uint8_t * pucPtr = malloc( buffer_size );

    __CPROVER_assume( pucPtr != NULL );

    /* uxIndex in an index into the buffer */
    size_t uxIndex;

    /* pxSocket can be any socket with some initialized values */
    FreeRTOS_Socket_t * pxSocket = malloc( sizeof( FreeRTOS_Socket_t ) );

    __CPROVER_assume( pxSocket != NULL );

    pxSocket->u.xTCP.txStream = malloc( sizeof( StreamBuffer_t ) );
    __CPROVER_assume( pxSocket->u.xTCP.txStream != NULL );

    vListInitialise( &pxSocket->u.xTCP.xTCPWindow.xWaitQueue );

    if( nondet_bool() )
    {
        TCPSegment_t * segment = malloc( sizeof( TCPSegment_t ) );
        __CPROVER_assume( segment != NULL );
        listSET_LIST_ITEM_OWNER( &segment->xQueueItem, ( void * ) segment );
        vListInsertEnd( &pxSocket->u.xTCP.xTCPWindow.xWaitQueue,
                        &segment->xQueueItem );
    }

    vListInitialise( &pxSocket->u.xTCP.xTCPWindow.xTxSegments );

    if( nondet_bool() )
    {
        TCPSegment_t * segment = malloc( sizeof( TCPSegment_t ) );
        __CPROVER_assume( segment != NULL );
        vListInitialiseItem( &segment->xSegmentItem );
        listSET_LIST_ITEM_OWNER( &segment->xQueueItem, ( void * ) segment );
        vListInsertEnd( &pxSocket->u.xTCP.xTCPWindow.xTxSegments,
                        &segment->xQueueItem );
    }

    vListInitialise( &pxSocket->u.xTCP.xTCPWindow.xPriorityQueue );

    extern List_t xSegmentList;

    vListInitialise( &xSegmentList );

    /****************************************************************
     * Specification and proof of CheckOptions inner loop
     ****************************************************************/

    /* Preconditions */

    /* CBMC model of pointers limits the size of the buffer */
    __CPROVER_assume( buffer_size < CBMC_MAX_OBJECT_SIZE );

    /* Both preconditions are required to avoid integer overflow in the */
    /* pointer offset of the pointer pucPtr + uxIndex + 8 */
    __CPROVER_assume( uxIndex <= buffer_size );
    __CPROVER_assume( uxIndex + 8 <= buffer_size );

    /* Assuming quite a bit more about the initialization of pxSocket */
    __CPROVER_assume( pucPtr != NULL );
    __CPROVER_assume( pxSocket != NULL );

    __CPROVER_file_local_FreeRTOS_TCP_Reception_c_prvReadSackOption( pucPtr,
                                                                     uxIndex,
                                                                     pxSocket );

    /* No postconditions required */
}
