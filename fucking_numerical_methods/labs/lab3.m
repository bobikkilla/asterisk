clear all; clc; format long;

eps = 1e-4;
max_iter = 100;

x2_vals = linspace(-2, 1, 1000);
valid = abs(x2_vals + 1) <= 1;
x1_curve1 = NaN(size(x2_vals));
x1_curve1(valid) = asin(x2_vals(valid) + 1) - 1;
x1_curve2 = (2 - cos(x2_vals)) / 2;

figure(1);
plot(x2_vals, x1_curve1, 'b-', 'LineWidth', 2); hold on;
plot(x2_vals, x1_curve2, 'r-', 'LineWidth', 2);
grid on; xlabel('x_2'); ylabel('x_1');
title('Графическая локализация корня');

x1_0 = 0.3;
x2_0 = -0.5;

fprintf('МЕТОД ПРОСТОЙ ИТЕРАЦИИ\n');

phi1 = @(x2) (2 - cos(x2)) / 2;
phi2 = @(x1) sin(x1 + 1) - 1;

J11 = 0;
J12 = 0.5 * sin(x2_0);
J21 = cos(x1_0 + 1);
J22 = 0;
J_matrix = [J11, J12; J21, J22];
q = norm(J_matrix, 1);
fprintf('q = %.6f\n', q);

x1_prev = x1_0;
x2_prev = x2_0;
fprintf('\n%-4s %-12s %-12s %-14s\n', 'k', 'x1', 'x2', '||x_k - x_(k-1)||_1');
fprintf('%-4d %-12.6f %-12.6f %-14s\n', 0, x1_prev, x2_prev, '-');

for k = 1:max_iter
    x1_new = phi1(x2_prev);
    x2_new = phi2(x1_new);
    dx_vec = [x1_new - x1_prev; x2_new - x2_prev];
    norm1 = norm(dx_vec, 1);
    fprintf('%-4d %-12.6f %-12.6f %-14.6e\n', k, x1_new, x2_new, norm1);
    if norm1 < eps
        x1_simple = x1_new;
        x2_simple = x2_new;
        iter_simple = k;
        break;
    end
    x1_prev = x1_new;
    x2_prev = x2_new;
    if k == max_iter
        x1_simple = x1_new;
        x2_simple = x2_new;
        iter_simple = k;
    end
end

fprintf('\n\nМЕТОД НЬЮТОНА\n');

f1 = @(x1, x2) sin(x1 + 1) - x2 - 1;
f2 = @(x1, x2) 2*x1 + cos(x2) - 2;
J = @(x1, x2) [cos(x1 + 1), -1; 2, -sin(x2)];

x1_prev = x1_0;
x2_prev = x2_0;
fprintf('\n%-4s %-12s %-12s %-14s\n', 'k', 'x1', 'x2', '||x_k - x_(k-1)||_1');
fprintf('%-4d %-12.6f %-12.6f %-14s\n', 0, x1_prev, x2_prev, '-');

for k = 1:max_iter
    F = [f1(x1_prev, x2_prev); f2(x1_prev, x2_prev)];
    J_mat = J(x1_prev, x2_prev);
    dx = -J_mat \ F;
    x1_new = x1_prev + dx(1);
    x2_new = x2_prev + dx(2);
    norm1 = norm(dx, 1);
    fprintf('%-4d %-12.6f %-12.6f %-14.6e\n', k, x1_new, x2_new, norm1);
    if norm1 < eps
        x1_newton = x1_new;
        x2_newton = x2_new;
        iter_newton = k;
        break;
    end
    x1_prev = x1_new;
    x2_prev = x2_new;
    if k == max_iter
        x1_newton = x1_new;
        x2_newton = x2_new;
        iter_newton = k;
    end
end

system_func = @(X) [sin(X(1) + 1) - X(2) - 1; 2*X(1) + cos(X(2)) - 2];
options = optimset('Display', 'off', 'TolX', 1e-12, 'TolFun', 1e-12);
[x_fsolve, ~, exitflag] = fsolve(system_func, [x1_0; x2_0], options);

if exitflag > 0
    fprintf('\nПроверка fsolve:\n');
    fprintf('  x1 = %.8f, x2 = %.8f\n', x_fsolve(1), x_fsolve(2));
    res_fsolve = [f1(x_fsolve(1), x_fsolve(2)); f2(x_fsolve(1), x_fsolve(2))];
    fprintf('  Невязка: ||f||_1 = %.2e\n', norm(res_fsolve, 1));
end
