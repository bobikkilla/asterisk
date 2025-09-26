% =============================================
% Расчёт работы насоса для ёмкости: цилиндр + полусфера
% =============================================

clear; clc; close all;

% ========== Параметры ёмкости ==========
R = 0.1;        % радиус (м) — диаметр 20 см → R = 10 см = 0.1 м
H_cyl = 0.3;    % высота цилиндра (м) = 30 см
rho = 1000;     % плотность жидкости (кг/м³), вода
g = 9.8;        % ускорение свободного падения (м/с²)

L = R + H_cyl;  % общая высота ёмкости от дна до верха (м)

fprintf('Параметры:\n');
fprintf('  Радиус R = %.2f м\n', R);
fprintf('  Высота цилиндра H = %.2f м\n', H_cyl);
fprintf('  Общая высота L = %.2f м\n', L);
fprintf('  Плотность rho = %.0f кг/м³\n', rho);
fprintf('  g = %.1f м/с²\n\n', g);

% ========== Аналитическое вычисление ==========

I1 = 2*R*L * (R^2 / 2) ...
     - (L + 2*R) * (R^3 / 3) ...
     + (R^4 / 4);

% Второй интеграл: I2 = ∫_R^L (L - x) dx = (L - R)^2 / 2
I2 = (L - R)^2 / 2;

% Полная работа:
A_analytic = pi * rho * g * (I1 + R^2 * I2);

% ========== Численная проверка ==========
% Определяем функцию площади сечения S(x)
S = @(x) arrayfun(@(xi) ...
    (xi <= R) .* pi * (2*R*xi - xi.^2) + ...
    (xi > R) .* pi * R^2, x);

% Функция подынтегральная: dA/dx = rho * g * (L - x) * S(x)
integrand = @(x) rho * g * (L - x) .* S(x);

A_numeric = integral(integrand, 0, L, 'AbsTol', 1e-10, 'RelTol', 1e-10);

% ========== Вывод результатов ==========
fprintf('=== Результаты ===\n');
fprintf('Аналитическая работа: %.4f Дж\n', A_analytic);
fprintf('Численная работа:     %.4f Дж\n', A_numeric);
fprintf('Разница:              %.2e Дж\n', abs(A_analytic - A_numeric));

% ========== Визуализация ёмкости (опционально) ==========
theta = linspace(0, 2*pi, 50);
z_cyl = linspace(R, L, 20);
[Z_cyl, Theta] = meshgrid(z_cyl, theta);
X_cyl = R * cos(Theta);
Y_cyl = R * sin(Theta);

% Полусфера
phi = linspace(0, pi/2, 20);
[Phi, Theta_s] = meshgrid(phi, theta);
X_sph = R * sin(Phi) .* cos(Theta_s);
Y_sph = R * sin(Phi) .* sin(Theta_s);
Z_sph = R - R * cos(Phi);

figure('Position', [100, 100, 700, 600]);
mesh(X_cyl, Y_cyl, Z_cyl, 'EdgeColor', 'blue', 'LineWidth', 1); % Сетка цилиндра
hold on;
mesh(X_sph, Y_sph, Z_sph, 'EdgeColor', 'red', 'LineWidth', 1); % Сетка полусферы

axis equal;
xlabel('X (м)'); ylabel('Y (м)'); zlabel('Z (м)');
title('Ёмкость: цилиндр + полусфера (сетка)');
view(30, 20);
grid on;
