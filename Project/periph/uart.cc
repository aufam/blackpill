#include "periph/uart.h"

using namespace Project::periph;

static UART* select(UART_HandleTypeDef *huart) {
    if (huart->Instance == uart2.huart.Instance) return &uart2;
    return nullptr;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    auto uart = select(huart);
    if (uart == nullptr)
        return;

    uart->rxCallback(uart->rxBuffer.data(), Size);
    uart->init();
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    auto uart = select(huart);
    if (uart == nullptr)
        return;

    uart->txCallback();
}