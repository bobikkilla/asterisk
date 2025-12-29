disp("Running");
function y = func_val(x)
  y = (2 .* (x.^2) + 6) ./ (x.^2 - 2 .* x + 5); % original
  % y =  (x.^2) - sqrt(x + 1); % test 1
  % y = tan(x + sqrt(x)); % test 2
  % y = (x.^2) - 2 .* x + (16 ./ (x - 1)) - 13; % test 3
  % y = exp(sin(sqrt(x))); % test 4
end

% ravnomernye uzly setki
xrnodes = [];
funcr = [];

% Original
a = -3;
b = 3;

% test 1
% a = 0;
% b = 3;

% test 2
% a = 0.1;
% b = 0.7;

% test 3
% a = 2;
% b = 5;

% test 4
% a = 1;
% b = 4;

for i = 0:3
  xrnodes(i + 1) = a + i * ((b - a)/ 3);
  funcr(i + 1) = func_val(xrnodes(i + 1));
end

% Chebyshevskaya setka
xcnodes = [];
funcc = [];

for i = 0:3
  xcnodes(i + 1) = (a + b) / 2 + ((b - a) / 2) * cos(((2*i + 1)/ 8) * pi());
  funcc(i + 1) = func_val(xcnodes(i + 1));
end

function L = lagrange_interpolation(x, y, x_interp)

    n = length(x);
    L = 0;

    for i = 1:n
        L_i = 1;
        for j = 1:n
            if j != i
                L_i = L_i * (x_interp - x(j)) / (x(i) - x(j));
            end
        end
        L = L + y(i) * L_i;
    end
    return;
end

function P = newton_interpolation(x, y, x0)
    n = length(x);
    coeff = zeros(1, n);
    coeff(1) = y(1);

    for j = 2:n
        coeff(j) = (y(j) - newton_eval(coeff(1:j-1), x(1:j-1), x(j))) / prod(x(j) - x(1:j-1));
    end

    P = newton_eval(coeff, x, x0);
    return;
end

function P = newton_eval(coeff, x, x0)
    n = length(coeff);
    P = coeff(1);
    for k = 2:n
        term = coeff(k);
        for j = 1:k-1
            term = term * (x0 - x(j));
        end
        P = P + term;
    end
    return;
end

printf("xj\tf(xj)\t\tLr(xj)\t\t|f(xj)-Lr(xj)|\t\tLc(xj)\t\t|f(xj)-Lc(xj)|\n");
x = [(5*a + b)/6, (a + b)/2, (a + 5*b)/6];
for j = 1:3
  Lr = lagrange_interpolation(xrnodes, funcr, x(j));
  f = func_val(x(j));
  Lc = lagrange_interpolation(xcnodes, funcc, x(j));
  printf("%d\t%f\t%f\t\t%f\t%f\t%f\n", j, f, Lr, abs(f - Lr), Lc, abs(f - Lc));
end

disp("\n");

printf("xj\tf(xj)\t\tPr(xj)\t\t|f(xj)-Pr(xj)|\t\tPc(xj)\t\t|f(xj)-Pc(xj)|\n");
for j = 1:3
  Lr = newton_interpolation(xrnodes, funcr, x(j));
  f = func_val(x(j));
  Lc = newton_interpolation(xcnodes, funcc, x(j));
  printf("%d\t%10f\t%10f\t%10f\t\t%10f\t%10f\n", j, f, Lr, abs(f - Lr), Lc, abs(f - Lc));
end

x = [a:0.001:b];
y = func_val(x);
ylagrange_r = [];
for i = 1:length(x)
  ylagrange_r(i) = lagrange_interpolation(xrnodes, funcr, x(i));
end

figure;
hold on;

plot(x, y);
plot(x, ylagrange_r);
legend('График функции', 'График многочлена Лагранжа по равномерной сетке')
hold off;

ylagrange_c = [];
for i = 1:length(x)
  ylagrange_c(i) = lagrange_interpolation(xcnodes, funcc, x(i));
end

figure;
hold on;

plot(x, y);
plot(x, ylagrange_c);
legend('График функции', 'График многочлена Лагранжа по Чебышёвской сетке')
hold off;

ynewton_r = [];
for i = 1:length(x)
  ynewton_r(i) = newton_interpolation(xrnodes, funcr, x(i));
end

figure;
hold on;

plot(x, y);
plot(x, ynewton_r);
legend('График функции', 'График многочлена Ньютона по равномерной сетке')
hold off;

ynewton_c = [];
for i = 1:length(x)
  ynewton_c(i) = newton_interpolation(xcnodes, funcc, x(i));
end

figure;
hold on;
plot(x, y);
plot(x, ynewton_c)
legend('График функции', 'График многочлена Ньютона по Чебышёвской сетке')
hold off;

figure;
hold on;
plot(x, abs(y - ylagrange_r))
plot(x, abs(y - ylagrange_c))
legend('График погрешности интерполирования многочленом Лагранжа по равномерной сетке', 'График погрешности интерполирования многочленом Лагранжа по Чебышёвской сетке')
hold off;

figure;
hold on;

plot(x, abs(y - ynewton_r))
plot(x, abs(y - ynewton_c))
legend('График погрешности интерполирования многочленом Ньютона по равномерной сетке', 'График погрешности интерполирования многочленом Ньютона по Чебышёвской сетке')
hold off;

