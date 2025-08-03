-- 1.1
SELECT cpf
FROM universidade.professor
WHERE departamento = 'DMA';
-- 1.2
SELECT id_turma
FROM universidade.cursa
WHERE mat_estudante = 'E101';
-- 1.3
SELECT cpf,
  primeiro_nome,
  sobrenome
FROM universidade.usuario
WHERE cardinality(telefone) > 0;
-- 1.4
SELECT nome,
  cod_disc
FROM universidade.disciplina
WHERE creditos > 2
  AND pre_req IS NOT NULL;
-- 1.5
SELECT AVG(nota)
FROM universidade.cursa AS media_notas
WHERE nota IS NOT NULL;
-- 1.6
SELECT COUNT(DISTINCT mat_professor)
FROM universidade.plano
WHERE mat_estudante IS NOT NULL;
-- 1.7
SELECT DISTINCT mat_professor
FROM universidade.plano
  LEFT JOIN universidade.departamento d ON d.chefe = mat_professor
WHERE mat_estudante IS NOT NULL
  OR d.chefe IS NOT NULL;
-- 1.8
SELECT DISTINCT mat_professor
FROM universidade.plano
  JOIN universidade.departamento d ON d.chefe = mat_professor
WHERE mat_estudante IS NOT NULL;
-- 1.9
SELECT DISTINCT mat_professor
FROM universidade.plano p
  LEFT JOIN universidade.departamento d ON d.chefe = mat_professor
WHERE p.mat_estudante IS NOT NULL
  AND d.chefe IS NULL;