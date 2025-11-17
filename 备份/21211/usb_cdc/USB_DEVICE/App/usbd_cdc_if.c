/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_cdc_if.c
  * @version        : v2.0_Cube
  * @brief          : Usb device for Virtual Com Port.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_if.h"

/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device library.
  * @{
  */

/** @addtogroup USBD_CDC_IF
  * @{
  */

/** @defgroup USBD_CDC_IF_Private_TypesDefinitions USBD_CDC_IF_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Defines USBD_CDC_IF_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */
/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Macros USBD_CDC_IF_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Variables USBD_CDC_IF_Private_Variables
  * @brief Private variables.
  * @{
  */
/* Create buffer for reception and transmission           */
/* It's up to user to redefine and/or remove those define */
/** Received data over USB are stored in this buffer      */
uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];

/** Data to send over USB CDC are stored in this buffer   */
uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

/* USER CODE BEGIN PRIVATE_VARIABLES */
/* Software double-buffer for Full-Speed 64B packets, with busy flags */
#include "string.h"
static volatile uint32_t s_rxLen = 0U;

/* Two 64-byte TX packet buffers (Full-Speed max packet size) */
static uint8_t s_txBuf[2][CDC_DATA_FS_MAX_PACKET_SIZE];
static uint16_t s_txLen[2] = {0U, 0U};
static volatile uint8_t s_txActiveIdx = 0U;
static volatile uint8_t s_txNextIdx = 1U;
static volatile uint8_t s_txPendingCount = 0U; /* 0..2 queued (including in-flight) */
static volatile uint8_t s_txInFlight = 0U;      /* 1 if a packet has been submitted */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Exported_Variables USBD_CDC_IF_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_FunctionPrototypes USBD_CDC_IF_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t* pbuf, uint32_t *Len);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */

/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS =
{
  CDC_Init_FS,
  CDC_DeInit_FS,
  CDC_Control_FS,
  CDC_Receive_FS
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the CDC media low layer over the FS USB IP
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Init_FS(void)
{
  /* USER CODE BEGIN 3 */
  /* Set Application Buffers */
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
  return (USBD_OK);
  /* USER CODE END 3 */
}

/**
  * @brief  DeInitializes the CDC media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_DeInit_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  Manage the CDC class requests
  * @param  cmd: Command code
  * @param  pbuf: Buffer containing command data (request parameters)
  * @param  length: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
  /* USER CODE BEGIN 5 */
  switch(cmd)
  {
    case CDC_SEND_ENCAPSULATED_COMMAND:

    break;

    case CDC_GET_ENCAPSULATED_RESPONSE:

    break;

    case CDC_SET_COMM_FEATURE:

    break;

    case CDC_GET_COMM_FEATURE:

    break;

    case CDC_CLEAR_COMM_FEATURE:

    break;

  /*******************************************************************************/
  /* Line Coding Structure                                                       */
  /*-----------------------------------------------------------------------------*/
  /* Offset | Field       | Size | Value  | Description                          */
  /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
  /* 4      | bCharFormat |   1  | Number | Stop bits                            */
  /*                                        0 - 1 Stop bit                       */
  /*                                        1 - 1.5 Stop bits                    */
  /*                                        2 - 2 Stop bits                      */
  /* 5      | bParityType |  1   | Number | Parity                               */
  /*                                        0 - None                             */
  /*                                        1 - Odd                              */
  /*                                        2 - Even                             */
  /*                                        3 - Mark                             */
  /*                                        4 - Space                            */
  /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
  /*******************************************************************************/
    case CDC_SET_LINE_CODING:

    break;

    case CDC_GET_LINE_CODING:

    break;

    case CDC_SET_CONTROL_LINE_STATE:

    break;

    case CDC_SEND_BREAK:

    break;

  default:
    break;
  }

  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will issue a NAK packet on any OUT packet received on
  *         USB endpoint until exiting this function. If you exit this function
  *         before transfer is complete on CDC interface (ie. using DMA controller)
  *         it will result in receiving more data while previous ones are still
  *         not sent.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
  /* USER CODE BEGIN 6 */
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);
  /* Forward received data to command RX double-buffer; no heavy work here */
  extern void usb_cmd_rx_push(const uint8_t* data, uint16_t length);
  uint16_t rlen = (Len != NULL) ? (uint16_t)(*Len) : 0U;
  if (rlen != 0U)
  {
    usb_cmd_rx_push(Buf, rlen);
  }
  return (USBD_OK);
  /* USER CODE END 6 */
}

/**
  * @brief  CDC_Transmit_FS
  *         Data to send over USB IN endpoint are sent over CDC interface
  *         through this function.
  *         @note
  *
  *
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 7 */
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;
  if (hcdc->TxState != 0){
    return USBD_BUSY;
  }
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
  result = USBD_CDC_TransmitPacket(&hUsbDeviceFS);
  /* USER CODE END 7 */
  return result;
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */
uint32_t CDC_RxLen_FS(void)
{
  return s_rxLen;
}

void CDC_ClearRxLen_FS(void)
{
  s_rxLen = 0U;
}

/* Kick next queued TX packet if link is idle; return BUSY if still transmitting. */
uint8_t CDC_TxBusy_FS(void)
{
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;
  if (hcdc == NULL)
  {
    return USBD_BUSY;
  }

  if (hcdc->TxState == 0U)
  {
    /* If a previously submitted transfer just completed, account and rotate */
    if (s_txInFlight == 1U)
    {
      s_txInFlight = 0U;
      if (s_txPendingCount > 0U)
      {
        s_txPendingCount--;
      }
      /* Move active index to the buffer that was previously queued */
      s_txActiveIdx ^= 1U;
      s_txNextIdx ^= 1U;
    }

    /* If there is still any pending packet, (re)start it now */
    if (s_txPendingCount > 0U)
    {
      USBD_CDC_SetTxBuffer(&hUsbDeviceFS, s_txBuf[s_txActiveIdx], s_txLen[s_txActiveIdx]);
      if (USBD_CDC_TransmitPacket(&hUsbDeviceFS) == USBD_OK)
      {
        s_txInFlight = 1U;
      }
      else
      {
        return USBD_BUSY;
      }
    }
  }
  return (hcdc->TxState == 0U) ? USBD_OK : USBD_BUSY;
}

/* Enqueue data for FS transmission with 64B packetization using software double buffer.
   - Non-blocking; returns USBD_OK if fully enqueued, USBD_BUSY if queue is full (some tail not queued).
   - Maintains send order; main loop should call CDC_TxBusy_FS() periodically to advance. */
uint8_t CDC_FS_Send(const uint8_t* data, uint16_t length)
{
  if (data == NULL || length == 0U)
  {
    return USBD_OK;
  }

  uint16_t remaining = length;
  const uint8_t* cursor = data;
  uint8_t status = USBD_OK;

  while (remaining > 0U)
  {
    uint16_t chunk = (remaining > CDC_DATA_FS_MAX_PACKET_SIZE) ? CDC_DATA_FS_MAX_PACKET_SIZE : remaining;

    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;
    if (hcdc == NULL)
    {
      return USBD_BUSY;
    }

    /* If link idle and nothing pending, send immediately using active buffer */
    if ((hcdc->TxState == 0U) && (s_txPendingCount == 0U))
    {
      memcpy(s_txBuf[s_txActiveIdx], cursor, chunk);
      s_txLen[s_txActiveIdx] = chunk;
      USBD_CDC_SetTxBuffer(&hUsbDeviceFS, s_txBuf[s_txActiveIdx], s_txLen[s_txActiveIdx]);
      if (USBD_CDC_TransmitPacket(&hUsbDeviceFS) != USBD_OK)
      {
        /* Could not start; treat as busy */
        status = USBD_BUSY;
        break;
      }
      s_txPendingCount = 1U; /* in-flight one */
      s_txInFlight = 1U;
    }
    else
    {
      /* Use the next queue slot if available (max 2 total including in-flight) */
      if (s_txPendingCount >= 2U)
      {
        status = USBD_BUSY;
        break;
      }
      memcpy(s_txBuf[s_txNextIdx], cursor, chunk);
      s_txLen[s_txNextIdx] = chunk;
      /* We only mark as pending; actual transmit will be kicked by CDC_TxBusy_FS when the link is idle */
      s_txPendingCount++;
      /* Rotate next index (ping-pong 0<->1) */
      s_txNextIdx ^= 1U;
      /* If we just queued the second buffer while first is in-flight, do nothing else now */
    }

    cursor += chunk;
    remaining = (uint16_t)(remaining - chunk);

    /* Attempt to advance if possible */
    (void)CDC_TxBusy_FS();

    /* If the in-flight transfer just finished and there is a queued packet in the "other" buffer,
       we need to rotate active index when the USB core clears TxState. This is polled via CDC_TxBusy_FS(). */
  }

  /* If the in-flight packet completed and a queued one exists, rotate indices.
     We detect completion by TxState==0 while we still have pending data not yet started.
     The rotation is applied when we actually start the next transfer in CDC_TxBusy_FS().
     To ensure the active/next indices are consistent after completion, whenever a transfer
     completes (TxState goes 0), we move the active index to the buffer that was previously queued. */

  return status;
}

/* Convenience API: send CRLF */
uint8_t CDC_Transmit_CRLF_FS(void)
{
  static const uint8_t crlf[2] = {'\r','\n'};
  return CDC_FS_Send(crlf, 2U);
}

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */
