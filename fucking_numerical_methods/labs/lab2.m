clc;

epsilon = 10^-4;

alpha = [80, 10, 0, -5;
		    -13, 100, 9, 0;
		     0, -14, 128, 7;
		      0, 0, 31, 256];

beta = [34; -128; 95; -69];

B = zeros(4, 4);
diag_elements = zeros(4, 1);

for i = 1:4
	for j = 1:4
		if i == j
			B(i, j) = 0;
		else
			B(i, j) = -alpha(i, j) / alpha(i, i);
		end
	end
	diag_elements(i) = beta(i) / alpha(i, i);
end

norm(B, 2)
Xk = [0; 0; 0; 0];

disp("Simple iterations: ")
printf("Номер итерации		xk+1_1		xk+1_2		xk+1_3		xk+1_4		eps\n");
iter = 1;
Xkp = B * Xk + diag_elements;
epsilon_zero = (1 - norm(B, Inf)) / norm(B, Inf) .* epsilon;

printf("	%d	%f	%f	%f	%f	%f\n", iter, Xkp(1), Xkp(2), Xkp(3), Xkp(4), epsilon_zero);
while norm(Xkp - Xk, Inf) >= epsilon_zero
  iter = iter + 1;
  Xk = Xkp;
  Xkp = B * Xk + diag_elements;
  printf("	%d	%f	%f	%f	%f	%f\n", iter, Xkp(1), Xkp(2), Xkp(3), Xkp(4), epsilon_zero);
end

B1 = zeros(4, 4);
B2 = zeros(4, 4);

for i = 1:4
	for j = i:4
		B1(i, j) = B(i, j);
	end
end

for i = 1:4
	for j = 1:i
		B2(i, j) = B(i, j);
	end
end

epsilon_zero = (1 - norm(B, Inf)) / norm(B2, Inf) .* epsilon;


disp("Zeidel :")

iter = 1
E = eye(4);
Xk = [0; 0; 0; 0];
Xkp = (E - B1)' * B2 * Xk + (E - B1)'*diag_elements;

printf("Номер итерации		xk+1_1		xk+1_2		xk+1_3		xk+1_4		eps\n");
printf("	%d	%f	%f	%f	%f	%f\n", iter, Xkp(1), Xkp(2), Xkp(3), Xkp(4), epsilon_zero);
while norm(Xkp - Xk, Inf) >= epsilon_zero
	iter = iter + 1;
	Xk = Xkp;
	Xkp = inv(E - B1) * B2 * Xk + inv(E - B1)*diag_elements;
	printf("	%d	%f	%f	%f	%f	%f\n", iter, Xkp(1), Xkp(2), Xkp(3), Xkp(4), epsilon_zero);
end

disp("Результат linsolve()")
x = linsolve(alpha, beta)
