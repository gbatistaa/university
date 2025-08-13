// Função que recebe o client já conectado e uma query string
export async function executeQuery(client, queryString) {
  try {
    const res = await client.query(queryString);
    return res.rows;
  } catch (err) {
    console.error("Erro ao executar query:", err.message);
    throw err;
  }
}
