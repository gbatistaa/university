const { MongoClient } = require("mongodb");

const uri = "mongodb://54.175.56.159:27017";
const client = new MongoClient(uri);

async function main() {
  try {
    await client.connect();
    const db = client.db("escola");
    const bibleBooks = db.collection("biblebooks");

    await bibleBooks.deleteMany({});

    const books = [
      {
        id: 1,
        name: "Matthew",
        pages: 28,
        author: "Matthew",
        testament: "NT",
        yearWritten: 70,
      },
      {
        id: 2,
        name: "Mark",
        pages: 16,
        author: "Mark",
        testament: "NT",
        yearWritten: 65,
      },
      {
        id: 3,
        name: "Luke",
        pages: 24,
        author: "Luke",
        testament: "NT",
        yearWritten: 80,
      },
      {
        id: 4,
        name: "Genesis",
        pages: 50,
        author: "Moses",
        testament: "AT",
        yearWritten: -1450,
      },
      {
        id: 5,
        name: "Exodus",
        pages: 40,
        author: "Moses",
        testament: "AT",
        yearWritten: -1400,
      },
      {
        id: 6,
        name: "Psalms",
        pages: 150,
        author: "David",
        testament: "AT",
        yearWritten: -1000,
      },
      {
        id: 7,
        name: "Isaiah",
        pages: 66,
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

main();
