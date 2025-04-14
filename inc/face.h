#ifndef __FACE_H__
#define __FACE_H__

void face_init(void);
void face_final(void);
double face_data(void);

#define WGET_CMD "wget http://192.168.10.36:8080/?action=snapshot -O /tmp/SearchFace.jpg"
#define FACE_FILE "/tmp/SearchFace.jpg"

#endif
