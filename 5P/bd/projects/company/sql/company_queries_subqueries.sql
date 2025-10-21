-- 1. Liste todos os funcionários cujo salário atual é maior que a média de salários da empresa.
SELECT e.first_name,
  e.last_name,
  s.amount,
  FROM company.employees e
  JOIN company.salary_history s USING (emp_id)
WHERE s.amount > (
    SELECT AVG(amount)
    FROM company.salary_history
  );
-- 2. Liste os nomes dos projetos cujo número de funcionários seja maior que a média de funcionários por projeto.
SELECT COUNT(*) AS employees_num,
  p.name AS project_name
FROM company.employees e
  JOIN company.employee_projects ep USING(emp_id)
  JOIN company.projects p USING(project_id)
GROUP BY p.name
HAVING COUNT(*) > (
    SELECT COUNT(*) * 1.0 / COUNT(DISTINCT p.name) AS avg_emp_per_project
    FROM company.employees e
      JOIN company.employee_projects ep USING(emp_id)
      JOIN company.projects p USING(project_id)
  );
-- 3. Encontre os funcionários que trabalham no mesmo departamento que 'Ana Oliveira'.
-- 4. Liste as linguagens de programação que são dominadas por mais funcionários que a média geral de proficiência.
-- 5. Liste os clientes que possuem mais projetos do que o cliente 'Tech Solutions'.
-- 6. Encontre o funcionário com o maior salário atual.
-- 7. Liste todos os funcionários que possuem mais habilidades que a média da empresa.
-- 8. Encontre o projeto mais recente iniciado pela empresa.
-- 9. Liste os nomes dos departamentos que possuem mais funcionários que a média geral de funcionários por departamento.
-- 10. Liste as tarefas cujo projeto tem mais de 5 funcionários atribuídos.
-- 11. Encontre os funcionários que possuem salário maior que qualquer funcionário do departamento de Marketing.
-- 12. Liste os funcionários que não participam de nenhum projeto.
-- 13. Encontre o cliente cujo projeto teve a maior quantidade de tarefas concluídas.
-- 14. Liste todos os projetos cujo cliente é do mesmo país que 'GlobalSoft'.
-- 15. Encontre os funcionários que possuem habilidades que nenhum outro funcionário possui.