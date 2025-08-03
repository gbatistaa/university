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
  console.log(filePath);
  let db;
  try {
    db = await pool.connect();
    console.log("Conexão bem-sucedida ao banco de dados PostgreSQL!");
    await executeSQLfile(db, filePath);
    // const rows = await executeQuery(
    //   db,
    //   `SELECT DISTINCT *
    //     FROM universidade.plano
    //   LEFT JOIN universidade.departamento d ON d.chefe = mat_professor
    //     WHERE mat_estudante IS NOT NULL
    //   OR d.chefe IS NOT NULL;`
    // );

    // console.table(rows);
  } catch (err) {
    console.error("Error when connecting to the database:", err.stack);
  } finally {
    await db.end(); // Encerre o pool APENAS aqui, uma vez só
    console.log("Conexão com o banco de dados encerrada.");
  }
}

await connectToDatabase("./sql/clinica_schema.sql");
