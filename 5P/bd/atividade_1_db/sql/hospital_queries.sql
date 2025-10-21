-- 2.1
SELECT *
FROM hospital.usuario;
-- 2.2
SELECT primeiroNome,
  sobrenome
FROM hospital.usuario;
-- 2.3
SELECT *
FROM hospital.usuario
WHERE sexo = 'F';
-- 2.4
SELECT *
FROM hospital.perfil
WHERE ativo = 'S';
-- 2.6
SELECT DISTINCT cidade
FROM hospital.endereco;
-- 2.7
SELECT primeiroNome,
  sobrenome
FROM hospital.usuario
ORDER BY primeiroNome,
  sobrenome;
-- 2.8
SELECT DISTINCT sobrenome
FROM hospital.usuario
WHERE sexo = 'F'
ORDER BY sobrenome;
-- 2.9
SELECT MIN(salario) AS menor_salario,
  MAX(salario) AS maior_salario
FROM hospital.medico;
-- 2.10
SELECT MAX(salario) AS maior_salario
FROM hospital.medico
WHERE especialidade = 'Clínico Geral';
-- 2.11
SELECT AVG(salario) AS media_salarial
FROM hospital.medico
WHERE especialidade = 'Cirurgião';
-- 2.12
SELECT COUNT(*) AS homens
FROM hospital.usuario
WHERE sexo = 'M';
-- 2.13
SELECT COUNT(*) AS homens_80
FROM hospital.usuario
WHERE sexo = 'M'
  AND dataNasc >= '1980-01-01';