-- Criar schema
DROP SCHEMA IF EXISTS company CASCADE;
CREATE SCHEMA company;
-- ========================
-- 1. Tabelas principais
-- ========================
-- Departamentos
CREATE TABLE company.departments (
  dept_id SERIAL PRIMARY KEY,
  name VARCHAR(50) NOT NULL,
  location VARCHAR(50)
);
-- Funcionários
CREATE TABLE company.employees (
  emp_id SERIAL PRIMARY KEY,
  first_name VARCHAR(50) NOT NULL,
  last_name VARCHAR(50) NOT NULL,
  position VARCHAR(50) NOT NULL,
  hire_date DATE NOT NULL,
  dept_id INT REFERENCES company.departments(dept_id)
);
-- Clientes
CREATE TABLE company.clients (
  client_id SERIAL PRIMARY KEY,
  name VARCHAR(100) NOT NULL,
  country VARCHAR(50)
);
-- Projetos
CREATE TABLE company.projects (
  project_id SERIAL PRIMARY KEY,
  name VARCHAR(100) NOT NULL,
  start_date DATE,
  end_date DATE,
  client_id INT REFERENCES company.clients(client_id)
);
-- Linguagens de programação
CREATE TABLE company.programming_languages (
  lang_id SERIAL PRIMARY KEY,
  name VARCHAR(50) NOT NULL,
  paradigm VARCHAR(50)
);
-- Habilidades (ligação entre funcionário e linguagem)
CREATE TABLE company.skills (
  emp_id INT REFERENCES company.employees(emp_id),
  lang_id INT REFERENCES company.programming_languages(lang_id),
  proficiency_level VARCHAR(20),
  PRIMARY KEY (emp_id, lang_id)
);
-- Relação funcionários-projetos (N:M)
CREATE TABLE company.employee_projects (
  emp_id INT REFERENCES company.employees(emp_id),
  project_id INT REFERENCES company.projects(project_id),
  role VARCHAR(50),
  PRIMARY KEY (emp_id, project_id)
);
-- Histórico de salários
CREATE TABLE company.salary_history (
  salary_id SERIAL PRIMARY KEY,
  emp_id INT REFERENCES company.employees(emp_id),
  amount DECIMAL(10, 2),
  start_date DATE,
  end_date DATE
);
-- Tarefas dos projetos
CREATE TABLE company.tasks (
  task_id SERIAL PRIMARY KEY,
  project_id INT REFERENCES company.projects(project_id),
  description TEXT,
  status VARCHAR(20),
  assigned_to INT REFERENCES company.employees(emp_id)
);
-- ========================
-- 2. Inserir dados
-- ========================
-- Departamentos
INSERT INTO company.departments (name, location)
VALUES ('Desenvolvimento', 'São Paulo'),
  ('Marketing', 'Rio de Janeiro'),
  ('RH', 'Curitiba'),
  ('Financeiro', 'Belo Horizonte'),
  ('Vendas', 'Porto Alegre'),
  ('Diretoria', 'São Paulo');
-- Funcionários
INSERT INTO company.employees (
    first_name,
    last_name,
    position,
    hire_date,
    dept_id
  )
VALUES ('Carlos', 'Silva', 'CEO', '2015-01-15', 6),
  ('Mariana', 'Souza', 'CTO', '2016-05-20', 1),
  (
    'João',
    'Pereira',
    'Desenvolvedor Backend',
    '2019-03-12',
    1
  ),
  (
    'Ana',
    'Oliveira',
    'Desenvolvedora Frontend',
    '2020-07-01',
    1
  ),
  ('Paulo', 'Lima', 'Designer UX', '2021-02-10', 1),
  (
    'Fernanda',
    'Costa',
    'Analista de Marketing',
    '2018-09-17',
    2
  ),
  (
    'Roberto',
    'Almeida',
    'Gerente de RH',
    '2017-04-05',
    3
  ),
  (
    'Juliana',
    'Mendes',
    'Recrutadora',
    '2020-08-14',
    3
  ),
  ('Ricardo', 'Ferreira', 'CFO', '2015-06-30', 4),
  (
    'Sofia',
    'Ramos',
    'Analista Financeiro',
    '2019-11-25',
    4
  ),
  ('Lucas', 'Martins', 'Vendedor', '2021-01-20', 5),
  ('Clara', 'Vieira', 'Vendedora', '2022-03-10', 5),
  (
    'Felipe',
    'Santos',
    'DevOps Engineer',
    '2019-06-18',
    1
  ),
  (
    'Camila',
    'Batista',
    'QA Engineer',
    '2020-10-08',
    1
  ),
  (
    'Pedro',
    'Gomes',
    'Product Manager',
    '2018-12-15',
    1
  ),
  (
    'André',
    'Farias',
    'Data Scientist',
    '2021-05-22',
    1
  ),
  (
    'Larissa',
    'Carvalho',
    'Scrum Master',
    '2019-09-09',
    1
  ),
  (
    'Gustavo',
    'Rocha',
    'Engenheiro de Software',
    '2022-01-11',
    1
  ),
  (
    'Tatiane',
    'Lopes',
    'Especialista em SEO',
    '2020-04-04',
    2
  ),
  (
    'Marcelo',
    'Barbosa',
    'Atendente Comercial',
    '2021-06-21',
    5
  );
-- Clientes
INSERT INTO company.clients (name, country)
VALUES ('Tech Solutions', 'Brasil'),
  ('GlobalSoft', 'EUA'),
  ('DataCorp', 'Canadá'),
  ('InnovaWeb', 'Alemanha'),
  ('Cloudify', 'Austrália');
-- Projetos
INSERT INTO company.projects (name, start_date, end_date, client_id)
VALUES (
    'Plataforma de E-commerce',
    '2021-01-10',
    NULL,
    1
  ),
  (
    'Sistema de Análise de Dados',
    '2020-05-15',
    '2022-06-30',
    3
  ),
  ('Aplicativo de Delivery', '2022-02-01', NULL, 2),
  (
    'Ferramenta de Gestão Financeira',
    '2021-09-20',
    NULL,
    5
  ),
  (
    'Website Corporativo',
    '2021-11-05',
    '2022-04-10',
    4
  );
-- Linguagens
INSERT INTO company.programming_languages (name, paradigm)
VALUES ('Python', 'Multiparadigma'),
  ('JavaScript', 'Multiparadigma'),
  ('Java', 'Orientado a Objetos'),
  ('Go', 'Concorrente'),
  ('C#', 'Orientado a Objetos');
-- Skills
INSERT INTO company.skills
VALUES (3, 1, 'Avançado'),
  (3, 3, 'Intermediário'),
  (4, 2, 'Avançado'),
  (4, 1, 'Intermediário'),
  (5, 2, 'Intermediário'),
  (13, 4, 'Avançado'),
  (14, 2, 'Avançado'),
  (14, 1, 'Intermediário'),
  (15, 1, 'Avançado'),
  (16, 1, 'Avançado'),
  (16, 2, 'Intermediário'),
  (18, 3, 'Avançado');
-- Funcionários em projetos
INSERT INTO company.employee_projects
VALUES (3, 1, 'Backend Developer'),
  (4, 1, 'Frontend Developer'),
  (5, 1, 'UI/UX Designer'),
  (13, 1, 'DevOps'),
  (14, 1, 'QA Engineer'),
  (15, 1, 'Product Manager'),
  (16, 2, 'Data Scientist'),
  (3, 2, 'Backend Developer'),
  (18, 4, 'Software Engineer'),
  (4, 3, 'Frontend Developer');
-- Histórico de salários
INSERT INTO company.salary_history (emp_id, amount, start_date, end_date)
VALUES (1, 25000, '2015-01-15', NULL),
  (2, 20000, '2016-05-20', NULL),
  (3, 8000, '2019-03-12', '2021-03-12'),
  (3, 9500, '2021-03-13', NULL),
  (4, 8500, '2020-07-01', NULL),
  (13, 10000, '2019-06-18', NULL),
  (16, 11000, '2021-05-22', NULL);
-- Tarefas
INSERT INTO company.tasks (project_id, description, status, assigned_to)
VALUES (1, 'Criar API de produtos', 'Em progresso', 3),
  (
    1,
    'Desenvolver interface principal',
    'Concluído',
    4
  ),
  (
    1,
    'Configurar infraestrutura de nuvem',
    'Em progresso',
    13
  ),
  (
    1,
    'Testar funcionalidades de pagamento',
    'Pendente',
    14
  ),
  (
    3,
    'Implementar tela de pedidos',
    'Em progresso',
    4
  ),
  (
    4,
    'Desenvolver módulo de relatórios',
    'Pendente',
    18
  );