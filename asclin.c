// include
#include "IfxAsclin_Asc.h"
//#include "IfxCpu_Irq.h"

#include "Cpu/Irq/IfxCpu_Irq.h"
#include "asclin.h"



// macro
#define UART_BAUDRATE           115200                                  /* UART baud rate in bit/s                  */

#define UART_PIN_RX             IfxAsclin3_RXD_P32_2_IN                 /* UART receive port pin                    */
#define UART_PIN_TX             IfxAsclin3_TX_P15_7_OUT                 /* UART transmit port pin                   */

/* Definition of the interrupt priorities */
#define INTPRIO_ASCLIN3_RX      18
#define INTPRIO_ASCLIN3_TX      19

#define UART_RX_BUFFER_SIZE     64                                      /* Definition of the receive buffer size    */
#define UART_TX_BUFFER_SIZE     64                                      /* Definition of the transmit buffer size   */
#define SIZE                    13                                      /* Size of the string                       */

// global variable
/* Declaration of the ASC handle */
static IfxAsclin_Asc g_ascHandle;

/* Declaration of the FIFOs parameters */
static uint8 g_ascTxBuffer[UART_TX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];
static uint8 g_ascRxBuffer[UART_RX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];


/* Adding of the interrupt service routines */
IFX_INTERRUPT(asclin3TxISR, 0, INTPRIO_ASCLIN3_TX);
void asclin3TxISR(void)
{
    IfxAsclin_Asc_isrTransmit(&g_ascHandle);
}

IFX_INTERRUPT(asclin3RxISR, 0, INTPRIO_ASCLIN3_RX);
void asclin3RxISR(void)
{
    IfxAsclin_Asc_isrReceive(&g_ascHandle);
}

/* This function initializes the ASCLIN UART module */
void init_ASCLIN_UART(void)
{
    /* Initialize an instance of IfxAsclin_Asc_Config with default values */
    IfxAsclin_Asc_Config ascConfig;
    IfxAsclin_Asc_initModuleConfig(&ascConfig, &MODULE_ASCLIN3);

    /* Set the desired baud rate */
    ascConfig.baudrate.baudrate = UART_BAUDRATE;

    /* ISR priorities and interrupt target */
    ascConfig.interrupt.txPriority = INTPRIO_ASCLIN3_TX;
    ascConfig.interrupt.rxPriority = INTPRIO_ASCLIN3_RX;
    ascConfig.interrupt.typeOfService = IfxCpu_Irq_getTos(IfxCpu_getCoreIndex());

    /* FIFO configuration */
    ascConfig.txBuffer = &g_ascTxBuffer;
    ascConfig.txBufferSize = UART_TX_BUFFER_SIZE;
    ascConfig.rxBuffer = &g_ascRxBuffer;
    ascConfig.rxBufferSize = UART_RX_BUFFER_SIZE;

    /* Pin configuration */
    const IfxAsclin_Asc_Pins pins =
    {
        NULL_PTR,       IfxPort_InputMode_pullUp,     /* CTS pin not used */
        &UART_PIN_RX,   IfxPort_InputMode_pullUp,     /* RX pin           */
        NULL_PTR,       IfxPort_OutputMode_pushPull,  /* RTS pin not used */
        &UART_PIN_TX,   IfxPort_OutputMode_pushPull,  /* TX pin           */
        IfxPort_PadDriver_cmosAutomotiveSpeed1
    };
    ascConfig.pins = &pins;

    IfxAsclin_Asc_initModule(&g_ascHandle, &ascConfig); /* Initialize module with above parameters */
}


// for debug
void tx_uart_pc_debug(const char *format, ...)
{
    char debugBuf[256];
    va_list args;
    va_start(args, format);
    vsnprintf(debugBuf, sizeof(debugBuf), format, args);
    va_end(args);
    Ifx_SizeT len = (Ifx_SizeT)strlen(debugBuf);

    IfxAsclin_Asc_write(&g_ascHandle, (const void *)debugBuf, &len, TIME_INFINITE);
    for (volatile int i = 0; i < 100000; i++);
}
