#include <stdio.h>
#include <stdlib.h>

#define FILEPSD2 "test.psd2" // name of file

// used to check that the .psd file has not been saved commpressed
#define COMPRESSED_FLAG 0x80000000UL

// PSD EOF marker
#define PS_EOF 10

unsigned short word2short(unsigned char *word){
  return (word[0] << 8) | word[1];
}
unsigned long dword2long(unsigned char *word){
  return (word[0] << 24) | (word[1] << 16) | (word[2] << 8) | word[3];
}

// dword に 読み込んだサイズ分、fp をずらす
void skip_dword(FILE *fp, unsigned char *dword){
  unsigned long length;
  fread(dword, 4, 1, fp);
  length = dword2long(dword);
  fseek(fp, length, SEEK_CUR);
}

void skip(FILE *fp, int size){
  fseek(fp, size, SEEK_CUR);
}

short read_word(FILE *fp, unsigned char *word){
  fread(word, 2, 1, fp);
  return word2short(word);
}

void skip_header(FILE *fp){
  // Skip psd header
  fseek(fp, 4 + 2 + 6 + 2 + 4 + 4 + 2 + 2, SEEK_SET);

  //fread(&word, sizeof(word), 1, fp);
  //fseek(fp, 6, SEEK_CUR);

  //カラーチャンネル数
  /* fread(&word, sizeof(word), 1, fp); */
  /* slength = word2short(word); */
  /* printf("%d\n", slength); */

  // 高さ
  /* fread(&dword, sizeof(dword), 1, fp); */
  /* length = dword2long(dword); */
  /* printf("%lu\n", length); */

  // 幅
  /* fread(&dword, sizeof(dword), 1, fp); */
  /* length = dword2long(dword); */
  /* printf("%lu\n", length); */

  // チャンネル当りのビット数
  /* fread(&word, sizeof(word), 1, fp); */
  /* slength = word2short(word); */
  /* printf("%d\n", slength); */

  // カラーモード
  /* fread(&word, sizeof(word), 1, fp); */
  /* slength = word2short(word); */
  /* printf("%d\n", slength); */

}

int main(void){
  FILE *ofp;
  FILE *fp;
  unsigned long length, layer_count;
  unsigned short slength;
  unsigned char byte;
  unsigned char word[2];
  unsigned char dword[4];
  unsigned int i;
  char *layer_name;

  fp = fopen(FILEPSD2, "rb");
  if(fp == NULL){
    printf("open error\n");
    exit(0);
  }
  skip_header(fp);

  // Skip color mode data section
  skip_dword(fp, dword);

  // Skip Image Resource Section
  skip_dword(fp, dword);

  // Length of the layer and mask information section.
  skip(fp, 4);

  // Length of the layers info section, rounded up to a multiple of 2.
  skip(fp, 4);

  // number of layers
  layer_count = read_word(fp, word);
  printf("%ld\n", layer_count);

  // top, left, bottom, right
  skip(fp, 4 * 4);
  /* for(i = 0; i < 4; i++){ */
  /*   fread(&dword, sizeof(dword), 1, fp); */
  /*   length = dword2long(dword); */
  /*   /\* printf("%ld\n", length); *\/ */
  //}

  // Number of channels in the layer
  slength = read_word(fp, word);

  // Channel information
  fseek(fp, slength * 6, SEEK_CUR);

  // Blend mode signature: '8BIM'
  fread(&dword, sizeof(dword), 1, fp);
  /* printf("%s\n", dword); */

  // Blend mode key:
  fread(&dword, sizeof(dword), 1, fp);
  /* printf("%s\n", dword); */

  // Opacity.
  fread(&byte, sizeof(byte), 1, fp);
  /* printf("%x\n", byte); */

  // Clipping:
  fread(&byte, sizeof(byte), 1, fp);
  /* printf("%x\n", byte); */

  // flags
  fread(&byte, sizeof(byte), 1, fp);
  /* printf("%x\n", byte); */

  // Filler
  fread(&byte, sizeof(byte), 1, fp);
  /* printf("%x\n", byte); */

  // Length of the extra data field ( = the total length of the next five fields).
  fread(&dword, sizeof(dword), 1, fp);
  length = dword2long(dword);

  // layer mask info
  fread(&dword, sizeof(dword), 1, fp);
  length = dword2long(dword);
  /* printf("%lu\n", length); */
  fseek(fp, length, SEEK_CUR);

  // Layer blending ranges data
  fread(&dword, sizeof(dword), 1, fp);
  length = dword2long(dword);
  /* printf("%lu\n", length); */
  fseek(fp, length, SEEK_CUR);

  // Layer Name
  fread(&byte, sizeof(byte), 1, fp);
  // printf("%i\n", byte);
  // sjis で入ってる
  layer_name = (char *)malloc(sizeof(char) * byte);
  fread(layer_name, sizeof(char), byte, fp);
  /* printf("%x\n", layer_name[0]); */
  /* printf("%x\n", layer_name[1]); */
  fclose(fp);

  ofp = fopen("tmp.txt", "wb");
  fwrite(layer_name, sizeof(char), byte, ofp);
  fwrite("\n", sizeof(char), 1, ofp);
  fclose(ofp);
}

  /* printf("%ld\n", sizeof(word)); */
  /* ok = fread(&word, sizeof(word), 1, fp); */
  /* printf("%ld\n", ftell(fp)); */
  /* printf("%d\n", word2short(word)); */
  /* printf("%d\n", word); */
  //printf("%ld\n", ftell(fp));
