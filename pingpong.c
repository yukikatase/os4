#define KBD_STATUS    0x64
#define KBD_DATA      0x60
#define KEY_UP_BIT    0x80
#define SCREEN_WIDTH  320

char* pptr = (char *)(0xa0000 + SCREEN_WIDTH - 10);
char* pptr2 = (char *)(0xa0000 + SCREEN_WIDTH * 2 - 10);
char* pptr3 = (char *)(0xa0000 + SCREEN_WIDTH * 3 - 10);
char* pptr4 = (char *)(0xa0000 + SCREEN_WIDTH * 4 - 10);
char* pptr5 = (char *)(0xa0000 + SCREEN_WIDTH * 5 - 10);
char* pptr6 = (char *)(0xa0000 + SCREEN_WIDTH * 6 - 10);
char* pptr7 = (char *)(0xa0000 + SCREEN_WIDTH * 7 - 10);

char* bptr = (char*)(0xa0000 + SCREEN_WIDTH * 80);

int* i = 0;

int kbd_handler();
int timer_handler();

/* 先頭は main 関数でなければならない */

int main() {
  /* 割り込み処理関数を登録し直す。
   * 本当はこのプログラムが終了したときに、
   * 割り込み処理関数を元に戻さなければならないが、
   * このプログラムは永遠に終わらないので、
   * 元に戻す処理は省略する。
   */
  int* ptr = (int*)0x7e00;
  *ptr = (int)kbd_handler;
  *(ptr + 1) = (int)timer_handler;

  *bptr = 14;
  *pptr = 15;
  *pptr2 = 15;
  *pptr3 = 15;
  *pptr4 = 15;
  *pptr5 = 15;
  *pptr6 = 15;
  *pptr7 = 15;

  /* 現状では何もおこらないが、取りあえずソフトウェア割り込みをかけてみる */
  syscall(1, 0);

  while (1)
    halt();
}

int kbd_handler() {
  out8(0x20, 0x61);    /* キーボード割り込み (IRQ1) を再度有効にする */
  int key = in8(KBD_DATA);

  /* ラケットを消去 */
  *pptr = 0;
  *pptr2 = 0;
  *pptr3 = 0;
  *pptr4 = 0;
  *pptr5 = 0;
  *pptr6 = 0;
  *pptr7 = 0;

  /* ラケットの位置を変更 */
  pptr += SCREEN_WIDTH;
  pptr2 += SCREEN_WIDTH;
  pptr3 += SCREEN_WIDTH;
  pptr4 += SCREEN_WIDTH;
  pptr5 += SCREEN_WIDTH;
  pptr6 += SCREEN_WIDTH;
  pptr7 += SCREEN_WIDTH;

  if (pptr > ((char*)0xa0000) + SCREEN_WIDTH * 90){
    pptr = ((char*)0xa0000) + SCREEN_WIDTH - 10;
    pptr2 = ((char*)0xa0000) + SCREEN_WIDTH * 2 - 10;
    pptr3 = ((char*)0xa0000) + SCREEN_WIDTH * 3 - 10;
    pptr4 = ((char*)0xa0000) + SCREEN_WIDTH * 4 - 10;
    pptr5 = ((char*)0xa0000) + SCREEN_WIDTH * 5 - 10;
    pptr6 = ((char*)0xa0000) + SCREEN_WIDTH * 6 - 10;
    pptr7 = ((char*)0xa0000) + SCREEN_WIDTH * 7 - 10;
  }

  /* ラケットを描画 */
  *pptr = 15;
  *pptr2 = 15;
  *pptr3 = 15;
  *pptr4 = 15;
  *pptr5 = 15;
  *pptr6 = 15;
  *pptr7 = 15;
}

int timer_handler() {
  out8(0x20, 0x60);    /* タイマー割り込み (IRQ0) を再度有効にする */

  /* ボールを消去 */
  *bptr = 0;

  /* ボールの位置を変更 */
  if(i == 0){
    bptr++;
  }
  if(i == 1){
    bptr--;
  }

  if(pptr == bptr){
    i = 1;
  }

  /* ボールを描画 */
  *bptr = 15;
}

/* ソフトウェア割り込み 0x30 を発生させる */
int syscall(int a, int b) {
  asm volatile ("int $0x30"
                : : "D" (a), "S" (b));
  return 0;
}

int in8(int port) {
  int value;
  asm volatile ("mov $0, %%eax\n\tin %%dx,%%al"
                : "=r" (value) : "d" (port));
  return value;
}

int out8(int port, int value) {
  asm volatile ("out %%al,%%dx"
                : : "d" (port), "a" (value));
  return 0;
}

int halt() {
  asm volatile ("hlt");
  return 0;
}

int sti() {
  asm volatile ("sti");
  return 0;
}

int cli() {
  asm volatile ("cli");
  return 0;
}
