#include "mbed.h"
#include "S6D0151_TFT.h"
#include "SPI_MX25R.h"
#include "exfonts.h"
#include "MACROS.h"
#include "MODSERIAL.h"

#include "font.h"

DigitalOut myled(LED1);

S6D0151_TFT TFT(P0_9, NC, P0_10, P0_2, P0_11, "TFT"); // mosi, miso, sclk, cs, reset
SPI_MX25R spi_mem(P0_21, P0_22, P1_15, P1_19);
exfonts ft;

MODSERIAL USART(P0_19, P0_18, 512);   // Pin 32, 31

int x = 0;
int y = 0;
int off = 0;
int line = 0;

void puthex(int n)
{
    if( n < 10)
        TFT.character(0, 0, n + '0');
    else
        TFT.character(0, 0, n  - 10 + 'a');
}

void bitdisp(uint8_t d) {
  for (byte i=0; i<8;i++) {
    if (d & 0x80>>i) 
      TFT.pixel(x + off, y, White);
    ++x;
  }
}

int printutf8(int _x, int _y, char *c)
{
    int i, j, k;
    ft.setFontSize(EXFONT16);
    int n;
    uint16_t pUTF16[256];
    n = exfonts::Utf8ToUtf16(pUTF16, c);  // UTF8からUTF16に変換する
    x = _x;
    off = 0;
    line = 0;

    for(k = 0;k < n; ++k) {
        y = _y + (ft.getHeight() + 2) * line;
        byte buf[MAXFONTLEN]; 
        ft.getFontData(buf,pUTF16[k]);        // フォントデータの取得
        byte bn= ft.getRowLength();   // 1行当たりのバイト数取得

        for (i = 0; i < ft.getLength(); i += bn ) {
            for (j = 0; j < bn; j++) {
                bitdisp(buf[i+j]);
            }
            x = _x;
            ++y;
        }
        off += bn * 8;
        if (_x + off > TFT.width() - ft.getWidth() - 2) {
            x = _x;
            y = _y;
            off = 0;
            ++line;
        }
    }

    return line + 1;
}
 
int main() {
    int i, j;
    char title[128];
    char artist[128];

    USART.baud(115200);

    ft.init(spi_mem);

    TFT.cls();
    TFT.set_font(Arial_Narrow8x12);

    uint32_t id = spi_mem.readREMS();
    for(i = 0; i < 4; ++i) {
        puthex((id >> (4 * (3 - i))) & 0xf);
    }
    TFT.character(0, 0, ':');
    id = spi_mem.readRES();
    for(i = 0; i < 4; ++i) {
        puthex((id >> (4 * (3 - i))) & 0xf);
    }

    TFT.character(0, 0, ' ');
    for(j = 0; j < 4; ++j) {
        uint32_t mem = spi_mem.read8(j);
        for(i = 0; i < 2; ++i) {
            puthex((mem >> (4 * (1 - i))) & 0xf);
        }
        TFT.character(0, 0, ' ');
    }

    char *c ="猫にコ・ン・バ・ン・ワさんの漢字フォントライブラリを利用してます。";
    printutf8(5, 30, c);

    int n = 0;
    char buf[1024];
    char ch;
    while(1) {
        if(!USART.readable())
            continue;
        ch = USART.getc();
        if (ch == '\n') {
            buf[n] = 0;
            myled = 1;
            if (strncmp(buf, "ICY-META:", 9) == 0 && buf[n - 2] == ';') {
                TFT.cls();
                buf[n - 3] = 0;
                printutf8(5, 5, buf + 23);
            } else if (strncmp(buf + 1, "Title:", 6) == 0) {   // id3v2
                if ((n - 10) < sizeof(title)) {
                    buf[n - 1] = 0;
                    strcpy(title, buf + 10);
                }
            } else if (strncmp(buf + 1, "Artist:", 7) == 0) {   // id3v2
                if ((n - 10) < sizeof(artist)) {
                    buf[n - 1] = 0;
                    strcpy(artist, buf + 10);
                }
            } else if (strncmp(buf + 1, "Album:", 6) == 0) {   // id3v2
                if (buf[n - 1] == '\r') {
                    TFT.cls();
                    buf[n - 1] = 0;
                    i = printutf8(5, 5, buf + 10);
                    j = printutf8(5, 5 + (ft.getHeight() + 2) * i, title);
                    printutf8(5, 5 + (ft.getHeight() + 2) * (i + j), artist);
                }
                title[0] = 0;
                artist[0] = 0;
            }
            n = 0;
            myled = 0;
        } else {
            if (n < sizeof(buf) - 1) {
                buf[n] = ch;
                ++n;
            }
        }
    }
}
