#include "periph/uart.h"

namespace Project::Periph {

    void UART::init(Callback::Function rxCBFn, void *rxCBArg, Callback::Function txCBFn, void *txCBArg) {
        setRxCallback(rxCBFn, rxCBArg);
        setTxCallback(txCBFn, txCBArg);
        HAL_UARTEx_ReceiveToIdle_DMA(&huart, rxBuffer.begin(), Buffer::len());
        __HAL_DMA_DISABLE_IT(huart.hdmarx, DMA_IT_HT);
    }

    void UART::deinit() {
        HAL_UART_DMAStop(&huart);
    }

    int UART::writeBlocking(uint8_t *buf, uint16_t len) {
        while (huart.gState != HAL_UART_STATE_READY);
        return HAL_UART_Transmit(&huart, buf, len, HAL_MAX_DELAY);
    }

    int UART::write(uint8_t *buf, uint16_t len) {
        return HAL_UART_Transmit_IT(&huart, buf, len);
    }

    void UART::setBaudRate(uint32_t baud) {
        huart.Init.BaudRate = baud;
        HAL_UART_Init(&huart);
    }

    uint32_t UART::getBaudRate() const {
        return huart.Init.BaudRate;
    }

} // namespace Project

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    using namespace Project::Periph;
    UART *uart;
    if (huart->Instance == uart2.huart.Instance) uart = &uart2;
    else return;

    auto &cb = uart->rxCallback;
    if (cb.fn) cb.fn(cb.arg, Size);
    HAL_UARTEx_ReceiveToIdle_DMA(&uart->huart, uart->rxBuffer.begin(), UART::Buffer::len());
    __HAL_DMA_DISABLE_IT(uart->huart.hdmarx, DMA_IT_HT);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    using namespace Project::Periph;
    UART *uart;
    if (huart->Instance == uart2.huart.Instance) uart = &uart2;
    else return;

    auto &cb = uart->txCallback;
    if (cb.fn) cb.fn(cb.arg, 0);
}