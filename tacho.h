#ifndef TACHO_H
#define TACHO_H

void tacho_init(uint8_t pin);

void tacho_init_home_assistant_device();

void tacho_loop();

unsigned long tacho_get_cur_rpm();


#endif
