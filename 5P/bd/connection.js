const { MongoClient } = require("mongodb");

const uri = "mongodb://54.175.56.159:27017";

async function run() {
  const client = new MongoClient(uri);

  try {
    await client.connect();
    console.log("Conectado ao MongoDB remoto!");

    const database = client.db("test"); // nome do banco
    const collection = database.collection("exemplo");

    // Exemplo: inserir documento
    const result = await collection.insertOne({
      nome: "AWS EC2",
      linguagem: "JavaScript",
    });
    console.log(`Documento inserido com id: ${result.insertedId}`);
  } catch (e) {
    console.error(e);
  } finally {
    await client.close();
  }
}

run();
