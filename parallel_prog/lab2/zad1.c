#include <stdio.h>
#include <math.h>
#include <omp.h>

// integrated function
double f(double x) {
    return x * sin(x);
}

// exact integral 
double exact_integral(double a, double b) {
    return M_PI;
}

int main() {
    double a = 0.0;
    double b = M_PI; // integration limits

    long N = 10000000; // steps number

    double h = (b - a) / N; // step size
    
    double sum = 0.0; // sum variable

    // parallel part
    #pragma omp parallel for reduction(+:sum)
    for (long i = 0; i < N; i++) {
        double x_mid = a + (i + 0.5) * h; // середина i-го подотрезка
        sum += f(x_mid);
    }

    double approx_integral = sum * h;
    double exact = exact_integral(a, b);

    // error size
    double error = fabs(approx_integral - exact);

    printf("Approx integral:      %.15f\n", approx_integral);
    printf("Exact integral:       %.15f\n", exact);
    printf("Absolute error:       %.5e\n", error);
    return 0;
}