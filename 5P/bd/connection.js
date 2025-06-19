const { MongoClient } = require("mongodb");

const uri = "mongodb://54.175.56.159:27017";

// ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABgQDKdEI4J39/qpitHJE0Ho9naj9YSf2zqwoqIRHaNlgIv3kMIr/oNM18496d5uNIlCbGgmMywU/Fdyhm9239khUpl9WRq2TFlTkXXRW3m1873E2F72Eo/8PufgWiV/75bxMxFjpoHJR235Tcz9Slgloc0gJuxYxFudkAhKGilA0ua0VJeY4DokCy6F7l4jRdJNq/FexvddmQR9suqrqj6I7YvfR9eRzO2s0qdBpdPT/hMOVhxeRK8dr0YreUBwvAnNg+Hnemjr5FiT1rRF+kevN9hJxAKoc4ZRpr1lPDtdN7v0dJnFZ6hzEWkM02WvjzDCNR/T2lnWR38YDPkYyLaYp17TjzxYQi0uB0fDt83ol/K4lQif/e6BvaXJdMZau6RstqcZVl8XXpFxol9Ht/R8hsfst/EZpzRjfjVFpvoE2vaYr4F2zRZcwSMNrnbhTeb5g6J9GrOaoe4dRJmoiJNQClhl8HpuGtiaEsX7vV4Q/m2KmFMrqtSdIG2y6T5Yz6siE= ec2-user@ip-172-31-91-226.ec2.internal

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
