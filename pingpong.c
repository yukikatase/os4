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

/* �擪�� main �֐��łȂ���΂Ȃ�Ȃ� */

int main() {
  /* ���荞�ݏ����֐���o�^�������B
   * �{���͂��̃v���O�������I�������Ƃ��ɁA
   * ���荞�ݏ����֐������ɖ߂��Ȃ���΂Ȃ�Ȃ����A
   * ���̃v���O�����͉i���ɏI���Ȃ��̂ŁA
   * ���ɖ߂������͏ȗ�����B
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

  /* ����ł͉���������Ȃ����A��肠�����\�t�g�E�F�A���荞�݂������Ă݂� */
  syscall(1, 0);

  while (1)
    halt();
}

int kbd_handler() {
  out8(0x20, 0x61);    /* �L�[�{�[�h���荞�� (IRQ1) ���ēx�L���ɂ��� */
  int key = in8(KBD_DATA);

  /* ���P�b�g������ */
  *pptr = 0;
  *pptr2 = 0;
  *pptr3 = 0;
  *pptr4 = 0;
  *pptr5 = 0;
  *pptr6 = 0;
  *pptr7 = 0;

  /* ���P�b�g�̈ʒu��ύX */
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

  /* ���P�b�g��`�� */
  *pptr = 15;
  *pptr2 = 15;
  *pptr3 = 15;
  *pptr4 = 15;
  *pptr5 = 15;
  *pptr6 = 15;
  *pptr7 = 15;
}

int timer_handler() {
  out8(0x20, 0x60);    /* �^�C�}�[���荞�� (IRQ0) ���ēx�L���ɂ��� */

  /* �{�[�������� */
  *bptr = 0;

  /* �{�[���̈ʒu��ύX */
  if(i == 0){
    bptr++;
  }
  if(i == 1){
    bptr--;
  }

  if(pptr == bptr){
    i = 1;
  }

  /* �{�[����`�� */
  *bptr = 15;
}

/* �\�t�g�E�F�A���荞�� 0x30 �𔭐������� */
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
