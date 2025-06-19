const { MongoClient } = require("mongodb");

const uri = "mongodb://52.91.46.22:27017";
const client = new MongoClient(uri);

async function clearDatabase() {
  try {
    await client.connect();
    const db = client.db("escola");

    // 1. Excluir todas as collections
    const collections = await db.listCollections().toArray();
    for (const coll of collections) {
      await db.collection(coll.name).drop();
      console.log(`Collection '${coll.name}' dropped.`);
    }

    // 2. Excluir todos os usuários do banco 'escola'
    const usersInfo = await db.command({ usersInfo: 1 });
    for (const user of usersInfo.users) {
      // Evitar apagar usuário 'admin' ou sistema se quiser:
      if (user.user !== "admin") {
        await db.command({ dropUser: user.user });
        console.log(`User '${user.user}' dropped.`);
      }
    }

    console.log("Database cleared: all collections and users removed.");
  } catch (err) {
    console.error("Error clearing database:", err);
  } finally {
    await client.close();
  }
}

clearDatabase();
