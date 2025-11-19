import { Module } from '@nestjs/common';
import { AppController } from './app.controller';
import { AppService } from './app.service';
import { ConductorModule } from './modules/conductor/conductor.module';
import { TypeOrmModule } from '@nestjs/typeorm';
import { VehicleModule } from './modules/vehicle/vehicle.module';
import path from 'path';

@Module({
  imports: [
    ConductorModule,
    TypeOrmModule.forRoot({
      type: 'sqlite',
      database: path.join(__dirname, '.', 'db', 'database.sqlite'),
      autoLoadEntities: true,
      logging: false,
    }),
    VehicleModule,
  ],
  controllers: [AppController],
  providers: [AppService],
})
export class AppModule {}
