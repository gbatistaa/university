import fs from "fs/promises";

export async function executeSQLfile(db, filePath) {
  try {
    const sqlContent = await fs.readFile(filePath, "utf8");

    const sqlCommands = sqlContent
      .split(";")
      .map((cmd) => cmd.trim())
      .filter((cmd) => cmd.length > 0);

    for (const command of sqlCommands) {
      try {
        await db.query(command);
        console.log(
          `Comando executado com sucesso: ${command.substring(0, 50)}...`
        );
      } catch (err) {
        console.error(
          `Erro ao executar comando: ${command.substring(0, 50)}...`,
          err.stack
        );
      }
    }

    console.log("Arquivo SQL executado com sucesso.");
  } catch (err) {
    console.error("Erro ao processar o arquivo SQL:", err.stack);
  } finally {
    if (db) db.release();
  }
}
