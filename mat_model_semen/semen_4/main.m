% Параметры системы
m = 1;          % масса, кг
k = 4;          % жёсткость, Н/м
L = 1.5;        % расстояние до стенки, м
omega = sqrt(k/m);

% Временной интервал
t_max = 10;
dt = 0.01;
t = 0:dt:t_max;

% --- Случай A: безопасный ---
r0_A = 1.0;
v0_A = 1.0;
r_A = r0_A * cos(omega * t) + (v0_A / omega) * sin(omega * t);

% --- Случай B: опасный ---
r0_B = -1.4;
v0_B = -2.0;
r_B = r0_B * cos(omega * t) + (v0_B / omega) * sin(omega * t);

% Проверка условия
cond_A = r0_A^2 + (m/k) * v0_A^2;
cond_B = r0_B^2 + (m/k) * v0_B^2;
L2 = L^2;

% Определяем статусы
if cond_A < L2
  status_A = "Безопасно";
else
  status_A = "Опасно";
endif

if cond_B < L2
  status_B = "Безопасно";
else
  status_B = "Опасно";
endif

fprintf('Условие для случая A: %.3f < %.3f → %s\n', cond_A, L2, status_A);
fprintf('Условие для случая B: %.3f < %.3f → %s\n', cond_B, L2, status_B);

% Построение графиков
figure;
hold on;
plot(t, r_A, 'b-', 'LineWidth', 1.5, 'DisplayName', 'Случай A (безопасный)');
plot(t, r_B, 'r--', 'LineWidth', 1.5, 'DisplayName', 'Случай B (опасный)');

% Замена yline(-L, ...) на ручную горизонтальную линию
plot([t(1), t(end)], [-L, -L], 'k:', 'LineWidth', 2, 'DisplayName', 'Стенка (r = -L)');

xlabel('Время t, с');
ylabel('Положение r(t), м');
title('Движение шарика на пружине');
legend('Location', 'best');
grid on;
ylim([-2.5, 2.5]);

% Численное решение через ode45 (для случая A)
f = @(t, y) [y(2); -(k/m) * y(1)];
[t_num, y_num] = ode45(f, t, [r0_A, v0_A]);

% Сравнение аналитического и численного решений
figure;
hold on;
plot(t, r_A, 'b-', 'LineWidth', 2, 'DisplayName', 'Аналитическое');
plot(t_num, y_num(:,1), 'ro', 'MarkerSize', 3, 'DisplayName', 'Численное (ode45)');

% Горизонтальная линия стенки
plot([t(1), t(end)], [-L, -L], 'k:', 'LineWidth', 2, 'DisplayName', 'Стенка');

xlabel('Время t, с');
ylabel('r(t), м');
title('Сравнение аналитического и численного решений (случай A)');
legend;
grid on;
