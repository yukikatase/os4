#define KBD_STATUS    0x64
#define KBD_DATA      0x60
#define KEY_UP_BIT    0x80
#define SCREEN_WIDTH  320


int i = 0;
int k = 0;
int l = 4;

char* pptr = (char *)(0xa0000 + (SCREEN_WIDTH * 50) - 10);

char* bptr = (char*)(0xa0000 + SCREEN_WIDTH * 50) + 1;


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
  pptr[SCREEN_WIDTH] = 15;
  pptr[SCREEN_WIDTH * 2] = 15;
  pptr[SCREEN_WIDTH * 3] = 15;
  pptr[SCREEN_WIDTH * 4] = 15;
  pptr[SCREEN_WIDTH * 5] = 15;
  pptr[SCREEN_WIDTH * 6] = 15;

  /* 現状では何もおこらないが、取りあえずソフトウェア割り込みをかけてみる */
  // syscall(1, 0);

  while (1)
    halt();
}

int kbd_handler() {
  out8(0x20, 0x61);    /* キーボード割り込み (IRQ1) を再度有効にする */
  int key = in8(KBD_DATA);
  int m;
  /* ラケットを消去 */
  if(key == 0x50){
    m = 0;
  }else if(key == 0x48){
    m = 1;
  }

  pptr[SCREEN_WIDTH * m * 6] = 0;
  if((pptr + SCREEN_WIDTH > ((char*)0xa0000) + SCREEN_WIDTH * 180) && (m == 0)){
  }else if((pptr - SCREEN_WIDTH < ((char*)0xa0000)) && (m == 1)){
    }else{
      /* ラケットの位置を変更 */
      if(m == 0){
        pptr += SCREEN_WIDTH;
      }else if(m == 1){
        pptr -= SCREEN_WIDTH;
      }
  }

  /* ラケットを描画 */
  pptr[0] = 15;
  pptr[SCREEN_WIDTH] = 15;
  pptr[SCREEN_WIDTH * 2] = 15;
  pptr[SCREEN_WIDTH * 3] = 15;
  pptr[SCREEN_WIDTH * 4] = 15;
  pptr[SCREEN_WIDTH * 5] = 15;
  pptr[SCREEN_WIDTH * 6] = 15;
}

int timer_handler() {
  out8(0x20, 0x60);    /* タイマー割り込み (IRQ0) を再度有効にする */
  int n;

  /* ボールを消去 */
  if(k == 0){
    *bptr = 0;
  }else{
    k = 0;
  }

  for(int j = 0; j < 188; j++){ //ヒット確認
    if(bptr == ((char*)0xa0000) + (SCREEN_WIDTH * j) && i == 1){
      i = 0;
      l = 8 - l;
    }
  }

  /* ボールの位置を変更 */
  if(i == 0){
    bptr++;
  }else{
    bptr--;
  }

  // if(l == 4){
  //   bptr -= SCREEN_WIDTH;
  // }

  // if(bptr < ((char*)0xa0000) + SCREEN_WIDTH){
  //   l = 5;
  // }else if(bptr > ((char*)0xa0000) + (SCREEN_WIDTH * 186)){
  //   l = 3;
  // }

  /* ボールを描画 */
  *bptr = 15;

  for(int j = 0; j < 7; j++){
    if(pptr + (SCREEN_WIDTH * j) == bptr){
      i = 1;
      k = 1;
      l = j;
    }
  }
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
