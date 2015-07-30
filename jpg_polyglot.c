/*
*    jpg_polyglot, Copyright (c) 2015 Dennis Goodlett <dennis@hurricanelabs.com>
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//*******************************************************************************************************************************
//  JPG example                                                                                                                **
//  00000000: ffd8 ffe0 0010 4a46 4946 0001 0100 0001  ......JFIF......                                                        **
//  00000010: 0001 0000 ffdb 0043 0003 0202 0202 0203  .......C........                                                        **
//  00000020: 0202 0203 0303 0304 0604 0404 0404 0806  ................                                                        **
//  00000030: 0605 0609 080a 0a09 0809 090a 0c0f 0c0a  ................                                                        **
//                                                                                                                             **
//  marker:              \xff\xd8                                                                                              **
// =======Application section?=========                                                                                        **
//  section marker:      \xff\xe0                                         ____                                                 **
//  section length:      \x00\x10   # This is of great interest to me :)      |                                                **
//  Identifyer:          \x4a\x46\x49\x46\x00   # = JFIF\0                    |                                                **
//  Version:             \x01\x01   # 1.1                                     |--- 16 bytes, ie: \x00\x10 section length       **
//  units:               \x00                                                 |                                                **
//  density:             \x00\x01\x00\x01   # 1x1 ?                           |                                                **
//  thumbnail:           \x00\x00     # 0x0                               ____|                                                **
//                                                                                                                             **
// =======Quantization section=========                                                                                        **
//  nxt marker:         \xff\xdb                                                                                               **
//*******************************************************************************************************************************
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define MARKER_LEN 11
// 0x2f2a - 0x0010 = 0x2f1a
#define LEN_ADDED 0x2f1a

struct jpg {
  size_t size;
  FILE *fp;
  char *mem;
};
typedef struct jpg JPG;


void error(char *msg){
  if (errno) {
    perror(msg);
    exit(errno);
  } 
  else
    fprintf(stderr, "args: <original.jpg> <polyglot.jpg> <js file>\n\n%s\n", msg);
  exit(-1);
}

static void memorfy(JPG *jx){
  fseek(jx->fp, 0, SEEK_END);
	jx->size = ftell(jx->fp);
  rewind(jx->fp);
  if (( jx->mem = malloc(jx->size + 1) ) == NULL)
    error("could not put file in memory");
	fread(jx->mem, 1, jx->size, jx->fp);  // put 1.size byptes from fp into ret 
	jx->mem[jx->size] = 0x00;            //  null_terminate :)
  fclose(jx->fp);

  // just checking
  /* int i; */
  /* for (i=0; i<16; i++) */
  /*   printf("%02x ", jx->mem[i] & 0xff); */
  /* printf("\n"); */
}

void jpg_check(JPG *jpg){
  int i;
  char marker[MARKER_LEN] = "\xff\xd8\xff\xe0\x00\x10\x4a\x46\x49\x46\x00"; // start of file
  if ( jpg->size <=  MARKER_LEN ) 
    error("File size is too small");
  for (i=0; i<MARKER_LEN; i++)
    if ( jpg->mem[i] != marker[i]) break;
  if ( i == 4 || i == 5) error("looks like this file has already been modified");
  else if ( i != MARKER_LEN) error("magic numbers did not check out, is this a jpg?");
}

void make_new_jpg(JPG *old, JPG *new, JPG *js){
  int i;  //iterator

  // fill up a couple buffers with random junk 
  int hbuff_s = random() % ( LEN_ADDED - js->size - 5); // -5 for "*/=0;"
  char hbuff[hbuff_s];
  for (i=0; i<hbuff_s; i++)
    hbuff[i] = random() & 0xff;
    /* hbuff[i] = 0x41; */

  int tbuff_s = LEN_ADDED - ( hbuff_s + js->size + 5  );
  char tbuff[tbuff_s];
  for (i=2; i<tbuff_s; i++)
    tbuff[i] = random() & 0xff;
    /* tbuff[i] = 0x41; */
  tbuff[0] = 0x2f;
  tbuff[1]   = 0x2a;

  printf("[*]DEBUG: hbuff_s(%d) + /x2a/x2f=0; + js(%d) + tbuff(%d) = %x\n", 
            hbuff_s, (int) js->size, tbuff_s,  (unsigned int) ( hbuff_s+5+js->size+tbuff_s ));

  if (
    (fwrite(old->mem, 1, 4, new->fp) != 4 )                          ||    // header
    (fwrite("/*", 1, 2, new->fp) != 2 )                              ||    // size *injected*
    (fwrite(old->mem+6, 1, 14, new->fp) != 14 )                      ||    // normal header part
    (fwrite(hbuff, 1, hbuff_s, new->fp) != hbuff_s )                 ||    // random buff padding
    (fwrite("*/=0;", 1, 5, new->fp) != 5 )                           ||    // start javascript
    (fwrite(js->mem, 1, js->size, new->fp) != js->size )             ||    // Javascript
    (fwrite(tbuff, 1, tbuff_s, new->fp) != tbuff_s )                 ||    // "/*" + random buff
    (fwrite(old->mem+20, 1, old->size-20, new->fp) != old->size-20 ) ||
    (fwrite("*/", 1, 2, new->fp) != 2 ) 
  ) error("file write error");


}


int main(int argc, char *argv[]){
  JPG old, new, js;

  if ( argc != 4 )
    error(
        "\tPoor user, you're not doing it right.\n"
        "\tYou've been putting too many arguments,\n"
        "\tor too little, I forget how it works with you.\n"
        "\tAnyway, you haven't drunk exactly the right amount.\n"
    );
  else if (( old.fp = fopen(argv[1], "r") ) == NULL)
    error("could not open original file");
  else if (( js.fp = fopen(argv[3], "r") ) == NULL)
    error("could not open javscript file");
  else if ( ( new.fp = fopen(argv[2], "r") ) != NULL ){
    fclose(new.fp);
    fclose(old.fp);
    error("new file exists already!!!");
  }else errno = 0;  // reset error num 

  if (( new.fp = fopen(argv[2], "w") ) == NULL){
    fclose(old.fp);
    error("could not open new file");
  } 

  
  // put javascript file in memory
  // make sure to free :)
  memorfy(&js);
  if (js.size >  LEN_ADDED - 2 - 5) error("js too big\n");

  memorfy(&old);
  jpg_check(&old);

  // all checks out, lets make a new file.
  make_new_jpg(&old, &new, &js);

  free(old.mem);
  free(js.mem);
  fclose(new.fp);
  return 0;
}
