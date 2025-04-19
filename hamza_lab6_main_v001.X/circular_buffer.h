/* 
 * File:   circular_buffer.h
 * Author: Hamza
 *
 * Created on April 11, 2025, 10:02 PM
 */

#ifndef CIRCULAR_BUFFER_H
#define	CIRCULAR_BUFFER_H

#ifdef	__cplusplus
extern "C" {
#endif

#define LEN 1000
#define k 16

int buf[LEN];
int start = 0;
int end = -k;  

void initBuffer(void) {
    for (int i = 0; i < LEN; i++) {
        buf[i] = 0;
    }
}

void putVal(int val) {
    buf[start] = val;
    start = (start + 1) % LEN;
    end = (end + 1) % LEN;
}

int getAvg(void) {
    int sum = 0;
    int count = 0;
    int i = end;
    
    while (i != start) {
        sum += buf[i];
        i = (i + 1) % LEN;
        count++;
    }
    
   if (count == 0) return 0;  // to avoid division by zero, in case buffer is empty and start =0
return sum / count;
}


#ifdef	__cplusplus
}
#endif

#endif	/* CIRCULAR_BUFFER_H */
