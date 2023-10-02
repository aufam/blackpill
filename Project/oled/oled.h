#ifndef PROJECT_OLED_OLED_H
#define PROJECT_OLED_OLED_H

#include "periph/i2c.h"
#include "oled/SSD1306init.h"
#include "oled/fonts/allFonts.h"

namespace Project {

    struct Oled {
        enum { ID_CMD = 0x00u, ID_DATA = 0x40u };
        typedef const uint8_t *Font;
        typedef const DevType &DeviceType;
        inline static const etl::Time timeout = etl::time::milliseconds(100);

        periph::I2C &i2c;
        uint8_t slaveAddr = 0x78;      
        Font font = Adafruit5x7;                 ///< see fonts/allFonts.h
        DeviceType device = Adafruit128x64;      ///< see SSD1306init.h
        uint8_t column = 0, row = 0;

        void init();    ///< write initial commands and clear the screen
        void deinit();  ///< 

        int writeCmd(uint8_t cmd) const; ///< write command
        int writeData(uint8_t *data, uint16_t len); ///< write data to current column and row

        struct ClearArgs { int columnStart = 0, rowStart = 0, columnEnd = 0xff, rowEnd = 0xff; bool invertColor = false; };

        /// clear selected region and set cursor to columnStart and rowStart
        /// @param args
        ///     - .columnStart 
        ///     - .rowStart 
        ///     - .columnEnd
        ///     - .rowEnd
        ///     - .invertColor
        void clear(ClearArgs args);

        void clear() { clear({}); }

        void clearRemainingRows(bool invertColor = false) {
            clear({
                .columnStart=column, 
                .rowStart=row, 
                .columnEnd=screenWidth() - 1, 
                .rowEnd=screenRows() - 1, 
                .invertColor=invertColor,
            });
        }

        /// set column: 0 to screen_width - 1
        void setColumn(int newColumn);

        /// set row: 0 to (screen_height / 8) - 1
        void setRow(int newRow);

        struct SetCursorArgs { int newColumn, newRow; };

        /// set column and row
        /// @param args
        ///     - .newColumn
        ///     - .newRow
        void setCursor(SetCursorArgs args) {
            setColumn(args.newColumn);
            setRow(args.newRow);
        }
        /// print one char
        /// @retval 0 = success, -1 = error font is null, -2 = column exceeds screen width, 1 = ch is not in font
        int print(char ch, bool invertColor = false);

        /// print string terminated with '\0'
        /// @retval 0 = success, -1 = error font is null
        int print(const char *str, bool invertColor = false, uint8_t columnStart = 0xFF, uint8_t rowStart = 0xFF);

        /// print operator
        Oled &operator<<(const char *str) { 
            print(str); 
            return *this; 
        }

        /// overload
        Oled &operator<<(char ch) {
            if (print(ch) != -2) return *this;
            if (row + fontRows() >= screenRows()) return *this; // last row
            setCursor({.newColumn=0, .newRow=row + fontRows()});
            return *this;
        }

        int screenWidth() const     { return device.lcdWidth; }
        int screenHeight() const    { return device.lcdHeight; }
        int screenRows() const      { return screenHeight() / 8; }
        int columnOffset() const    { return device.colOffset; }

        /// set new font, see fonts/allFonts.h
        void setFont(Font newFont)      { this->font = newFont; }

        /// font height in pixels
        int fontHeight() const      { return font ? font[FONT_HEIGHT] : 0; }
        
        /// font width in pixels
        int fontWidth() const       { return font ? font[FONT_WIDTH] : 0; }
        
        /// font height in rows, 1 row is 8 pixels roundup
        int fontRows() const        { return font ? (font[FONT_HEIGHT] + 7) / 8 : 0; }
        
        /// all characters size, 0 if fixed font width, 1 if letter spacing is 0
        int fontSize() const       { return font ? *(uint16_t *) font : 0; }
        
        /// first character in the font
        char fontFirstChar() const      { return font ? font[FONT_FIRST_CHAR] : 0; }
        
        /// number of chars in the font
        int fontCharCount() const   { return font ? font[FONT_CHAR_COUNT] : 0; }
    };

} // namespace Project


#endif // PROJECT_OLED_OLED_H