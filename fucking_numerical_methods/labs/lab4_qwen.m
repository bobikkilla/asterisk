clc; clear all; close all;
format long;

% ===========================
% ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
% ===========================

function y = lagrange_interp(x_nodes, y_nodes, x)
    % Интерполяция полиномом Лагранжа
    n = length(x_nodes);
    y = 0;

    for i = 1:n
        L_i = 1;
        for j = 1:n
            if j ~= i
                L_i = L_i * (x - x_nodes(j)) / (x_nodes(i) - x_nodes(j));
            end
        end
        y = y + y_nodes(i) * L_i;
    end
end

function coeff = newton_coeffs(x_nodes, y_nodes)
    % Вычисление коэффициентов полинома Ньютона
    n = length(x_nodes);
    coeff = zeros(1, n);
    coeff(1) = y_nodes(1);

    % Используем схему разделенных разностей
    divided_diff = y_nodes(:);

    for i = 2:n
        for j = n:-1:i
            divided_diff(j) = (divided_diff(j) - divided_diff(j-1)) / (x_nodes(j) - x_nodes(j-i+1));
        end
        coeff(i) = divided_diff(i);
    end
end

function y = newton_eval(x_nodes, coeff, x)
    % Вычисление значения полинома Ньютона
    n = length(coeff);
    y = coeff(n);

    for i = n-1:-1:1
        y = y.*(x - x_nodes(i)) + coeff(i);
    end
end

% ===========================
% ПАРАМЕТРЫ ЗАДАЧИ (вариант 10)
% ===========================
a = -3; b = 3;
f = @(x) (2*x.^2 + 6)./(x.^2 - 2*x + 5);

disp('ЛАБОРАТОРНАЯ РАБОТА №4. ИНТЕРПОЛИРОВАНИЕ ФУНКЦИЙ');
disp('Вариант 10: f(x) = (2x^2+6)/(x^2-2x+5), отрезок [-3, 3]');
disp('================================================================');

% ===========================
% ЗАДАЧА №1: ПОЛИНОМ ЛАГРАНЖА
% ===========================
disp('\n=== ЗАДАЧА №1: ИНТЕРПОЛЯЦИОННЫЙ МНОГОЧЛЕН ЛАГРАНЖА ===');

% 1. Равномерная сетка
disp('\n1. РАВНОМЕРНАЯ СЕТКА');
xr_nodes = linspace(a, b, 4); % 4 узла для полинома 3-й степени
fr_nodes = f(xr_nodes);

fprintf('Узлы равномерной сетки:\n');
fprintf('x = [%f, %f, %f, %f]\n', xr_nodes);
fprintf('f(x) = [%f, %f, %f, %f]\n', fr_nodes);

% Точки для вычисления погрешности
x_test = [(5*a+b)/6, (a+b)/2, (a+5*b)/6];
f_test = f(x_test);

% Интерполяция в тестовых точках
Lr_test = zeros(1, 3);
for j = 1:3
    Lr_test(j) = lagrange_interp(xr_nodes, fr_nodes, x_test(j));
end

% 2. Чебышевская сетка
disp('\n2. ЧЕБЫШЕВСКАЯ СЕТКА');
xc_nodes = zeros(1, 4);
for i = 0:3
    xc_nodes(i+1) = (a+b)/2 + (b-a)/2 * cos((2*i+1)*pi/8);
end
fc_nodes = f(xc_nodes);

fprintf('Узлы Чебышевской сетки:\n');
fprintf('x = [%f, %f, %f, %f]\n', xc_nodes);
fprintf('f(x) = [%f, %f, %f, %f]\n', fc_nodes);

% Интерполяция в тестовых точках
Lc_test = zeros(1, 3);
for j = 1:3
    Lc_test(j) = lagrange_interp(xc_nodes, fc_nodes, x_test(j));
end

% Вывод результатов в требуемом формате
disp('\nРезультаты интерполяции полиномом Лагранжа:');
fprintf('%-4s %-12s %-12s %-12s %-12s %-12s %-12s\n', ...
    'j', 'x_j', 'f(x_j)', 'L^r_3(x_j)', '|f-L^r_3|', 'L^c_3(x_j)', '|f-L^c_3|');
fprintf('---------------------------------------------------------------------------------------------------\n');

for j = 1:3
    fprintf('%-4d %-12.6f %-12.6f %-12.6f %-12.6e %-12.6f %-12.6e\n', ...
        j, x_test(j), f_test(j), Lr_test(j), abs(f_test(j)-Lr_test(j)), ...
        Lc_test(j), abs(f_test(j)-Lc_test(j)));
end

% Построение графиков для полинома Лагранжа
x_plot = linspace(a, b, 1000);
y_plot = f(x_plot);

% Вычисление значений интерполяционных полиномов
Lr_plot = arrayfun(@(x) lagrange_interp(xr_nodes, fr_nodes, x), x_plot);
Lc_plot = arrayfun(@(x) lagrange_interp(xc_nodes, fc_nodes, x), x_plot);

% График для равномерной сетки
figure;
plot(x_plot, y_plot, 'b-', 'LineWidth', 2); hold on;
plot(x_plot, Lr_plot, 'r--', 'LineWidth', 2);
plot(xr_nodes, fr_nodes, 'ko', 'MarkerSize', 8, 'MarkerFaceColor', 'g');
plot(x_test, f_test, 'ms', 'MarkerSize', 8, 'MarkerFaceColor', 'c');
grid on;
xlabel('x'); ylabel('y');
title('Интерполяция полиномом Лагранжа (равномерная сетка)');
legend('Исходная функция f(x)', 'Полином Лагранжа L^r_3(x)', ...
    'Узлы интерполяции', 'Точки вычисления погрешности', 'Location', 'best');

% График для Чебышевской сетки
figure;
plot(x_plot, y_plot, 'b-', 'LineWidth', 2); hold on;
plot(x_plot, Lc_plot, 'r--', 'LineWidth', 2);
plot(xc_nodes, fc_nodes, 'ko', 'MarkerSize', 8, 'MarkerFaceColor', 'g');
plot(x_test, f_test, 'ms', 'MarkerSize', 8, 'MarkerFaceColor', 'c');
grid on;
xlabel('x'); ylabel('y');
title('Интерполяция полиномом Лагранжа (Чебышевская сетка)');
legend('Исходная функция f(x)', 'Полином Лагранжа L^c_3(x)', ...
    'Узлы интерполяции', 'Точки вычисления погрешности', 'Location', 'best');

% График погрешностей
figure;
plot(x_plot, abs(arrayfun(@(x) f(x) - lagrange_interp(xr_nodes, fr_nodes, x), x_plot)), ...
    'b-', 'LineWidth', 2); hold on;
plot(x_plot, abs(arrayfun(@(x) f(x) - lagrange_interp(xc_nodes, fc_nodes, x), x_plot)), ...
    'r--', 'LineWidth', 2);
plot(x_test, abs(f_test - Lr_test), 'bo', 'MarkerSize', 8, 'MarkerFaceColor', 'c');
plot(x_test, abs(f_test - Lc_test), 'ro', 'MarkerSize', 8, 'MarkerFaceColor', 'm');
grid on;
xlabel('x'); ylabel('Погрешность');
title('Сравнение погрешностей интерполяции полиномом Лагранжа');
legend('Равномерная сетка', 'Чебышевская сетка', ...
    'Точки равномерной сетки', 'Точки Чебышевской сетки', 'Location', 'best');

% ===========================
% ЗАДАЧА №2: ПОЛИНОМ НЬЮТОНА
% ===========================
disp('\n\n=== ЗАДАЧА №2: ИНТЕРПОЛЯЦИОННЫЙ МНОГОЧЛЕН НЬЮТОНА ===');

% 1. Равномерная сетка
disp('\n1. РАВНОМЕРНАЯ СЕТКА');
% Коэффициенты полинома Ньютона для равномерной сетки
coeff_r = newton_coeffs(xr_nodes, fr_nodes);

% Интерполяция в тестовых точках
Pr_test = zeros(1, 3);
for j = 1:3
    Pr_test(j) = newton_eval(xr_nodes, coeff_r, x_test(j));
end

% 2. Чебышевская сетка
disp('\n2. ЧЕБЫШЕВСКАЯ СЕТКА');
% Коэффициенты полинома Ньютона для Чебышевской сетки
coeff_c = newton_coeffs(xc_nodes, fc_nodes);

% Интерполяция в тестовых точках
Pc_test = zeros(1, 3);
for j = 1:3
    Pc_test(j) = newton_eval(xc_nodes, coeff_c, x_test(j));
end

% Вывод результатов в требуемом формате
disp('\nРезультаты интерполяции полиномом Ньютона:');
fprintf('%-4s %-12s %-12s %-12s %-12s %-12s %-12s\n', ...
    'j', 'x_j', 'f(x_j)', 'P^r_3(x_j)', '|f-P^r_3|', 'P^c_3(x_j)', '|f-P^c_3|');
fprintf('---------------------------------------------------------------------------------------------------\n');

for j = 1:3
    fprintf('%-4d %-12.6f %-12.6f %-12.6f %-12.6e %-12.6f %-12.6e\n', ...
        j, x_test(j), f_test(j), Pr_test(j), abs(f_test(j)-Pr_test(j)), ...
        Pc_test(j), abs(f_test(j)-Pc_test(j)));
end

% Построение графиков для полинома Ньютона
% Вычисление значений интерполяционных полиномов
Pr_plot = arrayfun(@(x) newton_eval(xr_nodes, coeff_r, x), x_plot);
Pc_plot = arrayfun(@(x) newton_eval(xc_nodes, coeff_c, x), x_plot);

% График для равномерной сетки
figure;
plot(x_plot, y_plot, 'b-', 'LineWidth', 2); hold on;
plot(x_plot, Pr_plot, 'r--', 'LineWidth', 2);
plot(xr_nodes, fr_nodes, 'ko', 'MarkerSize', 8, 'MarkerFaceColor', 'g');
plot(x_test, f_test, 'ms', 'MarkerSize', 8, 'MarkerFaceColor', 'c');
grid on;
xlabel('x'); ylabel('y');
title('Интерполяция полиномом Ньютона (равномерная сетка)');
legend('Исходная функция f(x)', 'Полином Ньютона P^r_3(x)', ...
    'Узлы интерполяции', 'Точки вычисления погрешности', 'Location', 'best');

% График для Чебышевской сетки
figure;
plot(x_plot, y_plot, 'b-', 'LineWidth', 2); hold on;
plot(x_plot, Pc_plot, 'r--', 'LineWidth', 2);
plot(xc_nodes, fc_nodes, 'ko', 'MarkerSize', 8, 'MarkerFaceColor', 'g');
plot(x_test, f_test, 'ms', 'MarkerSize', 8, 'MarkerFaceColor', 'c');
grid on;
xlabel('x'); ylabel('y');
title('Интерполяция полиномом Ньютона (Чебышевская сетка)');
legend('Исходная функция f(x)', 'Полином Ньютона P^c_3(x)', ...
    'Узлы интерполяции', 'Точки вычисления погрешности', 'Location', 'best');

% График погрешностей
figure;
plot(x_plot, abs(arrayfun(@(x) f(x) - newton_eval(xr_nodes, coeff_r, x), x_plot)), ...
    'b-', 'LineWidth', 2); hold on;
plot(x_plot, abs(arrayfun(@(x) f(x) - newton_eval(xc_nodes, coeff_c, x), x_plot)), ...
    'r--', 'LineWidth', 2);
plot(x_test, abs(f_test - Pr_test), 'bo', 'MarkerSize', 8, 'MarkerFaceColor', 'c');
plot(x_test, abs(f_test - Pc_test), 'ro', 'MarkerSize', 8, 'MarkerFaceColor', 'm');
grid on;
xlabel('x'); ylabel('Погрешность');
title('Сравнение погрешностей интерполяции полиномом Ньютона');
legend('Равномерная сетка', 'Чебышевская сетка', ...
    'Точки равномерной сетки', 'Точки Чебышевской сетки', 'Location', 'best');

% ===========================
% ЗАДАЧА №3: СРАВНЕНИЕ МЕТОДОВ
% ===========================
disp('\n\n=== ЗАДАЧА №3: СРАВНЕНИЕ МЕТОДОВ ИНТЕРПОЛЯЦИИ ===');

% Сравнение максимальной погрешности
max_err_Lr = max(abs(arrayfun(@(x) f(x) - lagrange_interp(xr_nodes, fr_nodes, x), x_plot)));
max_err_Lc = max(abs(arrayfun(@(x) f(x) - lagrange_interp(xc_nodes, fc_nodes, x), x_plot)));
max_err_Pr = max(abs(arrayfun(@(x) f(x) - newton_eval(xr_nodes, coeff_r, x), x_plot)));
max_err_Pc = max(abs(arrayfun(@(x) f(x) - newton_eval(xc_nodes, coeff_c, x), x_plot)));

disp('Максимальные погрешности интерполяции на отрезке [-3, 3]:');
fprintf('Полином Лагранжа (равномерная сетка): %.6e\n', max_err_Lr);
fprintf('Полином Лагранжа (Чебышевская сетка): %.6e\n', max_err_Lc);
fprintf('Полином Ньютона (равномерная сетка): %.6e\n', max_err_Pr);
fprintf('Полином Ньютона (Чебышевская сетка): %.6e\n', max_err_Pc);

% Выводы
disp('\n\n=== ВЫВОДЫ ===');
if max_err_Lc < max_err_Lr
    fprintf('Для полинома Лагранжа Чебышевская сетка обеспечивает меньшую погрешность (%.6e < %.6e).\n', ...
        max_err_Lc, max_err_Lr);
else
    fprintf('Для полинома Лагранжа равномерная сетка обеспечивает меньшую погрешность (%.6e < %.6e).\n', ...
        max_err_Lr, max_err_Lc);
end

if max_err_Pc < max_err_Pr
    fprintf('Для полинома Ньютона Чебышевская сетка обеспечивает меньшую погрешность (%.6e < %.6e).\n', ...
        max_err_Pc, max_err_Pr);
else
    fprintf('Для полинома Ньютона равномерная сетка обеспечивает меньшую погрешность (%.6e < %.6e).\n', ...
        max_err_Pr, max_err_Pc);
end

% Сравнение полиномов Лагранжа и Ньютона
if max_err_Lr < max_err_Pr
    fprintf('На равномерной сетке полином Лагранжа точнее полинома Ньютона (%.6e < %.6e).\n', ...
        max_err_Lr, max_err_Pr);
else
    fprintf('На равномерной сетке полином Ньютона точнее полинома Лагранжа (%.6e < %.6e).\n', ...
        max_err_Pr, max_err_Lr);
end

if max_err_Lc < max_err_Pc
    fprintf('На Чебышевской сетке полином Лагранжа точнее полинома Ньютона (%.6e < %.6e).\n', ...
        max_err_Lc, max_err_Pc);
else
    fprintf('На Чебышевской сетке полином Ньютона точнее полинома Лагранжа (%.6e < %.6e).\n', ...
        max_err_Pc, max_err_Lc);
end
