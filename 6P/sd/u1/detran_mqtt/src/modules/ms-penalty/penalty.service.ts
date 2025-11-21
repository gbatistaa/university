import { Inject, Injectable } from '@nestjs/common';
import { CreatePenaltyDto } from './dto/create-penalty.dto';
import { Penalty } from './entities/penalty.entity';
import { Repository } from 'typeorm';
import { ClientProxy } from '@nestjs/microservices';
import { InjectRepository } from '@nestjs/typeorm';
import { firstValueFrom } from 'rxjs';
import { Vehicle } from '../ms-vehicle/entities/vehicle.entity';

@Injectable()
export class PenaltyService {
  constructor(
    @Inject('MQTT_CLIENT') private readonly mqttClient: ClientProxy,
    @InjectRepository(Penalty) private repo: Repository<Penalty>,
  ) {}

  createPenalty(createPenaltyDto: CreatePenaltyDto) {
    const penalty = this.repo.create(createPenaltyDto);
    return this.repo.save(penalty);
  }

  async getVehiclePenalties(sign: string) {
    const vehicle$ = this.mqttClient.send<Vehicle>('commands/vehicle/findOne', {
      sign,
    });

    const vehicle: Vehicle = await firstValueFrom<Vehicle>(vehicle$);

    console.log('Veículo encontrado:', vehicle);
    return vehicle;
  }
}
