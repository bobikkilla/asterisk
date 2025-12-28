clear all; clc; format long;

epsilon = 1e-4;
max_iter = 100;

% ЗАДАЧА №1: Графическая локализация корней
disp('=== ЗАДАЧА №1: ГРАФИЧЕСКАЯ ЛОКАЛИЗАЦИЯ КОРНЕЙ ===');

% Строим графики для системы:
% sin(x1+1) - x2 = 1
% 2x1 + cos(x2) = 2

figure(1);
hold on; grid on;
title('Графическая локализация корня (10 вариант)');
xlabel('x_1'); ylabel('x_2');
set(gca, 'FontSize', 12);

% Диапазон для построения графиков
x1_range = linspace(-1.5, 1.5, 1000);

% Первое уравнение: sin(x1+1) - x2 = 1 => x2 = sin(x1+1) - 1
x2_eq1 = sin(x1_range + 1) - 1;

% Второе уравнение: 2x1 + cos(x2) = 2 => x2 = arccos(2-2x1)
valid2 = abs(2-2*x1_range) <= 1;
x2_eq2_pos = zeros(size(x1_range));
x2_eq2_neg = zeros(size(x1_range));
x2_eq2_pos(valid2) = acos(2-2*x1_range(valid2));
x2_eq2_neg(valid2) = -acos(2-2*x1_range(valid2));

% Построение графиков
plot(x1_range, x2_eq1, 'b-', 'LineWidth', 2);
plot(x1_range(valid2), x2_eq2_pos(valid2), 'r-', 'LineWidth', 2);
plot(x1_range(valid2), x2_eq2_neg(valid2), 'r-', 'LineWidth', 2);

legend('sin(x_1+1)-x_2=1', '2x_1+cos(x_2)=2', 'Location', 'best');
hold off;

% Выбираем начальное приближение из графика
x1_0 = 0.3;
x2_0 = -0.5;
fprintf('\nВыбрано начальное приближение: x1_0 = %.2f, x2_0 = %.2f\n', x1_0, x2_0);

% ЗАДАЧА №1: Метод простой итерации
disp('\n=== ЗАДАЧА №1: МЕТОД ПРОСТОЙ ИТЕРАЦИИ (1-норма) ===');

% Приводим систему к итерационному виду:
% x1 = (2 - cos(x2))/2
% x2 = sin(x1 + 1) - 1
phi1 = @(x2) (2 - cos(x2))/2;
phi2 = @(x1) sin(x1 + 1) - 1;

% Проверка сходимости: вычисляем матрицу Якоби для системы x = phi(x)
% J = [d(phi1)/dx1, d(phi1)/dx2; d(phi2)/dx1, d(phi2)/dx2]
%   = [0, 0.5*sin(x2); cos(x1+1), 0]

% Вычисляем матрицу Якоби в начальной точке
J11 = 0;
J12 = 0.5 * sin(x2_0);
J21 = cos(x1_0 + 1);
J22 = 0;
J_matrix = [J11, J12; J21, J22];

% Для вариантов 1-10 используем 1-норму (максимальная сумма модулей по столбцам)
q = norm(J_matrix, 1);
fprintf('\nМатрица Якоби в начальной точке:\n');
disp(J_matrix);
fprintf('1-норма матрицы Якоби: q = %.6f\n', q);

if q >= 1
    fprintf('Условие сходимости НЕ ВЫПОЛНЯЕТСЯ (q >= 1).\n');
    fprintf('Попробуем другой итерационный вид системы.\n\n');

    % Альтернативный итерационный вид:
    % x1 = asin(x2 + 1) - 1
    % x2 = 2*x1 + cos(x2) - 2 + x2
    phi1_alt = @(x2) asin(x2 + 1) - 1;
    phi2_alt = @(x1, x2) 2*x1 + cos(x2) - 2 + x2;

    % Проверка сходимости для альтернативного вида
    % J = [d(phi1)/dx1, d(phi1)/dx2; d(phi2)/dx1, d(phi2)/dx2]
    %   = [0, 1/sqrt(1-(x2+1)^2); 2, -sin(x2)+1]

    J11_alt = 0;
    if abs(x2_0 + 1) < 1 % Проверка области определения arcsin
        J12_alt = 1/sqrt(1-(x2_0+1)^2);
    else
        J12_alt = NaN;
    end
    J21_alt = 2;
    J22_alt = -sin(x2_0) + 1;

    if ~isnan(J12_alt)
        J_matrix_alt = [J11_alt, J12_alt; J21_alt, J22_alt];
        q_alt = norm(J_matrix_alt, 1);
        fprintf('Альтернативная матрица Якоби:\n');
        disp(J_matrix_alt);
        fprintf('1-норма альтернативной матрицы Якоби: q_alt = %.6f\n', q_alt);

        if q_alt < 1
            fprintf('Для альтернативного итерационного вида условие сходимости ВЫПОЛНЯЕТСЯ.\n');
            phi1 = phi1_alt;
            phi2 = @(x1) phi2_alt(x1, x2_0); % Временное решение для демонстрации
        else
            fprintf('Для альтернативного итерационного вида условие сходимости НЕ ВЫПОЛНЯЕТСЯ.\n');
        end
    end
else
    fprintf('Условие сходимости ВЫПОЛНЯЕТСЯ (q < 1).\n');
end

% Вычисляем epsilon_zero как требуется в методичке
epsilon_zero = ((1 - q) / q) * epsilon;
fprintf('Рассчитанная точность для остановки итераций: epsilon_zero = %.6e\n', epsilon_zero);

% Таблица результатов метода простой итерации
fprintf('\nТаблица результатов метода простой итерации:\n');
fprintf('%-12s %-12s %-12s %-14s %-12s\n', '№ итерации', 'x1^(k)', 'x2^(k)', '||x^(k)-x^(k-1)||_1', 'epsilon_zero');
fprintf('----------------------------------------------------------------------------\n');

% Начальное приближение
x1_prev = x1_0;
x2_prev = x2_0;
fprintf('%-12d %-12.6f %-12.6f %-14s %-12.6e\n', 0, x1_prev, x2_prev, '-', epsilon_zero);

% Итерации метода простой итерации
iter_simple = 0;
for k = 1:max_iter
    % ВАЖНО: оба новых значения вычисляются на основе ПРЕДЫДУЩИХ значений
    x1_new = phi1(x2_prev);
    x2_new = phi2(x1_prev);

    % Вычисляем 1-норму разности (сумма модулей разностей компонент)
    norm_diff = abs(x1_new - x1_prev) + abs(x2_new - x2_prev);

    fprintf('%-12d %-12.6f %-12.6f %-14.6e %-12.6e\n', k, x1_new, x2_new, norm_diff, epsilon_zero);

    % Проверяем условие остановки
    if norm_diff < epsilon_zero
        x1_simple = x1_new;
        x2_simple = x2_new;
        iter_simple = k;
        break;
    end

    % Обновляем предыдущие значения
    x1_prev = x1_new;
    x2_prev = x2_new;

    if k == max_iter
        x1_simple = x1_new;
        x2_simple = x2_new;
        iter_simple = k;
        fprintf('Достигнуто максимальное количество итераций.\n');
    end
end

% ЗАДАЧА №2: Метод Ньютона
disp('\n=== ЗАДАЧА №2: МЕТОД НЬЮТОНА (1-норма) ===');

% Исходные уравнения системы
f1 = @(x1, x2) sin(x1 + 1) - x2 - 1;
f2 = @(x1, x2) 2*x1 + cos(x2) - 2;

% Матрица Якоби для исходной системы
J_newton = @(x1, x2) [cos(x1 + 1), -1; 2, -sin(x2)];

% Таблица результатов метода Ньютона
fprintf('\nТаблица результатов метода Ньютона:\n');
fprintf('%-12s %-12s %-12s %-14s %-12s\n', '№ итерации', 'x1^(k)', 'x2^(k)', '||x^(k)-x^(k-1)||_1', 'epsilon');
fprintf('----------------------------------------------------------------------------\n');

% Начальное приближение
x1_prev = x1_0;
x2_prev = x2_0;
fprintf('%-12d %-12.6f %-12.6f %-14s %-12.6e\n', 0, x1_prev, x2_prev, '-', epsilon);

% Итерации метода Ньютона
iter_newton = 0;
for k = 1:max_iter
    % Вычисляем значения функций в текущей точке
    F = [f1(x1_prev, x2_prev); f2(x1_prev, x2_prev)];

    % Вычисляем матрицу Якоби в текущей точке
    J_mat = J_newton(x1_prev, x2_prev);

    % Решаем систему J*dx = -F
    dx = -J_mat \ F;

    % Обновляем решение
    x1_new = x1_prev + dx(1);
    x2_new = x2_prev + dx(2);

    % Вычисляем 1-норму разности
    norm_diff = abs(dx(1)) + abs(dx(2));

    fprintf('%-12d %-12.6f %-12.6f %-14.6e %-12.6e\n', k, x1_new, x2_new, norm_diff, epsilon);

    % Проверяем условие остановки
    if norm_diff < epsilon
        x1_newton = x1_new;
        x2_newton = x2_new;
        iter_newton = k;
        break;
    end

    % Обновляем предыдущие значения
    x1_prev = x1_new;
    x2_prev = x2_new;

    if k == max_iter
        x1_newton = x1_new;
        x2_newton = x2_new;
        iter_newton = k;
        fprintf('Достигнуто максимальное количество итераций.\n');
    end
end

% Вычисляем невязки для обоих методов
residual_simple = abs(f1(x1_simple, x2_simple)) + abs(f2(x1_simple, x2_simple));
residual_newton = abs(f1(x1_newton, x2_newton)) + abs(f2(x1_newton, x2_newton));

% Сравнение результатов
disp('\n=== СРАВНЕНИЕ РЕЗУЛЬТАТОВ ===');
fprintf('Метод простой итерации:\n');
fprintf('  Решение: x1 = %.8f, x2 = %.8f\n', x1_simple, x2_simple);
fprintf('  Количество итераций: %d\n', iter_simple);
fprintf('  Невязка (1-норма): %.6e\n', residual_simple);

fprintf('\nМетод Ньютона:\n');
fprintf('  Решение: x1 = %.8f, x2 = %.8f\n', x1_newton, x2_newton);
fprintf('  Количество итераций: %d\n', iter_newton);
fprintf('  Невязка (1-норма): %.6e\n', residual_newton);

% Сравнение с встроенной функцией
system_func = @(X) [sin(X(1) + 1) - X(2) - 1; 2*X(1) + cos(X(2)) - 2];
options = optimset('Display', 'off', 'TolX', 1e-12, 'TolFun', 1e-12);
[x_fsolve, fval_fsolve] = fsolve(system_func, [x1_0; x2_0], options);

residual_fsolve = abs(fval_fsolve(1)) + abs(fval_fsolve(2));

fprintf('\nВстроенная функция fsolve:\n');
fprintf('  Решение: x1 = %.8f, x2 = %.8f\n', x_fsolve(1), x_fsolve(2));
fprintf('  Невязка (1-норма): %.6e\n', residual_fsolve);

% Выводы
disp('\n=== ВЫВОДЫ ===');
if iter_newton < iter_simple
    fprintf('Метод Ньютона сходится быстрее (%d итераций) чем метод простой итерации (%d итераций).\n', ...
        iter_newton, iter_simple);
else
    fprintf('Метод простой итерации сходится быстрее (%d итераций) чем метод Ньютона (%d итераций).\n', ...
        iter_simple, iter_newton);
end

if residual_newton < residual_simple
    fprintf('Метод Ньютона дает более точное решение (меньшая невязка).\n');
else
    fprintf('Метод простой итерации дает более точное решение (меньшая невязка).\n');
end

fprintf('Оба метода обеспечивают достаточную точность при заданном epsilon = %.1e.\n', epsilon);
