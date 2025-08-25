import { executeQuery } from "../../utils/executeQuery.js";
import { executeSQLfile } from "../../utils/executeSQLFile.js";
import { pool } from "../../utils/pool.js";

export async function connectToDatabaseAndExec(pool, filePath) {
  let db;
  try {
    console.clear();
    db = await pool.connect();
    console.log("Conexão bem-sucedida ao banco de dados PostgreSQL!");
    if (filePath) await executeSQLfile(db, filePath);
    const rows = await executeQuery(
      db,
      `WITH anas_department AS (
        SELECT *
        FROM company.departments d
        JOIN company.employees e USING(dept_id)
        WHERE CONCAT(e.first_name, ' ', e.last_name) = 'Ana Oliveira'
       )

       SELECT first_name, last_name
       FROM company.employees e
       JOIN company.departments d USING(dept_id)
       WHERE anas_department.dept_id = d.dept_id ;
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

await connectToDatabaseAndExec(pool);
