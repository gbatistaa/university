const { MongoClient } = require("mongodb");

// aqui é a url com o ip da estância ec2 (amazon linux) e a porta do banco MongoDB
// Vou inserir dados e criar um usuário com nome e senha professor
const uri = "mongodb://52.91.46.22:27017";
const client = new MongoClient(uri);

async function main() {
  try {
    await client.connect();
    const db = client.db("escola");
    // await createUser(db, "professor", "professor");
    const bibleBooks = db.collection("bibleBooks");

    await bibleBooks.deleteMany({});

    const books = [
      {
        name: "Matthew",
        chapters: 28,
        author: "Matthew",
        testament: "NT",
        yearWritten: 70,
      },
      {
        name: "Mark",
        chapters: 16,
        author: "Mark",
        testament: "NT",
        yearWritten: 65,
      },
      {
        name: "Luke",
        chapters: 24,
        author: "Luke",
        testament: "NT",
        yearWritten: 80,
      },
      {
        name: "Genesis",
        chapters: 50,
        author: "Moses",
        testament: "AT",
        yearWritten: -1450,
      },
      {
        name: "Exodus",
        chapters: 40,
        author: "Moses",
        testament: "AT",
        yearWritten: -1400,
      },
      {
        name: "Psalms",
        chapters: 150,
        author: "David",
        testament: "AT",
        yearWritten: -1000,
      },
      {
        name: "Isaiah",
        chapters: 66,
        author: "Isaiah",
        testament: "AT",
        yearWritten: -700,
      },
    ];

    await bibleBooks.insertMany(books);
    console.log("Books inserted.");

    const allBooks = await bibleBooks.find().toArray();
    console.log("\nAll books:");
    console.table(allBooks);

    const oldTestamentBooks = await bibleBooks
      .find({ testament: "AT" })
      .toArray();
    console.log("\nOld Testament books:");
    console.table(oldTestamentBooks);
  } catch (err) {
    console.error("Error:", err);
  } finally {
    await client.close();
  }
}

async function createUser(db, userName, pwd) {
  try {
    await db.command({
      createUser: userName,
      pwd: pwd,
      roles: [{ role: "readWrite", db: "escola" }],
    });
    console.log("User 'professor' created.");
  } catch (err) {
    // Se o usuário já existir, evita erro
    if (err.codeName === "DuplicateKey") {
      console.log("User 'professor' already exists.");
    } else {
      throw err;
    }
  }
}

main();
