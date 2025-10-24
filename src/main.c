/* mandelbrot-viewer -- Mandelbrot set viewer
   Copyright (C) 2025 Joaquim

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <stdio.h>
#include <stdbool.h>
#include <complex.h>
#include <tgmath.h>

void print_c(double complex c);
int escape(double complex z_0, int iterations);

void print_c(double complex c)
{
     printf("%.5f%+.5fi\n", creal(c), cimag(c));
}

int escape(double complex z_0, int iterations)
{
    int i = 0;
    int x_squared, y_squared;
    double complex n = z_0;

    if (z_0 == 0) {
        return false;
    }

    do {
        x_squared = creal(n) * creal(n);
        y_squared = cimag(n) * cimag(n);

        n = (n*n + z_0);
    } while ((x_squared + y_squared <= 4) && (++i < iterations));

    return (i != iterations);
}

int main(void)
{
    int i;
    int escape_success;
    const int iter = 1000;

    double complex tests[] = {1, 0};
    double complex j;

    for (i = 0; i < 2; ++i) {
        j = tests[i];

        escape_success = escape(j, iter);
        print_c(j);

        if (escape_success) {
            puts("Escaped");
        } else {
            puts("Did not escape");
        }

        puts("");
    }

    return 0;
}

