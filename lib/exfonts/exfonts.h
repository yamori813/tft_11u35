//
// フォント利用ライブラリ クラス定義 exfonts.h 
// 作成 2014/10/20  by Tamakitchi
// 修正 2015/01/24  by Tamakitchi
// 修正 2017/11/18  by Tamakitchi,init()でcsピン指定可能に修正
// 修正 2017/11/18  by Tamakitchi,Utf8ToUtf16()の戻り値をint16_t型に修正
// 修正 2018/07/14  by Tamakitchi,init()でSPIオブジェクト、SPIクロック周波数指定可能に修正

#ifndef ___exfonts_h___
 #define ___exfonts_h___

#ifdef arudino
#include <Arduino.h>
#include <SPI.h>
#include <W25Q64.h>
#else
#include <stdint.h>
#include "SPI_MX25R.h"
typedef uint8_t byte;
typedef bool boolean;
#endif

#define EXFONTNUM  14    // 登録フォント数
#define FULL_OFST   7    // フォントサイズから全角フォント種類変換用オフセット値
#define MAXFONTLEN  72   // 最大フォントバイトサイズ(=24x24フォント)
#define MAXSIZETYPE 7    // フォントサイズの種類数

// フォントサイズ
#define  EXFONT8    0    // 8ドット美咲フォント
#define  EXFONT10   1    // 10ドット nagaフォント
#define  EXFONT12   2    // 12ドット東雲フォント
#define  EXFONT14   3    // 14ドット東雲フォント
#define  EXFONT16   4    // 16ドット東雲フォント
#define  EXFONT20   5    // 20ドットJiskanフォント
#define  EXFONT24   6    // 24ドットXフォント

// フォント管理テーブル
typedef struct FontInfo {
  uint32_t    idx_addr;  // インデックス格納先頭アドレス
  uint32_t    dat_addr;  // フォントデータ格納先頭アドレス
  uint16_t    f_num;     // フォント格納数
  uint8_t     b_num;     // フォントあたりのバイト数
  uint8_t     w;         // 文字幅
  uint8_t     h;         // 文字高さ
} FontInfo;

class exfonts {
  // メンバー変数
  private:
    uint8_t _fontNo;      // 利用フォント種類
    uint8_t _fontSize;    // 利用フォントサイズ
    SPI_MX25R *spi_mem;

  // メンバー関数
  public:
    exfonts() {                                         // コンストラクタ
      _fontSize=EXFONT8; 
      _fontNo =EXFONT8+FULL_OFST;
    };
  
#ifdef arduino
    void init(uint8_t cs=10,                            // 初期化
              SPIClass& rSPI =SPI,
              uint32_t frq=8000000); 
#else
    void init(SPI_MX25R& spi_mem); // 初期化
#endif
    void setFontSize(uint8_t sz);                       // 利用サイズの設定
    uint8_t getFontSize();                              // 現在利用フォントサイズの取得      
    boolean getFontData(byte* fontdata,uint16_t utf16); // サイズに該当するフォントデータの取得
    char*   getFontData(byte* fontdata,char *pUTF8);    // 指定したUTF8文字列の先頭のフォントデータの取得
    uint8_t getRowLength();                             // 1行のバイト数
    uint8_t getWidth();                                 // 現在利用フォントの幅の取得
    uint8_t getHeight();                                // 現在利用フォントの高さの取得
    uint8_t getLength();                                // 現在利用フォントのデータサイズ
 
  private:
    void setFontNo(uint8_t fno);                        // 利用フォント種類の設定 fno : フォント種別番号 (0-13)
    uint8_t getFontNo();                                // 現在の利用フォント種類の取得
    uint16_t read_code(uint16_t pos);                   // ROM上検索テーブルのフォントコードを取得する
    int16_t findcode(uint16_t  ucode);                  // UTF16コードに該当するテーブル上のフォントコードを取得する
    uint16_t hkana2kana(uint16_t ucode);                // 半角カナ全角変換 JISX0208 -> UTF16の不整合対応
    uint16_t hkana2uhkana(uint16_t ucode);              // UTF半角カナ半角utf16コード変換 JISX0208 -> UTF16の不整合対応
    boolean getFontDataByUTF16(byte* fontdata, uint16_t utf16); // 種類に該当するフォントデータの取得
    FontInfo const * getFontInfo();               // フォント情報の取得
    uint8_t isHkana(uint16_t ucode);                    // 半角カナ判定

 // クラスメンバ関数
 public:
   static byte charUFT8toUTF16(char *pUTF8, uint16_t *pUTF16);   // UTF8文字(1～3バイト)をUTF16に変換する
   static int16_t Utf8ToUtf16(uint16_t* pUTF16, char *pUTF8);       // UTF8文字列をUTF16文字列に変換す
};
#endif
