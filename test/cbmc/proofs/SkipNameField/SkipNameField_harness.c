/* Standard includes. */
#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "list.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_DNS.h"
#include "FreeRTOS_DNS_Parser.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_UDP_IP.h"
#include "IPTraceMacroDefaults.h"
#include "NetworkBufferManagement.h"
#include "NetworkInterface.h"

#include "cbmc.h"

/****************************************************************
 * Signature of function under test
 ****************************************************************/

size_t DNS_SkipNameField( const uint8_t * pucByte, size_t uxLength );

/****************************************************************
 * Proof of DNS_SkipNameField function contract
 ****************************************************************/

void harness()
{
    __CPROVER_assert( NETWORK_BUFFER_SIZE < CBMC_MAX_OBJECT_SIZE,
                      "NETWORK_BUFFER_SIZE < CBMC_MAX_OBJECT_SIZE" );

    size_t uxLength;
    uint8_t * pucByte = malloc( uxLength );

    /* Preconditions */

    __CPROVER_assume( uxLength < CBMC_MAX_OBJECT_SIZE );
    __CPROVER_assume( uxLength <= NETWORK_BUFFER_SIZE );
    __CPROVER_assume( pucByte != NULL );

    size_t index = DNS_SkipNameField( pucByte, uxLength );

    /* Postconditions */

    __CPROVER_assert( index <= uxLength,
                      "DNS_SkipNameField: index <= uxLength" );
}
