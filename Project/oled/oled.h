#ifndef PROJECT_OLED_OLED_H
#define PROJECT_OLED_OLED_H

#include "periph/i2c.h"
#include "oled/SSD1306init.h"
#include "oled/fonts/allFonts.h"

namespace Project {

    /// OLED driver (text only). requirements: I2C class, I2C clock speed 400k
    struct Oled {
        enum { ID_CMD = 0x00u, ID_DATA = 0x40u };
        typedef const uint8_t *Font;
        typedef const DevType &DeviceType;

        Periph::I2C &i2c;
        Font font; ///< see fonts/allFonts.h
        DeviceType device; ///< see SSD1306init.h
        uint8_t slaveAddr;
        uint8_t column, row;

        constexpr explicit Oled(Periph::I2C &i2c,
                                Font font = Adafruit5x7,
                                DeviceType device = Adafruit128x64,
                                uint8_t slaveAddr = 0x78)
        : i2c(i2c)
        , font(font)
        , device(device)
        , slaveAddr(slaveAddr)
        , column(0), row(0) {}

        void init(); ///< write initial commands and clear the screen
        void deinit();

        int writeCmd(uint8_t cmd) const; ///< write command
        int writeData(uint8_t *data, uint16_t len); ///< write data to current column and row

        /// clear selected region and set cursor to columnStart and rowStart
        void clear(uint8_t columnStart = 0, uint8_t rowStart = 0,
                   uint8_t columnEnd = 0xFF, uint8_t rowEnd = 0xFF, bool invertColor = false);
        void clearRemainingRows(bool invertColor = false) {
            clear(column, row, screenWidth() - 1, screenRows() - 1, invertColor);
        }
        /// set column: 0 to screen_width - 1
        void setColumn(uint8_t newColumn);
        /// set row: 0 to (screen_height / 8) - 1
        void setRow(uint8_t newRow);

        /// set column and row
        void setCursor(uint8_t newColumn, uint8_t newRow) {
            setColumn(newColumn);
            setRow(newRow);
        }
        /// print one char
        /// @retval 0 = success, -1 = error font is null, -2 = column exceeds screen width, 1 = ch is not in font
        int print(char ch, bool invertColor = false);
        /// print string terminated with '\0'
        /// @retval 0 = success, -1 = error font is null
        int print(const char *str, bool invertColor = false, uint8_t columnStart = 0xFF, uint8_t rowStart = 0xFF);

        Oled &operator<<(const char *str)   { print(str); return *this; }
        Oled &operator<<(char ch) {
            if (print(ch) != -2) return *this;
            if (row + fontRows() >= screenRows()) return *this; // last row
            setCursor(0, row + fontRows());
            return *this;
        }

        uint8_t screenWidth() const     { return device.lcdWidth; }
        uint8_t screenHeight() const    { return device.lcdHeight; }
        uint8_t screenRows() const      { return screenHeight() / 8; }
        uint8_t columnOffset() const    { return device.colOffset; }

        /// set new font, see fonts/allFonts.h
        void setFont(Font newFont)      { this->font = newFont; }
        /// font height in pixels
        uint8_t fontHeight() const      { return font ? font[FONT_HEIGHT] : 0; }
        /// font width in pixels
        uint8_t fontWidth() const       { return font ? font[FONT_WIDTH] : 0; }
        /// font height in rows, 1 row is 8 pixels roundup
        uint8_t fontRows() const        { return font ? (font[FONT_HEIGHT] + 7) / 8 : 0; }
        /// all characters size, 0 if fixed font width, 1 if letter spacing is 0
        uint16_t fontSize() const       { return font ? *(uint16_t *) font : 0; }
        /// first character in the font
        char fontFirstChar() const      { return font ? font[FONT_FIRST_CHAR] : 0; }
        /// number of chars in the font
        uint8_t fontCharCount() const   { return font ? font[FONT_CHAR_COUNT] : 0; }
    };

} // namespace Project


#endif // PROJECT_OLED_OLED_H