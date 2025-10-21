-- JOIN E GROUP BY
-- 1. Quantos programadores possuem um salário atual maior que 10000?
SELECT e.first_name,
  e.last_name,
  s.amount
FROM company.employees e
  JOIN company.salary_history s USING(emp_id)
WHERE s.amount > 10000;
-- 2. Liste o número de funcionários por departamento.
SELECT COUNT(*) as employees_quantity,
  d.name
FROM company.employees e
  JOIN company.departments d USING(dept_id)
GROUP BY d.dept_id;
-- 3. Quantos projetos cada cliente possui contratados?
SELECT COUNT(*) AS projects
FROM company.projects p
  JOIN company.clients c USING(client_id)
GROUP BY client_id;
-- 4. Liste, para cada projeto, a quantidade de funcionários envolvidos.
SELECT COUNT(*) as project_employees,
  p.project_id,
  p.name
FROM company.employees e
  JOIN company.employee_projects ep USING(emp_id)
  JOIN company.projects p USING(project_id)
GROUP BY p.project_id
ORDER BY p.project_id;
-- 5. Qual é a média salarial por departamento?
SELECT d.name,
  ROUND(AVG(s.amount), 2) AS average_salary
FROM company.salary_history s
  JOIN company.employees e USING(emp_id)
  JOIN company.departments d USING(dept_id)
GROUP BY d.name
ORDER BY average_salary;
-- 6. Liste todos os funcionários e os projetos em que estão trabalhando (incluindo aqueles sem projeto).
SELECT e.first_name,
  e.last_name,
  e.position,
  p.name AS project_name
FROM company.employees e
  LEFT OUTER JOIN company.employee_projects ep USING(emp_id)
  LEFT OUTER JOIN company.projects p USING (project_id)
ORDER BY e.first_name,
  e.last_name;
-- 7. Quais linguagens de programação são mais dominadas pelos funcionários (contagem de funcionários por linguagem)?
-- 8. Qual cliente possui o maior número de projetos ativos?
-- 9. Liste o total de tarefas por status (Pendente, Em progresso, Concluído).
SELECT t.status,
  COUNT(*) tasks_quantity
FROM company.tasks t
GROUP BY t.status;
-- 10. Liste o número de funcionários por função (position) no departamento de Desenvolvimento.
-- 11. Liste todos os projetos e seus respectivos clientes, mesmo que não tenham cliente associado.
SELECT p.name AS project_name,
  c.name AS client_name
FROM company.projects p
  LEFT OUTER JOIN company.clients c USING(client_id);
-- 12. Para cada projeto, liste a quantidade total de tarefas e quantas já estão concluídas.
SELECT DISTINCT COUNT(*) AS done_tasks,
  p.name AS project_name
FROM company.tasks t
  JOIN company.projects p USING(project_id)
WHERE t.status = 'Concluído'
GROUP BY p.name;
-- 13. Liste a média de proficiência por linguagem de programação.
SELECT l.name,
  ROUND (
    AVG(
      CASE
        s.proficiency_level
        WHEN 'Básico' THEN 1
        WHEN 'Intermediário' THEN 2
        WHEN 'Avançado' THEN 3
      END
    )::numeric,
    1
  ) AS avg_prof_level
FROM company.skills s
  JOIN company.programming_languages l USING (lang_id)
GROUP BY l.lang_id,
  l.name;
-- 14. Quais funcionários trabalham em mais de um projeto?
-- 15. Liste a soma total dos salários por departamento, considerando apenas o salário mais recente de cada funcionário.
-- 16. Para cada funcionário, mostre seu nome e a data de início do projeto mais antigo em que trabalhou.
-- 17. Quais departamentos não possuem nenhum funcionário?
-- 18. Liste todos os funcionários que não possuem nenhuma habilidade registrada no banco.
-- 19. Qual é o salário mais alto registrado e quem é o funcionário que o recebe atualmente?
-- 20. Para cada cliente, liste os nomes dos projetos e a quantidade de funcionários em cada um.
SELECT c.name AS client_name,
  p.name AS project_name,
  COUNT(DISTINCT e.emp_id) AS employees_num
FROM company.projects p
  JOIN company.employee_projects ep USING(project_id)
  JOIN company.employees e USING(emp_id)
  JOIN company.clients c USING(client_id)
GROUP BY c.name,
  p.name;