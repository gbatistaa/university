import { Module } from '@nestjs/common';
import { ConductorService } from './conductor.service';
import { ConductorController } from './conductor.controller';
import { ClientsModule, Transport } from '@nestjs/microservices';
import { TypeOrmModule } from '@nestjs/typeorm';
import { Conductor } from './entities/conductor.entity';

@Module({
  imports: [
    TypeOrmModule.forFeature([Conductor]),
    ClientsModule.register([
      {
        name: 'MQTT_CLIENT',
        transport: Transport.MQTT,
        options: { url: 'mqtt://localhost:1883' },
      },
    ]),
  ],
  controllers: [ConductorController],
  providers: [ConductorService],
})
export class ConductorModule {}
