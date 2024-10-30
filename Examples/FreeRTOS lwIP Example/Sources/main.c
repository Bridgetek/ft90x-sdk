/**
  @file main.c
  @brief
  FreeRTOS project template.

*/
/*
 * ============================================================================
 * (C) Copyright Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 * ("Bridgetek") subject to the licence terms set out
 * http://brtchip.com/BRTSourceCodeLicenseAgreement/("the Licence Terms").
 * You must read the Licence Terms before downloading or using the Software.
 * By installing or using the Software you agree to the Licence Terms. If you
 * do not agree to the Licence Terms then do not download or use the Software.
 *
 * Without prejudice to the Licence Terms, here is a summary of some of the key
 * terms of the Licence Terms (and in the event of any conflict between this
 * summary and the Licence Terms then the text of the Licence Terms will
 * prevail).
 *
 * The Software is provided "as is".
 * There are no warranties (or similar) in relation to the quality of the
 * Software. You use it at your own risk.
 * The Software should not be used in, or for, any medical device, system or
 * appliance. There are exclusions of Bridgetek liability for certain types of loss
 * such as: special loss or damage; incidental loss or damage; indirect or
 * consequential loss or damage; loss of income; loss of business; loss of
 * profits; loss of revenue; loss of contracts; business interruption; loss of
 * the use of money or anticipated savings; loss of information; loss of
 * opportunity; loss of goodwill or reputation; and/or loss of, damage to or
 * corruption of data.
 * There is a monetary cap on Bridgetek's liability.
 * The Software may have subsequently been amended by another user and then
 * distributed by that other user ("Adapted Software").  If so that user may
 * have additional licence terms that apply to those amendments. However, Bridgetek
 * has no liability in relation to those amendments.
 * ============================================================================
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "ft900.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "list.h"

static SemaphoreHandle_t xMutex = NULL;
static TaskHandle_t task[2] = {NULL};

static void frt_demo_setup(void);
static void prvPrintTask(void *pvParameters);
static void prvNewPrintString(portCHAR *pcString);

int main(void)
{
  /* Peripheral reset */
  sys_reset_all();

  /* enable uart */
  sys_enable(sys_device_uart0);

  /* Set UART0 GPIO functions to UART0_TXD and UART0_RXD... */
#if defined(__FT900__)
  gpio_function(48, pad_uart0_txd); /* UART0 TXD */
  gpio_function(49, pad_uart0_rxd); /* UART0 RXD */
#elif defined(__FT930__)
  gpio_function(23, pad_uart0_txd); /* UART0 TXD */
  gpio_function(22, pad_uart0_rxd); /* UART0 RXD */
#endif

  uart_open(UART0,                   /* Device */
            1,                       /* Prescaler = 1 */
            UART_DIVIDER_19200_BAUD, /* Divider = 1302 */
            uart_data_bits_8,        /* No. Data Bits */
            uart_parity_none,        /* Parity */
            uart_stop_bits_1);       /* No. Stop Bits */

  /* Print out a welcome message... */
  uart_puts(UART0,
            "\x1B[2J" /* ANSI/VT100 - Clear the Screen */
            "\x1B[H"  /* ANSI/VT100 - Move Cursor to Home */
            "Copyright (C) Bridgetek Pte Ltd \r\n"
            "--------------------------------------------------------------------- \r\n"
            "Welcome to Free RTOS Template... \r\n"
            "--------------------------------------------------------------------- \r\n\r\n");
  frt_demo_setup();

  printf("Should never reach here!\n");
  for (;;)
  {
    // Do nothing
  };
}

static void prvPrintTask(void *pvParameters)
{
  char *pcStringToPrint;
  int random = 0;

  pcStringToPrint = (char *)pvParameters;
  for (int i = 0;; i++)
  {
    random = rand() & 0x1FF;
    prvNewPrintString(pcStringToPrint);
#if (INCLUDE_vTaskDelay == 1)
    vTaskDelay(random);
#endif
  }
}

static void prvNewPrintString(portCHAR *pcString)
{
  xSemaphoreTake(xMutex, portMAX_DELAY);
  printf("\r\n%s\r\n", pcString);
  xSemaphoreGive(xMutex);
}

static void frt_demo_setup(void)
{
  srand(567);

#if ((configUSE_MUTEXES == 1) && (configSUPPORT_DYNAMIC_ALLOCATION == 1))
  xMutex = xSemaphoreCreateMutex();
#endif
  if (xMutex != NULL)
  {
#if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
    uint16_t stackDepth = 1024; // equivalent to 1024*4

    xTaskCreate(prvPrintTask, "Print0", stackDepth,
                "Print0 oooooooooooooooooooooooooooooooooooooooooo\r\n", 1, &task[0]);

    xTaskCreate(prvPrintTask, "Print1", stackDepth,
                "Print1 ******************************************\r\n", 1, &task[1]);
#endif
  }

  // Start the scheduler so the created tasks start executing.
  vTaskStartScheduler();
}
