import { Pool } from "pg";
import { executeSQLfile } from "./executeSQLFile.js";

const pool = new Pool({
  user: "postgres",
  host: "localhost",
  database: "university",
  password: "muitosegura",
  port: 5432,
});

// Função que recebe o client já conectado e uma query string
async function executeQuery(client, queryString) {
  try {
    const res = await client.query(queryString);
    return res.rows;
  } catch (err) {
    console.error("Erro ao executar query:", err.message);
    throw err;
  }
}

async function connectToDatabase(filePath) {
  let db;
  try {
    console.clear();
    db = await pool.connect();
    console.log("Conexão bem-sucedida ao banco de dados PostgreSQL!");
    if (filePath) await executeSQLfile(db, filePath);
    const rows = await executeQuery(
      db,
      `SELECT u.primeiro_nome, u.sobrenome, COUNT(DISTINCT e.id_exemplar) as livros_emprestados
       FROM universidade.emprestimo e
       JOIN universidade.exemplar ex ON ex.id_exemplar = e.id_exemplar
       JOIN universidade.usuario u ON u.cpf = e.cpf
       GROUP BY u.primeiro_nome, u.sobrenome;
       `
    );

    console.table(rows);
  } catch (err) {
    console.error("Error when connecting to the database:", err.stack);
  } finally {
    await db.end(); // Encerre o pool APENAS aqui, uma vez só
    console.log("Conexão com o banco de dados encerrada.");
  }
}

await connectToDatabase("./sql/universidade_schema.sql");
