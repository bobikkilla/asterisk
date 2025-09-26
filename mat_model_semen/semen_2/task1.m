% =============================================
% ВИЗУАЛИЗАЦИЯ УСЕЧЁННОГО КОНУСА (КУХОННОЙ ВЫТЯЖКИ)
% =============================================

clear; clc; close all;

% ========== Параметры усечённого конуса ==========
R = 0.4;   % радиус нижнего основания (метры)
r = 0.2;   % радиус верхнего основания
H = 0.5;   % высота (метры)

% ========== Функция образующей ==========
% y = f(x) = R - ((R - r)/H)*x
f = @(x) R - ((R - r)/H).*x;

% ========== Генерация точек для визуализации ==========
x = linspace(0, H, 50);    % 50 точек по высоте
y = f(x);                  % радиусы на каждом уровне

% ========== Создание сетки для поверхности вращения ==========
theta = linspace(0, 2*pi, 60);  % углы поворота (в радианах)
[X, Theta] = meshgrid(x, theta);

% Радиус на каждом уровне — y(x)
Y = f(X);  % радиус как функция x

% Координаты поверхности в 3D:
X_surf = X;
Y_surf = Y .* cos(Theta);
Z_surf = Y .* sin(Theta);

% ========== Визуализация ==========
figure('Position', [100, 100, 800, 600]);
mesh(X_surf, Y_surf, Z_surf, 'EdgeColor', 'blue', 'FaceColor', 'none');
hold on;

% Добавим оси для наглядности
plot3([0, H], [0, 0], [0, 0], 'k-', 'LineWidth', 2); % ось X
plot3([0, 0], [-R, R], [0, 0], 'k--', 'LineWidth', 1); % ось Y
plot3([0, 0], [0, 0], [-R, R], 'k--', 'LineWidth', 1); % ось Z

% Подписи осей
xlabel('Высота (x), м');
ylabel('Y');
zlabel('Z');
title(['Усечённый конус (вытяжка): R=', num2str(R), ', r=', num2str(r), ', H=', num2str(H)]);
axis equal;
view(30, 30);  % угол обзора
grid on;
shading interp;
colormap jet;

% ========== Вычисление площади боковой поверхности ==========
l = sqrt(H^2 + (R - r)^2);  % длина образующей
S_lateral = pi * (R + r) * l;

fprintf('\n=== Результаты ===\n');
fprintf('Радиус нижнего основания R = %.2f м\n', R);
fprintf('Радиус верхнего основания r = %.2f м\n', r);
fprintf('Высота H = %.2f м\n', H);
fprintf('Длина образующей l = %.4f м\n', l);
fprintf('Площадь боковой поверхности S = %.4f м²\n', S_lateral);

% ========== Окружности оснований (для наглядности) ==========
% Нижнее основание
theta_base = linspace(0, 2*pi, 100);
x_bottom = zeros(size(theta_base));
y_bottom = R * cos(theta_base);
z_bottom = R * sin(theta_base);
plot3(x_bottom, y_bottom, z_bottom, 'r-', 'LineWidth', 2);

% Верхнее основание
x_top = H * ones(size(theta_base));
y_top = r * cos(theta_base);
z_top = r * sin(theta_base);
plot3(x_top, y_top, z_top, 'g-', 'LineWidth', 2);

legend('Боковая поверхность', 'Ось X', 'Основания (нижн. красн., верхн. зел.)', 'Location', 'best');