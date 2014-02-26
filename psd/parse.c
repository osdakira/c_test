#include <stdio.h>
#include <stdlib.h>

#define FILEPSD2 "test2.psd" // name of file

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

long read_dword(FILE *fp, unsigned char *dword){
  unsigned long length;
  fread(dword, 4, 1, fp);
  return dword2long(dword);
}

void writeline(FILE *ofp, char *str, int byte){
  fwrite(str, sizeof(char), byte, ofp);
  fwrite("\n", sizeof(char), 1, ofp);
}

char* read_pascal_string(FILE *fp, unsigned char *byte){
  char *layer_name;
  fread(byte, sizeof(char), 1, fp);
  layer_name = (char *)malloc(sizeof(char) * *byte);
  fread(layer_name, sizeof(char), *byte, fp);
  return layer_name;
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

void search_layer(FILE *fp, FILE *ofp){
  unsigned long length, layer_count, layer_length, layer_head_fpt;
  unsigned short slength;
  unsigned char byte, layer_flag;
  unsigned char word[2];
  unsigned char dword[4];
  char *layer_name;
  fpos_t fpos;

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
  fread(&layer_flag, sizeof(byte), 1, fp);

  // Filler
  fread(&byte, sizeof(byte), 1, fp);
  /* printf("filter zero %x\n", byte); */

  // Length of the extra data field ( = the total length of the next five fields).
  fread(&dword, sizeof(dword), 1, fp);
  layer_length = dword2long(dword);
  /* printf("extra %ld\n", layer_length); */
  /* printf("fp %ld\n", ftell(fp)); */

  // extra field 前の位置
  fgetpos(fp, &fpos);

  // layer mask info
  skip_dword(fp, dword);

  // Layer blending ranges data
  skip_dword(fp, dword);

  // Layer Name; sjis で入ってる
  layer_name = read_pascal_string(fp, &byte);

  writeline(ofp, layer_name, byte);
  char layer_set[] = "<LayerSet>";
  if( (layer_flag & 16) && (*layer_name != '<') ){
    writeline(ofp, layer_set, 10);
  }
  free(layer_name);

  //次のレイヤーへ
  /* printf("fp %ld\n", ftell(fp)); */
  fsetpos(fp, &fpos);
  /* printf("fp %ld\n", ftell(fp)); */

  fseek(fp, layer_length, SEEK_CUR);
}

int main(void){
  FILE *ofp;
  FILE *fp;
  unsigned long length, layer_count, layer_length, layer_head_fpt;
  unsigned short slength;
  unsigned char byte;
  unsigned char word[2];
  unsigned char dword[4];
  unsigned int i;
  char *layer_name;
  fpos_t fpos;

  fp = fopen(FILEPSD2, "rb");
  if(fp == NULL){
    printf("open error\n");
    exit(0);
  }
  ofp = fopen("tmp.txt", "wb");

  skip_header(fp);

  // Skip color mode data section
  skip_dword(fp, dword);

  // Skip Image Resource Section
  skip_dword(fp, dword);

  // Length of the layer and mask information section.
  read_dword(fp, dword);

  // Length of the layers info section, rounded up to a multiple of 2.
  skip(fp, 4);

  // number of layers
  layer_count = read_word(fp, word);
  // printf("num layers %ld\n", layer_count);

  ///layer records 開始
  for(i = 0; i < layer_count; i++){
    search_layer(fp, ofp);
  }

  fclose(ofp);
  fclose(fp);
}
