
#include <stdio.h>
#include <iomanip>
#include <iostream>
#include <cmath>


void print(double d)
{
    printf("%.0lf ", d);
}

void stream(double d)
{
    std::cout << std::fixed << std::setprecision(0) << d << " ";
}

void rounding(double d)
{
    auto r = round(d);
    printf("%.0lf ", r);
}

int main()
{
    double a = 1.3125;
    printf("round %.5f: %.3f\n\n", a, a);

    for (auto i = 0; i < 10; ++i)
        print(i + 0.5);
    printf("\n");

    for (auto i = 0; i < 10; ++i)
        stream(i + 0.5);
    printf("\n");

    for (auto i = 0; i < 10; ++i)
        rounding(i + 0.5);
    printf("\n");
}