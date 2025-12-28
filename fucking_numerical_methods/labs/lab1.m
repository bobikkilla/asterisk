clc;
x = 20:0.001:30;
func = @(x) 2 .* log(x) - x ./ 3 + 1;

figure;
hold on;
plot(x, func(x));
legend("График функции");
grid on;
hold off;

a = 20;
b = 22;
intrvl = a:0.001:b;

phi1 = @(x) 6 * log(x) + 3;
phi2 = @(x) sqrt(exp(x ./ 3 - 1));

figure;
hold on;
plot(intrvl, func(intrvl), intrvl, phi1(intrvl), intrvl, intrvl);
legend('График функции', 'x = \phi_1(x)', 'y = x');
grid on;
hold off;

figure;
hold on;
plot(intrvl, func(intrvl), intrvl, phi2(intrvl), intrvl, intrvl);
legend('График функции', 'x = \phi_2(x)', 'y = x');
grid on;
hold off;

phi1_der_mod = @(x) abs(6 ./ x);
phi2_der_mod = @(x) abs(exp((x - 3) / 6) / 6);

figure;
hold on;
plot(intrvl, phi1_der_mod(intrvl), intrvl, phi2_der_mod(intrvl));
legend('y = |\phi_1`(x)|', 'y = |\phi_2`(x)|');
grid on;
hold off;

% МЕТОД ПРОСТЫХ ИТЕРАЦИЙ
q = max(phi1_der_mod(intrvl));

epsilon = 10^-4;
epsilon_zero = (1 - q)/q * epsilon;

xk = (a + b) / 2;
xkp = phi1(xk);
iter = 1;

printf("МЕТОД ПРОСТЫХ ИТЕРАЦИЙ\n");
printf("№ итерации    корень    разность    точность\n");
printf("%d          |%f        |%d        |%f\n", iter, xkp, abs(xk - xkp), epsilon_zero);
while abs(xk - xkp) >= epsilon_zero
  iter += 1;
  xk = xkp;
  xkp = phi1(xk);
  printf("%d          |%f        |%d        |%f\n", iter, xkp, abs(xk - xkp), epsilon_zero);
end

% МОДИФИЦИРОВАННЫЙ МЕТОД ПРОСТЫХ ИТЕРАЦИЙ
func_der = @(x) 2 ./ x - 1 ./ 3;
func_der_usable = @(x) -2 ./ x + 1 ./ 3;

figure;
hold on;
plot(intrvl, func_der(intrvl));
legend("График y = f`(x)");
grid on;
hold off;

func_new = @(x) - 2 * log(x) + x ./ 3 - 1;
m = min(func_der_usable(intrvl));
M = max(func_der_usable(intrvl));

alpha = 2 / (M + m);
q = (M - m) / (M + m);

iter = 1;
epsilon_zero = (1 - q) / 1 * epsilon;
xk = (a + b) / 2;
xkp = xk - alpha .* func_new(xk);

printf("МОДИФИЦИРОВАННЫЙ МЕТОД ПРОСТЫХ ИТЕРАЦИЙ\n");
printf("№ итерации    корень    разность    точность\n");
printf("%d          |%f        |%d        |%f\n", iter, xkp, abs(xk - xkp), epsilon_zero);
while abs(xk - xkp) >= epsilon_zero
  iter += 1;
  xk = xkp;
  xkp = xk - alpha .* func_new(xk);
  printf("%d          |%f        |%d        |%f\n", iter, xkp, abs(xk - xkp), epsilon_zero);
end

% МЕТОД НЬЮТОНА
iter = 1;
xk = (a + b) / 2;
xkp = xk - func(xk) ./ func_der(xk);

printf("МЕТОД НЬЮТОНА\n");
printf("№ итерации    корень    разность    точность\n");
printf("%d          |%f        |%d        |%f\n", iter, xkp, abs(xk - xkp), epsilon);
while abs(xk - xkp) >= epsilon
  iter += 1;
  xk = xkp;
  xkp = xk - func(xk) ./ func_der(xk);
  printf("%d          |%f        |%d        |%f\n", iter, xkp, abs(xk - xkp), epsilon);
end
