CREATE SCHEMA universidade;
CREATE DOMAIN universidade.matricula AS VARCHAR(7);
CREATE DOMAIN universidade.tipo_cpf AS BIGINT;
CREATE TABLE universidade.usuario(
  cpf universidade.tipo_cpf,
  primeiro_nome VARCHAR(30) NOT NULL,
  sobrenome VARCHAR(60) NOT NULL,
  data_nascimento DATE,
  email VARCHAR [],
  telefone VARCHAR [],
  CONSTRAINT pk_usuario PRIMARY KEY (cpf)
);
CREATE TABLE universidade.endereco(
  id_endereco SERIAL,
  cep VARCHAR(8) NOT NULL,
  rua VARCHAR(255) NOT NULL,
  bairro VARCHAR(60),
  cidade VARCHAR(60) NOT NULL,
  estado VARCHAR(60) NOT NULL,
  pais VARCHAR(60) NOT NULL,
  CONSTRAINT pk_endereco PRIMARY KEY (id_endereco),
  CONSTRAINT uq_endereco UNIQUE (cep, rua, bairro, cidade, estado)
);
CREATE TABLE universidade.mora(
  id_endereco INT NOT NULL,
  cpf universidade.tipo_cpf NOT NULL,
  numero INT,
  complemento VARCHAR,
  CONSTRAINT pk_mora PRIMARY KEY (id_endereco, cpf),
  CONSTRAINT fk_endereco FOREIGN KEY (id_endereco) REFERENCES universidade.endereco(id_endereco) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT fk_usuario FOREIGN KEY (cpf) REFERENCES universidade.usuario(cpf) ON DELETE CASCADE ON UPDATE CASCADE
);
CREATE TABLE universidade.livro(
  ISBN bigint NOT NULL,
  titulo VARCHAR NOT NULL,
  autor VARCHAR NOT NULL,
  numero_paginas INT,
  edicao INT,
  CONSTRAINT pk_livro PRIMARY KEY (ISBN),
  CONSTRAINT ck_paginas CHECK (numero_paginas > 0)
);
CREATE TYPE universidade.status_exemplar AS ENUM ('disponivel', 'indisponivel', 'emprestado');
CREATE TABLE universidade.exemplar(
  id_exemplar SERIAL NOT NULL,
  ISBN bigint NOT NULL,
  numero INT NOT NULL,
  status universidade.status_exemplar,
  CONSTRAINT pk_exemplar PRIMARY KEY (id_exemplar),
  CONSTRAINT uq_exemplar UNIQUE (ISBN, numero),
  CONSTRAINT fk_livro FOREIGN KEY (ISBN) REFERENCES universidade.livro(ISBN) ON DELETE CASCADE ON UPDATE CASCADE
);
CREATE TABLE universidade.status_emprestimo(
  id_status SERIAL,
  status VARCHAR NOT NULL,
  PRIMARY KEY (id_status)
);
CREATE TABLE universidade.emprestimo(
  id_emprestimo SERIAL NOT NULL,
  id_exemplar INT NOT NULL,
  cpf universidade.tipo_cpf NOT NULL,
  status INT NOT NULL,
  data_emprestimo DATE NOT NULL,
  data_entrega DATE NOT NULL,
  CONSTRAINT pk_emprestimo PRIMARY KEY(id_emprestimo),
  CONSTRAINT fk_exemplar FOREIGN KEY (id_exemplar) REFERENCES universidade.exemplar(id_exemplar) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT fk_status FOREIGN KEY (status) REFERENCES universidade.status_emprestimo(id_status) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT fk_usuario FOREIGN KEY (cpf) REFERENCES universidade.usuario(cpf) ON DELETE CASCADE ON UPDATE CASCADE
);
CREATE TABLE universidade.cargo(
  id_cargo INT,
  carga_horaria VARCHAR,
  salario REAL,
  CONSTRAINT pk_cargo PRIMARY KEY(id_cargo),
  CONSTRAINT ck_sal_negativo CHECK(salario > 0)
);
CREATE TABLE universidade.professor(
  mat_professor universidade.matricula,
  cpf universidade.tipo_cpf UNIQUE,
  cargo INT NOT NULL,
  departamento VARCHAR(5),
  CONSTRAINT pk_professor PRIMARY KEY(mat_professor),
  CONSTRAINT fk_usuario FOREIGN KEY (cpf) REFERENCES universidade.usuario(cpf) ON DELETE
  SET NULL ON UPDATE CASCADE,
    CONSTRAINT fk_cargo FOREIGN KEY (cargo) REFERENCES universidade.cargo(id_cargo) ON DELETE
  SET NULL ON UPDATE CASCADE
);
CREATE TABLE universidade.departamento(
  cod_depto VARCHAR(5),
  nome VARCHAR(50) NOT NULL,
  chefe universidade.matricula,
  orcamento REAL CONSTRAINT ck_orcamento CHECK(orcamento > 0),
  CONSTRAINT pk_departamento PRIMARY KEY(cod_depto)
);
ALTER TABLE universidade.professor
ADD CONSTRAINT fk_alocacao FOREIGN KEY (departamento) REFERENCES universidade.departamento(cod_depto) ON DELETE
SET NULL ON UPDATE CASCADE;
ALTER TABLE universidade.departamento
ADD CONSTRAINT fk_chefia FOREIGN KEY (chefe) REFERENCES universidade.professor(mat_professor) ON DELETE
SET NULL ON UPDATE CASCADE;
CREATE TABLE universidade.estudante(
  mat_estudante universidade.matricula,
  cpf universidade.tipo_cpf,
  MC float,
  CONSTRAINT pk_estudante PRIMARY KEY(mat_estudante),
  CONSTRAINT fk_usuario FOREIGN KEY (cpf) REFERENCES universidade.usuario(cpf) ON DELETE
  SET NULL ON UPDATE CASCADE,
    CONSTRAINT uq_cpf UNIQUE(cpf)
);
CREATE TABLE universidade.projeto(
  id_projeto INT,
  descricao VARCHAR,
  CONSTRAINT pk_projeto PRIMARY KEY(id_projeto)
);
CREATE TABLE universidade.plano(
  id_projeto INT,
  mat_professor universidade.matricula,
  mat_estudante universidade.matricula,
  ano INT,
  CONSTRAINT pk_plano PRIMARY KEY(mat_estudante, ano),
  CONSTRAINT fk_projeto FOREIGN KEY (id_projeto) REFERENCES universidade.projeto(id_projeto) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT fk_professor FOREIGN KEY (mat_professor) REFERENCES universidade.professor(mat_professor) ON DELETE
  SET NULL ON UPDATE CASCADE,
    CONSTRAINT fk_mat_estudante FOREIGN KEY (mat_estudante) REFERENCES universidade.estudante(mat_estudante) ON DELETE
  SET NULL ON UPDATE CASCADE
);
CREATE TABLE universidade.disciplina(
  cod_disc VARCHAR(8),
  nome VARCHAR(40) NOT NULL,
  pre_req VARCHAR(8),
  creditos SMALLINT CONSTRAINT ck_creditos CHECK (
    1 <= creditos
    AND creditos < 12
  ),
  depto_responsavel VARCHAR(5),
  CONSTRAINT pk_disciplina PRIMARY KEY (cod_disc),
  CONSTRAINT fk_pre_req FOREIGN KEY(pre_req) REFERENCES universidade.disciplina(cod_disc) ON DELETE
  SET NULL ON UPDATE CASCADE,
    CONSTRAINT fk_responsavel FOREIGN KEY(depto_responsavel) REFERENCES universidade.departamento(cod_depto) ON DELETE
  SET NULL ON UPDATE CASCADE
);
CREATE SEQUENCE universidade.seq_turma INCREMENT 1 START 1;
CREATE TABLE universidade.semestre(
  ano SMALLINT,
  semestre SMALLINT,
  data_inicio DATE,
  data_fom DATE,
  CONSTRAINT pk_semestre PRIMARY KEY (ano, semestre)
);
CREATE TABLE universidade.sala(
  id_sala SERIAL,
  descricao VARCHAR,
  CONSTRAINT pk_sala PRIMARY KEY (id_sala)
);
CREATE TABLE universidade.horario(
  id_horario SERIAL,
  dia VARCHAR(15) NOT NULL,
  slot SMALLINT NOT NULL,
  CONSTRAINT pk_horario PRIMARY KEY (id_horario)
);
CREATE TABLE universidade.turma(
  id_turma INT DEFAULT nextval('universidade.seq_turma'),
  cod_disc VARCHAR(8) NOT NULL,
  numero INT,
  ano SMALLINT,
  semestre SMALLINT,
  CONSTRAINT pk_turma PRIMARY KEY(id_turma),
  CONSTRAINT uq_turma UNIQUE(cod_disc, numero, semestre, ano),
  CONSTRAINT fk_disciplina FOREIGN KEY(cod_disc) REFERENCES universidade.disciplina(cod_disc) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT fk_semestre FOREIGN KEY(ano, semestre) REFERENCES universidade.semestre(ano, semestre) ON DELETE NO ACTION ON UPDATE CASCADE
);
CREATE TABLE universidade.leciona(
  id_turma INT NOT NULL,
  mat_professor universidade.matricula NOT NULL,
  CONSTRAINT pk_leciona PRIMARY KEY (id_turma, mat_professor),
  CONSTRAINT fk_turma FOREIGN KEY (id_turma) REFERENCES universidade.turma(id_turma) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT fk_professor FOREIGN KEY (mat_professor) REFERENCES universidade.professor(mat_professor) ON DELETE CASCADE ON UPDATE CASCADE
);
CREATE TABLE universidade.alocacao(
  id_turma INT,
  id_horario INT,
  id_sala INT,
  CONSTRAINT pk_alocacao PRIMARY KEY (id_turma, id_horario),
  CONSTRAINT uq_alocacao UNIQUE(id_horario, id_sala)
);
CREATE TABLE universidade.cursa(
  mat_estudante universidade.matricula,
  id_turma INT,
  nota REAL,
  CONSTRAINT pk_cursa PRIMARY KEY(mat_estudante, id_turma),
  CONSTRAINT fk_turma FOREIGN KEY(id_turma) REFERENCES universidade.turma(id_turma) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT fk_estudante FOREIGN KEY(mat_estudante) REFERENCES universidade.estudante(mat_estudante) ON DELETE CASCADE ON UPDATE CASCADE
);
-- insert_universidade.sql
-- Script to populate the universidade schema with data
-- Endereço (Addresses)
INSERT INTO universidade.endereco
VALUES (
    1,
    '49100000',
    'Rua A',
    'Jd. Rosa Elze',
    'São Cristóvão',
    'Sergipe',
    'Brasil'
  ),
  (
    2,
    '49100001',
    'Rua B',
    'Centro',
    'São Cristóvão',
    'Sergipe',
    'Brasil'
  ),
  (
    3,
    '49010000',
    'Rua C',
    'Centro',
    'Aracaju',
    'Sergipe',
    'Brasil'
  ),
  (
    4,
    '49025000',
    'Rua A',
    'Salgado Filho',
    'Aracaju',
    'Sergipe',
    'Brasil'
  ),
  (
    5,
    '49095000',
    'Rua A',
    'Atalaia',
    'Aracaju',
    'Sergipe',
    'Brasil'
  ),
  (
    6,
    '49100002',
    'Rua ABC',
    'Jd. Rosa Elze',
    'São Cristóvão',
    'Sergipe',
    'Brasil'
  ),
  (
    7,
    '49100003',
    'Rua C',
    'Jd. Rosa Elze',
    'São Cristóvão',
    'Sergipe',
    'Brasil'
  ),
  (
    8,
    '49100004',
    'Rua A',
    'Centro',
    'São Cristóvão',
    'Sergipe',
    'Brasil'
  ),
  (
    9,
    '49020000',
    'Rua A',
    'Cirurgia',
    'Aracaju',
    'Sergipe',
    'Brasil'
  ),
  (
    10,
    '49030000',
    'Rua A1',
    'Bairro América',
    'Aracaju',
    'Sergipe',
    'Brasil'
  ),
  (
    11,
    '49040000',
    'Rua A',
    'Siqueira Campos',
    'Aracaju',
    'Sergipe',
    'Brasil'
  ),
  (
    12,
    '49050000',
    'Rua A',
    'Getúlio Vargas',
    'Aracaju',
    'Sergipe',
    'Brasil'
  ),
  (
    13,
    '40010000',
    'Rua Salvador',
    'Pelourinho',
    'Salvador',
    'Bahia',
    'Brasil'
  ),
  (
    14,
    '50010000',
    'Rua Recife',
    'Boa Viagem',
    'Recife',
    'Pernambuco',
    'Brasil'
  ),
  (
    15,
    '60010000',
    'Rua Fortaleza',
    'Meireles',
    'Fortaleza',
    'Ceará',
    'Brasil'
  ),
  (
    16,
    '70010000',
    'Rua Brasília',
    'Asa Sul',
    'Brasília',
    'Distrito Federal',
    'Brasil'
  ),
  (
    17,
    '80010000',
    'Rua Curitiba',
    'Centro',
    'Curitiba',
    'Paraná',
    'Brasil'
  ),
  (
    18,
    '90010000',
    'Rua Porto Alegre',
    'Cidade Baixa',
    'Porto Alegre',
    'Rio Grande do Sul',
    'Brasil'
  ),
  (
    19,
    '20010000',
    'Rua Rio',
    'Copacabana',
    'Rio de Janeiro',
    'Rio de Janeiro',
    'Brasil'
  ),
  (
    20,
    '01010000',
    'Rua São Paulo',
    'Consolação',
    'São Paulo',
    'São Paulo',
    'Brasil'
  );
-- Departamento (Departments)
INSERT INTO universidade.departamento
VALUES (
    'DCOMP',
    'Departamento de Computação',
    NULL,
    150000
  ),
  (
    'DCOM2',
    'Departamento de Computação Avançada',
    NULL,
    80000
  ),
  (
    'DMA',
    'Departamento de Matemática',
    NULL,
    120000
  ),
  ('DFI', 'Departamento de Física', NULL, 200000),
  (
    'DECAT',
    'Departamento de Estatística e Ciências Atuariais',
    NULL,
    90000
  ),
  (
    'DEL',
    'Departamento de Engenharia Elétrica',
    NULL,
    110000
  ),
  ('DBIO', 'Departamento de Biologia', NULL, 95000),
  ('DQUI', 'Departamento de Química', NULL, 100000),
  ('DHIS', 'Departamento de História', NULL, 70000),
  ('DLET', 'Departamento de Letras', NULL, 60000);
-- Cargo (Roles)
INSERT INTO universidade.cargo
VALUES (1, 'DE', 6000),
  (2, '20h', 3000),
  (3, '40h', 5500),
  (4, '30h', 4500),
  (5, 'Coordenador', 8000),
  (6, 'Pesquisador', 7000);
-- Usuário (Users)
INSERT INTO universidade.usuario
VALUES (
    '11111111100',
    'Ana',
    'Silva',
    '1975-03-15',
    '{"ana.silva@email.com"}',
    '{"99991234"}'
  ),
  (
    '11111111101',
    'Bruno',
    'Costa',
    '1980-07-20',
    '{"bruno.costa@email.com"}',
    '{"99992345"}'
  ),
  (
    '11111111102',
    'Clara',
    'Mendes',
    '1978-05-10',
    '{"clara.mendes@email.com"}',
    '{"99993456"}'
  ),
  (
    '11111111103',
    'Diego',
    'Santos',
    '1965-01-30',
    NULL,
    NULL
  ),
  (
    '11111111104',
    'Elisa',
    'Ferreira',
    '1982-04-25',
    '{"elisa.ferreira@email.com"}',
    '{"99994567"}'
  ),
  (
    '11111111105',
    'Fábio',
    'Oliveira',
    '1970-06-05',
    '{"fabio.oliveira@email.com"}',
    '{"99995678"}'
  ),
  (
    '11111111106',
    'Gabriela',
    'Pereira',
    '1985-08-15',
    '{"gabriela.pereira@email.com"}',
    '{"99996789"}'
  ),
  (
    '11111111107',
    'Hugo',
    'Almeida',
    '1972-09-03',
    '{"hugo.almeida@email.com"}',
    '{"99997890"}'
  ),
  (
    '11111111108',
    'Isabela',
    'Ribeiro',
    '1988-02-01',
    '{"isabela.ribeiro@email.com"}',
    '{"99998901"}'
  ),
  (
    '11111111109',
    'João',
    'Lima',
    '1976-11-10',
    '{"joao.lima@email.com"}',
    NULL
  ),
  (
    '11111111110',
    'Karina',
    'Gomes',
    '1983-12-05',
    '{"karina.gomes@email.com"}',
    '{"99990123"}'
  ),
  (
    '11111111111',
    'Lucas',
    'Martins',
    '1979-03-04',
    '{"lucas.martins@email.com"}',
    '{"99991234"}'
  ),
  (
    '11111111112',
    'Mariana',
    'Rocha',
    '1981-10-15',
    '{"mariana.rocha@email.com"}',
    '{"99992345"}'
  ),
  (
    '11111111113',
    'Nelson',
    'Barbosa',
    '1968-06-20',
    '{"nelson.barbosa@email.com"}',
    '{"99993456"}'
  ),
  (
    '11111111114',
    'Olga',
    'Souza',
    '1974-07-25',
    '{"olga.souza@email.com"}',
    '{"99994567"}'
  ),
  (
    '22222222201',
    'Pedro',
    'Sousa',
    '1995-03-05',
    '{"pedro.sousa@email.com"}',
    NULL
  ),
  (
    '22222222202',
    'Quintino',
    'Nunes',
    '1997-09-15',
    '{"quintino.nunes@email.com"}',
    NULL
  ),
  (
    '22222222203',
    'Rafaela',
    'Campos',
    '1998-07-23',
    NULL,
    NULL
  ),
  (
    '22222222204',
    'Sofia',
    'Moraes',
    '1996-10-07',
    '{"sofia.moraes@email.com"}',
    NULL
  ),
  (
    '22222222205',
    'Tiago',
    'Viana',
    '1994-11-27',
    NULL,
    NULL
  ),
  (
    '22222222206',
    'Ursula',
    'Barros',
    '1999-12-10',
    '{"ursula.barros@email.com"}',
    NULL
  ),
  (
    '22222222207',
    'Vinicius',
    'Freitas',
    '1997-12-26',
    NULL,
    NULL
  ),
  (
    '22222222208',
    'Wendy',
    'Dias',
    '1995-12-26',
    NULL,
    NULL
  ),
  (
    '22222222209',
    'Xavier',
    'Correia',
    '1998-06-11',
    '{"xavier.correia@email.com"}',
    NULL
  ),
  (
    '22222222210',
    'Yasmin',
    'Fonseca',
    '1996-01-10',
    '{"yasmin.fonseca@email.com"}',
    NULL
  ),
  (
    '22222222211',
    'Zeca',
    'Melo',
    '1997-01-10',
    '{"zeca.melo@email.com"}',
    NULL
  ),
  (
    '22222222212',
    'Alice',
    'Carvalho',
    '1999-03-24',
    NULL,
    NULL
  ),
  (
    '22222222213',
    'Bernardo',
    'Teixeira',
    '1998-08-22',
    NULL,
    NULL
  ),
  (
    '22222222214',
    'Cecilia',
    'Lopes',
    '1995-04-15',
    '{"cecilia.lopes@email.com"}',
    NULL
  ),
  (
    '22222222215',
    'Daniel',
    'Castro',
    '1996-05-20',
    '{"daniel.castro@email.com"}',
    NULL
  );
-- Mora (Residency)
INSERT INTO universidade.mora
VALUES (1, '11111111100', 100, NULL),
  (2, '11111111101', 75, 'Apto 101'),
  (3, '11111111102', 200, NULL),
  (4, '11111111103', 150, NULL),
  (5, '11111111104', 500, 'Casa 2'),
  (6, '11111111105', 1024, NULL),
  (7, '11111111106', 300, NULL),
  (8, '11111111107', 400, 'Apto 202'),
  (9, '11111111108', 477, NULL),
  (10, '11111111109', 1000, NULL),
  (11, '11111111110', 120, NULL),
  (12, '11111111111', 600, NULL),
  (13, '11111111112', 800, 'Apto 303'),
  (14, '11111111113', 250, NULL),
  (15, '11111111114', 700, NULL),
  (16, '22222222201', 100, NULL),
  (17, '22222222202', 200, NULL),
  (18, '22222222203', 300, NULL),
  (19, '22222222204', 400, NULL),
  (20, '22222222205', 500, NULL),
  (1, '22222222206', 600, NULL),
  (2, '22222222207', 700, NULL),
  (3, '22222222208', 800, NULL),
  (4, '22222222209', 900, NULL),
  (5, '22222222210', 1000, NULL),
  (6, '22222222211', 1100, NULL),
  (7, '22222222212', 1200, NULL),
  (8, '22222222213', 1300, NULL),
  (9, '22222222214', 1400, NULL),
  (10, '22222222215', 1500, NULL);
-- Professor (Professors)
INSERT INTO universidade.professor
VALUES ('P100', '11111111100', 1, 'DCOMP'),
  ('P200', '11111111101', 2, 'DCOMP'),
  ('P300', '11111111102', 3, 'DCOMP'),
  ('P400', '11111111103', 1, 'DCOMP'),
  ('P500', '11111111104', 1, 'DCOMP'),
  ('P600', '11111111105', 1, 'DMA'),
  ('P700', '11111111106', 3, 'DFI'),
  ('P800', '11111111107', 2, 'DCOMP'),
  ('P900', '11111111108', 2, 'DMA'),
  ('P1000', '11111111109', 3, 'DMA'),
  ('P1100', '11111111110', 2, 'DECAT'),
  ('P1200', '11111111111', 1, 'DMA'),
  ('P1300', '11111111112', 1, 'DMA'),
  ('P1400', '11111111113', 1, 'DFI'),
  ('P1500', '11111111114', 3, 'DCOM2');
-- Departamento (Update Heads)
UPDATE universidade.departamento
SET chefe = 'P100'
WHERE cod_depto = 'DCOMP';
UPDATE universidade.departamento
SET chefe = 'P600'
WHERE cod_depto = 'DMA';
UPDATE universidade.departamento
SET chefe = 'P1100'
WHERE cod_depto = 'DECAT';
UPDATE universidade.departamento
SET chefe = 'P1400'
WHERE cod_depto = 'DFI';
UPDATE universidade.departamento
SET chefe = 'P1500'
WHERE cod_depto = 'DCOM2';
UPDATE universidade.departamento
SET chefe = 'P700'
WHERE cod_depto = 'DEL';
UPDATE universidade.departamento
SET chefe = 'P900'
WHERE cod_depto = 'DBIO';
UPDATE universidade.departamento
SET chefe = 'P1000'
WHERE cod_depto = 'DQUI';
UPDATE universidade.departamento
SET chefe = 'P1200'
WHERE cod_depto = 'DHIS';
UPDATE universidade.departamento
SET chefe = 'P1300'
WHERE cod_depto = 'DLET';
-- Estudante (Students)
INSERT INTO universidade.estudante
VALUES ('E101', '22222222201', 7.5),
  ('E102', '22222222202', 8.0),
  ('E103', '22222222203', 6.5),
  ('E104', '22222222204', 7.8),
  ('E105', '22222222205', 9.0),
  ('E106', '22222222206', 8.2),
  ('E107', '22222222207', 6.0),
  ('E108', '22222222208', 7.3),
  ('E109', '22222222209', 6.8),
  ('E110', '22222222210', 5.5),
  ('E111', '22222222211', 7.0),
  ('E112', '22222222212', 6.2),
  ('E113', '22222222213', 8.5),
  ('E114', '22222222214', 7.7),
  ('E115', '22222222215', 8.3);
-- Projeto (Projects)
INSERT INTO universidade.projeto
VALUES (1, 'Inteligência Artificial em Educação'),
  (2, 'Desenvolvimento de Software Livre'),
  (3, 'Análise de Dados em Saúde'),
  (4, 'Energia Renovável'),
  (5, 'Preservação Ambiental'),
  (6, 'Computação Quântica'),
  (7, 'História da Ciência no Brasil'),
  (8, 'Química Sustentável'),
  (9, 'Engenharia de Sistemas Embarcados'),
  (10, 'Literatura Brasileira Contemporânea');
-- Plano (Plans)
INSERT INTO universidade.plano
VALUES (1, 'P100', 'E101', 2020),
  (2, 'P200', 'E102', 2020),
  (3, 'P300', 'E103', 2020),
  (4, 'P400', 'E104', 2020),
  (5, 'P600', 'E105', 2020),
  (6, 'P700', 'E106', 2021),
  (7, 'P900', 'E107', 2021),
  (8, 'P1000', 'E108', 2021),
  (9, 'P1100', 'E109', 2021),
  (10, 'P1200', 'E110', 2021),
  (1, 'P100', 'E111', 2022),
  (2, 'P200', 'E112', 2022),
  (3, 'P300', 'E113', 2022),
  (4, 'P400', 'E114', 2022),
  (5, 'P600', 'E115', 2022);
-- Disciplina (Disciplines)
INSERT INTO universidade.disciplina
VALUES (
    'COMP0196',
    'Fundamentos da Computação',
    NULL,
    4,
    'DCOMP'
  ),
  (
    'COMP0197',
    'Programação Imperativa',
    NULL,
    6,
    'DCOMP'
  ),
  (
    'COMP0198',
    'Programação Orientada à Objetos',
    'COMP0197',
    4,
    'DCOMP'
  ),
  (
    'COMP0199',
    'Programação Declarativa',
    'COMP0197',
    4,
    'DCOMP'
  ),
  (
    'COMP0212',
    'Estrutura de Dados I',
    'COMP0197',
    6,
    'DCOMP'
  ),
  (
    'COMP0213',
    'Estrutura de Dados II',
    'COMP0212',
    4,
    'DCOMP'
  ),
  (
    'COMP0222',
    'Inteligência Artificial',
    'COMP0199',
    4,
    'DCOMP'
  ),
  (
    'COMP0233',
    'Lógica para Computação',
    NULL,
    4,
    'DCOMP'
  ),
  (
    'COMP0279',
    'Desenvolvimento de Software I',
    'COMP0197',
    4,
    'DCOMP'
  ),
  (
    'COMP0280',
    'Desenvolvimento de Software II',
    'COMP0279',
    4,
    'DCOMP'
  ),
  (
    'COMP0281',
    'Desenvolvimento de Software III',
    'COMP0280',
    4,
    'DCOMP'
  ),
  (
    'COMP0298',
    'Redes de Computadores',
    NULL,
    4,
    NULL
  ),
  (
    'COMP0311',
    'Banco de Dados',
    'COMP0213',
    4,
    'DCOMP'
  ),
  (
    'COMP0326',
    'Sistemas Distribuídos',
    'COMP0298',
    4,
    'DCOMP'
  ),
  ('MAT0064', 'Cálculo I', NULL, 6, 'DMA'),
  ('MAT0065', 'Cálculo II', 'MAT0064', 6, 'DMA'),
  ('MAT0096', 'Cálculo Numérico', NULL, 4, 'DMA'),
  ('FISI0050', 'Física A', NULL, 4, 'DFI'),
  ('FISI0051', 'Física B', 'FISI0050', 4, 'DFI'),
  ('BIO0100', 'Biologia Molecular', NULL, 4, 'DBIO'),
  ('QUI0200', 'Química Orgânica', NULL, 4, 'DQUI'),
  ('HIS0300', 'História do Brasil', NULL, 4, 'DHIS'),
  (
    'LET0400',
    'Literatura Portuguesa',
    NULL,
    4,
    'DLET'
  );
-- Semestre (Semesters)
INSERT INTO universidade.semestre
VALUES (2017, 1, '2017-02-01', '2017-06-30'),
  (2017, 2, '2017-08-01', '2017-12-20'),
  (2018, 1, '2018-02-01', '2018-06-30'),
  (2018, 2, '2018-08-01', '2018-12-20'),
  (2019, 1, '2019-02-01', '2019-06-30'),
  (2019, 2, '2019-08-01', '2019-12-20'),
  (2020, 1, '2020-02-01', '2020-06-30'),
  (2020, 2, '2020-08-01', '2020-12-20');
-- Turma (Classes)
INSERT INTO universidade.turma
VALUES (1, 'COMP0212', 1, 2017, 2),
  (2, 'COMP0213', 1, 2017, 2),
  (3, 'COMP0311', 1, 2017, 2),
  (4, 'COMP0198', 1, 2017, 2),
  (5, 'COMP0199', 1, 2017, 2),
  (6, 'COMP0233', 1, 2017, 2),
  (7, 'COMP0196', 1, 2017, 2),
  (8, 'COMP0197', 1, 2017, 2),
  (9, 'COMP0279', 1, 2017, 2),
  (10, 'COMP0280', 1, 2017, 2),
  (11, 'COMP0281', 1, 2017, 2),
  (12, 'COMP0298', 1, 2017, 2),
  (13, 'COMP0326', 1, 2017, 2),
  (14, 'MAT0096', 1, 2017, 2),
  (15, 'MAT0064', 1, 2017, 2),
  (16, 'MAT0096', 2, 2017, 2),
  (17, 'MAT0096', 3, 2017, 2),
  (18, 'MAT0065', 1, 2017, 2),
  (19, 'FISI0050', 1, 2017, 2),
  (20, 'COMP0199', 2, 2017, 2),
  (21, 'COMP0233', 2, 2017, 2),
  (22, 'COMP0222', 1, 2017, 2),
  (23, 'MAT0065', 1, 2019, 1),
  (24, 'FISI0050', 1, 2019, 1),
  (25, 'COMP0199', 2, 2019, 1),
  (26, 'COMP0233', 2, 2019, 1),
  (27, 'COMP0222', 1, 2019, 1),
  (28, 'BIO0100', 1, 2018, 1),
  (29, 'QUI0200', 1, 2018, 1),
  (30, 'HIS0300', 1, 2018, 1),
  (31, 'LET0400', 1, 2018, 1),
  (32, 'COMP0196', 1, 2018, 2),
  (33, 'COMP0197', 1, 2018, 2),
  (34, 'MAT0064', 1, 2018, 2);
-- Leciona (Teaching Assignments)
INSERT INTO universidade.leciona
VALUES (1, 'P100'),
  (2, 'P100'),
  (3, 'P100'),
  (4, 'P200'),
  (5, 'P200'),
  (6, 'P300'),
  (7, 'P300'),
  (8, 'P300'),
  (9, 'P400'),
  (10, 'P400'),
  (11, 'P400'),
  (12, 'P500'),
  (13, 'P500'),
  (14, 'P600'),
  (15, 'P600'),
  (16, 'P900'),
  (17, 'P1300'),
  (18, 'P1200'),
  (19, 'P700'),
  (20, 'P1500'),
  (21, 'P1500'),
  (22, 'P100'),
  (23, 'P600'),
  (24, 'P700'),
  (25, 'P1500'),
  (26, 'P1500'),
  (27, 'P100'),
  (28, 'P900'),
  (29, 'P1000'),
  (30, 'P1200'),
  (31, 'P1300'),
  (32, 'P300'),
  (33, 'P300'),
  (34, 'P600');
-- Cursa (Enrollments)
INSERT INTO universidade.cursa
VALUES ('E101', 1, 7.5),
  ('E102', 1, 8.0),
  ('E103', 1, 6.5),
  ('E104', 1, 9.0),
  ('E105', 2, 8.5),
  ('E106', 2, 7.0),
  ('E107', 2, 6.0),
  ('E108', 2, 8.0),
  ('E109', 3, 7.0),
  ('E110', 3, 6.5),
  ('E111', 3, 8.5),
  ('E112', 3, 9.0),
  ('E113', 4, 7.5),
  ('E114', 4, 8.0),
  ('E115', 4, 6.5),
  ('E101', 5, 8.5),
  ('E102', 6, NULL),
  ('E103', 6, NULL),
  ('E104', 6, NULL),
  ('E105', 6, NULL),
  ('E106', 7, 9.0),
  ('E107', 7, 8.0),
  ('E108', 7, 7.5),
  ('E109', 7, 8.5),
  ('E110', 8, 6.0),
  ('E111', 8, 7.0),
  ('E112', 8, 8.0),
  ('E113', 8, 9.0),
  ('E114', 9, 7.5),
  ('E115', 9, 8.5),
  ('E101', 10, 6.5),
  ('E102', 10, 7.0),
  ('E103', 11, 8.0),
  ('E104', 11, 9.0),
  ('E105', 11, 7.5),
  ('E106', 12, 8.5),
  ('E107', 12, 7.0),
  ('E108', 12, 6.5),
  ('E109', 13, 8.0),
  ('E110', 13, 9.0),
  ('E111', 14, 7.5),
  ('E112', 14, 8.0),
  ('E113', 15, 6.5),
  ('E114', 15, 7.0),
  ('E115', 16, 8.5),
  ('E101', 16, 9.0),
  ('E102', 17, 7.5),
  ('E103', 17, 8.0),
  ('E104', 18, 6.5),
  ('E105', 18, 7.0),
  ('E106', 19, 8.5),
  ('E107', 19, 9.0),
  ('E108', 20, 7.5),
  ('E109', 20, 8.0),
  ('E110', 21, 6.5),
  ('E111', 21, 7.0),
  ('E112', 22, 8.5),
  ('E113', 22, 9.0),
  ('E114', 23, 7.5),
  ('E115', 23, 8.0),
  ('E101', 24, 6.5),
  ('E102', 24, 7.0),
  ('E103', 25, 8.5),
  ('E104', 25, 9.0),
  ('E105', 26, 7.5),
  ('E106', 26, 8.0),
  ('E107', 27, 6.5),
  ('E108', 27, 7.0),
  ('E109', 28, 8.5),
  ('E110', 28, 9.0),
  ('E111', 29, 7.5),
  ('E112', 29, 8.0),
  ('E113', 30, 6.5),
  ('E114', 30, 7.0),
  ('E115', 31, 8.5),
  ('E101', 31, 9.0),
  ('E102', 32, 7.5),
  ('E103', 32, 8.0),
  ('E104', 33, 6.5),
  ('E105', 33, 7.0),
  ('E106', 34, 8.5),
  ('E107', 34, 9.0);
-- Livro (Books)
INSERT INTO universidade.livro
VALUES (
    9788579360855,
    'Sistemas de Banco de Dados',
    'Ramez Elmasri',
    808,
    6
  ),
  (
    9788535245356,
    'Sistemas de Banco de Dados',
    'Ramez Elmasri',
    904,
    6
  ),
  (
    9780133970777,
    'Introduction to Algorithms',
    'Thomas H. Cormen',
    1312,
    3
  ),
  (
    9780131103627,
    'C Programming Language',
    'Brian Kernighan',
    272,
    2
  ),
  (
    9780134444284,
    'Computer Networking',
    'James Kurose',
    864,
    7
  ),
  (
    9780262033848,
    'Introduction to Machine Learning',
    'Ethem Alpaydin',
    640,
    4
  ),
  (
    9780134093413,
    'Calculus',
    'James Stewart',
    1392,
    8
  ),
  (
    9780321573513,
    'Physics for Scientists and Engineers',
    'Paul A. Tipler',
    1350,
    6
  ),
  (
    9780198504634,
    'Molecular Biology',
    'Robert Weaver',
    912,
    5
  ),
  (
    9780470658000,
    'Organic Chemistry',
    'David R. Klein',
    1344,
    3
  );
-- Exemplar (Book Copies)
INSERT INTO universidade.exemplar
VALUES (1, 9788579360855, 1, 'disponivel'),
  (2, 9788579360855, 2, 'disponivel'),
  (3, 9788579360855, 3, 'indisponivel'),
  (4, 9788579360855, 4, 'emprestado'),
  (5, 9788535245356, 1, 'disponivel'),
  (6, 9788535245356, 2, 'emprestado'),
  (7, 9788535245356, 3, 'emprestado'),
  (8, 9780133970777, 1, 'disponivel'),
  (9, 9780133970777, 2, 'emprestado'),
  (10, 9780131103627, 1, 'disponivel'),
  (11, 9780131103627, 2, 'disponivel'),
  (12, '9780134444284', 1, 'emprestado'),
  (13, 9780134444284, 2, 'disponivel'),
  (14, 9780262033848, 1, 'disponivel'),
  (15, 9780262033848, 2, 'emprestado'),
  (16, 9780134093413, 1, 'disponivel'),
  (17, 9780134093413, 2, 'disponivel'),
  (18, 9780321573513, 1, 'emprestado'),
  (19, 9780321573513, 2, 'disponivel'),
  (20, 9780198504634, 1, 'disponivel'),
  (21, 9780198504634, 2, 'emprestado'),
  (22, 9780470658000, 1, 'disponivel'),
  (23, 9780470658000, 2, 'disponivel');
-- Status_Emprestimo (Loan Statuses)
INSERT INTO universidade.status_emprestimo
VALUES (1, 'ativo'),
  (2, 'finalizado'),
  (3, 'atrasado');
-- Emprestimo (Loans)
INSERT INTO universidade.emprestimo
VALUES (
    1,
    1,
    '11111111100',
    1,
    '2022-01-10',
    '2022-02-10'
  ),
  (
    2,
    2,
    '11111111100',
    2,
    '2021-11-15',
    '2021-12-15'
  ),
  (
    3,
    4,
    '11111111100',
    3,
    '2021-09-01',
    '2021-10-01'
  ),
  (
    4,
    6,
    '11111111100',
    1,
    '2022-03-15',
    '2022-04-15'
  ),
  (
    5,
    7,
    '11111111100',
    2,
    '2021-12-01',
    '2022-01-01'
  ),
  (
    6,
    9,
    '11111111100',
    1,
    '2022-02-01',
    '2022-03-01'
  ),
  (
    7,
    12,
    '11111111100',
    2,
    '2021-10-15',
    '2021-11-15'
  ),
  (
    8,
    15,
    '11111111101',
    1,
    '2022-04-01',
    '2022-05-01'
  ),
  (
    9,
    18,
    '11111111101',
    2,
    '2021-11-01',
    '2021-12-01'
  ),
  (
    10,
    21,
    '11111111101',
    3,
    '2022-01-15',
    '2022-02-15'
  ),
  (
    11,
    1,
    '11111111101',
    1,
    '2022-03-01',
    '2022-04-01'
  ),
  (
    12,
    2,
    '11111111101',
    2,
    '2021-12-15',
    '2022-01-15'
  ),
  (
    13,
    4,
    '11111111101',
    1,
    '2022-02-15',
    '2022-03-15'
  ),
  (
    14,
    6,
    '11111111102',
    1,
    '2022-01-20',
    '2022-02-20'
  ),
  (
    15,
    7,
    '11111111102',
    2,
    '2021-10-01',
    '2021-11-01'
  ),
  (
    16,
    9,
    '11111111102',
    3,
    '2022-03-01',
    '2022-04-01'
  ),
  (
    17,
    12,
    '11111111102',
    1,
    '2021-11-15',
    '2021-12-15'
  ),
  (
    18,
    15,
    '11111111102',
    2,
    '2022-02-01',
    '2022-03-01'
  ),
  (
    19,
    18,
    '11111111102',
    1,
    '2021-12-01',
    '2022-01-01'
  ),
  (
    20,
    21,
    '11111111103',
    1,
    '2022-04-15',
    '2022-05-15'
  ),
  (
    21,
    1,
    '11111111103',
    2,
    '2021-09-15',
    '2021-10-15'
  ),
  (
    22,
    2,
    '11111111103',
    3,
    '2022-01-01',
    '2022-02-01'
  ),
  (
    23,
    4,
    '11111111103',
    1,
    '2021-11-01',
    '2021-12-01'
  ),
  (
    24,
    6,
    '11111111103',
    2,
    '2022-03-15',
    '2022-04-15'
  ),
  (
    25,
    7,
    '11111111103',
    1,
    '2022-02-15',
    '2022-03-15'
  ),
  (
    26,
    9,
    '11111111104',
    1,
    '2022-01-10',
    '2022-02-10'
  ),
  (
    27,
    12,
    '11111111104',
    2,
    '2021-12-01',
    '2022-01-01'
  ),
  (
    28,
    15,
    '11111111104',
    3,
    '2022-03-01',
    '2022-04-01'
  ),
  (
    29,
    18,
    '11111111104',
    1,
    '2021-11-15',
    '2021-12-15'
  ),
  (
    30,
    21,
    '11111111104',
    2,
    '2022-04-01',
    '2022-05-01'
  ),
  (
    31,
    1,
    '11111111104',
    1,
    '2022-02-01',
    '2022-03-01'
  ),
  (
    32,
    2,
    '11111111105',
    1,
    '2022-01-15',
    '2022-02-15'
  ),
  (
    33,
    4,
    '11111111105',
    2,
    '2021-10-01',
    '2021-11-01'
  ),
  (
    34,
    6,
    '11111111105',
    3,
    '2022-03-01',
    '2022-04-01'
  ),
  (
    35,
    7,
    '11111111105',
    1,
    '2021-11-15',
    '2021-12-15'
  ),
  (
    36,
    9,
    '11111111105',
    2,
    '2022-04-15',
    '2022-05-15'
  ),
  (
    37,
    12,
    '11111111105',
    1,
    '2022-02-01',
    '2022-03-01'
  ),
  (
    38,
    15,
    '11111111106',
    1,
    '2022-01-10',
    '2022-02-10'
  ),
  (
    39,
    18,
    '11111111106',
    2,
    '2021-12-01',
    '2022-01-01'
  ),
  (
    40,
    21,
    '11111111106',
    3,
    '2022-03-15',
    '2022-04-15'
  ),
  (
    41,
    1,
    '11111111106',
    1,
    '2021-11-01',
    '2021-12-01'
  ),
  (
    42,
    2,
    '11111111106',
    2,
    '2022-04-01',
    '2022-05-01'
  ),
  (
    43,
    4,
    '11111111106',
    1,
    '2022-02-15',
    '2022-03-15'
  ),
  (
    44,
    6,
    '11111111107',
    1,
    '2022-01-20',
    '2022-02-20'
  ),
  (
    45,
    7,
    '11111111107',
    2,
    '2021-10-15',
    '2021-11-15'
  ),
  (
    46,
    9,
    '11111111107',
    3,
    '2022-03-01',
    '2022-04-01'
  ),
  (
    47,
    12,
    '11111111107',
    1,
    '2021-12-01',
    '2022-01-01'
  ),
  (
    48,
    15,
    '11111111107',
    2,
    '2022-04-15',
    '2022-05-15'
  ),
  (
    49,
    18,
    '11111111107',
    1,
    '2022-02-01',
    '2022-03-01'
  ),
  (
    50,
    21,
    '11111111108',
    1,
    '2022-01-10',
    '2022-02-10'
  ),
  (
    51,
    1,
    '11111111108',
    2,
    '2021-11-15',
    '2021-12-15'
  ),
  (
    52,
    2,
    '11111111108',
    3,
    '2022-03-15',
    '2022-04-15'
  ),
  (
    53,
    4,
    '11111111108',
    1,
    '2021-12-01',
    '2022-01-01'
  ),
  (
    54,
    6,
    '11111111108',
    2,
    '2022-04-01',
    '2022-05-01'
  ),
  (
    55,
    7,
    '11111111108',
    1,
    '2022-02-15',
    '2022-03-15'
  ),
  (
    56,
    9,
    '11111111109',
    1,
    '2022-01-20',
    '2022-02-20'
  ),
  (
    57,
    12,
    '11111111109',
    2,
    '2021-10-01',
    '2021-11-01'
  ),
  (
    58,
    15,
    '11111111109',
    3,
    '2022-03-01',
    '2022-04-01'
  ),
  (
    59,
    18,
    '11111111109',
    1,
    '2021-11-15',
    '2021-12-15'
  ),
  (
    60,
    21,
    '11111111109',
    2,
    '2022-04-15',
    '2022-05-15'
  ),
  (
    61,
    1,
    '11111111109',
    1,
    '2022-02-01',
    '2022-03-01'
  ),
  (
    62,
    2,
    '11111111110',
    1,
    '2022-01-10',
    '2022-02-10'
  ),
  (
    63,
    4,
    '11111111110',
    2,
    '2021-12-01',
    '2022-01-01'
  ),
  (
    64,
    6,
    '11111111110',
    3,
    '2022-03-15',
    '2022-04-15'
  ),
  (
    65,
    7,
    '11111111110',
    1,
    '2021-11-01',
    '2021-12-01'
  ),
  (
    66,
    9,
    '11111111110',
    2,
    '2022-04-01',
    '2022-05-01'
  ),
  (
    67,
    12,
    '11111111110',
    1,
    '2022-02-15',
    '2022-03-15'
  ),
  (
    68,
    15,
    '11111111111',
    1,
    '2022-01-20',
    '2022-02-20'
  ),
  (
    69,
    18,
    '11111111111',
    2,
    '2021-10-15',
    '2021-11-15'
  ),
  (
    70,
    21,
    '11111111111',
    3,
    '2022-03-01',
    '2022-04-01'
  ),
  (
    71,
    1,
    '11111111111',
    1,
    '2021-12-01',
    '2022-01-01'
  ),
  (
    72,
    2,
    '11111111111',
    2,
    '2022-04-15',
    '2022-05-15'
  ),
  (
    73,
    4,
    '11111111111',
    1,
    '2022-02-01',
    '2022-03-01'
  ),
  (
    74,
    6,
    '11111111112',
    1,
    '2022-01-10',
    '2022-02-10'
  ),
  (
    75,
    7,
    '11111111112',
    2,
    '2021-11-15',
    '2021-12-15'
  ),
  (
    76,
    9,
    '11111111112',
    3,
    '2022-03-15',
    '2022-04-15'
  ),
  (
    77,
    12,
    '11111111112',
    1,
    '2021-12-01',
    '2022-01-01'
  ),
  (
    78,
    15,
    '11111111112',
    2,
    '2022-04-01',
    '2022-05-01'
  ),
  (
    79,
    18,
    '11111111112',
    1,
    '2022-02-15',
    '2022-03-15'
  ),
  (
    80,
    21,
    '11111111113',
    1,
    '2022-01-20',
    '2022-02-20'
  ),
  (
    81,
    1,
    '11111111113',
    2,
    '2021-10-01',
    '2021-11-01'
  ),
  (
    82,
    2,
    '11111111113',
    3,
    '2022-03-01',
    '2022-04-01'
  ),
  (
    83,
    4,
    '11111111113',
    1,
    '2021-11-15',
    '2021-12-15'
  ),
  (
    84,
    6,
    '11111111113',
    2,
    '2022-04-15',
    '2022-05-15'
  ),
  (
    85,
    7,
    '11111111113',
    1,
    '2022-02-01',
    '2022-03-01'
  ),
  (
    86,
    9,
    '11111111114',
    1,
    '2022-01-10',
    '2022-02-10'
  ),
  (
    87,
    12,
    '11111111114',
    2,
    '2021-12-01',
    '2022-01-01'
  ),
  (
    88,
    15,
    '11111111114',
    3,
    '2022-03-15',
    '2022-04-15'
  ),
  (
    89,
    18,
    '11111111114',
    1,
    '2021-11-01',
    '2021-12-01'
  ),
  (
    90,
    21,
    '11111111114',
    2,
    '2022-04-01',
    '2022-05-01'
  ),
  (
    91,
    1,
    '11111111114',
    1,
    '2022-02-15',
    '2022-03-15'
  ),
  (
    92,
    2,
    '22222222201',
    1,
    '2022-01-20',
    '2022-02-20'
  ),
  (
    93,
    4,
    '22222222201',
    2,
    '2021-10-15',
    '2021-11-15'
  ),
  (
    94,
    6,
    '22222222201',
    3,
    '2022-03-01',
    '2022-04-01'
  ),
  (
    95,
    7,
    '22222222201',
    1,
    '2021-12-01',
    '2022-01-01'
  ),
  (
    96,
    9,
    '22222222201',
    2,
    '2022-04-15',
    '2022-05-15'
  ),
  (
    97,
    12,
    '22222222201',
    1,
    '2022-02-01',
    '2022-03-01'
  ),
  (
    98,
    15,
    '22222222201',
    1,
    '2022-01-10',
    '2022-02-10'
  ),
  (
    99,
    18,
    '22222222202',
    2,
    '2021-11-15',
    '2021-12-15'
  ),
  (
    100,
    21,
    '22222222202',
    3,
    '2022-03-15',
    '2022-04-15'
  ),
  (
    101,
    1,
    '22222222202',
    1,
    '2021-12-01',
    '2022-01-01'
  ),
  (
    102,
    2,
    '22222222202',
    2,
    '2022-04-01',
    '2022-05-01'
  ),
  (
    103,
    4,
    '22222222202',
    1,
    '2022-02-15',
    '2022-03-15'
  ),
  (
    104,
    6,
    '22222222202',
    1,
    '2022-01-20',
    '2022-02-20'
  ),
  (
    105,
    7,
    '22222222203',
    2,
    '2021-10-01',
    '2021-11-01'
  ),
  (
    106,
    9,
    '22222222203',
    3,
    '2022-03-01',
    '2022-04-01'
  ),
  (
    107,
    12,
    '22222222203',
    1,
    '2021-11-15',
    '2021-12-15'
  ),
  (
    108,
    15,
    '22222222203',
    2,
    '2022-04-15',
    '2022-05-15'
  ),
  (
    109,
    18,
    '22222222203',
    1,
    '2022-02-01',
    '2022-03-01'
  ),
  (
    110,
    21,
    '22222222203',
    1,
    '2022-01-10',
    '2022-02-10'
  ),
  (
    111,
    1,
    '22222222204',
    2,
    '2021-12-01',
    '2022-01-01'
  ),
  (
    112,
    2,
    '22222222204',
    3,
    '2022-03-15',
    '2022-04-15'
  ),
  (
    113,
    4,
    '22222222204',
    1,
    '2021-11-01',
    '2021-12-01'
  ),
  (
    114,
    6,
    '22222222204',
    2,
    '2022-04-01',
    '2022-05-01'
  ),
  (
    115,
    7,
    '22222222204',
    1,
    '2022-02-15',
    '2022-03-15'
  ),
  (
    116,
    9,
    '22222222204',
    1,
    '2022-01-20',
    '2022-02-20'
  ),
  (
    117,
    12,
    '22222222205',
    2,
    '2021-10-15',
    '2021-11-15'
  ),
  (
    118,
    15,
    '22222222205',
    3,
    '2022-03-01',
    '2022-04-01'
  ),
  (
    119,
    18,
    '22222222205',
    1,
    '2021-12-01',
    '2022-01-01'
  ),
  (
    120,
    21,
    '22222222205',
    2,
    '2022-04-15',
    '2022-05-15'
  ),
  (
    121,
    1,
    '22222222205',
    1,
    '2022-02-01',
    '2022-03-01'
  ),
  (
    122,
    2,
    '22222222205',
    1,
    '2022-01-10',
    '2022-02-10'
  ),
  (
    123,
    4,
    '22222222206',
    2,
    '2021-11-15',
    '2021-12-15'
  ),
  (
    124,
    6,
    '22222222206',
    3,
    '2022-03-15',
    '2022-04-15'
  ),
  (
    125,
    7,
    '22222222206',
    1,
    '2021-12-01',
    '2022-01-01'
  ),
  (
    126,
    9,
    '22222222206',
    2,
    '2022-04-01',
    '2022-05-01'
  ),
  (
    127,
    12,
    '22222222206',
    1,
    '2022-02-15',
    '2022-03-15'
  ),
  (
    128,
    15,
    '22222222206',
    1,
    '2022-01-20',
    '2022-02-20'
  ),
  (
    129,
    18,
    '22222222207',
    2,
    '2021-10-01',
    '2021-11-01'
  ),
  (
    130,
    21,
    '22222222207',
    3,
    '2022-03-01',
    '2022-04-01'
  ),
  (
    131,
    1,
    '22222222207',
    1,
    '2021-11-15',
    '2021-12-15'
  ),
  (
    132,
    2,
    '22222222207',
    2,
    '2022-04-15',
    '2022-05-15'
  ),
  (
    133,
    4,
    '22222222207',
    1,
    '2022-02-01',
    '2022-03-01'
  ),
  (
    134,
    6,
    '22222222207',
    1,
    '2022-01-10',
    '2022-02-10'
  ),
  (
    135,
    7,
    '22222222208',
    2,
    '2021-12-01',
    '2022-01-01'
  ),
  (
    136,
    9,
    '22222222208',
    3,
    '2022-03-15',
    '2022-04-15'
  ),
  (
    137,
    12,
    '22222222208',
    1,
    '2021-11-01',
    '2021-12-01'
  ),
  (
    138,
    15,
    '22222222208',
    2,
    '2022-04-01',
    '2022-05-01'
  ),
  (
    139,
    18,
    '22222222208',
    1,
    '2022-02-15',
    '2022-03-15'
  ),
  (
    140,
    21,
    '22222222208',
    1,
    '2022-01-20',
    '2022-02-20'
  ),
  (
    141,
    1,
    '22222222209',
    2,
    '2021-10-15',
    '2021-11-15'
  ),
  (
    142,
    2,
    '22222222209',
    3,
    '2022-03-01',
    '2022-04-01'
  ),
  (
    143,
    4,
    '22222222209',
    1,
    '2021-12-01',
    '2022-01-01'
  ),
  (
    144,
    6,
    '22222222209',
    2,
    '2022-04-15',
    '2022-05-15'
  ),
  (
    145,
    7,
    '22222222209',
    1,
    '2022-02-01',
    '2022-03-01'
  ),
  (
    146,
    9,
    '22222222209',
    1,
    '2022-01-10',
    '2022-02-10'
  ),
  (
    147,
    12,
    '22222222210',
    2,
    '2021-11-15',
    '2021-12-15'
  ),
  (
    148,
    15,
    '22222222210',
    3,
    '2022-03-15',
    '2022-04-15'
  ),
  (
    149,
    18,
    '22222222210',
    1,
    '2021-12-01',
    '2022-01-01'
  ),
  (
    150,
    21,
    '22222222210',
    2,
    '2022-04-01',
    '2022-05-01'
  ),
  (
    151,
    1,
    '22222222210',
    1,
    '2022-02-15',
    '2022-03-15'
  ),
  (
    152,
    2,
    '22222222210',
    1,
    '2022-01-20',
    '2022-02-20'
  ),
  (
    153,
    4,
    '22222222211',
    2,
    '2021-10-01',
    '2021-11-01'
  ),
  (
    154,
    6,
    '22222222211',
    3,
    '2022-03-01',
    '2022-04-01'
  ),
  (
    155,
    7,
    '22222222211',
    1,
    '2021-11-15',
    '2021-12-15'
  ),
  (
    156,
    9,
    '22222222211',
    2,
    '2022-04-15',
    '2022-05-15'
  ),
  (
    157,
    12,
    '22222222211',
    1,
    '2022-02-01',
    '2022-03-01'
  ),
  (
    158,
    15,
    '22222222211',
    1,
    '2022-01-10',
    '2022-02-10'
  ),
  (
    159,
    18,
    '22222222212',
    2,
    '2021-12-01',
    '2022-01-01'
  ),
  (
    160,
    21,
    '22222222212',
    3,
    '2022-03-15',
    '2022-04-15'
  ),
  (
    161,
    1,
    '22222222212',
    1,
    '2021-11-01',
    '2021-12-01'
  ),
  (
    162,
    2,
    '22222222212',
    2,
    '2022-04-01',
    '2022-05-01'
  ),
  (
    163,
    4,
    '22222222212',
    1,
    '2022-02-15',
    '2022-03-15'
  ),
  (
    164,
    6,
    '22222222212',
    1,
    '2022-01-20',
    '2022-02-20'
  ),
  (
    165,
    7,
    '22222222213',
    2,
    '2021-10-15',
    '2021-11-15'
  ),
  (
    166,
    9,
    '22222222213',
    3,
    '2022-03-01',
    '2022-04-01'
  ),
  (
    167,
    12,
    '22222222213',
    1,
    '2021-12-01',
    '2022-01-01'
  ),
  (
    168,
    15,
    '22222222213',
    2,
    '2022-04-15',
    '2022-05-15'
  ),
  (
    169,
    18,
    '22222222213',
    1,
    '2022-02-01',
    '2022-03-01'
  ),
  (
    170,
    21,
    '22222222213',
    1,
    '2022-01-10',
    '2022-02-10'
  ),
  (
    171,
    1,
    '22222222214',
    2,
    '2021-11-15',
    '2021-12-15'
  ),
  (
    172,
    2,
    '22222222214',
    3,
    '2022-03-15',
    '2022-04-15'
  ),
  (
    173,
    4,
    '22222222214',
    1,
    '2021-12-01',
    '2022-01-01'
  ),
  (
    174,
    6,
    '22222222214',
    2,
    '2022-04-01',
    '2022-05-01'
  ),
  (
    175,
    7,
    '22222222214',
    1,
    '2022-02-15',
    '2022-03-15'
  ),
  (
    176,
    9,
    '22222222214',
    1,
    '2022-01-20',
    '2022-02-20'
  ),
  (
    177,
    12,
    '22222222215',
    2,
    '2021-10-01',
    '2021-11-01'
  ),
  (
    178,
    15,
    '22222222215',
    3,
    '2022-03-01',
    '2022-04-01'
  ),
  (
    179,
    18,
    '22222222215',
    1,
    '2021-11-15',
    '2021-12-15'
  ),
  (
    180,
    21,
    '22222222215',
    2,
    '2022-04-15',
    '2022-05-15'
  ),
  (
    181,
    1,
    '22222222215',
    1,
    '2022-02-01',
    '2022-03-01'
  ),
  (
    182,
    2,
    '22222222215',
    1,
    '2022-01-10',
    '2022-02-10'
  );