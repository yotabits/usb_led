#ifndef USB_LED_CPU_COLOR_H
#define USB_LED_CPU_COLOR_H

void get_cpu_color(uint8_t output[3]);
void HSVtoRGB(int H, double S, double V, uint8_t output[3]);

#endif //USB_LED_CPU_COLOR_H
