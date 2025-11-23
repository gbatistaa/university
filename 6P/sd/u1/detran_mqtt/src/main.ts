import { NestFactory } from '@nestjs/core';
import { AppModule } from './app.module';
import { Transport } from '@nestjs/microservices';
import path from 'path';

async function bootstrap() {
  // 1. Cria a aplicação HTTP principal
  const app = await NestFactory.create(AppModule);

  // 2. Conecta o Microsserviço MQTT
  app.connectMicroservice({
    transport: Transport.MQTT,
    options: {
      url: 'mqtt://localhost:1883',
    },
  });

  // 3. Inicia ambos os listeners

  // Inicia o listener do MQTT
  await app.startAllMicroservices();

  // Inicia o servidor HTTP na porta (ex: 3000)
  const HTTP_PORT = 3000;
  await app.listen(HTTP_PORT);

  console.log(`🚀 Gateway HTTP rodando na porta ${HTTP_PORT}`);
}

void bootstrap();
