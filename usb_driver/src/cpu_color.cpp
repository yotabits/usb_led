#include <iostream>
#include <stdint.h>
#include <cmath>
#include <stdio.h>
#include <unistd.h>
#define MAX_H 230
void HSVtoRGB(uint16_t H, double S, double V, uint8_t output[3])
{
    double C = S * V;
    double X = C * (1 - std::abs(std::fmod(H / 60.0, 2) - 1));
    double m = V - C;
    double Rs, Gs, Bs;

    if(H >= 0 && H < 60)
    {
        Rs = C;
        Gs = X;
        Bs = 0;
    }
    else if(H >= 60 && H < 120)
    {
        Rs = X;
        Gs = C;
        Bs = 0;
    }
    else if(H >= 120 && H < 180)
    {
        Rs = 0;
        Gs = C;
        Bs = X;
    }
    else if(H >= 180 && H < 240)
    {
        Rs = 0;
        Gs = X;
        Bs = C;
    }
    else if(H >= 240 && H < 300)
    {
        Rs = X;
        Gs = 0;
        Bs = C;
    }
    else
    {
        Rs = C;
        Gs = 0;
        Bs = X;
    }
    output[0] = (Rs + m) * 255;
    output[1] = (Gs + m) * 255;
    output[2] = (Bs + m) * 255;
    std::cout << "r: " << (int) output[0] << std::endl;
    std::cout << "g: " << (int) output[1] << std::endl;
    std::cout << "b: " << (int) output[2] << std::endl;
}

float average(void)
{
    long double a[4], b[4], loadavg;
    FILE *fp;
    fp = fopen("/proc/stat","r");
    fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&a[0],&a[1],&a[2],&a[3]);
    fclose(fp);
    sleep(1);
    fp = fopen("/proc/stat","r");
    fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
    fclose(fp);
    loadavg = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));
    printf("The current CPU utilization is : %Lf\n",loadavg);
    return(loadavg);
}

void get_cpu_color(uint16_t &hue)
{
    float cpu_usage_percent = average();
    int h = MAX_H - ( (float)MAX_H * cpu_usage_percent);
    std::cout << "h: " << (int) h << std::endl;
    hue = h;
}

void get_cpu_color(uint8_t rgb_color[3])
{
    double s = 1;
    double v = 1;
    uint16_t h;
    get_cpu_color(h);
    HSVtoRGB(h, s, v, rgb_color);
}